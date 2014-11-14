/*
 * simpleMemory.h
 *
 * Copyright (C) 2014, GreenSocs Ltd.
 *
 * Developped by Konrad Frederic <fred.konrad@greensocs.com>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or (at
 * your option) any later version.
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
 * General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, see <http://www.gnu.org/licenses>.
 *
 * Linking GreenSocs code, statically or dynamically with other modules
 * is making a combined work based on GreenSocs code. Thus, the terms and
 * conditions of the GNU General Public License cover the whole
 * combination.
 *
 * In addition, as a special exception, the copyright holders, GreenSocs
 * Ltd, give you permission to combine GreenSocs code with free software
 * programs or libraries that are released under the GNU LGPL, under the
 * OSCI license, under the OCP TLM Kit Research License Agreement or
 * under the OVP evaluation license.You may copy and distribute such a
 * system following the terms of the GNU GPL and the licenses of the
 * other code concerned.
 *
 * Note that people who make modified versions of GreenSocs code are not
 * obligated to grant this special exception for their modified versions;
 * it is their choice whether to do so. The GNU General Public License
 * gives permission to release a modified version without this exception;
 * this exception also makes it possible to release a modified version
 * which carries forward this exception.
 *
 */

/*
 * This is a simple Memory for SystemC using GreenLib.
 */

#ifndef MEMORY_H
#define MEMORY_H

//#define MEMORY_DEBUG
#ifdef MEMORY_DEBUG
#define DCOUT(var) std::cout << "Memory: " << sc_core::sc_time_stamp() << " :" \
                     << var << std::endl;
#else
#define DCOUT(var)
#endif

#include <systemc.h>
#include <sys/syscall.h>
#include <sys/mman.h>

#include "gsgpsocket/transport/GSGPSlaveSocket.h"

class Memory:
  public sc_module,
  public gs::tlm_b_if<gs::gp::GenericSlaveAccessHandle>
{
  public:
    Memory(sc_core::sc_module_name name):
    sc_module(name),
    target_port("target_port"),
    m_size("size", 262144),
    m_ro("read_only", false)
    {
      this->target_port.bind_b_if(*this);
      this->target_port.register_get_direct_mem_ptr(this,
                                                   &Memory::get_direct_mem_ptr);
    }

    ~Memory()
    {
      if (m_ptr) {
        munmap(m_ptr, m_size * 1024);
      }
      DCOUT("destructor.");
    }

    void end_of_elaboration()
    {
      m_ptr = mmap(0, m_size * 1024, PROT_READ | PROT_WRITE,
                         MAP_ANONYMOUS | MAP_PRIVATE, -1, 0);

      if (!m_ptr)
      {
        std::cout << "Memory: error: can't create memory." << std::endl;
        sc_core::sc_stop();
        return;
      }
    }

    void setReadOnly(bool read_only)
    {
      m_ro = read_only;
    }

    void b_transact(gs::gp::GenericSlaveAccessHandle ah)
    {
      gs::gp::GenericSlavePort<32>::accessHandle t = _getSlaveAccessHandle(ah);
      uint32_t offset = t->getMAddr() - target_port.base_addr;
      gs::GSDataType data;
      size_t size;

      data.set(t->getMData());
      size = data.getSize();

      if (offset + size > m_size * 1024)
      {
        std::cout << "error out of bound memory access.." << std::endl;
        sc_core::sc_stop();
        return;
      }

      if (t->getMCmd() == gs::Generic_MCMD_RD)
      {
        memcpy(data.getDataPtr(), &(((uint8_t *)m_ptr)[offset]), size);
      }
      else if (t->getMCmd() == gs::Generic_MCMD_WR)
      {
        /*
         * FIXME: We should give a bad status for that...
         */
        if (!m_ro)
        {
          memcpy(&(((uint8_t *)m_ptr)[offset]), data.getDataPtr(), size);
        }
      }
      else
      {
        std::cout << "invalid command.." << std::endl;
        sc_core::sc_stop();
        return;
      }
    }

    /*
     * XXX: What about Read Only??
     */
    bool get_direct_mem_ptr(unsigned int from,
                            tlm::tlm_generic_payload& trans,
                            tlm::tlm_dmi& dmi_data)
    {
      dmi_data.set_dmi_ptr((unsigned char *)m_ptr);
      dmi_data.set_start_address(0);
      dmi_data.set_end_address(m_size * 1024 - 1);
      trans.set_dmi_allowed(true);
      return true;
    }

    /*
     * Socket for memory access.
     */
    gs::gp::GenericSlavePort<32> target_port;
  private:
    gs::gs_param<uint32_t> m_size;
    gs::gs_param<bool> m_ro; /*!< read only? */
    void *m_ptr;
};

#endif /* MEMORY_H */

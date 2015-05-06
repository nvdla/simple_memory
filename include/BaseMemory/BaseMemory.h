/*
* @file BaseMemory.h
* @author Guillaume Delbergue <guillaume.delbergue@greensocs.com>
* @author Frederic Konrad <fred.konrad@greensocs.com>
* @date May, 2015
* @copyright Copyright (C) 2015, GreenSocs Ltd.
*
* @brief Base memory for SystemC using GreenLib.
* @details You shouldn't directly instantiate this class.
*/

#ifndef BASE_MEMORY_H
#define BASE_MEMORY_H

//#define MEMORY_DEBUG
#ifdef MEMORY_DEBUG
#define DCOUT(var) std::cout << "BaseMemory: " << sc_core::sc_time_stamp()     \
                             << " :" << var << std::endl;
#else
#define DCOUT(var)
#endif

#include <systemc>
#include <stdint.h>
#include <sys/mman.h>

#include "gsgpsocket/transport/GSGPSlaveSocket.h"

class BaseMemory: public sc_core::sc_module
{
public:
    BaseMemory(sc_core::sc_module_name name):
    sc_core::sc_module(name),
    m_size("size", 1000),
    m_ro("read_only", false)
    {
    }

    ~BaseMemory()
    {
        if (m_ptr)
        {
            munmap(m_ptr, m_size * 1024);
        }
    }

    void end_of_elaboration()
    {
        m_ptr = mmap(0, m_size * 1024, PROT_READ | PROT_WRITE,
                                       MAP_ANONYMOUS | MAP_PRIVATE,
                                       -1, 0);

        if (!m_ptr)
        {
            std::cout << "BaseMemory: error: can't create memory." << std::endl;
            sc_core::sc_stop();
            return;
        }
    }

    void setReadOnly(bool read_only)
    {
        m_ro = read_only;
    }

    void b_transact(gs::gp::GenericSlaveAccessHandle ah, uint32_t offset)
    {
        gs::gp::GenericSlavePort<32>::accessHandle t =
            _getSlaveAccessHandle(ah);
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
            std::cout << "Invalid command ..." << std::endl;
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

private:
    gs::gs_param<uint32_t> m_size;
    gs::gs_param<bool> m_ro; /*!< read only? */
    void *m_ptr;
};

#endif /* BASE_MEMORY_H */

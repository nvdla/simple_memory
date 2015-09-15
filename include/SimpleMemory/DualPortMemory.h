/*
* @file DualPortMemory.h
* @author Guillaume Delbergue <guillaume.delbergue@greensocs.com>
* @date May, 2015
* @copyright Copyright (C) 2015, GreenSocs Ltd.
*
* @brief Dual port memory for SystemC using GreenLib.
*/

#ifndef DUAL_PORT_MEMORY_H
#define DUAL_PORT_MEMORY_H

#include <systemc>
#include <gsgpsocket/transport/GSGPSlaveSocket.h>

#define BASE_MEMORY_INCLUSION_PROTECTOR 
#include "SimpleMemory/BaseMemory.h"
#undef BASE_MEMORY_INCLUSION_PROTECTOR

class DualPortMemory:
  public sc_core::sc_module,
  public BaseMemory
{
public:
    DualPortMemory(sc_core::sc_module_name name):
    sc_core::sc_module(name),
    BaseMemory(),
    targetPort0("target_port_0"),
    targetPort1("target_port_1"),
    m_size("size", 1000),
    m_ro("read_only", false)    
    {
        wrap_1.register_b_transport(&DualPortMemory::b_transact_0, this);
        this->targetPort0.bind_b_if(wrap_1);
        this->targetPort0.register_get_direct_mem_ptr(this,
            &DualPortMemory::get_direct_mem_ptr);

        wrap_2.register_b_transport(&DualPortMemory::b_transact_1, this);
        this->targetPort1.bind_b_if(wrap_2);
        this->targetPort1.register_get_direct_mem_ptr(this,
            &DualPortMemory::get_direct_mem_ptr);
    }

    void b_transact_0(gs::gp::GenericSlaveAccessHandle ah)
    {
        uint32_t offset;
        gs::gp::GenericSlavePort<32>::accessHandle t =
            _getSlaveAccessHandle(ah);

        offset = t->getMAddr() - targetPort0.base_addr;
        BaseMemory::b_transact(t, offset);
    }

    void b_transact_1(gs::gp::GenericSlaveAccessHandle ah)
    {
        uint32_t offset;
        gs::gp::GenericSlavePort<32>::accessHandle t =
            _getSlaveAccessHandle(ah);

        offset = t->getMAddr() - targetPort1.base_addr;
        BaseMemory::b_transact(t, offset);
    }

    void end_of_elaboration()
    {
      this->allocate_memory(m_size, m_ro);
    }

    // Sockets for memory access
    gs::gp::GenericSlavePort<32> targetPort0;
    gs::gp::GenericSlavePort<32> targetPort1;

  private:
    gs::gs_param<uint32_t> m_size;
    gs::gs_param<bool> m_ro; /*!< read only? */
    gs::tlm_b_if_wrapper<DualPortMemory,
                         gs::gp::GenericSlaveAccessHandle> wrap_1;
    gs::tlm_b_if_wrapper<DualPortMemory,
                         gs::gp::GenericSlaveAccessHandle> wrap_2;
};

#endif /* DUAL_PORT_MEMORY_H */

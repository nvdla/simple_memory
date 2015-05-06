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

#include "BaseMemory/BaseMemory.h"

class DualPortMemory : public BaseMemory,
    public gs::tlm_multi_b_if<gs::gp::GenericSlaveAccessHandle>
{
public:
    DualPortMemory(sc_core::sc_module_name name):
    BaseMemory(name),
    targetPort0("target_port_0"),
    targetPort1("target_port_1")
    {
        this->targetPort0.bind_b_if(*this);
        this->targetPort0.register_get_direct_mem_ptr(this,
            &DualPortMemory::get_direct_mem_ptr);

        this->targetPort1.bind_b_if(*this);
        this->targetPort1.register_get_direct_mem_ptr(this,
            &DualPortMemory::get_direct_mem_ptr);
    }

    void b_transact(gs::gp::GenericSlaveAccessHandle ah, unsigned int index)
    {
        uint32_t offset;
        gs::gp::GenericSlavePort<32>::accessHandle t =
            _getSlaveAccessHandle(ah);
        if(index == 0) {
            offset = t->getMAddr() - targetPort0.base_addr;
        } else {
            offset = t->getMAddr() - targetPort1.base_addr;
        }
        b_transact(ah, offset);
    }

    // Sockets for memory access
    gs::gp::GenericSlavePort<32> targetPort0;
    gs::gp::GenericSlavePort<32> targetPort1;
};

#endif /* DUAL_PORT_MEMORY_H */

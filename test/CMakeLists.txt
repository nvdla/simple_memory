#
# CMakeLists.txt
#
# Copyright (C) 2014, GreenSocs ltd.
#
# Developed by Frederic Konrad <fred.konrad@greensocs.com>
#              Guillaume Delbergue <guillaume.delbergue@greensocs.com>
#

add_executable(simplememory-test SimpleMemoryTest.cpp)
add_executable(dualportmemory-test DualPortMemoryTest.cpp)

target_link_libraries(simplememory-test ${SystemC_LIBRARIES})

set(DUALPORTMEMORY_TEST_LINK_LIBRARIES ${SystemC_LIBRARIES})
if(MINGW)
    list(APPEND DUALPORTMEMORY_TEST_LINK_LIBRARIES mman)
endif(MINGW)
target_link_libraries(dualportmemory-test ${DUALPORTMEMORY_TEST_LINK_LIBRARIES})

add_test(NAME simplememory COMMAND ./simplememory-test)
add_test(NAME dualportmemory COMMAND ./dualportmemory-test)
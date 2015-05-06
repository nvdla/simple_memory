#
# CMakeLists.txt
#
# Copyright (C) 2014, GreenSocs ltd.
#
# Developed by Frederic Konrad <fred.konrad@greensocs.com>
#

add_executable(simplememory-test SimpleMemoryTest.cpp)

target_link_libraries(simplememory-test ${SystemC_LIBRARIES})

add_test(NAME simplememory COMMAND ./simplememory-test)
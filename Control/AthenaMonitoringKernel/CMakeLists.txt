# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

# Declare the package name:
atlas_subdir( AthenaMonitoringKernel )

# External dependencies:
find_package( nlohmann_json )
find_package( valgrind )
find_package( ROOT COMPONENTS Core Hist )
find_package( Boost COMPONENTS program_options )

atlas_add_library(
    AthenaMonitoringKernelLib
    AthenaMonitoringKernel/*.h
    src/*.cxx
    src/HistogramFiller/*.cxx
    PUBLIC_HEADERS AthenaMonitoringKernel
    INCLUDE_DIRS ${ROOT_INCLUDE_DIRS}
    LINK_LIBRARIES ${ROOT_LIBRARIES} GaudiKernel AthenaBaseComps StoreGateLib
                   CxxUtils xAODEventInfo
    PRIVATE_INCLUDE_DIRS ${Boost_INCLUDE_DIRS}
    PRIVATE_LINK_LIBRARIES ${Boost_LIBRARIES} nlohmann_json::nlohmann_json
)

atlas_add_component(
    AthenaMonitoringKernel
    src/components/*.cxx
    PRIVATE_LINK_LIBRARIES AthenaMonitoringKernelLib
)

# Install files from the package:
atlas_install_python_modules( python/*.py 
                              POST_BUILD_CMD ${ATLAS_FLAKE8} )

# Decide whether to use Valgrind in the unit tests.
set( _extra_includes )
set( _test_definitions )
if( VALGRIND_FOUND )
   list( APPEND _extra_includes ${VALGRIND_INCLUDE_DIRS} )
   list( APPEND _test_definitions DEFINITIONS ATHMON_VALGRIND )
endif()

# Unit tests C++:
file( GLOB CXX_TEST_FILES CONFIGURE_DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/test/*.cxx )
foreach ( test_file ${CXX_TEST_FILES} )
    get_filename_component( name ${test_file} NAME_WE)
    set( rundir ${CMAKE_CURRENT_BINARY_DIR}/unitTestRun_cxx_${name} )
    file( REMOVE_RECURSE ${rundir} )
    file( MAKE_DIRECTORY ${rundir} )
    atlas_add_test( ${name}
        SOURCES ${test_file}
        INCLUDE_DIRS ${ROOT_INCLUDE_DIRS} ${Boost_INCLUDE_DIRS}
                     ${_extra_includes}
        ${_test_definitions}
        LINK_LIBRARIES ${ROOT_LIBRARIES} ${Boost_LIBRARIES} TestTools
                       GaudiKernel AthenaKernel CxxUtils
                       AthenaMonitoringKernelLib nlohmann_json::nlohmann_json
        ENVIRONMENT "JOBOPTSEARCHPATH=${CMAKE_CURRENT_SOURCE_DIR}/share"
        POST_EXEC_SCRIPT nopost.sh
        PROPERTIES TIMEOUT 300
        PROPERTIES WORKING_DIRECTORY ${rundir}
    )
endforeach()

# Unit tests Python:
file( GLOB PYTHON_TEST_FILES CONFIGURE_DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/test/*.py )
foreach ( test_file ${PYTHON_TEST_FILES} )
    get_filename_component( name ${test_file} NAME_WE )
    set( rundir ${CMAKE_CURRENT_BINARY_DIR}/unitTestRun_py_${name} )
    file( REMOVE_RECURSE ${rundir} )
    file( MAKE_DIRECTORY ${rundir} )
    atlas_add_test( ${name}
        SCRIPT python ${test_file}
        POST_EXEC_SCRIPT nopost.sh
        PROPERTIES WORKING_DIRECTORY ${rundir}
    )
endforeach()

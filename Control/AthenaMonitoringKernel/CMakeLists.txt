################################################################################
# Package: AthenaMonitoringKernel
################################################################################

# Declare the package name:
atlas_subdir( AthenaMonitoringKernel )

# Declare the package's dependencies:
atlas_depends_on_subdirs(
    PUBLIC
        Control/AthenaBaseComps
        Event/EventInfo
)

atlas_add_library(
    AthenaMonitoringKernelLib
    src/*.cxx
    src/HistogramFiller/*.cxx
    PUBLIC_HEADERS AthenaMonitoringKernel
    LINK_LIBRARIES AthenaBaseComps EventInfo
)

atlas_add_component(
    AthenaMonitoringKernel
    src/components/*.cxx
    LINK_LIBRARIES AthenaMonitoringKernelLib
)

# Install files from the package:
atlas_install_python_modules( python/*.py 
                              POST_BUILD_CMD ${ATLAS_FLAKE8} )
atlas_install_joboptions( share/*.py )

# Unit tests C++:
file( GLOB CXX_TEST_FILES CONFIGURE_DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/test/*.cxx )
foreach ( test_file ${CXX_TEST_FILES} )
    get_filename_component( name ${test_file} NAME_WE)
    set( rundir ${CMAKE_CURRENT_BINARY_DIR}/unitTestRun_cxx_${name} )
    file( REMOVE_RECURSE ${rundir} )
    file( MAKE_DIRECTORY ${rundir} )
    atlas_add_test( ${name}
        SOURCES ${test_file}
        LINK_LIBRARIES TestTools AthenaMonitoringKernelLib
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

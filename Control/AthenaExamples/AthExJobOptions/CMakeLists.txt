################################################################################
# Package: AthExJobOptions
################################################################################

# Declare the package name:
atlas_subdir( AthExJobOptions )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PUBLIC
                          Control/AthenaBaseComps
                          Control/AthenaKernel
                          GaudiKernel
                          PRIVATE
                          AtlasTest/TestTools
                          Control/StoreGate
                          Event/EventInfo )

# Component(s) in the package:
atlas_add_component( AthExJobOptions
                     src/*.cxx
                     src/components/*.cxx
                     LINK_LIBRARIES AthenaBaseComps AthenaKernel GaudiKernel StoreGateLib SGtests EventInfo )

# Install files from the package:
atlas_install_headers( AthExJobOptions )
atlas_install_python_modules( python/*.py )
atlas_install_joboptions( share/*.py )


function (athexjoboptions_run_test testName)
  cmake_parse_arguments( ARG "" "DEPENDS" "" ${ARGN} )

  configure_file( ${CMAKE_CURRENT_SOURCE_DIR}/test/athexjoboptions_test.sh.in
                  ${CMAKE_CURRENT_BINARY_DIR}/athexjoboptions_${testName}.sh
                  @ONLY )
  atlas_add_test( ${testName}
                  SCRIPT ${CMAKE_CURRENT_BINARY_DIR}/athexjoboptions_${testName}.sh
                  PROPERTIES TIMEOUT 300
                   )
  if( ARG_DEPENDS )
    set_tests_properties( AthExJobOptions_${testName}_ctest
                          PROPERTIES DEPENDS AthExJobOptions_${ARG_DEPENDS}_ctest )
  endif()
endfunction (athexjoboptions_run_test)


athexjoboptions_run_test (BasicJobOptions)
athexjoboptions_run_test (CustomToolJobOptions)
athexjoboptions_run_test (CustomTopAlgorithmJobOptions)

################################################################################
# Package: AthenaRootComps
################################################################################

# Declare the package name:
atlas_subdir( AthenaRootComps )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PUBLIC
                          Control/SGTools
                          PRIVATE
                          Control/AthenaBaseComps
                          Control/AthenaKernel
                          Control/AthenaPython
                          Control/DataModelRoot
                          Control/RootUtils
                          Control/StoreGate
                          Database/APR/CollectionBase
                          Database/APR/CollectionUtilities
                          Database/APR/FileCatalog
                          Database/APR/PersistencySvc
                          Database/AthenaRoot/AthenaRootKernel
                          Database/PersistentDataModel
                          Database/AthenaPOOL/PoolSvc
                          Event/EventInfo
			  Event/xAOD/xAODEventInfo
                          GaudiKernel
                          TestPolicy 
			  Control/xAODRootAccess)

# External dependencies:
find_package( ROOT COMPONENTS RIO Core Graf Tree MathCore Hist pthread )
find_package( Xrootd )
find_package( Boost )

# Component(s) in the package:
atlas_add_component( AthenaRootComps
                     src/*.cxx
                     src/components/*.cxx
                     INCLUDE_DIRS ${ROOT_INCLUDE_DIRS} ${Boost_INCLUDE_DIRS}
                     LINK_LIBRARIES ${ROOT_LIBRARIES} ${Boost_LIBRARIES} SGTools AthenaBaseComps AthenaKernel DataModelRoot RootUtils RootUtilsPyROOT StoreGateLib SGtests CollectionBase CollectionUtilities FileCatalog PersistencySvc AthenaRootKernel PersistentDataModel EventInfo GaudiKernel xAODRootAccess xAODEventInfo)

# Install files from the package:
atlas_install_headers( AthenaRootComps )
atlas_install_python_modules( python/*.py )
atlas_install_joboptions( share/tests/*.py )
atlas_install_scripts( test/*.sh test/ref.* test/*.py )


atlas_add_test( test_athena_ntuple_dumper_multiple
                SCRIPT test/test_athena_ntuple_dumper_multiple.sh
                EXTRA_PATTERNS ".*"
                PROPERTIES TIMEOUT 300 )


atlas_add_test( test_athena_ntuple_dumper_varhandles_nooutput
                SCRIPT test/test_athena_ntuple_dumper_varhandles_nooutput.sh
                EXTRA_PATTERNS ".*"
                PROPERTIES TIMEOUT 300 )

atlas_add_test( test_athena_ntuple_dumper_varhandles
                SCRIPT test/test_athena_ntuple_dumper_varhandles.sh
                EXTRA_PATTERNS ".*"
                PROPERTIES TIMEOUT 300 )

atlas_add_test( test_athena_ntuple_dumper_novarhandles
                SCRIPT test/test_athena_ntuple_dumper_novarhandles.sh
                EXTRA_PATTERNS ".*"
                PROPERTIES TIMEOUT 600 )

atlas_add_test( test_athena_variable_shape1
                SCRIPT test/test_athena_variable_shape1.sh
                EXTRA_PATTERNS ".*"
                PROPERTIES TIMEOUT 300 )

atlas_add_test( test_athena_variable_shape2
                SCRIPT test/test_athena_variable_shape2.sh
                EXTRA_PATTERNS ".*"
                PROPERTIES TIMEOUT 300 )

atlas_add_test( test_athena_variable_shape3
                SCRIPT test/test_athena_variable_shape3.sh
                EXTRA_PATTERNS ".*"
                PROPERTIES TIMEOUT 300 )

configure_file( ${CMAKE_CURRENT_SOURCE_DIR}/test/test_athena_variable_shape_merged.sh.in
                ${CMAKE_CURRENT_BINARY_DIR}/test_athena_variable_shape_merged_script.sh
                @ONLY )
atlas_add_test( test_athena_variable_shape_merged
                SCRIPT ${CMAKE_CURRENT_BINARY_DIR}/test_athena_variable_shape_merged_script.sh
                EXTRA_PATTERNS ".*"
                PROPERTIES TIMEOUT 300 )
set_tests_properties( AthenaRootComps_test_athena_variable_shape_merged_ctest
                      PROPERTIES DEPENDS
                      "AthenaRootComps_test_athena_variable_shape1_ctest;AthenaRootComps_test_athena_variable_shape2_ctest;AthenaRootComps_test_athena_variable_shape3_ctest" )
                    
atlas_add_test( test_athena_multtuple
                SCRIPT test/test_athena_multtuple.sh
                EXTRA_PATTERNS ".*"
                PROPERTIES TIMEOUT 300 )
                    
atlas_add_test( test_athena_multtuple_seek
                SCRIPT test/test_athena_multtuple_seek.sh
                EXTRA_PATTERNS ".*"
                PROPERTIES TIMEOUT 300 )


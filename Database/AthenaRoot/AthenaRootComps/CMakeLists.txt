# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

# Declare the package name:
atlas_subdir( AthenaRootComps )

# External dependencies:
find_package( ROOT COMPONENTS RIO Core Graf Tree MathCore Hist pthread )
find_package( Xrootd )
find_package( Boost )

# Component(s) in the package:
atlas_add_library( AthenaRootCompsLib
                   AthenaRootComps/*.h
                   INTERFACE
                   PUBLIC_HEADERS AthenaRootComps
                   INCLUDE_DIRS ${ROOT_INCLUDE_DIRS}
                   LINK_LIBRARIES ${ROOT_LIBRARIES} AthenaKernel )

atlas_add_component( AthenaRootComps
                     src/*.cxx
                     src/components/*.cxx
                     INCLUDE_DIRS ${ROOT_INCLUDE_DIRS} ${Boost_INCLUDE_DIRS}
                     LINK_LIBRARIES ${Boost_LIBRARIES} ${ROOT_LIBRARIES} AthenaBaseComps AthenaKernel AthenaRootCompsLib AthenaRootKernel DataModelRoot EventInfo FileCatalog GaudiKernel PersistentDataModel PoolSvcLib RootUtils SGTools StoreGateLib xAODCore xAODEventInfo xAODRootAccess )

# Install files from the package:
atlas_install_python_modules( python/*.py )
atlas_install_joboptions( share/tests/*.py )
atlas_install_scripts( test/*.sh test/ref.* test/*.py )

atlas_add_test( test_athena_ntuple_dumper_multiple
                SCRIPT test/test_athena_ntuple_dumper_multiple.sh
                POST_EXEC_SCRIPT nopost.sh
                PROPERTIES TIMEOUT 600 )

atlas_add_test( test_athena_ntuple_dumper_varhandles_nooutput
                SCRIPT test/test_athena_ntuple_dumper_varhandles_nooutput.sh
                POST_EXEC_SCRIPT nopost.sh
                PROPERTIES TIMEOUT 600 )

atlas_add_test( test_athena_ntuple_dumper_varhandles
                SCRIPT test/test_athena_ntuple_dumper_varhandles.sh
                POST_EXEC_SCRIPT nopost.sh
                PROPERTIES TIMEOUT 600 )

atlas_add_test( test_athena_ntuple_dumper_novarhandles
                SCRIPT test/test_athena_ntuple_dumper_novarhandles.sh
                POST_EXEC_SCRIPT nopost.sh
                PROPERTIES TIMEOUT 600 )

atlas_add_test( test_athena_variable_shape1
                SCRIPT test/test_athena_variable_shape1.sh
                POST_EXEC_SCRIPT nopost.sh
                PROPERTIES TIMEOUT 600 )

atlas_add_test( test_athena_variable_shape2
                SCRIPT test/test_athena_variable_shape2.sh
                POST_EXEC_SCRIPT nopost.sh
                PROPERTIES TIMEOUT 600 )

atlas_add_test( test_athena_variable_shape3
                SCRIPT test/test_athena_variable_shape3.sh
                POST_EXEC_SCRIPT nopost.sh
                PROPERTIES TIMEOUT 600 )

configure_file( ${CMAKE_CURRENT_SOURCE_DIR}/test/test_athena_variable_shape_merged.sh.in
                ${CMAKE_CURRENT_BINARY_DIR}/test_athena_variable_shape_merged_script.sh
                @ONLY )
atlas_add_test( test_athena_variable_shape_merged
                SCRIPT ${CMAKE_CURRENT_BINARY_DIR}/test_athena_variable_shape_merged_script.sh
                POST_EXEC_SCRIPT nopost.sh
                PROPERTIES TIMEOUT 600 )
set_tests_properties( AthenaRootComps_test_athena_variable_shape_merged_ctest
                      PROPERTIES DEPENDS
                      "AthenaRootComps_test_athena_variable_shape1_ctest;AthenaRootComps_test_athena_variable_shape2_ctest;AthenaRootComps_test_athena_variable_shape3_ctest" )
                    
atlas_add_test( test_athena_multtuple
                SCRIPT test/test_athena_multtuple.sh
                POST_EXEC_SCRIPT nopost.sh
                PROPERTIES TIMEOUT 600 )
                    
atlas_add_test( test_athena_multtuple_seek
                SCRIPT test/test_athena_multtuple_seek.sh
                POST_EXEC_SCRIPT nopost.sh
                PROPERTIES TIMEOUT 600 )


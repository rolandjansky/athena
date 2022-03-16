# Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration

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
atlas_install_python_modules( python/*.py POST_BUILD_CMD ${ATLAS_FLAKE8} )
atlas_install_joboptions( share/tests/*.py )
atlas_install_scripts( test/*.sh test/ref.* test/*.py )

atlas_add_test( test_athena_ntuple_dumper_multiple
                SCRIPT athena.py -c"DOWRITE=2" -lERROR AthenaRootComps/test_athena_ntuple_dumper.py
                POST_EXEC_SCRIPT "arc_refcomp.sh ref.d3pd.ascii"
                PRIVATE_WORKING_DIRECTORY )

atlas_add_test( test_athena_ntuple_dumper_multiple_diffroot  # diff-root on the test above
                SCRIPT acmd.py diff-root ../unitTestRun_test_athena_ntuple_dumper_multiple/d3pd.root ../unitTestRun_test_athena_ntuple_dumper_multiple/d3pd_2.root -t egamma
                PRIVATE_WORKING_DIRECTORY
                POST_EXEC_SCRIPT nopost.sh
                PROPERTIES DEPENDS AthenaRootComps_test_athena_ntuple_dumper_multiple_ctest )

atlas_add_test( test_athena_ntuple_dumper_varhandles_nooutput
                SCRIPT athena.py -c"DOWRITE=0" -lERROR AthenaRootComps/test_athena_ntuple_dumper.py
                POST_EXEC_SCRIPT "arc_refcomp.sh ref.d3pd.ascii"
                PRIVATE_WORKING_DIRECTORY )

atlas_add_test( test_athena_ntuple_dumper_varhandles
                SCRIPT athena.py -lERROR AthenaRootComps/test_athena_ntuple_dumper.py
                POST_EXEC_SCRIPT "arc_refcomp.sh ref.d3pd.ascii"
                PRIVATE_WORKING_DIRECTORY )

atlas_add_test( test_athena_ntuple_dumper_novarhandles
                SCRIPT athena.py -c"USEVARHANDLE=0" -lERROR AthenaRootComps/test_athena_ntuple_dumper.py
                POST_EXEC_SCRIPT "arc_refcomp.sh ref.d3pd.ascii"
                PRIVATE_WORKING_DIRECTORY )

atlas_add_test( test_athena_ntuple_dumper_read
                SCRIPT athena.py -c"FNAMES=['../unitTestRun_test_athena_ntuple_dumper_novarhandles/d3pd.root']" -lERROR AthenaRootComps/test_athena_ntuple_dumper.py
                POST_EXEC_SCRIPT "arc_refcomp.sh ref.d3pd.ascii collectionName"
                PRIVATE_WORKING_DIRECTORY
                PROPERTIES DEPENDS AthenaRootComps_test_athena_ntuple_dumper_novarhandles_ctest )

atlas_add_test( test_athena_variable_shape1
                SCRIPT test/test_athena_variable_shape1.sh
                POST_EXEC_SCRIPT nopost.sh )

atlas_add_test( test_athena_variable_shape2
                SCRIPT test/test_athena_variable_shape2.sh
                POST_EXEC_SCRIPT nopost.sh )

atlas_add_test( test_athena_variable_shape3
                SCRIPT test/test_athena_variable_shape3.sh
                POST_EXEC_SCRIPT nopost.sh )

atlas_add_test( test_athena_variable_shape_merged
                SCRIPT test_athena_variable_shape_merged.sh ${CMAKE_CURRENT_SOURCE_DIR}/test/ref.data.merged.ascii
                POST_EXEC_SCRIPT nopost.sh
                PROPERTIES DEPENDS "AthenaRootComps_test_athena_variable_shape1_ctest;AthenaRootComps_test_athena_variable_shape2_ctest;AthenaRootComps_test_athena_variable_shape3_ctest" )

atlas_add_test( test_athena_multtuple
                SCRIPT test/test_athena_multtuple.sh
                POST_EXEC_SCRIPT "arc_refcomp.sh ref.d3pd_der.ascii"
                PRIVATE_WORKING_DIRECTORY )
                    
atlas_add_test( test_athena_multtuple_seek
                SCRIPT test/test_athena_multtuple_seek.sh
                POST_EXEC_SCRIPT "arc_refcomp.sh ref.d3pd_seek.ascii"
                PRIVATE_WORKING_DIRECTORY )

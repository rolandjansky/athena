# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

# Declare the package name:
atlas_subdir( EventBookkeeperTools )

if( XAOD_STANDALONE )
   set( extra_libs )
   set( xaod_access_lib xAODRootAccess )
# ... for AthAnalysisBase (Athena calls this POOLRootAccess)
else()
   set( extra_libs GaudiKernel AthenaKernel AthenaPoolUtilities EventInfo IOVDbDataModel StoreGateLib )
   set( xaod_access_lib xAODRootAccess POOLRootAccessLib )
endif()

if( NOT GENERATIONBASE )
   set( extra_libs ${extra_libs} AnaAlgorithmLib xAODMetaData xAODTruth )
endif()

# Component(s) in the package:
atlas_add_library( EventBookkeeperToolsLib
                   EventBookkeeperTools/*.h Root/*.cxx
                   PUBLIC_HEADERS EventBookkeeperTools
                   LINK_LIBRARIES ${extra_libs} AsgDataHandlesLib AsgMessagingLib AsgTools
                                  xAODCutFlow xAODEventInfo )

atlas_add_dictionary( EventBookkeeperToolsDict
                      EventBookkeeperTools/EventBookkeeperToolsDict.h
                      EventBookkeeperTools/selection.xml
                      LINK_LIBRARIES EventBookkeeperToolsLib )

if( NOT XAOD_STANDALONE )
   atlas_add_component( EventBookkeeperTools
                        src/*.cxx
                        src/components/*.cxx
                        LINK_LIBRARIES GaudiKernel AthenaKernel EventBookkeeperToolsLib)
endif()

atlas_add_executable( dump-cbk
                      util/dump-cbk.cxx
                      LINK_LIBRARIES ${xaod_access_lib} AsgTools )  

# Install files from the package:
atlas_install_python_modules( python/*.py POST_BUILD_CMD ${ATLAS_FLAKE8} )
atlas_install_joboptions( share/*.py )

# Tests
atlas_add_test( DumpCbkTest
                SCRIPT dump-cbk $ASG_TEST_FILE_MC
                PROPERTIES TIMEOUT 300
                POST_EXEC_SCRIPT nopost.sh )

if( NOT XAOD_STANDALONE )
   atlas_add_test( BookkeeperDumperTool
                   SCRIPT test/test_BookkeeperDumperTool.py
                   PROPERTIES TIMEOUT 300
                   POST_EXEC_SCRIPT nopost.sh )

   atlas_add_test( CutFlowSvcTest
                   SCRIPT test/test_CutFlowSvc.py
                   PROPERTIES TIMEOUT 300
                   POST_EXEC_SCRIPT nopost.sh )

   atlas_add_test( CutFlowSvcDummyAlg
                   SCRIPT athena EventBookkeeperTools/TestCutFlowSvcDummyAlg.py
                   PROPERTIES TIMEOUT 300
                   POST_EXEC_SCRIPT nopost.sh )

   atlas_add_test( CutFlowSvcOutput
                   SCRIPT athena EventBookkeeperTools/TestCutFlowSvcOutput.py
                   PROPERTIES TIMEOUT 300
                   POST_EXEC_SCRIPT nopost.sh )

   atlas_add_test( CutFlowSvcOutputMT
                   SCRIPT athena EventBookkeeperTools/TestCutFlowSvcOutput.py --threads=2
                   PROPERTIES TIMEOUT 300
                   POST_EXEC_SCRIPT nopost.sh )

   if( NOT XAOD_ANALYSIS )
      atlas_add_test( CutFlowSvcOutputMP
                      SCRIPT athena EventBookkeeperTools/TestCutFlowSvcOutput.py --nprocs=2
                      PROPERTIES TIMEOUT 300
                      POST_EXEC_SCRIPT nopost.sh )
   endif()
endif()

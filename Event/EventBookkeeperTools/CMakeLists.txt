################################################################################
# Package: EventBookkeeperTools
################################################################################

# Declare the package name:
atlas_subdir( EventBookkeeperTools )

if( XAOD_STANDALONE )
   set( athena_libs )
   set( xaod_access_lib xAODRootAccess )
# ... for AthAnalysisBase (Athena calls this POOLRootAccess)
else()
   set( athena_libs AthenaKernel GaudiKernel )
   set( xaod_access_lib POOLRootAccessLib )
endif()

# Component(s) in the package:
atlas_add_library( EventBookkeeperToolsLib
                   EventBookkeeperTools/*.h Root/*.cxx
                   PUBLIC_HEADERS EventBookkeeperTools
                   LINK_LIBRARIES ${athena_libs} AsgDataHandlesLib AsgMessagingLib AsgTools xAODCutFlow xAODEventInfo )

atlas_add_dictionary( EventBookkeeperToolsDict
                      EventBookkeeperTools/EventBookkeeperToolsDict.h
                      EventBookkeeperTools/selection.xml
                      LINK_LIBRARIES EventBookkeeperToolsLib )

if( NOT XAOD_STANDALONE )
   atlas_add_component( EventBookkeeperTools
                        src/*.cxx
                        src/components/*.cxx
                        LINK_LIBRARIES AthenaBaseComps AthenaKernel GaudiKernel EventBookkeeperToolsLib
                                       SGTools StoreGateLib EventBookkeeperMetaData EventInfo )
endif()

atlas_add_executable( dump-cbk
                      util/dump-cbk.cxx
                      LINK_LIBRARIES ${xaod_access_lib} AsgTools )  

# Install files from the package:
atlas_install_python_modules( python/*.py )
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

   atlas_add_test( CutFlowSvcDummyAlg
                   SCRIPT athena EventBookkeeperTools/TestCutFlowSvcDummyAlg.py
                   PROPERTIES TIMEOUT 300
                   POST_EXEC_SCRIPT nopost.sh )

   atlas_add_test( CutFlowSvcOutput
                   SCRIPT athena EventBookkeeperTools/TestCutFlowSvcOutput.py
                   PROPERTIES TIMEOUT 300
                   POST_EXEC_SCRIPT nopost.sh )
endif()

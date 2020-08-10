################################################################################
# Package: EventBookkeeperTools
################################################################################

# Declare the package name:
atlas_subdir( EventBookkeeperTools )

if( XAOD_STANDALONE )
   set( xaod_access_lib xAODRootAccess )
# ... for AthAnalysisBase (Athena calls this POOLRootAccess)
else()
   set( xaod_access_lib POOLRootAccessLib )
endif()

# Component(s) in the package:
atlas_add_component( EventBookkeeperTools
                     src/*.cxx
                     src/components/*.cxx
                     LINK_LIBRARIES AsgTools AthenaBaseComps AthenaKernel GaudiKernel
                                    SGTools StoreGateLib EventBookkeeperMetaData EventInfo xAODCutFlow xAODEventInfo )

atlas_add_executable( dump-cbk
                      util/dump-cbk.cxx
                      LINK_LIBRARIES ${xaod_access_lib} AsgTools )  

# Install files from the package:
atlas_install_python_modules( python/*.py )
atlas_install_joboptions( share/*.py )

# Tests
atlas_add_test( BookkeeperDumperTool
                SCRIPT test/test_BookkeeperDumperTool.py
                PROPERTIES TIMEOUT 300
                POST_EXEC_SCRIPT nopost.sh )

atlas_add_test( CutflowSvcDummyAlg
                SCRIPT athena EventBookkeeperTools/TestCutFlowSvcDummyAlg.py
                PROPERTIES TIMEOUT 300
                POST_EXEC_SCRIPT nopost.sh )
if ( NOT GENERATIONBASE )
   atlas_add_test( CutflowSvcOutput
                   SCRIPT athena EventBookkeeperTools/TestCutFlowSvcOutput.py
                   PROPERTIES TIMEOUT 300
                   POST_EXEC_SCRIPT nopost.sh )
endif()

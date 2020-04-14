################################################################################
# Package: EventBookkeeperTools
################################################################################

# Declare the package name:
atlas_subdir( EventBookkeeperTools )

if( XAOD_STANDALONE )
   set( xaod_access_deps Control/xAODRootAccess )
   set( xaod_access_lib xAODRootAccess )
# ... for AthAnalysisBase (Athena calls this POOLRootAccess)
else()
   set( xaod_access_deps PhysicsAnalysis/POOLRootAccess )
   set( xaod_access_lib POOLRootAccessLib )
endif()

# Declare the package's dependencies:
atlas_depends_on_subdirs( PUBLIC
                          Control/AthToolSupport/AsgTools
                          Control/AthenaBaseComps
                          Control/AthenaKernel
                          Event/xAOD/xAODCutFlow
                          GaudiKernel
                          PRIVATE
                          ${xaod_access_deps}
                          Control/SGTools
                          Control/StoreGate
                          Event/EventBookkeeperMetaData
                          Event/EventInfo
                          Event/xAOD/xAODEventInfo )

# External dependencies:
find_package( ROOT COMPONENTS Core Tree MathCore Hist RIO pthread )

# Component(s) in the package:
atlas_add_component( EventBookkeeperTools
                     src/*.cxx
                     src/components/*.cxx
                     INCLUDE_DIRS ${ROOT_INCLUDE_DIRS}
                     LINK_LIBRARIES ${ROOT_LIBRARIES} AsgTools AthenaBaseComps AthenaKernel xAODCutFlow
                                    GaudiKernel SGTools StoreGateLib SGtests EventBookkeeperMetaData EventInfo xAODEventInfo )

atlas_add_executable( dump-cbk
                      util/dump-cbk.cxx
                      INCLUDE_DIRS ${ROOT_INCLUDE_DIRS}
                      LINK_LIBRARIES ${ROOT_LIBRARIES} ${xaod_access_lib} AsgTools )  

# Install files from the package:
atlas_install_python_modules( python/*.py )
atlas_install_joboptions( share/*.py )
atlas_install_runtime( scripts/dump-cbk )

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

################################################################################
# Package: TrigAnalysisTest
################################################################################

# Declare the package name:
atlas_subdir( TrigAnalysisTest )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PUBLIC
                          Control/AthenaBaseComps
                          GaudiKernel
                          TestPolicy
                          Trigger/TrigAnalysis/TrigDecisionTool
                          PRIVATE
                          Event/xAOD/xAODJet
                          Event/xAOD/xAODMuon
                          Event/xAOD/xAODTrigBphys
                          Event/xAOD/xAODTrigMissingET
                          Trigger/TrigConfiguration/TrigConfxAOD )

# External dependencies:
find_package( ROOT COMPONENTS Core Tree MathCore Hist RIO pthread )

# Component(s) in the package:
atlas_add_library( TrigAnalysisTestLib
                   Root/BasicTriggerFired.cxx
                   Root/Run1BStoxAODTrigger.cxx
                   Root/TestFactory.cxx
                   src/*.cxx
                   PUBLIC_HEADERS TrigAnalysisTest
                   PRIVATE_INCLUDE_DIRS ${ROOT_INCLUDE_DIRS}
                   LINK_LIBRARIES AthenaBaseComps GaudiKernel TrigDecisionToolLib
                   PRIVATE_LINK_LIBRARIES ${ROOT_LIBRARIES} xAODJet xAODMuon xAODTrigBphys xAODTrigMissingET )

atlas_add_component( TrigAnalysisTest
                     src/components/*.cxx
                     INCLUDE_DIRS ${ROOT_INCLUDE_DIRS}
                     LINK_LIBRARIES ${ROOT_LIBRARIES} AthenaBaseComps GaudiKernel TrigDecisionToolLib xAODJet xAODMuon xAODTrigBphys xAODTrigMissingET TrigAnalysisTestLib )

# Install files from the package:
atlas_install_python_modules( python/__init__.py python/PassedTriggerFilter.py )
atlas_install_joboptions( share/*.py )
atlas_install_runtime( test/*.xml share/*py Testing/*.conf bin/RttScriptRunner_TrigAnalysisTest.py bin/CheckLog_TrigAnalysisTest.py bin/LogTail_TrigAnalysisTest.py bin/TruncateLog_TrigAnalysisTest.py python/ValgrindAnalysisScript_TrigAnalysisTest.py python/ValGrindPostScript_TrigAnalysisTest.py bin/checkFileTrigSize_RTT.py )
atlas_install_scripts( Testing/*.sh bin/*.py )


################################################################################
# Package: TrigAnalysisExamples
################################################################################

# Declare the package name:
atlas_subdir( TrigAnalysisExamples )

# External dependencies:
find_package( ROOT COMPONENTS Core Tree Hist )

# Component(s) in the package:
atlas_add_component( TrigAnalysisExamples
   src/*.h src/*.cxx src/components/*.cxx
   INCLUDE_DIRS ${ROOT_INCLUDE_DIRS}
   LINK_LIBRARIES ${ROOT_LIBRARIES} AthenaBaseComps AthenaKernel FourMomUtils GaudiKernel StoreGateLib TrigConfHLTData TrigDecisionToolLib TrigSteeringEvent TrigT1Interfaces TriggerMatchingToolLib xAODBase xAODCaloEvent xAODEgamma xAODEventInfo xAODJet xAODMissingET xAODMuon xAODTau xAODTrigCalo xAODTrigEgamma xAODTrigMissingET xAODTrigger )

atlas_add_executable( TrigAnalysisExApp
   src/apps/TrigAnalysisExApp.cxx
   INCLUDE_DIRS ${ROOT_INCLUDE_DIRS}
   LINK_LIBRARIES ${ROOT_LIBRARIES} AthAnalysisBaseCompsLib POOLRootAccessLib
   GaudiKernel TrigDecisionToolLib )
                     
# Install files from the package:
atlas_install_python_modules( python/*.py POST_BUILD_CMD ${ATLAS_FLAKE8} )
atlas_install_joboptions( share/*.py )

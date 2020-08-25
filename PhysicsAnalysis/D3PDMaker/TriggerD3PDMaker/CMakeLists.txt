################################################################################
# Package: TriggerD3PDMaker
################################################################################

# Declare the package name:
atlas_subdir( TriggerD3PDMaker )

# External dependencies:
find_package( Boost COMPONENTS filesystem thread system )

# Component(s) in the package:
atlas_add_library( TriggerD3PDMakerLib
                   src/*.cxx
                   PUBLIC_HEADERS TriggerD3PDMaker
                   INCLUDE_DIRS ${Boost_INCLUDE_DIRS}
                   LINK_LIBRARIES ${Boost_LIBRARIES} AthenaBaseComps AthenaKernel CxxUtils EventKernel FourMomUtils GaudiKernel D3PDMakerUtils TrigSteeringEvent TrigDecisionToolLib TrigObjectMatchingLib StoreGateLib SGtests
                   PRIVATE_LINK_LIBRARIES xAODTrigger AnalysisTriggerEvent TrigConfHLTData TrigConfL1Data TrigMonitoringEvent TrigT1Interfaces TrigT1Result TrigAnalysisInterfaces )

atlas_add_component( TriggerD3PDMaker
                     src/components/*.cxx
                     INCLUDE_DIRS ${Boost_INCLUDE_DIRS}
                     LINK_LIBRARIES ${Boost_LIBRARIES} AthenaBaseComps AthenaKernel CxxUtils EventKernel FourMomUtils GaudiKernel D3PDMakerUtils TrigDecisionToolLib TrigObjectMatchingLib TrigSteeringEvent StoreGateLib SGtests xAODTrigger AnalysisTriggerEvent TrigConfHLTData TrigConfL1Data TrigMonitoringEvent TrigT1Interfaces TrigT1Result TriggerD3PDMakerLib TrigAnalysisInterfaces )

# Install files from the package:
atlas_install_python_modules( python/*.py )
atlas_install_joboptions( share/*.py )


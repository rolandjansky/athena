# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

# Declare the package name:
atlas_subdir( TrigBphysHypo )

# External dependencies:
find_package( CLHEP )
find_package( ROOT COMPONENTS MathCore )

# Component(s) in the package:
atlas_add_component( TrigBphysHypo
                     src/*.cxx
                     src/components/*.cxx
                     INCLUDE_DIRS ${CLHEP_INCLUDE_DIRS} ${ROOT_INCLUDE_DIRS}
                     LINK_LIBRARIES ${CLHEP_LIBRARIES} ${ROOT_LIBRARIES} AthLinks AthViews AthenaBaseComps AthenaKernel AthenaMonitoringKernelLib BeamSpotConditionsData DecisionHandlingLib ElectronPhotonSelectorToolsLib FourMomUtils InDetConversionFinderToolsLib LumiBlockCompsLib StoreGateLib TrigBphysicsEvent TrigCompositeUtilsLib TrigConfHLTData TrigInDetEvent TrigInDetToolInterfacesLib TrigInterfacesLib TrigNavigationLib TrigParticle TrigSteeringEvent TrigTimeAlgsLib TrkVKalVrtFitterLib xAODBase xAODEgamma xAODEventInfo xAODMuon xAODTracking xAODTrigBphys xAODTrigMuon xAODTrigger )

# Install files from the package:
atlas_install_python_modules( python/*.py POST_BUILD_CMD ${ATLAS_FLAKE8} --extend-extensions=ATL900,ATL901 )

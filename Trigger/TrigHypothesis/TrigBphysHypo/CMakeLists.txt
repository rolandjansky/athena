# Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration

# Declare the package name:
atlas_subdir( TrigBphysHypo )

# External dependencies:
find_package( ROOT COMPONENTS MathCore )

# Component(s) in the package:
atlas_add_component( TrigBphysHypo
                     src/*.cxx
                     src/components/*.cxx
                     INCLUDE_DIRS ${ROOT_INCLUDE_DIRS}
                     LINK_LIBRARIES ${ROOT_LIBRARIES} AthContainers AthViews AthenaBaseComps AthenaMonitoringKernelLib BeamSpotConditionsData DecisionHandlingLib GeoPrimitives InDetConversionFinderToolsLib ITrackToVertex StoreGateLib TrigCompositeUtilsLib TrkVKalVrtFitterLib xAODMuon xAODTracking xAODTrigBphys xAODTrigger xAODTrigMuon )

# Install files from the package:
atlas_install_python_modules( python/*.py POST_BUILD_CMD ${ATLAS_FLAKE8} --extend-extensions=ATL900,ATL901 )

# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

# Declare the package name:
atlas_subdir( TrigT2BeamSpot )

# External dependencies:
find_package( ROOT COMPONENTS MathCore )

# Component(s) in the package:
atlas_add_component( TrigT2BeamSpot
                     src/*.cxx
                     src/components/TrigT2*.cxx
                     INCLUDE_DIRS ${ROOT_INCLUDE_DIRS}
                     LINK_LIBRARIES ${ROOT_LIBRARIES} AthContainers AthenaBaseComps AthenaMonitoringKernelLib BeamSpotConditionsData EventPrimitives GaudiKernel StoreGateLib TrigInDetEvent TrigInDetToolInterfacesLib TrigInterfacesLib TrigNavigationLib TrigSteeringEvent TrigTimeAlgsLib TrkParameters TrkTrack TrkTrackSummary xAODEventInfo )

# Install files from the package:
atlas_install_python_modules( python/*.py POST_BUILD_CMD ${ATLAS_FLAKE8} )
atlas_install_joboptions( share/*.py )


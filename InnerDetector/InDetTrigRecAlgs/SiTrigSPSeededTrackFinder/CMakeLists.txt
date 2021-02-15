# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

# Declare the package name:
atlas_subdir( SiTrigSPSeededTrackFinder )

# Component(s) in the package:
atlas_add_component( SiTrigSPSeededTrackFinder
                     src/*.cxx
                     src/components/*.cxx
                     LINK_LIBRARIES GaudiKernel TrigInterfacesLib TrigNavigationLib AthenaKernel GeoPrimitives IRegionSelector EventPrimitives SiSPSeededTrackFinderData InDetRecToolInterfaces TrkMeasurementBase TrkParameters TrkRIO_OnTrack TrkTrack TrigInDetEvent TrigSteeringEvent TrigTimeAlgsLib TrkEventUtils )

# Install files from the package:
atlas_install_python_modules( python/*.py POST_BUILD_CMD ${ATLAS_FLAKE8} )

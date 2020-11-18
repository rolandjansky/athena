# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

# Declare the package name:
atlas_subdir( TRT_TrigTrackSegmentsFinder )

# Component(s) in the package:
atlas_add_component( TRT_TrigTrackSegmentsFinder
                     src/*.cxx
                     src/components/*.cxx
                     LINK_LIBRARIES GaudiKernel InDetRecToolInterfaces TrigInterfacesLib TrigTimeAlgsLib IRegionSelector TrkSegment TrigSteeringEvent )

# Install files from the package:
atlas_install_python_modules( python/*.py )

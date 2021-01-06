# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

# Declare the package name:
atlas_subdir( InDetTrigParticleCreation )

# Component(s) in the package:
atlas_add_component( InDetTrigParticleCreation
                     src/*.cxx
                     src/components/*.cxx
                     LINK_LIBRARIES AthenaBaseComps AthenaKernel AtlasDetDescr CxxUtils GaudiKernel IdDictDetDescr Identifier InDetIdentifier Particle StoreGateLib TrigInterfacesLib TrigNavigationLib TrigSteeringEvent TrigTimeAlgsLib TrkEventUtils TrkParameters TrkParticleBase TrkToolInterfaces TrkTrack TrkTrackLink TrkTrackSummary VxVertex xAODTracking )

# Install files from the package:
atlas_install_python_modules( python/*.py )

# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

# Declare the package name:
atlas_subdir( InDetTrigPriVxFinder )

# Component(s) in the package:
atlas_add_component( InDetTrigPriVxFinder
                     src/*.cxx
                     src/components/*.cxx
                     LINK_LIBRARIES AthContainers EventPrimitives GaudiKernel InDetBeamSpotServiceLib InDetRecToolInterfaces MagFieldConditions TrigInterfacesLib TrkEventPrimitives TrkParameters TrkParticleBase TrkTrack TrkTrackLink VxVertex xAODTracking )

# Install files from the package:
atlas_install_python_modules( python/*.py )

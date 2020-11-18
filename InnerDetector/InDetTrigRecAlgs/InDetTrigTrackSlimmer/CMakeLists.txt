# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

# Declare the package name:
atlas_subdir( InDetTrigTrackSlimmer )

# Component(s) in the package:
atlas_add_component( InDetTrigTrackSlimmer
                     src/*.cxx
                     src/components/*.cxx
                     LINK_LIBRARIES GaudiKernel TrkTrack TrigInterfacesLib TrkToolInterfaces )

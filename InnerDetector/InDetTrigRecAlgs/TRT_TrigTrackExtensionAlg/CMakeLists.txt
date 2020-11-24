# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

# Declare the package name:
atlas_subdir( TRT_TrigTrackExtensionAlg )

# Component(s) in the package:
atlas_add_component( TRT_TrigTrackExtensionAlg
                     src/*.cxx
                     src/components/*.cxx
                     LINK_LIBRARIES GaudiKernel TrigInterfacesLib InDetRecToolInterfaces TrkTrack )

# Install files from the package:
atlas_install_python_modules( python/*.py )

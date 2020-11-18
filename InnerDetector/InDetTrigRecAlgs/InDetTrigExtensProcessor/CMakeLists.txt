# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

# Declare the package name:
atlas_subdir( InDetTrigExtensProcessor )

# Component(s) in the package:
atlas_add_component( InDetTrigExtensProcessor
                     src/*.cxx
                     src/components/*.cxx
                     LINK_LIBRARIES GaudiKernel TrkEventPrimitives TrkTrack TrkFitterUtils TrigInterfacesLib TrkMeasurementBase TrkParameters TrkPrepRawData TrkRIO_OnTrack TrkFitterInterfaces TrkToolInterfaces )

# Install files from the package:
atlas_install_python_modules( python/*.py )

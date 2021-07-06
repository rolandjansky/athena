# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

# Declare the package name:
atlas_subdir( InDetExtensionProcessor )

# Component(s) in the package:
atlas_add_component( InDetExtensionProcessor
                     src/*.cxx
                     src/components/*.cxx
                     LINK_LIBRARIES AthenaBaseComps CxxUtils GaudiKernel TrkEventPrimitives TrkTrack TrkFitterUtils StoreGateLib TrkParameters TrkPrepRawData TrkRIO_OnTrack TrkFitterInterfaces TrkToolInterfaces )

# Install files from the package:
atlas_install_headers( InDetExtensionProcessor )


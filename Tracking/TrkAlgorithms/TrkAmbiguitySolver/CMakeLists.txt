# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

# Declare the package name:
atlas_subdir( TrkAmbiguitySolver )

# Component(s) in the package:
atlas_add_component( TrkAmbiguitySolver
                     src/*.cxx
                     src/components/*.cxx
                     LINK_LIBRARIES AthenaBaseComps AthContainers GaudiKernel TrkTrack TrkToolInterfaces )

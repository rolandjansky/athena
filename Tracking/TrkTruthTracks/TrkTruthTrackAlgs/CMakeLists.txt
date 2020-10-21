# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

# Declare the package name:
atlas_subdir( TrkTruthTrackAlgs )

# Component(s) in the package:
atlas_add_component( TrkTruthTrackAlgs
                     src/*.cxx
                     src/components/*.cxx
                     INCLUDE_DIRS 
                     LINK_LIBRARIES AtlasHepMCLib AthenaBaseComps GaudiKernel StoreGateLib TrkTrack TrkToolInterfaces TrkTruthTrackInterfaces )

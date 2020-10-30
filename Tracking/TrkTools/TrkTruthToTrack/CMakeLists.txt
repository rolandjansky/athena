# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

# Declare the package name:
atlas_subdir( TrkTruthToTrack )

# External dependencies:
find_package( HepPDT )

# Component(s) in the package:
atlas_add_component( TrkTruthToTrack
                     src/*.cxx
                     src/components/*.cxx
                     INCLUDE_DIRS ${HEPPDT_INCLUDE_DIRS} 
                     LINK_LIBRARIES ${HEPPDT_LIBRARIES} AtlasHepMCLib AthenaBaseComps GaudiKernel StoreGateLib TrackRecordLib TrkParameters TrkToolInterfaces xAODTruth TrkExInterfaces )

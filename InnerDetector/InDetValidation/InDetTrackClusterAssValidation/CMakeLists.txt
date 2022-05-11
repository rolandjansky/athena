# Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration

# Declare the package name:
atlas_subdir( InDetTrackClusterAssValidation )

# External dependencies:
find_package( HepPDT )

# Component(s) in the package:
atlas_add_component( InDetTrackClusterAssValidation
                     src/*.cxx
                     src/components/*.cxx
                     INCLUDE_DIRS ${HEPPDT_INCLUDE_DIRS} 
                     LINK_LIBRARIES ${HEPPDT_LIBRARIES} AtlasHepMCLib AthenaBaseComps CxxUtils StoreGateLib InDetPrepRawData InDetReadoutGeometry TrkSpacePoint TrkTruthData GaudiKernel TrkRIO_OnTrack TrkTrack )

# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

# Declare the package name:
atlas_subdir( TrkTruthAthenaPool )

# Component(s) in the package:
atlas_add_poolcnv_library( TrkTruthAthenaPoolPoolCnv
                           src/*.cxx
                           FILES TrkTruthData/PRD_MultiTruthCollection.h TrkTruthData/TrackTruthCollection.h TrkTruthData/DetailedTrackTruthCollection.h
                           LINK_LIBRARIES GaudiKernel StoreGateLib AthenaPoolCnvSvcLib TrkTruthData TrkTruthTPCnv )

# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

# Declare the package name:
atlas_subdir( TrkTruthTPCnv )

# Component(s) in the package:
atlas_add_tpcnv_library( TrkTruthTPCnv
                         src/*.cxx
                         PUBLIC_HEADERS TrkTruthTPCnv
                         LINK_LIBRARIES AthLinks AthContainers DataModelAthenaPoolLib AthenaKernel Identifier StoreGateLib AthenaPoolCnvSvcLib GeneratorObjects GeneratorObjectsTPCnv InDetIdentifier TrkTrack TrkTruthData
                         PRIVATE_LINK_LIBRARIES CxxUtils )

atlas_add_dictionary( TrkTruthTPCnvDict
                      src/TrkTruthTPCnvDict.h
                      TrkTruthTPCnv/selection.xml
                      LINK_LIBRARIES TrkTrack TrkTruthTPCnv )

# Test(s) in the package:
atlas_add_test( TrackTruthCollectionCnv_p1_test
                SOURCES
                test/TrackTruthCollectionCnv_p1_test.cxx
                LINK_LIBRARIES AtlasHepMCLib GeneratorObjectsTPCnv SGTools TrkTruthData TrkTruthTPCnv )

atlas_add_test( TrackTruthCollectionCnv_p2_test
                SOURCES
                test/TrackTruthCollectionCnv_p2_test.cxx
                LINK_LIBRARIES AtlasHepMCLib GeneratorObjectsTPCnv SGTools TrkTruthData TrkTruthTPCnv )

atlas_add_test( TruthTrajectoryCnv_p1_test
                SOURCES
                test/TruthTrajectoryCnv_p1_test.cxx
                LINK_LIBRARIES AtlasHepMCLib GeneratorObjectsTPCnv SGTools TrkTruthData TrkTruthTPCnv )

atlas_add_test( TruthTrajectoryCnv_p2_test
                SOURCES
                test/TruthTrajectoryCnv_p2_test.cxx
                LINK_LIBRARIES AtlasHepMCLib GeneratorObjectsTPCnv SGTools TrkTruthData TrkTruthTPCnv )

atlas_add_test( PRD_MultiTruthCollectionCnv_p2_test
                SOURCES
                test/PRD_MultiTruthCollectionCnv_p2_test.cxx
                LINK_LIBRARIES AtlasHepMCLib GeneratorObjectsTPCnv SGTools StoreGateLib TrkTruthData TrkTruthTPCnv )

atlas_add_test( PRD_MultiTruthCollectionCnv_p3_test
                SOURCES
                test/PRD_MultiTruthCollectionCnv_p3_test.cxx
                LINK_LIBRARIES AtlasHepMCLib GeneratorObjectsTPCnv SGTools StoreGateLib TrkTruthData TrkTruthTPCnv )

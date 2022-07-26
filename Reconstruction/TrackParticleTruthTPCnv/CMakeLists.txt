# Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration

# Declare the package name:
atlas_subdir( TrackParticleTruthTPCnv )

# Component(s) in the package:
atlas_add_tpcnv_library( TrackParticleTruthTPCnv
                         src/*.cxx
                         PUBLIC_HEADERS TrackParticleTruthTPCnv
                         LINK_LIBRARIES AthContainers AthLinks CxxUtils DataModelAthenaPoolLib AthenaPoolCnvSvcLib AthenaPoolUtilities GeneratorObjectsTPCnv Particle ParticleTruth AthenaKernel StoreGateLib )

atlas_add_dictionary( TrackParticleTruthTPCnvDict
                      TrackParticleTruthTPCnv/TrackParticleTruthTPCnvDict.h
                      TrackParticleTruthTPCnv/selection.xml
                      LINK_LIBRARIES TrackParticleTruthTPCnv )

atlas_add_test( TrackParticleTruthCollectionCnv_p1_test
                SOURCES
                test/TrackParticleTruthCollectionCnv_p1_test.cxx
                LINK_LIBRARIES AthenaPoolCnvSvcLib AtlasHepMCLib DataModelAthenaPoolLib GeneratorObjectsTPCnv Particle ParticleTruth SGTools TestTools TrackParticleTruthTPCnv CxxUtils )

atlas_add_test( TrackParticleTruthCollectionCnv_p2_test
                SOURCES
                test/TrackParticleTruthCollectionCnv_p2_test.cxx
                LINK_LIBRARIES AthenaPoolCnvSvcLib AtlasHepMCLib DataModelAthenaPoolLib GeneratorObjectsTPCnv Particle ParticleTruth SGTools TrackParticleTruthTPCnv )

# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

# Declare the package name:
atlas_subdir( TrigTruthEventTPCnv )

# Component(s) in the package:
atlas_add_tpcnv_library( TrigTruthEventTPCnv
                         src/*.cxx
                         PUBLIC_HEADERS TrigTruthEventTPCnv
                         PRIVATE_LINK_LIBRARIES AthenaKernel
                         LINK_LIBRARIES AthLinks AthenaPoolCnvSvcLib AthenaPoolUtilities DataModelAthenaPoolLib GeneratorObjectsTPCnv RootConversions TrigInDetEvent TrigInDetEventTPCnv TrigInDetTruthEvent )

atlas_add_dictionary( TrigTruthEventTPCnvDict
                      TrigTruthEventTPCnv/TrigTruthEventTPCnvDict.h
                      TrigTruthEventTPCnv/selection.xml
                      LINK_LIBRARIES TrigTruthEventTPCnv )

atlas_add_dictionary( OLD_TrigTruthEventTPCnvDict
                      TrigTruthEventTPCnv/TrigTruthEventTPCnvDict.h
                      TrigTruthEventTPCnv/OLD_selection.xml
                      LINK_LIBRARIES TrigTruthEventTPCnv )

# Tests in the package:
atlas_add_test( TrigIDHitStatsCnv_p1_test
                SOURCES
                test/TrigIDHitStatsCnv_p1_test.cxx
                LINK_LIBRARIES TrigTruthEventTPCnv )

atlas_add_test( TrigInDetTrackTruthCnv_p1_test
                SOURCES
                test/TrigInDetTrackTruthCnv_p1_test.cxx
                LINK_LIBRARIES AtlasHepMCLib TrigTruthEventTPCnv )

atlas_add_test( TrigInDetTrackTruthMapCnv_p1_test
                SOURCES
                test/TrigInDetTrackTruthMapCnv_p1_test.cxx
                LINK_LIBRARIES AtlasHepMCLib StoreGateLib TrigTruthEventTPCnv )

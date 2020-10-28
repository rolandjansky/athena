# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

# Declare the package name:
atlas_subdir( InDetSimEventTPCnv )

# External dependencies:
find_package( CLHEP )

# Component(s) in the package:
atlas_add_library( InDetSimEventTPCnv
                   src/InDetHits/*.cxx
                   PUBLIC_HEADERS InDetSimEventTPCnv
                   PRIVATE_INCLUDE_DIRS ${CLHEP_INCLUDE_DIRS}
                   PRIVATE_DEFINITIONS ${CLHEP_DEFINITIONS}
                   LINK_LIBRARIES GeneratorObjectsTPCnv InDetSimEvent AthenaPoolCnvSvcLib
                   PRIVATE_LINK_LIBRARIES ${CLHEP_LIBRARIES} AthenaKernel GaudiKernel GeneratorObjects Identifier StoreGateLib )

atlas_add_dictionary( InDetSimEventTPCnvDict
                      InDetSimEventTPCnv/InDetSimEventTPCnvDict.h
                      InDetSimEventTPCnv/selection.xml
                      LINK_LIBRARIES InDetSimEventTPCnv )

# Tests in the package:
atlas_add_test( SiHitCnv_p1_test
                SOURCES
                test/SiHitCnv_p1_test.cxx
                LINK_LIBRARIES AtlasHepMCLib GeneratorObjectsTPCnv InDetSimEventTPCnv TestTools )

atlas_add_test( SiHitCollectionCnv_p2_test
                SOURCES
                test/SiHitCollectionCnv_p2_test.cxx
                LINK_LIBRARIES AtlasHepMCLib GeneratorObjectsTPCnv InDetSimEventTPCnv TestTools )

atlas_add_test( SiHitCollectionCnv_p3_test
                SOURCES
                test/SiHitCollectionCnv_p3_test.cxx
                LINK_LIBRARIES AtlasHepMCLib GeneratorObjectsTPCnv InDetSimEventTPCnv TestTools )

atlas_add_test( TRT_HitCnv_p1_test
                SOURCES
                test/TRT_HitCnv_p1_test.cxx
                LINK_LIBRARIES AtlasHepMCLib GeneratorObjectsTPCnv InDetSimEventTPCnv TestTools )

atlas_add_test( TRT_HitCollectionCnv_p2_test
                SOURCES
                test/TRT_HitCollectionCnv_p2_test.cxx
                LINK_LIBRARIES AtlasHepMCLib GeneratorObjectsTPCnv InDetSimEventTPCnv TestTools )

atlas_add_test( TRT_HitCollectionCnv_p3_test
                SOURCES
                test/TRT_HitCollectionCnv_p3_test.cxx
                LINK_LIBRARIES AtlasHepMCLib GeneratorObjectsTPCnv InDetSimEventTPCnv TestTools )

atlas_add_test( TRT_HitCollectionCnv_p4_test
                SOURCES
                test/TRT_HitCollectionCnv_p4_test.cxx
                LINK_LIBRARIES AtlasHepMCLib GeneratorObjectsTPCnv InDetSimEventTPCnv TestTools )

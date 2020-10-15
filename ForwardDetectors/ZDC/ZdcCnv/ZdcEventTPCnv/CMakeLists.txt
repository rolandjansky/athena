# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

# Declare the package name:
atlas_subdir( ZdcEventTPCnv )

# Component(s) in the package:
atlas_add_library( ZdcEventTPCnv
                   src/*.cxx
                   PUBLIC_HEADERS ZdcEventTPCnv
                   LINK_LIBRARIES AthenaPoolCnvSvcLib ZDC_SimEvent ZdcEvent
                   PRIVATE_LINK_LIBRARIES Identifier GaudiKernel )

atlas_add_dictionary( ZdcEventTPCnvDict
                      ZdcEventTPCnv/ZdcEventTPCnvDict.h
                      ZdcEventTPCnv/selection.xml
                      LINK_LIBRARIES ZdcEventTPCnv )

# Tests in the package:
atlas_add_test( ZdcDigitsCnv_p1_test
                SOURCES
                test/ZdcDigitsCnv_p1_test.cxx
                LINK_LIBRARIES TestTools ZdcEventTPCnv )

atlas_add_test( ZdcDigitsCollectionCnv_p1_test
                SOURCES
                test/ZdcDigitsCollectionCnv_p1_test.cxx
                LINK_LIBRARIES TestTools ZdcEventTPCnv )

atlas_add_test( ZdcRawChannelCnv_p1_test
                SOURCES
                test/ZdcRawChannelCnv_p1_test.cxx
                LINK_LIBRARIES TestTools ZdcEventTPCnv )

atlas_add_test( ZdcRawChannelCollectionCnv_p1_test
                SOURCES
                test/ZdcRawChannelCollectionCnv_p1_test.cxx
                LINK_LIBRARIES TestTools ZdcEventTPCnv )

atlas_add_test( ZDC_SimPixelHitCnv_p1_test
                SOURCES
                test/ZDC_SimPixelHitCnv_p1_test.cxx
                LINK_LIBRARIES TestTools ZdcEventTPCnv )

atlas_add_test( ZDC_SimPixelHit_CollectionCnv_p1_test
                SOURCES
                test/ZDC_SimPixelHit_CollectionCnv_p1_test.cxx
                LINK_LIBRARIES TestTools ZdcEventTPCnv )

atlas_add_test( ZDC_SimStripHitCnv_p1_test
                SOURCES
                test/ZDC_SimStripHitCnv_p1_test.cxx
                LINK_LIBRARIES TestTools ZdcEventTPCnv )

atlas_add_test( ZDC_SimStripHit_CollectionCnv_p1_test
                SOURCES
                test/ZDC_SimStripHit_CollectionCnv_p1_test.cxx
                LINK_LIBRARIES TestTools ZdcEventTPCnv )

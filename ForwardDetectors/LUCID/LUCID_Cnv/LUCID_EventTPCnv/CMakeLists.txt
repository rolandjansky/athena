# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

# Declare the package name:
atlas_subdir( LUCID_EventTPCnv )

# Component(s) in the package:
atlas_add_library( LUCID_EventTPCnv
                   src/*.cxx
                   PUBLIC_HEADERS LUCID_EventTPCnv
                   LINK_LIBRARIES AthenaPoolUtilities LUCID_RawEvent LUCID_SimEvent GeneratorObjectsTPCnv AthenaPoolCnvSvcLib
                   PRIVATE_LINK_LIBRARIES Identifier GaudiKernel )

atlas_add_dictionary( LUCID_EventTPCnvDict
                      LUCID_EventTPCnv/LUCID_EventTPCnvDict.h
                      LUCID_EventTPCnv/selection.xml
                      LINK_LIBRARIES LUCID_EventTPCnv )

# Tests in the package:
atlas_add_test( LUCID_DigitCnv_p1_test
                SOURCES
                test/LUCID_DigitCnv_p1_test.cxx
                LINK_LIBRARIES LUCID_EventTPCnv TestTools )

atlas_add_test( LUCID_DigitCnv_p2_test
                SOURCES
                test/LUCID_DigitCnv_p2_test.cxx
                LINK_LIBRARIES LUCID_EventTPCnv TestTools )

atlas_add_test( LUCID_DigitContainerCnv_p1_test
                SOURCES
                test/LUCID_DigitContainerCnv_p1_test.cxx
                LINK_LIBRARIES LUCID_EventTPCnv TestTools )

atlas_add_test( LUCID_DigitContainerCnv_p2_test
                SOURCES
                test/LUCID_DigitContainerCnv_p2_test.cxx
                LINK_LIBRARIES LUCID_EventTPCnv TestTools )

atlas_add_test( LUCID_RawDataCnv_p1_test
                SOURCES
                test/LUCID_RawDataCnv_p1_test.cxx
                LINK_LIBRARIES LUCID_EventTPCnv TestTools )

atlas_add_test( LUCID_RawDataContainerCnv_p1_test
                SOURCES
                test/LUCID_RawDataContainerCnv_p1_test.cxx
                LINK_LIBRARIES LUCID_EventTPCnv TestTools )

atlas_add_test( LUCID_SimHitCnv_p1_test
                SOURCES
                test/LUCID_SimHitCnv_p1_test.cxx
                LINK_LIBRARIES AtlasHepMCLib GeneratorObjectsTPCnv LUCID_EventTPCnv TestTools )

atlas_add_test( LUCID_SimHitCnv_p2_test
                SOURCES
                test/LUCID_SimHitCnv_p2_test.cxx
                LINK_LIBRARIES AtlasHepMCLib GeneratorObjectsTPCnv LUCID_EventTPCnv TestTools )

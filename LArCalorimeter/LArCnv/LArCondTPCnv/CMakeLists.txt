# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

# Declare the package name:
atlas_subdir( LArCondTPCnv )

# Component(s) in the package:
atlas_add_tpcnv_library( LArCondTPCnv
                         src/*.cxx
                         PUBLIC_HEADERS LArCondTPCnv
                         LINK_LIBRARIES AthenaPoolCnvSvcLib LArRawConditions
                         PRIVATE_LINK_LIBRARIES AthenaKernel LArElecCalib )

atlas_add_dictionary( LArCondTPCnvDict
                      LArCondTPCnv/LArCondTPCnvDict.h
                      LArCondTPCnv/selection.xml
                      LINK_LIBRARIES LArCondTPCnv )

atlas_add_dictionary( OLD_LArCondTPCnvDict
                      LArCondTPCnv/LArCondTPCnvDict.h
                      LArCondTPCnv/OLD_selection.xml
                      LINK_LIBRARIES LArCondTPCnv )

atlas_add_test( LArAutoCorrSubsetCnv_p1_test
                SOURCES
                test/LArAutoCorrSubsetCnv_p1_test.cxx
                LINK_LIBRARIES GaudiKernel LArCondTPCnv TestTools )

atlas_add_test( LArCaliWaveSubsetCnv_p1_test
                SOURCES
                test/LArCaliWaveSubsetCnv_p1_test.cxx
                LINK_LIBRARIES GaudiKernel LArCondTPCnv TestTools )

atlas_add_test( LArCaliWaveSubsetCnv_p2_test
                SOURCES
                test/LArCaliWaveSubsetCnv_p2_test.cxx
                LINK_LIBRARIES GaudiKernel LArCondTPCnv TestTools )

atlas_add_test( LArDSPThresholdsSubsetCnv_p1_test
                SOURCES
                test/LArDSPThresholdsSubsetCnv_p1_test.cxx
                LINK_LIBRARIES GaudiKernel LArCondTPCnv TestTools )

atlas_add_test( LArMphysOverMcalSubsetCnv_p1_test
                SOURCES
                test/LArMphysOverMcalSubsetCnv_p1_test.cxx
                LINK_LIBRARIES GaudiKernel LArCondTPCnv TestTools )

atlas_add_test( LArOFCBinSubsetCnv_p1_test
                SOURCES
                test/LArOFCBinSubsetCnv_p1_test.cxx
                LINK_LIBRARIES GaudiKernel LArCondTPCnv TestTools )

atlas_add_test( LArPedestalMCCnv_p1_test
                SOURCES
                test/LArPedestalMCCnv_p1_test.cxx
                LINK_LIBRARIES GaudiKernel LArCondTPCnv TestTools )

atlas_add_test( LArPedestalSubsetCnv_p1_test
                SOURCES
                test/LArPedestalSubsetCnv_p1_test.cxx
                LINK_LIBRARIES GaudiKernel LArCondTPCnv TestTools )

atlas_add_test( LArPedestalSubsetCnv_p2_test
                SOURCES
                test/LArPedestalSubsetCnv_p2_test.cxx
                LINK_LIBRARIES GaudiKernel LArCondTPCnv TestTools )

atlas_add_test( LArPhysWaveSubsetCnv_p1_test
                SOURCES
                test/LArPhysWaveSubsetCnv_p1_test.cxx
                LINK_LIBRARIES GaudiKernel LArCondTPCnv TestTools )

atlas_add_test( LArRampSubsetCnv_p1_test
                SOURCES
                test/LArRampSubsetCnv_p1_test.cxx
                LINK_LIBRARIES GaudiKernel LArCondTPCnv TestTools )

atlas_add_test( LArShapeSubsetCnv_p1_test
                SOURCES
                test/LArShapeSubsetCnv_p1_test.cxx
                LINK_LIBRARIES GaudiKernel LArCondTPCnv TestTools )

atlas_add_test( LArShapeSubsetCnv_p2_test
                SOURCES
                test/LArShapeSubsetCnv_p2_test.cxx
                LINK_LIBRARIES GaudiKernel LArCondTPCnv TestTools )

atlas_add_test( LArSingleFloatSubsetCnv_p1_test
                SOURCES
                test/LArSingleFloatSubsetCnv_p1_test.cxx
                LINK_LIBRARIES GaudiKernel LArCondTPCnv TestTools )

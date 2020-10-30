# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

# Declare the package name:
atlas_subdir( LArSimEventTPCnv )

# Component(s) in the package:
atlas_add_tpcnv_library( LArSimEventTPCnv
                         src/*.cxx
                         PUBLIC_HEADERS LArSimEventTPCnv
                         LINK_LIBRARIES AthenaPoolCnvSvcLib LArSimEvent
                         PRIVATE_LINK_LIBRARIES AthenaKernel CaloIdentifier GaudiKernel Identifier StoreGateLib )

atlas_add_dictionary( LArSimEventTPCnvDict
                      LArSimEventTPCnv/LArSimEventTPCnvDict.h
                      LArSimEventTPCnv/selection.xml
                      LINK_LIBRARIES LArSimEventTPCnv )

atlas_add_dictionary( OLD_LArSimEventTPCnvDict
                      LArSimEventTPCnv/LArSimEventTPCnvDict.h
                      LArSimEventTPCnv/OLD_selection.xml
                      LINK_LIBRARIES LArSimEventTPCnv )

atlas_add_test( LArHitCnv_p1_test
                SOURCES
                test/LArHitCnv_p1_test.cxx
                LINK_LIBRARIES LArSimEventTPCnv TestTools )

atlas_add_test( LArHitContainerCnv_p1_test
                SOURCES
                test/LArHitContainerCnv_p1_test.cxx
                LINK_LIBRARIES LArSimEventTPCnv TestTools )

atlas_add_test( LArHitContainerCnv_p2_test
                SOURCES
                test/LArHitContainerCnv_p2_test.cxx
                LINK_LIBRARIES CaloIdentifier IdDictParser LArSimEventTPCnv StoreGateLib TestTools
                ENVIRONMENT "JOBOPTSEARCHPATH=${CMAKE_CURRENT_SOURCE_DIR}/share" )

atlas_add_test( LArHitFloatCnv_p1_test
                SOURCES
                test/LArHitFloatCnv_p1_test.cxx
                LINK_LIBRARIES LArSimEventTPCnv TestTools )

atlas_add_test( LArHitFloatContainerCnv_p2_test
                SOURCES
                test/LArHitFloatContainerCnv_p2_test.cxx
                LINK_LIBRARIES CaloIdentifier IdDictParser LArSimEventTPCnv StoreGateLib TestTools
                ENVIRONMENT "JOBOPTSEARCHPATH=${CMAKE_CURRENT_SOURCE_DIR}/share" )

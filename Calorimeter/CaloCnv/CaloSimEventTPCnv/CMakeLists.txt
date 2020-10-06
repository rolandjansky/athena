# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

# Declare the package name:
atlas_subdir( CaloSimEventTPCnv )

# Component(s) in the package:
atlas_add_tpcnv_library( CaloSimEventTPCnv
                         src/*.cxx
                         PUBLIC_HEADERS CaloSimEventTPCnv
                         LINK_LIBRARIES CaloSimEvent AthenaPoolCnvSvcLib Identifier TestTools CaloIdentifier AthenaKernel GaudiKernel )

atlas_add_dictionary( CaloSimEventTPCnvDict
                      CaloSimEventTPCnv/CaloSimEventTPCnvDict.h
                      CaloSimEventTPCnv/selection.xml
                      LINK_LIBRARIES CaloSimEventTPCnv )

atlas_add_dictionary( OLD_CaloSimEventTPCnvDict
                      CaloSimEventTPCnv/CaloSimEventTPCnvDict.h
                      CaloSimEventTPCnv/OLD_selection.xml
                      LINK_LIBRARIES CaloSimEventTPCnv )

atlas_add_test( CaloCalibrationHitCnv_p1_test
                SOURCES
                test/CaloCalibrationHitCnv_p1_test.cxx
                LINK_LIBRARIES CaloSimEventTPCnv )

atlas_add_test( CaloCalibrationHitCnv_p2_test
                SOURCES
                test/CaloCalibrationHitCnv_p2_test.cxx
                LINK_LIBRARIES CaloSimEventTPCnv )

atlas_add_test( CaloCalibrationHitContainerCnv_p1_test
                SOURCES
                test/CaloCalibrationHitContainerCnv_p1_test.cxx
                LINK_LIBRARIES CaloSimEventTPCnv
                LOG_IGNORE_PATTERN "Athena::getMessageSvc" )

atlas_add_test( CaloCalibrationHitContainerCnv_p2_test
                SOURCES
                test/CaloCalibrationHitContainerCnv_p2_test.cxx
                LINK_LIBRARIES CaloSimEventTPCnv
                LOG_IGNORE_PATTERN "Athena::getMessageSvc" )

atlas_add_test( CaloCalibrationHitContainerCnv_p3_test
                SOURCES
                test/CaloCalibrationHitContainerCnv_p3_test.cxx
                LINK_LIBRARIES CaloSimEventTPCnv
                LOG_IGNORE_PATTERN "Athena::getMessageSvc" )


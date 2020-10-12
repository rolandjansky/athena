# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

# Declare the package name:
atlas_subdir( CaloTPCnv )

# External dependencies:
find_package( CLHEP )

# Component(s) in the package:
atlas_add_tpcnv_library( CaloTPCnv
                         src/*.cxx
                         PUBLIC_HEADERS CaloTPCnv
                         PRIVATE_INCLUDE_DIRS ${CLHEP_INCLUDE_DIRS}
                         PRIVATE_DEFINITIONS ${CLHEP_DEFINITIONS}
                         LINK_LIBRARIES ${CLHEP_LIBRARIES} CaloEvent AthenaKernel AthAllocators AthLinks DataModelAthenaPoolLib Navigation AthenaPoolCnvSvcLib EventCommonTPCnv GaudiKernel TestTools CaloConditions CaloGeoHelpers CaloUtilsLib SGTools EventKernel LArRecEvent TileEvent )

atlas_add_dictionary( CaloTPCnvDict
                      CaloTPCnv/CaloTPCnvDict.h
                      CaloTPCnv/selection.xml
                      LINK_LIBRARIES CaloTPCnv )

atlas_add_dictionary( OLD_CaloTPCnvDict
                      CaloTPCnv/CaloTPCnvDict.h
                      CaloTPCnv/OLD_selection.xml
                      LINK_LIBRARIES CaloTPCnv )

atlas_add_test( CaloCellLinkContainerCnv_p2_test
                SOURCES
                test/CaloCellLinkContainerCnv_p2_test.cxx
                LINK_LIBRARIES CaloTPCnv
                LOG_IGNORE_PATTERN "Athena::getMessageSvc" )

atlas_add_test( CaloClusterContainerCnv_test
                SOURCES
                test/CaloClusterContainerCnv_test.cxx
                LINK_LIBRARIES CaloTPCnv )

atlas_add_test( CaloClusterCellLinkContainerCnv_p1_test
                SOURCES
                test/CaloClusterCellLinkContainerCnv_p1_test.cxx
                LINK_LIBRARIES CaloTPCnv )

atlas_add_test( CaloCellPackerUtils_test
   SOURCES test/CaloCellPackerUtils_test.cxx
   LINK_LIBRARIES GaudiKernel )

atlas_add_test( CaloCompactCellTool_test
   SOURCES test/CaloCompactCellTool_test.cxx
   LINK_LIBRARIES CaloDetDescrLib CaloEvent CaloIdentifier
   AthenaKernel StoreGateLib GaudiKernel LArRecEvent TileEvent TestTools
   IdDictParser CaloTPCnv
   LOG_IGNORE_PATTERN "SGAudSvc +INFO|^lar decode|initialize_from_dict|mask/zero|^AtlasDetectorID::"
   ENVIRONMENT "JOBOPTSEARCHPATH=${CMAKE_CURRENT_SOURCE_DIR}/share" )


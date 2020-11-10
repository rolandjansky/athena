# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

# Declare the package name:
atlas_subdir( RecTPCnv )

# External dependencies:
find_package( ROOT COMPONENTS Core Tree MathCore Hist RIO pthread )

# Component(s) in the package:
atlas_add_tpcnv_library( RecTPCnv
                         src/*.cxx
                         PUBLIC_HEADERS RecTPCnv
                         LINK_LIBRARIES AthenaPoolCnvSvcLib CxxUtils DataModelAthenaPoolLib EventCommonTPCnv GaudiKernel MissingETEvent ParticleEventTPCnv StoreGateLib TrkEventTPCnv TrkMaterialOnTrack muonEvent
                         PRIVATE_LINK_LIBRARIES AthenaKernel RootConversions )

atlas_add_dictionary( RecTPCnvDict
                      RecTPCnv/RecTPCnvDict.h
                      RecTPCnv/selection.xml
                      LINK_LIBRARIES RecTPCnv )

atlas_add_dictionary( OLD_RecTPCnvDict
                      RecTPCnv/RecTPCnvDict.h
                      RecTPCnv/OLD_selection.xml
                      LINK_LIBRARIES RecTPCnv )

# Tests in the package:
atlas_add_test( CaloEnergyCnv_p1_test
                SOURCES
                test/CaloEnergyCnv_p1_test.cxx
                LINK_LIBRARIES RecTPCnv )

atlas_add_test( CaloEnergyCnv_p2_test
                SOURCES
                test/CaloEnergyCnv_p2_test.cxx
                LINK_LIBRARIES RecTPCnv )

atlas_add_test( DepositInCaloCnv_p1_test
                SOURCES
                test/DepositInCaloCnv_p1_test.cxx
                LINK_LIBRARIES RecTPCnv )

atlas_add_test( DepositInCaloCnv_p2_test
                SOURCES
                test/DepositInCaloCnv_p2_test.cxx
                LINK_LIBRARIES RecTPCnv )

atlas_add_test( MissingETCnv_p1_test
                SOURCES
                test/MissingETCnv_p1_test.cxx
                LINK_LIBRARIES RecTPCnv )

atlas_add_test( MissingETCnv_p2_test
                SOURCES
                test/MissingETCnv_p2_test.cxx
                LINK_LIBRARIES RecTPCnv )

atlas_add_test( MissingETCnv_p3_test
                SOURCES
                test/MissingETCnv_p3_test.cxx
                LINK_LIBRARIES RecTPCnv )

atlas_add_test( MissingETCompositionCnv_p1_test
                SOURCES
                test/MissingETCompositionCnv_p1_test.cxx
                LINK_LIBRARIES RecTPCnv )

atlas_add_test( MissingETCompositionCnv_p2_test
                SOURCES
                test/MissingETCompositionCnv_p2_test.cxx
                LINK_LIBRARIES RecTPCnv )

atlas_add_test( MissingEtCaloCnv_p1_test
                SOURCES
                test/MissingEtCaloCnv_p1_test.cxx
                LINK_LIBRARIES RecTPCnv )

atlas_add_test( MissingEtCaloCnv_p2_test
                SOURCES
                test/MissingEtCaloCnv_p2_test.cxx
                LINK_LIBRARIES RecTPCnv )

atlas_add_test( MissingEtCaloCnv_p3_test
                SOURCES
                test/MissingEtCaloCnv_p3_test.cxx
                LINK_LIBRARIES RecTPCnv )

atlas_add_test( MissingEtRegionsCnv_p1_test
                SOURCES
                test/MissingEtRegionsCnv_p1_test.cxx
                LINK_LIBRARIES RecTPCnv )

atlas_add_test( MissingEtRegionsCnv_p2_test
                SOURCES
                test/MissingEtRegionsCnv_p2_test.cxx
                LINK_LIBRARIES RecTPCnv )

atlas_add_test( MissingEtRegionsCnv_p3_test
                SOURCES
                test/MissingEtRegionsCnv_p3_test.cxx
                LINK_LIBRARIES RecTPCnv )

atlas_add_test( MissingEtTruthCnv_p1_test
                SOURCES
                test/MissingEtTruthCnv_p1_test.cxx
                LINK_LIBRARIES RecTPCnv )

atlas_add_test( MissingEtTruthCnv_p2_test
                SOURCES
                test/MissingEtTruthCnv_p2_test.cxx
                LINK_LIBRARIES RecTPCnv )

atlas_add_test( MissingEtTruthCnv_p3_test
                SOURCES
                test/MissingEtTruthCnv_p3_test.cxx
                LINK_LIBRARIES RecTPCnv )

atlas_add_test( MuonCaloEnergyContainerCnv_p1_test
                SOURCES
                test/MuonCaloEnergyContainerCnv_p1_test.cxx
                LINK_LIBRARIES RecTPCnv )

atlas_add_test( MuonCnv_p1_test
                SOURCES
                test/MuonCnv_p1_test.cxx
                LINK_LIBRARIES RecTPCnv )

atlas_add_test( MuonCnv_p2_test
                SOURCES
                test/MuonCnv_p2_test.cxx
                LINK_LIBRARIES RecTPCnv )

atlas_add_test( MuonCnv_p3_test
                SOURCES
                test/MuonCnv_p3_test.cxx
                LINK_LIBRARIES RecTPCnv )

atlas_add_test( MuonCnv_p4_test
                SOURCES
                test/MuonCnv_p4_test.cxx
                LINK_LIBRARIES RecTPCnv )

atlas_add_test( MuonCnv_p5_test
                SOURCES
                test/MuonCnv_p5_test.cxx
                LINK_LIBRARIES RecTPCnv )

atlas_add_test( MuonCnv_p6_test
                SOURCES
                test/MuonCnv_p6_test.cxx
                LINK_LIBRARIES RecTPCnv )

atlas_add_test( MuonSpShowerCnv_p1_test
                SOURCES
                test/MuonSpShowerCnv_p1_test.cxx
                LINK_LIBRARIES RecTPCnv )

atlas_add_test( MuonSpShowerContainerCnv_p1_test
                SOURCES
                test/MuonSpShowerContainerCnv_p1_test.cxx
                LINK_LIBRARIES RecTPCnv )


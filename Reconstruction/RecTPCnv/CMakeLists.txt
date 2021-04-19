# Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration

# Declare the package name:
atlas_subdir( RecTPCnv )

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
function( _add_test test )
   atlas_add_test( ${test}
                   SOURCES
                   test/${test}.cxx
                   LINK_LIBRARIES RecTPCnv TestTools )
endfunction()

_add_test( CaloEnergyCnv_p1_test )
_add_test( CaloEnergyCnv_p2_test )
_add_test( DepositInCaloCnv_p1_test )
_add_test( DepositInCaloCnv_p2_test )
_add_test( MissingETCnv_p1_test )
_add_test( MissingETCnv_p2_test )
_add_test( MissingETCnv_p3_test )
_add_test( MissingETCompositionCnv_p1_test )
_add_test( MissingETCompositionCnv_p2_test )
_add_test( MissingEtCaloCnv_p1_test )
_add_test( MissingEtCaloCnv_p2_test )
_add_test( MissingEtCaloCnv_p3_test )
_add_test( MissingEtRegionsCnv_p1_test )
_add_test( MissingEtRegionsCnv_p2_test )
_add_test( MissingEtRegionsCnv_p3_test )
_add_test( MissingEtTruthCnv_p1_test )
_add_test( MissingEtTruthCnv_p2_test )
_add_test( MissingEtTruthCnv_p3_test )
_add_test( MuonCaloEnergyContainerCnv_p1_test )
_add_test( MuonCnv_p1_test )
_add_test( MuonCnv_p2_test )
_add_test( MuonCnv_p3_test )
_add_test( MuonCnv_p4_test )
_add_test( MuonCnv_p5_test )
_add_test( MuonCnv_p6_test )
_add_test( MuonSpShowerCnv_p1_test )
_add_test( MuonSpShowerContainerCnv_p1_test )

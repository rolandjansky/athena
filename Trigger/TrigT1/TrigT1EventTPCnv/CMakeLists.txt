# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

# Declare the package name:
atlas_subdir( TrigT1EventTPCnv )

# Component(s) in the package:
atlas_add_tpcnv_library( TrigT1EventTPCnv
                         src/*.cxx
                         PUBLIC_HEADERS TrigT1EventTPCnv
                         LINK_LIBRARIES AthenaPoolCnvSvcLib AthenaPoolUtilities TrigT1CaloEventLib TrigT1Result
                         PRIVATE_LINK_LIBRARIES AthenaKernel GaudiKernel )

atlas_add_dictionary( TrigT1EventTPCnvDict
                      TrigT1EventTPCnv/TrigT1EventTPCnvDict.h
                      TrigT1EventTPCnv/selection.xml
                      LINK_LIBRARIES TrigT1EventTPCnv )

# Tests in the package:
function( _add_test test )
   atlas_add_test( ${test}
                   SOURCES
                   test/${test}.cxx
                   LINK_LIBRARIES TestTools TrigT1EventTPCnv )
endfunction()

_add_test( CMMCPHitsCnv_p1_test )
_add_test( CMMEtSumsCnv_p1_test )
_add_test( CMMJetHitsCnv_p1_test )
_add_test( CMMRoICnv_p1_test )
_add_test( CMXCPHitsCnv_p1_test )
_add_test( CMXCPTobCnv_p1_test )
_add_test( CMXEtSumsCnv_p1_test )
_add_test( CMXJetHitsCnv_p1_test )
_add_test( CMXJetTobCnv_p1_test )
_add_test( CMXRoICnv_p1_test )
_add_test( CPMHitsCnv_p1_test )
_add_test( CPMRoICnv_p1_test )
_add_test( CPMTobRoICnv_p1_test )
_add_test( CPMTowerCnv_p1_test )
_add_test( CTP_RDOCnv_p1_test )
_add_test( CTP_RDOCnv_p2_test )
_add_test( JEMEtSumsCnv_p1_test )
_add_test( JEMHitsCnv_p1_test )
_add_test( JEMRoICnv_p1_test )
_add_test( JEMTobRoICnv_p1_test )
_add_test( JetElementCnv_p1_test )
_add_test( MuCTPI_RDOCnv_p1_test )
_add_test( RODHeaderCnv_p1_test )
_add_test( RoIBResultCnv_p1_test )
_add_test( TriggerTowerCnv_p1_test )

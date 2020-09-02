# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

# Declare the package name:
atlas_subdir( TrigCaloEventTPCnv )

# Component(s) in the package:
atlas_add_tpcnv_library( TrigCaloEventTPCnv
                         src/*.cxx
                         PUBLIC_HEADERS TrigCaloEventTPCnv
                         PRIVATE_LINK_LIBRARIES AthenaKernel
                         LINK_LIBRARIES AthLinks AthenaPoolCnvSvcLib AthenaPoolUtilities CxxUtils DataModelAthenaPoolLib TrigCaloEvent )

atlas_add_dictionary( TrigCaloEventTPCnvDict
                      TrigCaloEventTPCnv/TrigCaloEventTPCnvDict.h
                      TrigCaloEventTPCnv/selection.xml
                      LINK_LIBRARIES TrigCaloEventTPCnv )

# Tests:
function( _add_test test )
   atlas_add_test( ${test}
                   SOURCES
                   test/${test}.cxx
                   LINK_LIBRARIES TestTools TrigCaloEventTPCnv )
endfunction()

_add_test( TrigCaloClusterCnv_p1_test )
_add_test( TrigCaloClusterCnv_p2_test )
_add_test( TrigCaloClusterCnv_p3_test )
_add_test( RingerRingsCnv_p1_test )
_add_test( RingerRingsCnv_p2_test )
_add_test( TrigEMClusterCnv_p2_test )
_add_test( TrigEMClusterCnv_p3_test )
_add_test( TrigEMClusterCnv_p4_test )
_add_test( TrigRNNOutputCnv_p1_test )
_add_test( TrigRNNOutputCnv_p2_test )
_add_test( TrigT2JetCnv_p2_test )
_add_test( TrigT2JetCnv_p3_test )
_add_test( TrigT2MbtsBitsCnv_p1_test )
_add_test( TrigT2MbtsBitsCnv_p2_test )
_add_test( TrigT2MbtsBitsCnv_p3_test )
_add_test( TrigT2ZdcSignalsCnv_p1_test )
_add_test( TrigTauClusterCnv_p2_test )
_add_test( TrigTauClusterCnv_p3_test )
_add_test( TrigTauClusterCnv_p4_test )
_add_test( TrigTauClusterCnv_p5_test )
_add_test( TrigTauClusterDetailsCnv_p1_test )
_add_test( TrigTauClusterDetailsCnv_p2_test )

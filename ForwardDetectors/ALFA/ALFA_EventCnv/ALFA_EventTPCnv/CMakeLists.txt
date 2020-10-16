# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

# Declare the package name:
atlas_subdir( ALFA_EventTPCnv )

# Component(s) in the package:
atlas_add_library( ALFA_EventTPCnv
                   src/*.cxx
                   PUBLIC_HEADERS ALFA_EventTPCnv
                   LINK_LIBRARIES AthenaPoolUtilities ALFA_CLinkEv ALFA_RawEv ALFA_GloRecEv ALFA_LocRecCorrEv ALFA_LocRecEv ALFA_SimEv DataModelAthenaPoolLib AthenaPoolCnvSvcLib
                   PRIVATE_LINK_LIBRARIES Identifier GaudiKernel )

atlas_add_dictionary( ALFA_EventTPCnvDict
                      ALFA_EventTPCnv/ALFA_EventTPCnvDict.h
                      ALFA_EventTPCnv/selection.xml
                      LINK_LIBRARIES ALFA_EventTPCnv )

# Tests in the package:
function( _add_test test )
   atlas_add_test( ${test}
                   SOURCES
                   test/${test}.cxx
                   LINK_LIBRARIES ALFA_EventTPCnv TestTools  )
endfunction()

_add_test( ALFA_CLinkEventCnv_p1_test )
_add_test( ALFA_DigitCnv_p1_test )
_add_test( ALFA_DigitCollectionCnv_p1_test )
_add_test( ALFA_ODDigitCnv_p1_test )
_add_test( ALFA_ODDigitCollectionCnv_p1_test )
_add_test( ALFA_GloRecEventCnv_p1_test )
_add_test( ALFA_GloRecEvCollectionCnv_p1_test )
_add_test( ALFA_HitCnv_p1_test )
_add_test( ALFA_HitCollectionCnv_p1_test )
_add_test( ALFA_ODHitCnv_p1_test )
_add_test( ALFA_ODHitCollectionCnv_p1_test )
_add_test( ALFA_LocRecCorrEventCnv_p1_test )
_add_test( ALFA_LocRecCorrEvCollectionCnv_p1_test )
_add_test( ALFA_LocRecCorrODEventCnv_p1_test )
_add_test( ALFA_LocRecCorrODEvCollectionCnv_p1_test )
_add_test( ALFA_LocRecEventCnv_p1_test )
_add_test( ALFA_LocRecEvCollectionCnv_p1_test )
_add_test( ALFA_LocRecODEventCnv_p1_test )
_add_test( ALFA_LocRecODEvCollectionCnv_p1_test )
_add_test( ALFA_RawDataCnv_p1_test )
_add_test( ALFA_RawDataCnv_charge_p1_test )
_add_test( ALFA_RawDataCollectionCnv_p1_test )
_add_test( ALFA_RawDataCollectionCnv_charge_p1_test )
_add_test( ALFA_RawDataContainerCnv_p1_test )
_add_test( ALFA_RawDataContainerCnv_charge_p1_test )

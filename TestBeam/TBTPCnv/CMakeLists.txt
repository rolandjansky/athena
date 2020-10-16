# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

# Declare the package name:
atlas_subdir( TBTPCnv )

# Component(s) in the package:
atlas_add_library( TBTPCnv
                   src/*.cxx
                   PUBLIC_HEADERS TBTPCnv
                   LINK_LIBRARIES TBEvent AthenaPoolCnvSvcLib
                   PRIVATE_LINK_LIBRARIES Identifier )

atlas_add_dictionary( TBTPCnvDict
                      TBTPCnv/TBTPCnvDict.h
                      TBTPCnv/selection.xml
                      LINK_LIBRARIES TBEvent TBTPCnv )

# Tests in the package:
function( _add_test name )
   atlas_add_test( ${name}
                   SOURCES test/${name}.cxx
                   LINK_LIBRARIES GaudiKernel TBTPCnv TestTools )
endfunction( _add_test )

_add_test( TBADCRawContCnv_p1_test )
_add_test( TBBPCContCnv_p1_test )
_add_test( TBEventInfoCnv_p1_test )
_add_test( TBLArDigitContainerCnv_p1_test )
_add_test( TBMWPCContCnv_p1_test )
_add_test( TBPhaseCnv_p1_test )
_add_test( TBScintillatorContCnv_p1_test )
_add_test( TBTailCatcherCnv_p1_test )
_add_test( TBTDCCnv_p1_test )
_add_test( TBTDCRawContCnv_p1_test )
_add_test( TBTrackCnv_p1_test )
_add_test( TBTrackInfoCnv_p1_test )
_add_test( TBTriggerPatternUnitCnv_p1_test )

# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

# Declare the package name:
atlas_subdir( TrigSteeringEventTPCnv )

# Component(s) in the package:
atlas_add_tpcnv_library( TrigSteeringEventTPCnv
                         src/*.cxx
                         PUBLIC_HEADERS TrigSteeringEventTPCnv
                         LINK_LIBRARIES AthenaPoolCnvSvcLib AthenaPoolUtilities TrigSteeringEvent
                         PRIVATE_LINK_LIBRARIES AthenaKernel GaudiKernel )

atlas_add_dictionary( TrigSteeringEventTPCnvDict
                      TrigSteeringEventTPCnv/TrigSteeringEventTPCnvDict.h
                      TrigSteeringEventTPCnv/selection.xml
                      LINK_LIBRARIES TrigSteeringEventTPCnv )

atlas_add_dictionary( OLD_TrigSteeringEventTPCnvDict
                      TrigSteeringEventTPCnv/TrigSteeringEventTPCnvDict.h
                      TrigSteeringEventTPCnv/OLD_selection.xml
                      LINK_LIBRARIES TrigSteeringEventTPCnv )

# Tests:
function( _add_test test )
   atlas_add_test( ${test}
                   SOURCES
                   test/${test}.cxx
                   LINK_LIBRARIES CxxUtils TestTools TrigSteeringEventTPCnv )
endfunction()

_add_test( HLTResultCnv_p1_test )
_add_test( Lvl1ResultCnv_p2_test )
_add_test( TrigOperationalInfoCnv_p1_test )
_add_test( TrigPassBitsCnv_p1_test )
_add_test( TrigPassFlagsCnv_p1_test )
_add_test( TrigRoiDescriptorCnv_p1_test )
_add_test( TrigRoiDescriptorCnv_p2_test )
_add_test( TrigRoiDescriptorCnv_p3_test )

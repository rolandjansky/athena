# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

# Declare the package name:
atlas_subdir( TrigMonitoringEventTPCnv )

# Component(s) in the package:
atlas_add_tpcnv_library( TrigMonitoringEventTPCnv
                         src/*.cxx
                         PUBLIC_HEADERS TrigMonitoringEventTPCnv
                         LINK_LIBRARIES AthenaPoolCnvSvcLib AthenaPoolUtilities TrigMonitoringEvent
                         PRIVATE_LINK_LIBRARIES AthenaKernel )

atlas_add_dictionary( TrigMonitoringEventTPCnvDict
                      TrigMonitoringEventTPCnv/TrigMonitoringEventTPCnvDict.h
                      TrigMonitoringEventTPCnv/selection.xml
                      LINK_LIBRARIES TrigMonitoringEventTPCnv )

# Tests:
function( _add_test test )
   atlas_add_test( ${test}
                   SOURCES
                   test/${test}.cxx
                   LINK_LIBRARIES GaudiKernel TestTools TrigMonitoringEventTPCnv )
endfunction()

_add_test( TrigConfAlgCnv_p1_test )
_add_test( TrigConfChainCnv_p1_test )
_add_test( TrigConfSeqCnv_p1_test )
_add_test( TrigConfSigCnv_p1_test )
_add_test( TrigMonAlgCnv_p1_test )
_add_test( TrigMonConfigCnv_p1_test )
_add_test( TrigMonEventCnv_p1_test )
_add_test( TrigMonROBDataCnv_p1_test )
_add_test( TrigMonROBDataCnv_p2_test )
_add_test( TrigMonROBCnv_p1_test )
_add_test( TrigMonRoiCnv_p1_test )
_add_test( TrigMonSeqCnv_p1_test )
_add_test( TrigMonTECnv_p1_test )

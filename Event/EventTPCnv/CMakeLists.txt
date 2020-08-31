# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

# Declare the package name:
atlas_subdir( EventTPCnv )

# Component(s) in the package:
atlas_add_tpcnv_library( EventTPCnv
                         src/*.cxx
                         PUBLIC_HEADERS EventTPCnv
                         LINK_LIBRARIES AthenaPoolCnvSvcLib EventInfo
                         PRIVATE_LINK_LIBRARIES AthenaKernel GaudiKernel )

atlas_add_dictionary( EventTPCnvDict
                      EventTPCnv/EventTPCnvDict.h
                      EventTPCnv/selection.xml
                      LINK_LIBRARIES EventTPCnv )

atlas_add_dictionary( OLD_EventTPCnvDict
                      EventTPCnv/EventTPCnvDict.h
                      EventTPCnv/OLD_selection.xml
                      LINK_LIBRARIES EventTPCnv )

foreach( name
    EventIDCnv_p1_test
    EventIDCnv_p2_test
    EventInfoCnv_p1_test
    EventInfoCnv_p2_test
    EventInfoCnv_p3_test
    EventInfoCnv_p4_test
    EventStreamInfoCnv_p1_test
    EventStreamInfoCnv_p2_test
    EventStreamInfoCnv_p3_test
    EventTypeCnv_p1_test
    EventTypeCnv_p2_test
    EventTypeCnv_p3_test
    MergedEventInfoCnv_p1_test
    MergedEventInfoCnv_p2_test
    PileUpEventInfoCnv_p1_test
    PileUpEventInfoCnv_p2_test
    PileUpEventInfoCnv_p3_test
    PileUpEventInfoCnv_p4_test
    PileUpEventInfoCnv_p5_test
    TriggerInfoCnv_p1_test
    TriggerInfoCnv_p2_test
    TriggerInfoCnv_p3_test
    AtlasMcWeight_test
    vectorize_test
         )

       atlas_add_test( ${name}
         SOURCES
         test/${name}.cxx
         LINK_LIBRARIES TestTools EventTPCnv )

endforeach()

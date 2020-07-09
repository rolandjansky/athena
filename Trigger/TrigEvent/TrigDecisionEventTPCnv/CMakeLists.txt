# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

# Declare the package name:
atlas_subdir( TrigDecisionEventTPCnv )

# Component(s) in the package:
atlas_add_tpcnv_library( TrigDecisionEventTPCnv
                         src/*.cxx
                         PUBLIC_HEADERS TrigDecisionEventTPCnv
                         LINK_LIBRARIES AthenaPoolCnvSvcLib DataModelAthenaPoolLib TrigDecisionEvent TrigSteeringEventTPCnv
                         PRIVATE_LINK_LIBRARIES AthenaKernel )

atlas_add_dictionary( TrigDecisionEventTPCnvDict
                      TrigDecisionEventTPCnv/TrigDecisionEventTPCnvDict.h
                      TrigDecisionEventTPCnv/selection.xml
                      LINK_LIBRARIES TrigDecisionEventTPCnv )

atlas_add_dictionary( OLD_TrigDecisionEventTPCnvDict
                      TrigDecisionEventTPCnv/TrigDecisionEventTPCnvDict.h
                      TrigDecisionEventTPCnv/OLD_selection.xml
                      LINK_LIBRARIES TrigDecisionEventTPCnv )

atlas_add_test( TrigDecisionCnv_p1_test
                SOURCES
                test/TrigDecisionCnv_p1_test.cxx
                LINK_LIBRARIES TestTools TrigDecisionEventTPCnv )

atlas_add_test( TrigDecisionCnv_p2_test
                SOURCES
                test/TrigDecisionCnv_p2_test.cxx
                LINK_LIBRARIES TestTools TrigDecisionEventTPCnv )

atlas_add_test( TrigDecisionCnv_p3_test
                SOURCES
                test/TrigDecisionCnv_p3_test.cxx
                LINK_LIBRARIES TestTools TrigDecisionEventTPCnv )

atlas_add_test( TrigDecisionCnv_p4_test
                SOURCES
                test/TrigDecisionCnv_p4_test.cxx
                LINK_LIBRARIES TestTools TrigDecisionEventTPCnv )

atlas_add_test( TrigDecisionCnv_p5_test
                SOURCES
                test/TrigDecisionCnv_p5_test.cxx
                LINK_LIBRARIES TestTools TrigDecisionEventTPCnv )

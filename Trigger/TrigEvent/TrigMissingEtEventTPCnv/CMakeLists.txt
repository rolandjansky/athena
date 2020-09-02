# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

# Declare the package name:
atlas_subdir( TrigMissingEtEventTPCnv )

# Component(s) in the package:
atlas_add_tpcnv_library( TrigMissingEtEventTPCnv
                         src/*.cxx
                         PUBLIC_HEADERS TrigMissingEtEventTPCnv
                         PRIVATE_LINK_LIBRARIES AthenaKernel
                         LINK_LIBRARIES AthenaPoolCnvSvcLib AthenaPoolUtilities TrigMissingEtEvent )

atlas_add_dictionary( TrigMissingEtEventTPCnvDict
                      TrigMissingEtEventTPCnv/TrigMissingEtEventTPCnvDict.h
                      TrigMissingEtEventTPCnv/selection.xml
                      LINK_LIBRARIES TrigMissingEtEventTPCnv )

atlas_add_test( TrigMissingETCnv_p2_test
                SOURCES
                test/TrigMissingETCnv_p2_test.cxx
                LINK_LIBRARIES GaudiKernel TestTools TrigMissingEtEventTPCnv )

atlas_add_test( TrigMissingETCnv_p3_test
                SOURCES
                test/TrigMissingETCnv_p3_test.cxx
                LINK_LIBRARIES GaudiKernel TestTools TrigMissingEtEventTPCnv )

# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

# Declare the package name:
atlas_subdir( TrigCombinedEventTPCnv )

# External dependencies:
find_package( Boost )

# Component(s) in the package:
atlas_add_tpcnv_library( TrigCombinedEventTPCnv
                         src/*.cxx
                         PUBLIC_HEADERS TrigCombinedEventTPCnv
                         PRIVATE_INCLUDE_DIRS ${Boost_INCLUDE_DIRS}
                         LINK_LIBRARIES AthenaPoolCnvSvcLib AthenaPoolUtilities TrigCombinedEvent TrigNavigationLib TestTools AthenaKernel GaudiKernel )

atlas_add_dictionary( TrigCombinedEventTPCnvDict
                      TrigCombinedEventTPCnv/TrigCombinedEventTPCnvDict.h
                      TrigCombinedEventTPCnv/selection.xml
                      LINK_LIBRARIES TrigCombinedEventTPCnv )

atlas_add_test( TrigCompositeCnv_p1_test
                SOURCES
                test/TrigCompositeCnv_p1_test.cxx
                LINK_LIBRARIES TrigCombinedEventTPCnv )

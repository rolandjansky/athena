# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

# Declare the package name.
atlas_subdir( CaloGeoHelpers )

# Component(s) in the package.
atlas_add_library( CaloGeoHelpers
   CaloGeoHelpers/*.h CaloGeoHelpers/*.def Root/*.cxx
   PUBLIC_HEADERS CaloGeoHelpers
   LINK_LIBRARIES CxxUtils )

atlas_add_dictionary( CaloGeoHelpersDict
   CaloGeoHelpers/CaloGeoHelpersDict.h
   CaloGeoHelpers/selection.xml
   LINK_LIBRARIES CaloGeoHelpers )

# Test(s) in the package.
atlas_add_test( CaloPhiRange_test
   SOURCES test/CaloPhiRange_test.cxx
   LINK_LIBRARIES CaloGeoHelpers )

atlas_add_test( CaloSampling_test
   SOURCES test/CaloSampling_test.cxx
   LINK_LIBRARIES CaloGeoHelpers )

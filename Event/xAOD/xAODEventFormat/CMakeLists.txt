# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

# Declare the package name.
atlas_subdir( xAODEventFormat )

# Component(s) in the package.
atlas_add_library( xAODEventFormat
   xAODEventFormat/*.h xAODEventFormat/versions/*.h Root/*.cxx
   PUBLIC_HEADERS xAODEventFormat
   LINK_LIBRARIES xAODCore )

atlas_add_dictionary( xAODEventFormatDict
   xAODEventFormat/xAODEventFormatDict.h
   xAODEventFormat/selection.xml
   LINK_LIBRARIES xAODCore xAODEventFormat )

# Test(s) in the package.
atlas_add_test( eventformat_test
   SOURCES test/ut_xaodeventformat_eventformat_test.cxx
   LINK_LIBRARIES xAODEventFormat )

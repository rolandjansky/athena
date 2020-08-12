# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

# Declare the package name:
atlas_subdir( AthContainersInterfaces )

# Extra dependencies, based on the build environment:
if( NOT XAOD_STANDALONE )
   set( extra_libs AthenaKernel )
endif()

# Interface library for the package:
atlas_add_library( AthContainersInterfaces
   AthContainersInterfaces/*.h AthContainersInterfaces/*.icc
   INTERFACE
   PUBLIC_HEADERS AthContainersInterfaces
   LINK_LIBRARIES CxxUtils ${extra_libs} )

# Test(s) in the package:
atlas_add_test( AuxStore_traits_test
   SOURCES test/AuxStore_traits_test.cxx
   LINK_LIBRARIES AthContainersInterfaces )

atlas_add_test( AuxDataOption_test
   SOURCES test/AuxDataOption_test.cxx
   LINK_LIBRARIES AthContainersInterfaces )

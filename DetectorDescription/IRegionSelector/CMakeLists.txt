# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

# Declare the package name:
atlas_subdir( IRegionSelector )

# Component(s) in the package:
if( XAOD_STANDALONE )
   atlas_add_library( IRegionSelector
      IRegionSelector/*.h
      INTERFACE
      PUBLIC_HEADERS IRegionSelector )
else()
   atlas_add_library( IRegionSelector
      IRegionSelector/*.h
      INTERFACE
      PUBLIC_HEADERS IRegionSelector
      LINK_LIBRARIES AthenaKernel Identifier GaudiKernel )
endif()

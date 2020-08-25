# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

# Declare the package name:
atlas_subdir( MagFieldElements )

# External dependencies:
find_package( ROOT COMPONENTS Core Tree RIO  )

# Component(s) in the package:
atlas_add_library( MagFieldElements
                   src/*.cxx
                   PUBLIC_HEADERS MagFieldElements
                   INCLUDE_DIRS ${ROOT_INCLUDE_DIRS}
                   LINK_LIBRARIES CxxUtils EventPrimitives GaudiKernel
                   PRIVATE_LINK_LIBRARIES ${ROOT_LIBRARIES} PathResolver )

		 
atlas_add_test( BFieldExample_test
                SOURCES  test/BFieldExample_test.cxx
                INCLUDE_DIRS 
                LINK_LIBRARIES MagFieldElements)		

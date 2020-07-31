# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

# Declare the package name:
atlas_subdir( Identifier )

# External dependencies:
find_package( Boost )

# Component(s) in the package:
atlas_add_library( Identifier
   src/Identifier.cxx src/Identifier32.cxx
   src/ExpandedIdentifier.cxx src/Range.cxx
   src/Identifiable.cxx src/IdHelper.cxx
   PUBLIC_HEADERS Identifier
   INCLUDE_DIRS ${Boost_INCLUDE_DIRS}
   LINK_LIBRARIES ${Boost_LIBRARIES} GaudiKernel )

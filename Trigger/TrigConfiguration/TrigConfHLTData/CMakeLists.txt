# Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration

# Declare the package name:
atlas_subdir( TrigConfHLTData )

# External dependencies:
find_package( Boost )

# Component(s) in the package:
atlas_add_library( TrigConfHLTData
   TrigConfHLTData/*.h Root/*.cxx
   PUBLIC_HEADERS TrigConfHLTData
   INCLUDE_DIRS ${Boost_INCLUDE_DIRS}
   LINK_LIBRARIES ${Boost_LIBRARIES} TrigConfL1Data TrigConfHLTUtilsLib )

# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

# Declare the package name:
atlas_subdir( TrigConfHLTData )

# External dependencies:
find_package( Boost COMPONENTS system thread )

# Component(s) in the package:
atlas_add_library( TrigConfHLTData
   TrigConfHLTData/*.h Root/*.cxx
   PUBLIC_HEADERS TrigConfHLTData
   INCLUDE_DIRS ${Boost_INCLUDE_DIRS}
   LINK_LIBRARIES ${Boost_LIBRARIES} TrigConfL1Data TrigConfHLTUtilsLib )

atlas_install_python_modules( python/*.py POST_BUILD_CMD ${ATLAS_FLAKE8} )

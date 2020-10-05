# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

# Declare the package name:
atlas_subdir( AthenaIPCTools )

# External dependencies:
find_package( Boost )
find_package( yampl )

# Component(s) in the package:
atlas_add_component( AthenaIPCTools 
		     src/*.cxx 
		     src/components/*.cxx
   		     INCLUDE_DIRS ${Boost_INCLUDE_DIRS} ${YAMPL_INCLUDE_DIRS} 
		     LINK_LIBRARIES ${Boost_LIBRARIES} ${YAMPL_LIBRARIES} AthenaBaseComps AthenaKernel GaudiKernel )


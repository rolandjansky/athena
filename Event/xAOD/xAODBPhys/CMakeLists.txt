# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

# Declare the package name:
atlas_subdir( xAODBPhys )

# Used external(s):
find_package( ROOT COMPONENTS Core Physics Matrix )

# Build the main library of the package: 
atlas_add_library( xAODBPhysLib
		           xAODBPhys/*.h Root/*.cxx src/*.cxx
		           PUBLIC_HEADERS xAODBPhys
		           INCLUDE_DIRS ${ROOT_INCLUDE_DIRS}
		           LINK_LIBRARIES ${ROOT_LIBRARIES} xAODEgamma xAODMetaData xAODMuon xAODTracking
                   PRIVATE_LINK_LIBRARIES AthContainers )

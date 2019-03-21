#
# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
#

# $Id: CMakeLists.txt 805745 2017-05-31 17:23:48Z wwalko $
#
# Build configuration for the package.
#
#********************************************
# The name of the package:
atlas_subdir( xAODBPhys )

# Package dependencies:
atlas_depends_on_subdirs(
   PUBLIC
   Event/xAOD/xAODTracking
   Event/xAOD/xAODMuon
   Event/xAOD/xAODMetaData
   Event/xAOD/xAODEgamma
   PRIVATE
   )

# Used external(s):
find_package( ROOT COMPONENTS Core Physics Matrix )

# Build the main library of the package: 
atlas_add_library( xAODBPhysLib
		   xAODBPhys/*.h Root/*.cxx src/*.cxx
		   PUBLIC_HEADERS xAODBPhys
		   INCLUDE_DIRS ${ROOT_INCLUDE_DIRS}
		   LINK_LIBRARIES ${ROOT_LIBRARIES} 
		   xAODTracking
		   xAODMuon
                   xAODEgamma
		   xAODMetaData
		   )


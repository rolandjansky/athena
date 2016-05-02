# $Id: CMakeLists.txt 744230 2016-05-02 14:44:18Z krasznaa $
#
# Build configuration for the package.
#

# The name of the package:
atlas_subdir( xAODBPhys )

# Package dependencies:
atlas_depends_on_subdirs(
   PUBLIC
   Event/xAOD/xAODTracking
   Event/xAOD/xAODMuon )

# Used external(s):
find_package( ROOT COMPONENTS Core Physics Matrix )

# Build the main library of the package:
atlas_add_library( xAODBPhys
   xAODBPhys/*.h Root/*.cxx
   PUBLIC_HEADERS xAODBPhys
   INCLUDE_DIRS ${ROOT_INCLUDE_DIRS}
   LINK_LIBRARIES ${ROOT_LIBRARIES} xAODTracking xAODMuon )

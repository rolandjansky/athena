#
# Build configuration for the standalone AthLinksSA package.
#

# Declare the name of this package:
atlas_subdir( AthLinks )

# This package depends on other packages:
atlas_depends_on_subdirs(
   PUBLIC Control/xAODRootAccessInterfaces )

# This package uses ROOT:
find_package( ROOT COMPONENTS Core )

# Build a library that other components can link against:
atlas_add_library( AthLinks
   AthLinks/*.h AthLinks/*.icc AthLinks/tools/*.h Root/*.cxx
   PUBLIC_HEADERS AthLinks
   INCLUDE_DIRS ${ROOT_INCLUDE_DIRS}
   LINK_LIBRARIES ${ROOT_LIBRARIES} xAODRootAccessInterfaces )

# Build a (Reflex) dictionary library:
atlas_add_dictionary( AthLinksDict
   AthLinks/AthLinksDict.h
   AthLinks/selection.xml
   LINK_LIBRARIES AthLinks )

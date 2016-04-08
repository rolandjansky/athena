################################################################################
# Package: AtlasSTLAddReflex
################################################################################

# Declare the package name:
atlas_subdir( AtlasSTLAddReflex )

# External dependencies:
find_package( Boost COMPONENTS filesystem thread system )
find_package( ROOT COMPONENTS Core Tree MathCore Hist RIO pthread )

# Component(s) in the package:
atlas_add_dictionary( AtlasSTLAddReflexDict
                      AtlasSTLAddReflex/AtlasSTLAddReflexDict.h
                      AtlasSTLAddReflex/selection.xml
                      INCLUDE_DIRS ${ROOT_INCLUDE_DIRS} ${Boost_INCLUDE_DIRS}
                      LINK_LIBRARIES ${ROOT_LIBRARIES} ${Boost_LIBRARIES} )

# Install files from the package:
atlas_install_headers( AtlasSTLAddReflex )


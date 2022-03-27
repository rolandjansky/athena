# Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration

# Declare the package name:
atlas_subdir( AtlasSTLAddReflex )

# External dependencies:
find_package( Boost )

# Component(s) in the package:
atlas_add_dictionary( AtlasSTLAddReflexDict
                      AtlasSTLAddReflex/AtlasSTLAddReflexDict.h
                      AtlasSTLAddReflex/selection.xml
                      INCLUDE_DIRS ${Boost_INCLUDE_DIRS}
                      LINK_LIBRARIES ${Boost_LIBRARIES} )

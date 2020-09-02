# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

# Declare the package name:
atlas_subdir( RelationalCollection )

# External dependencies:
find_package( CORAL COMPONENTS CoralBase RelationalAccess )

# Component(s) in the package:
atlas_add_library( RelationalCollection
                   src/*.cpp
                   NO_PUBLIC_HEADERS
                   PRIVATE_INCLUDE_DIRS ${CORAL_INCLUDE_DIRS}
                   PRIVATE_LINK_LIBRARIES ${CORAL_LIBRARIES} CollectionBase POOLCore PersistentDataModel )

# Component list generation:
atlas_generate_componentslist( RelationalCollection )

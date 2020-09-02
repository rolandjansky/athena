# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

# Declare the package name:
atlas_subdir( ImplicitCollection )

# External dependencies:
find_package( CORAL COMPONENTS CoralBase )

# Component(s) in the package:
atlas_add_library( ImplicitCollection
                   src/*.cpp
                   NO_PUBLIC_HEADERS
                   PRIVATE_INCLUDE_DIRS ${CORAL_INCLUDE_DIRS}
                   PRIVATE_LINK_LIBRARIES ${CORAL_LIBRARIES} CollectionBase POOLCore PersistencySvc PersistentDataModel StorageSvc )

# Component list generation:
atlas_generate_componentslist( ImplicitCollection )

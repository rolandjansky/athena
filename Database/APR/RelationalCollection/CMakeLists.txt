################################################################################
# Package: RelationalCollection
################################################################################

# Declare the package name:
atlas_subdir( RelationalCollection )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PRIVATE
                          Database/APR/CollectionBase
                          Database/APR/POOLCore
                          Database/PersistentDataModel
                          GaudiKernel
                          TestPolicy )

# External dependencies:
find_package( CORAL COMPONENTS CoralBase CoralKernel RelationalAccess )

# Component(s) in the package:
atlas_add_library( RelationalCollection
                   src/*.cpp
                   NO_PUBLIC_HEADERS
                   PRIVATE_INCLUDE_DIRS ${CORAL_INCLUDE_DIRS}
                   PRIVATE_LINK_LIBRARIES ${CORAL_LIBRARIES} CollectionBase POOLCore PersistentDataModel GaudiKernel )

# Component list generation:
atlas_generate_componentslist( RelationalCollection )

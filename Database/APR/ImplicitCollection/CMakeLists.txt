################################################################################
# Package: ImplicitCollection
################################################################################

# Declare the package name:
atlas_subdir( ImplicitCollection )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PRIVATE
                          Control/AthenaKernel
                          Database/APR/CollectionBase
                          Database/APR/POOLCore
                          Database/APR/PersistencySvc
                          Database/APR/StorageSvc
                          Database/PersistentDataModel
                          GaudiKernel )

# External dependencies:
find_package( CORAL COMPONENTS CoralBase CoralKernel RelationalAccess )

# Component(s) in the package:
atlas_add_library( ImplicitCollection
                   src/*.cpp
                   NO_PUBLIC_HEADERS
                   PRIVATE_INCLUDE_DIRS ${CORAL_INCLUDE_DIRS}
                   PRIVATE_LINK_LIBRARIES ${CORAL_LIBRARIES} AthenaKernel CollectionBase POOLCore PersistencySvc StorageSvc PersistentDataModel GaudiKernel )

# Component list generation:
atlas_generate_componentslist( ImplicitCollection )

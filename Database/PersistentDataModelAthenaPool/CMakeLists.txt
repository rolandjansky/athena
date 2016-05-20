################################################################################
# Package: PersistentDataModelAthenaPool
################################################################################

# Declare the package name:
atlas_subdir( PersistentDataModelAthenaPool )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PRIVATE
                          Database/AthenaPOOL/AthenaPoolCnvSvc
                          Database/AthenaPOOL/AthenaPoolUtilities
                          Database/PersistentDataModel
                          Database/PersistentDataModelTPCnv )

# External dependencies:
find_package( CORAL COMPONENTS CoralBase CoralKernel RelationalAccess )

# Component(s) in the package:
atlas_add_poolcnv_library( PersistentDataModelAthenaPoolPoolCnv
                           src/*.cxx
                           FILES PersistentDataModel/DataHeader.h
                           INCLUDE_DIRS ${CORAL_INCLUDE_DIRS}
                           LINK_LIBRARIES ${CORAL_LIBRARIES} AthenaPoolCnvSvcLib AthenaPoolUtilities PersistentDataModel PersistentDataModelTPCnv )


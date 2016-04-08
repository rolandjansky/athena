################################################################################
# Package: IOVDbAthenaPool
################################################################################

# Declare the package name:
atlas_subdir( IOVDbAthenaPool )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PUBLIC
                          Database/AthenaPOOL/AthenaPoolUtilities
                          PRIVATE
                          Database/AthenaPOOL/AthenaPoolCnvSvc
                          Database/IOVDbDataModel
                          Database/IOVDbTPCnv )

# Component(s) in the package:
atlas_add_poolcnv_library( IOVDbAthenaPoolPoolCnv
                           src/*.cxx
                           FILES IOVDbDataModel/IOVMetaDataContainer.h
                           LINK_LIBRARIES AthenaPoolUtilities AthenaPoolCnvSvcLib IOVDbDataModel IOVDbTPCnv )


################################################################################
# Package: xAODMetaDataAthenaPool
################################################################################

# Declare the package name:
atlas_subdir( xAODMetaDataAthenaPool )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PRIVATE
                          Database/AthenaPOOL/AthenaPoolCnvSvc
                          Database/AthenaPOOL/AthenaPoolUtilities
                          Event/xAOD/xAODMetaData )

# Component(s) in the package:
atlas_add_poolcnv_library( xAODMetaDataAthenaPoolPoolCnv
                           src/*.cxx
                           FILES xAODMetaData/FileMetaData.h xAODMetaData/FileMetaDataAuxInfo.h
                           TYPES_WITH_NAMESPACE xAOD::FileMetaData xAOD::FileMetaDataAuxInfo
                           CNV_PFX xAOD
                           LINK_LIBRARIES AthenaPoolCnvSvcLib AthenaPoolUtilities xAODMetaData )


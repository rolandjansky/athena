################################################################################
# Package: xAODEventFormatAthenaPool
################################################################################

# Declare the package name:
atlas_subdir( xAODEventFormatAthenaPool )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PRIVATE
                          Database/AthenaPOOL/AthenaPoolCnvSvc
                          Database/AthenaPOOL/AthenaPoolUtilities
                          Event/xAOD/xAODEventFormat )

# Component(s) in the package:
atlas_add_poolcnv_library( xAODEventFormatAthenaPoolPoolCnv
                           src/*.cxx
                           FILES xAODEventFormat/EventFormat.h
                           TYPES_WITH_NAMESPACE xAOD::EventFormat
                           CNV_PFX xAOD
                           LINK_LIBRARIES AthenaPoolCnvSvcLib AthenaPoolUtilities xAODEventFormat )


################################################################################
# Package: xAODLuminosityAthenaPool
################################################################################

# Declare the package name:
atlas_subdir( xAODLuminosityAthenaPool )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PRIVATE
                          Database/AthenaPOOL/AthenaPoolCnvSvc
                          Database/AthenaPOOL/AthenaPoolUtilities
                          Event/xAOD/xAODLuminosity )

# Component(s) in the package:
atlas_add_poolcnv_library( xAODLuminosityAthenaPoolPoolCnv
                           src/*.cxx
                           FILES xAODLuminosity/LumiBlockRangeContainer.h xAODLuminosity/LumiBlockRangeAuxContainer.h
                           TYPES_WITH_NAMESPACE xAOD::LumiBlockRangeContainer xAOD::LumiBlockRangeAuxContainer
                           CNV_PFX xAOD
                           LINK_LIBRARIES AthenaPoolCnvSvcLib AthenaPoolUtilities xAODLuminosity )


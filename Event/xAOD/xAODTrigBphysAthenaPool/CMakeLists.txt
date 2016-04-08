################################################################################
# Package: xAODTrigBphysAthenaPool
################################################################################

# Declare the package name:
atlas_subdir( xAODTrigBphysAthenaPool )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PRIVATE
                          Control/AthContainers
                          Control/AthenaKernel
                          Database/AthenaPOOL/AthenaPoolCnvSvc
                          Database/AthenaPOOL/AthenaPoolUtilities
                          Event/xAOD/xAODTrigBphys )

# Component(s) in the package:
atlas_add_poolcnv_library( xAODTrigBphysAthenaPoolPoolCnv
                           src/*.cxx
                           FILES xAODTrigBphys/TrigBphysContainer.h xAODTrigBphys/TrigBphysAuxContainer.h
                           TYPES_WITH_NAMESPACE xAOD::TrigBphys
                           CNV_PFX xAOD
                           LINK_LIBRARIES AthContainers AthenaKernel AthenaPoolCnvSvcLib AthenaPoolUtilities xAODTrigBphys )


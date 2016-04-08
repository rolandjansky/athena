################################################################################
# Package: xAODAssociationsAthenaPool
################################################################################

# Declare the package name:
atlas_subdir( xAODAssociationsAthenaPool )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PRIVATE
                          Control/AthContainers
                          Control/AthenaKernel
                          Database/AthenaPOOL/AthenaPoolCnvSvc
                          Database/AthenaPOOL/AthenaPoolUtilities
                          Event/xAOD/xAODAssociations
                          Event/xAOD/xAODCaloEvent
                          Event/xAOD/xAODTracking )

# External dependencies:
find_package( ROOT COMPONENTS Core Tree MathCore Hist RIO pthread )

# Component(s) in the package:
atlas_add_poolcnv_library( xAODAssociationsAthenaPoolPoolCnv
                           src/*.cxx
                           FILES xAODAssociations/TrackParticleClusterAssociationContainer.h xAODAssociations/TrackParticleClusterAssociationAuxContainer.h
                           TYPES_WITH_NAMESPACE xAOD::TrackParticleClusterAssociationContainer xAOD::TrackParticleClusterAssociationAuxContainer
                           CNV_PFX xAOD
                           INCLUDE_DIRS ${ROOT_INCLUDE_DIRS}
                           LINK_LIBRARIES ${ROOT_LIBRARIES} AthContainers AthenaKernel AthenaPoolCnvSvcLib AthenaPoolUtilities xAODAssociations xAODCaloEvent xAODTracking )


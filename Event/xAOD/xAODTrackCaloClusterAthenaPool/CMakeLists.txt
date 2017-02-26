# $Id: CMakeLists.txt $
################################################################################
# Package: xAODTrackCaloClusterAthenaPool
################################################################################

# Declare the package name:
atlas_subdir( xAODTrackCaloClusterAthenaPool )

# Declare the package's dependencies:
atlas_depends_on_subdirs(
   PRIVATE
   Control/AthContainers
   Control/AthenaKernel
   Database/AthenaPOOL/AthenaPoolCnvSvc
   Database/AthenaPOOL/AthenaPoolUtilities
   Event/xAOD/xAODTrackCaloCluster
   Event/xAOD/xAODTrackCaloClusterCnv )

# External dependencies:
find_package( ROOT COMPONENTS Core )

# Component(s) in the package:
atlas_add_poolcnv_library( xAODTrackCaloClusterAthenaPoolPoolCnv
   src/*.h src/*.cxx
   FILES xAODTrackCaloCluster/TrackCaloClusterContainer.h
   xAODTrackCaloCluster/TrackCaloClusterAuxContainer.h
   TYPES_WITH_NAMESPACE xAOD::TrackCaloClusterContainer
   xAOD::TrackCaloClusterAuxContainer 
   CNV_PFX xAOD
   INCLUDE_DIRS ${ROOT_INCLUDE_DIRS}
   LINK_LIBRARIES ${ROOT_LIBRARIES} AthContainers AthenaKernel
   AthenaPoolCnvSvcLib AthenaPoolUtilities xAODTrackCaloCluster xAODTrackCaloClusterCnvLib )
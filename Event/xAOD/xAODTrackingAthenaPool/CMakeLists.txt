# $Id: CMakeLists.txt 789663 2016-12-14 14:48:57Z krasznaa $
################################################################################
# Package: xAODTrackingAthenaPool
################################################################################

# Declare the package name:
atlas_subdir( xAODTrackingAthenaPool )

# Declare the package's dependencies:
atlas_depends_on_subdirs(
   PRIVATE
   Control/AthContainers
   Control/AthenaKernel
   Database/AthenaPOOL/AthenaPoolCnvSvc
   Database/AthenaPOOL/AthenaPoolUtilities
   Event/xAOD/xAODTracking
   Event/xAOD/xAODTrackingCnv
   Event/xAOD/xAODTruth )

# External dependencies:
find_package( ROOT COMPONENTS Core )

# Component(s) in the package:
atlas_add_poolcnv_library( xAODTrackingAthenaPoolPoolCnv
   src/*.h src/*.cxx
   FILES xAODTracking/TrackParticleContainer.h
   xAODTracking/TrackParticleAuxContainer.h
   xAODTracking/NeutralParticleContainer.h
   xAODTracking/NeutralParticleAuxContainer.h xAODTracking/VertexContainer.h
   xAODTracking/VertexAuxContainer.h xAODTracking/SCTRawHitValidationContainer.h
   xAODTracking/SCTRawHitValidationAuxContainer.h
   xAODTracking/TrackMeasurementValidationContainer.h
   xAODTracking/TrackMeasurementValidationAuxContainer.h
   xAODTracking/TrackStateValidationContainer.h
   xAODTracking/TrackStateValidationAuxContainer.h
   TYPES_WITH_NAMESPACE xAOD::TrackParticleContainer
   xAOD::TrackParticleAuxContainer xAOD::NeutralParticleContainer
   xAOD::NeutralParticleAuxContainer xAOD::VertexContainer
   xAOD::VertexAuxContainer xAOD::SCTRawHitValidationContainer
   xAOD::SCTRawHitValidationAuxContainer
   xAOD::TrackMeasurementValidationContainer
   xAOD::TrackMeasurementValidationAuxContainer
   xAOD::TrackStateValidationContainer xAOD::TrackStateValidationAuxContainer
   CNV_PFX xAOD
   INCLUDE_DIRS ${ROOT_INCLUDE_DIRS}
   LINK_LIBRARIES ${ROOT_LIBRARIES} AthContainers AthenaKernel
   AthenaPoolCnvSvcLib AthenaPoolUtilities xAODTracking xAODTrackingCnvLib
   xAODTruth )

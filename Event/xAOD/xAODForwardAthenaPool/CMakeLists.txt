################################################################################
# Package: xAODForwardAthenaPool
################################################################################

# Declare the package name:
atlas_subdir( xAODForwardAthenaPool )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PRIVATE
                          Control/AthContainers
                          Control/AthenaKernel
                          Database/AthenaPOOL/AthenaPoolCnvSvc
                          Database/AthenaPOOL/AthenaPoolUtilities
                          Event/xAOD/xAODForward )

# Component(s) in the package:
atlas_add_poolcnv_library( xAODForwardAthenaPoolPoolCnv
                           src/*.cxx
                           FILES xAODForward/ALFADataContainer.h
			   xAODForward/ALFADataAuxContainer.h
			   xAODForward/AFPDataContainer.h
			   xAODForward/AFPDataAuxContainer.h
   			   xAODForward/AFPSiHitContainer.h
			   xAODForward/AFPSiHitAuxContainer.h
                           xAODForward/AFPToFHitContainer.h
            		   xAODForward/AFPToFHitAuxContainer.h
        	           xAODForward/AFPSiHitsClusterContainer.h
			   xAODForward/AFPSiHitsClusterAuxContainer.h
   			   xAODForward/AFPTrackContainer.h
			   xAODForward/AFPTrackAuxContainer.h
			   xAODForward/ZdcModuleContainer.h
			   xAODForward/ZdcModuleAuxContainer.h
			   xAODForward/MBTSModuleContainer.h
			   xAODForward/MBTSModuleAuxContainer.h
			   xAODForward/ForwardEventInfoContainer.h
			   xAODForward/ForwardEventInfoAuxContainer.h
               TYPES_WITH_NAMESPACE xAOD::ALFADataContainer xAOD::ALFADataAuxContainer xAOD::AFPDataContainer xAOD::AFPDataAuxContainer xAOD::AFPSiHitContainer xAOD::AFPSiHitAuxContainer xAOD::AFPToFHitContainer xAOD::AFPToFHitAuxContainer xAOD::AFPSiHitsClusterContainer xAOD::AFPSiHitsClusterAuxContainer xAOD::AFPTrackContainer xAOD::AFPTrackAuxContainer xAOD::ZdcModuleContainer xAOD::ZdcModuleAuxContainer xAOD::MBTSModuleContainer xAOD::MBTSModuleAuxContainer xAOD::ForwardEventInfoContainer xAOD::ForwardEventInfoAuxContainer
                           CNV_PFX xAOD
                           LINK_LIBRARIES AthContainers AthenaKernel AthenaPoolCnvSvcLib AthenaPoolUtilities xAODForward )


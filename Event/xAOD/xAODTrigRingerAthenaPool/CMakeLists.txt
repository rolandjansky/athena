################################################################################
# Package: xAODTrigRingerAthenaPool
################################################################################

# Declare the package name:
atlas_subdir( xAODTrigRingerAthenaPool )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PRIVATE
                          Control/AthContainers
                          Control/AthenaKernel
                          Database/AthenaPOOL/AthenaPoolCnvSvc
                          Database/AthenaPOOL/AthenaPoolUtilities
                          Event/xAOD/xAODTrigRinger
                          GaudiKernel )

# Component(s) in the package:
atlas_add_poolcnv_library( xAODTrigRingerAthenaPoolPoolCnv
                           src/*.cxx
                           FILES xAODTrigRinger/TrigRingerRingsContainer.h xAODTrigRinger/TrigRingerRingsAuxContainer.h xAODTrigRinger/TrigRNNOutputContainer.h xAODTrigRinger/TrigRNNOutputAuxContainer.h
                           TYPES_WITH_NAMESPACE xAOD::TrigRingerRingsContainer xAOD::TrigRingerRingsAuxContainer xAOD::TrigRNNOutputContainer xAOD::TrigRNNOutputAuxContainer
                           CNV_PFX xAOD
                           LINK_LIBRARIES AthContainers AthenaKernel AthenaPoolCnvSvcLib AthenaPoolUtilities xAODTrigRinger GaudiKernel )


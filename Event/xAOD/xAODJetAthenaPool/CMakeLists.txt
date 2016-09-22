################################################################################
# Package: xAODJetAthenaPool
################################################################################

# Declare the package name:
atlas_subdir( xAODJetAthenaPool )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PRIVATE
                          Control/AthContainers
                          Control/AthenaKernel
                          Database/AthenaPOOL/AthenaPoolCnvSvc
                          Database/AthenaPOOL/AthenaPoolUtilities
                          Event/xAOD/xAODJet )

# External dependencies:
find_package( ROOT COMPONENTS Core Tree MathCore Hist RIO pthread )

set( poolcnv_files )
set( poolcnv_types )

if( NOT SIMULATIONBASE )
   set( poolcnv_files xAODJet/JetContainer.h xAODJet/JetAuxContainer.h xAODJet/JetTrigAuxContainer.h)
   set( poolcnv_types xAOD::JetContainer xAOD::JetAuxContainer xAOD::JetTrigAuxContainer)
else()
   set( poolcnv_files xAODJet/JetContainer.h xAODJet/JetAuxContainer.h)
   set( poolcnv_types xAOD::JetContainer xAOD::JetAuxContainer)
endif()

# Component(s) in the package:
atlas_add_poolcnv_library( xAODJetAthenaPoolPoolCnv
                           src/*.cxx
                           FILES ${poolcnv_files}
                           TYPES_WITH_NAMESPACE ${poolcnv_types}
                           CNV_PFX xAOD
                           INCLUDE_DIRS ${ROOT_INCLUDE_DIRS}
                           LINK_LIBRARIES ${ROOT_LIBRARIES} AthContainers AthenaKernel AthenaPoolCnvSvcLib AthenaPoolUtilities xAODJet )


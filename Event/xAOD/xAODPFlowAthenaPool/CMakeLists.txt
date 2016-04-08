################################################################################
# Package: xAODPFlowAthenaPool
################################################################################

# Declare the package name:
atlas_subdir( xAODPFlowAthenaPool )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PRIVATE
                          Control/AthContainers
                          Control/AthenaKernel
                          Database/AthenaPOOL/AthenaPoolCnvSvc
                          Database/AthenaPOOL/AthenaPoolUtilities
                          Event/xAOD/xAODPFlow )

# Component(s) in the package:
atlas_add_poolcnv_library( xAODPFlowAthenaPoolPoolCnv
                           src/*.cxx
                           FILES xAODPFlow/PFOContainer.h xAODPFlow/PFOAuxContainer.h
                           TYPES_WITH_NAMESPACE xAOD::PFOContainer xAOD::PFOAuxContainer
                           CNV_PFX xAOD
                           LINK_LIBRARIES AthContainers AthenaKernel AthenaPoolCnvSvcLib AthenaPoolUtilities xAODPFlow )


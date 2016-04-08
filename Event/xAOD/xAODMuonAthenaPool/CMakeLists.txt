################################################################################
# Package: xAODMuonAthenaPool
################################################################################

# Declare the package name:
atlas_subdir( xAODMuonAthenaPool )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PRIVATE
                          Control/AthContainers
                          Control/AthenaKernel
                          Database/AthenaPOOL/AthenaPoolCnvSvc
                          Database/AthenaPOOL/AthenaPoolUtilities
                          Event/xAOD/xAODMuon
                          Event/xAOD/xAODTracking )

# External dependencies:
find_package( ROOT COMPONENTS Core Tree MathCore Hist RIO pthread )

# Component(s) in the package:
atlas_add_poolcnv_library( xAODMuonAthenaPoolPoolCnv
                           src/*.cxx
                           FILES xAODMuon/MuonContainer.h xAODMuon/MuonAuxContainer.h xAODMuon/SlowMuonContainer.h xAODMuon/SlowMuonAuxContainer.h xAODMuon/MuonSegmentContainer.h xAODMuon/MuonSegmentAuxContainer.h
                           TYPES_WITH_NAMESPACE xAOD::MuonContainer xAOD::MuonAuxContainer xAOD::SlowMuonContainer xAOD::SlowMuonAuxContainer xAOD::MuonSegmentContainer xAOD::MuonSegmentAuxContainer
                           CNV_PFX xAOD
                           INCLUDE_DIRS ${ROOT_INCLUDE_DIRS}
                           LINK_LIBRARIES ${ROOT_LIBRARIES} AthContainers AthenaKernel AthenaPoolCnvSvcLib AthenaPoolUtilities xAODMuon xAODTracking )


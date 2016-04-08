################################################################################
# Package: xAODBTaggingAthenaPool
################################################################################

# Declare the package name:
atlas_subdir( xAODBTaggingAthenaPool )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PRIVATE
                          Control/AthContainers
                          Control/AthenaKernel
                          Database/AthenaPOOL/AthenaPoolCnvSvc
                          Database/AthenaPOOL/AthenaPoolUtilities
                          Event/xAOD/xAODBTagging
                          Event/xAOD/xAODTracking
                          GaudiKernel )

# External dependencies:
find_package( ROOT COMPONENTS Core Tree MathCore Hist RIO pthread )

# Component(s) in the package:
atlas_add_poolcnv_library( xAODBTaggingAthenaPoolPoolCnv
                           src/*.cxx
                           FILES xAODBTagging/BTaggingContainer.h xAODBTagging/BTaggingAuxContainer.h xAODBTagging/BTaggingTrigAuxContainer.h xAODBTagging/BTagVertexContainer.h xAODBTagging/BTagVertexAuxContainer.h
                           TYPES_WITH_NAMESPACE xAOD::BTaggingContainer xAOD::BTaggingAuxContainer xAOD::BTaggingTrigAuxContainer xAOD::BTagVertexContainer xAOD::BTagVertexAuxContainer
                           CNV_PFX xAOD
                           INCLUDE_DIRS ${ROOT_INCLUDE_DIRS}
                           LINK_LIBRARIES ${ROOT_LIBRARIES} AthContainers AthenaKernel AthenaPoolCnvSvcLib AthenaPoolUtilities xAODBTagging xAODTracking GaudiKernel )


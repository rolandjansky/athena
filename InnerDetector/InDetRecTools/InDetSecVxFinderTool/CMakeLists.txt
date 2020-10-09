################################################################################
# Package: InDetSecVxFinderTool
################################################################################

# Declare the package name:
atlas_subdir( InDetSecVxFinderTool )


# External dependencies:
find_package( CLHEP )
find_package( Eigen )
find_package( ROOT COMPONENTS Core Tree MathCore Hist RIO pthread )

# Component(s) in the package:
atlas_add_library( InDetSecVxFinderToolLib
                   src/*.cxx
                   PUBLIC_HEADERS InDetSecVxFinderTool
                   INCLUDE_DIRS ${CLHEP_INCLUDE_DIRS} ${EIGEN_INCLUDE_DIRS}
                   PRIVATE_INCLUDE_DIRS ${ROOT_INCLUDE_DIRS}
                   DEFINITIONS ${CLHEP_DEFINITIONS}
                   LINK_LIBRARIES ${CLHEP_LIBRARIES} ${EIGEN_LIBRARIES} AthenaBaseComps GeoPrimitives GaudiKernel InDetRecToolInterfaces TrkParameters TrkParticleBase TrkJetVxFitterLib TrkVertexFittersLib TrkVxEdmCnvLib
                   PRIVATE_LINK_LIBRARIES ${ROOT_LIBRARIES} EventPrimitives xAODTracking TrkSurfaces TrkEventPrimitives TrkLinks TrkNeutralParameters TrkTrack VxJetVertex VxSecVertex VxVertex TrkExInterfaces TrkToolInterfaces TrkVertexFitterInterfaces TrkVertexSeedFinderUtilsLib TrkVertexSeedFinderToolsLib )

atlas_add_component( InDetSecVxFinderTool
                     src/components/*.cxx
                     INCLUDE_DIRS ${ROOT_INCLUDE_DIRS} ${CLHEP_INCLUDE_DIRS} ${EIGEN_INCLUDE_DIRS}
                     LINK_LIBRARIES ${ROOT_LIBRARIES} ${CLHEP_LIBRARIES} ${EIGEN_LIBRARIES} AthenaBaseComps GeoPrimitives GaudiKernel InDetRecToolInterfaces TrkParameters TrkParticleBase EventPrimitives xAODTracking TrkSurfaces TrkEventPrimitives TrkLinks TrkNeutralParameters TrkTrack VxJetVertex VxSecVertex VxVertex TrkExInterfaces TrkToolInterfaces TrkJetVxFitterLib TrkVertexFitterInterfaces TrkVertexFittersLib TrkVxEdmCnvLib InDetSecVxFinderToolLib TrkVertexSeedFinderToolsLib )


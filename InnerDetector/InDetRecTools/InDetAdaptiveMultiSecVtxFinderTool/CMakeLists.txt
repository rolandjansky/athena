################################################################################
# Package: InDetAdaptiveMultiSecVtxFinderTool
################################################################################

# Declare the package name:
atlas_subdir( InDetAdaptiveMultiSecVtxFinderTool )

# External dependencies:
find_package( CLHEP )
find_package( ROOT COMPONENTS Core Tree MathCore Hist RIO pthread )

# Component(s) in the package:
atlas_add_component( InDetAdaptiveMultiSecVtxFinderTool
                     src/*.cxx
                     Root/InDetAdaptiveMultiSecVtxFinderTool.h
                     src/components/*.cxx
		             INCLUDE_DIRS ${ROOT_INCLUDE_DIRS} ${CLHEP_INCLUDE_DIRS}
                     LINK_LIBRARIES ${ROOT_LIBRARIES} ${CLHEP_LIBRARIES} AthenaBaseComps AthContainers CxxUtils xAODTracking GaudiKernel InDetRecToolInterfaces GeoPrimitives TrkTrack TrkParticleBase TrkParameters TrkEventPrimitives VxVertex VxMultiVertex TrkLinks TrkTrack TrkSurfaces TrkVertexFitterInterfaces TrkVxEdmCnvLib TrkVertexAnalysisUtilsLib EventPrimitives TrkVertexFittersLib InDetTrackSelectionToolLib TrkVertexSeedFinderUtilsLib )

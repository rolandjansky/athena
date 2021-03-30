# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

# Declare the package name:
atlas_subdir( InDetIncSecVxFinderTool )

# External dependencies:
find_package( CLHEP )
find_package( ROOT COMPONENTS Core Tree MathCore Hist RIO pthread )

# Component(s) in the package:
atlas_add_component( InDetIncSecVxFinderTool
                     src/*.cxx
                     src/components/*.cxx
                     INCLUDE_DIRS ${ROOT_INCLUDE_DIRS} ${CLHEP_INCLUDE_DIRS}
                     LINK_LIBRARIES ${ROOT_LIBRARIES} ${CLHEP_LIBRARIES} AthenaBaseComps AthContainers CxxUtils xAODTracking GaudiKernel InDetRecToolInterfaces GeoPrimitives TrkTrack TrkParticleBase TrkParameters TrkEventPrimitives VxVertex TrkLinks TrkTrack TrkTrackLink TrkTrackSummary TrkSurfaces TrkVertexFitterInterfaces EventPrimitives InDetTrackSelectionToolLib TrkVertexSeedFinderUtilsLib BeamSpotConditionsData )

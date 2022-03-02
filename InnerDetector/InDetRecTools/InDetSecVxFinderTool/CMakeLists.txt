# Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration

# Declare the package name:
atlas_subdir( InDetSecVxFinderTool )

# External dependencies:
find_package( CLHEP )
find_package( ROOT COMPONENTS Core MathCore )

# Component(s) in the package:
atlas_add_library( InDetSecVxFinderToolLib
                   src/*.cxx
                   PUBLIC_HEADERS InDetSecVxFinderTool
                   INCLUDE_DIRS ${CLHEP_INCLUDE_DIRS} ${ROOT_INCLUDE_DIRS}
                   DEFINITIONS ${CLHEP_DEFINITIONS}
                   LINK_LIBRARIES ${CLHEP_LIBRARIES} ${ROOT_LIBRARIES} AthenaBaseComps GaudiKernel GeoPrimitives InDetRecToolInterfaces TrkExInterfaces TrkJetVxFitterLib TrkLinks TrkParameters TrkParticleBase TrkToolInterfaces TrkTrackLink TrkVertexFitterInterfaces TrkVertexSeedFinderUtilsLib VxJetVertex VxVertex xAODTracking
                   PRIVATE_LINK_LIBRARIES EventPrimitives InDetVKalVxInJetToolLib TrkEventPrimitives TrkNeutralParameters TrkSurfaces TrkTrack TrkVertexFittersLib TrkVertexSeedFinderToolsLib VxSecVertex )

atlas_add_component( InDetSecVxFinderTool
                     src/components/*.cxx
                     LINK_LIBRARIES InDetSecVxFinderToolLib )

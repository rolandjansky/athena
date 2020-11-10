# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

# Declare the package name:
atlas_subdir( TrkAlignGenTools )

# External dependencies:
find_package( Boost COMPONENTS filesystem thread system )
find_package( CLHEP )
find_package( ROOT COMPONENTS Graf Gpad Core Tree MathCore Hist RIO pthread MathMore Minuit Minuit2 Matrix Physics HistPainter Rint Graf3d Html Postscript Gui GX11TTF GX11 )

# Component(s) in the package:
atlas_add_library( TrkAlignGenToolsLib
                   src/*.cxx
                   PUBLIC_HEADERS TrkAlignGenTools
                   INCLUDE_DIRS ${ROOT_INCLUDE_DIRS} ${CLHEP_INCLUDE_DIRS} ${EIGEN_INCLUDE_DIRS}
                   PRIVATE_INCLUDE_DIRS ${Boost_INCLUDE_DIRS}
                   DEFINITIONS ${CLHEP_DEFINITIONS}
                   LINK_LIBRARIES ${CLHEP_LIBRARIES} ${ROOT_LIBRARIES} AthContainers AthenaBaseComps BeamSpotConditionsData EventPrimitives GaudiKernel GeoPrimitives InDetAlignGenToolsLib InDetTrackSelectionToolLib MuonRecToolInterfaces TrkAlignEvent TrkAlignInterfaces TrkEventPrimitives TrkFitterUtils TrkVertexFitterInterfaces VxVertex muonEvent xAODTracking
                   PRIVATE_LINK_LIBRARIES ${Boost_LIBRARIES} AtlasDetDescr MuonSegment Particle TrkAlgebraUtils TrkCompetingRIOsOnTrack TrkDetElementBase TrkEventUtils TrkExInterfaces TrkFitterInterfaces TrkMaterialOnTrack TrkMeasurementBase TrkParameters TrkParticleBase TrkPseudoMeasurementOnTrack TrkRIO_OnTrack TrkSurfaces TrkToolInterfaces TrkTrack TrkVertexOnTrack xAODEventInfo )

atlas_add_component( TrkAlignGenTools
                     src/components/*.cxx
                     LINK_LIBRARIES TrkAlignGenToolsLib )

# Install files from the package:
atlas_install_python_modules( python/*.py POST_BUILD_CMD ${ATLAS_FLAKE8} )

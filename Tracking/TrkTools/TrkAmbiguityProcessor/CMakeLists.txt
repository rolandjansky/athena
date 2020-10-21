# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

# Declare the package name:
atlas_subdir( TrkAmbiguityProcessor )

# External dependencies:
find_package( CLHEP )
find_package( ROOT COMPONENTS Core )

# Component(s) in the package:
atlas_add_component( TrkAmbiguityProcessor
                     src/DenseEnvironmentsAmbiguityProcessorTool.cxx
                     src/DenseEnvironmentsAmbiguityScoreProcessorTool.cxx
                     src/AmbiguityProcessorUtility.cxx
                     src/AmbiguityProcessorBase.cxx
                     src/SimpleAmbiguityProcessorTool.cxx
                     src/TrackScoringTool.cxx
                     src/TrackSelectionProcessorTool.cxx
                     src/components/*.cxx
                     INCLUDE_DIRS ${CLHEP_INCLUDE_DIRS} ${ROOT_INCLUDE_DIRS}
                     LINK_LIBRARIES ${CLHEP_LIBRARIES} ${ROOT_LIBRARIES} AthContainers AthenaBaseComps GaudiKernel InDetIdentifier InDetRecToolInterfaces InDetPrepRawData TrkEventPrimitives TrkEventUtils TrkParameters TrkRIO_OnTrack TrkTrack TrkTrackSummary TrkFitterInterfaces TrkToolInterfaces TrkExInterfaces )

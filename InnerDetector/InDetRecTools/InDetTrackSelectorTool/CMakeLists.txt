# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

# Declare the package name:
atlas_subdir( InDetTrackSelectorTool )

# External dependencies:
find_package( CLHEP )

# Component(s) in the package:
atlas_add_component( InDetTrackSelectorTool
                     src/*.cxx
                     src/components/*.cxx
                     INCLUDE_DIRS ${CLHEP_INCLUDE_DIRS}
                     LINK_LIBRARIES ${CLHEP_LIBRARIES} AthenaBaseComps xAODTracking GaudiKernel InDetRecToolInterfaces TrkEventPrimitives TrkParameters TrkToolInterfaces AthContainers GeoPrimitives EventPrimitives InDetRIO_OnTrack StoreGateLib TrkSurfaces TrkParticleBase TrkRIO_OnTrack TrkTrack TrkTrackSummary VxVertex TrkExInterfaces TRT_ConditionsData MagFieldElements MagFieldConditions BeamSpotConditionsData xAODEventInfo )

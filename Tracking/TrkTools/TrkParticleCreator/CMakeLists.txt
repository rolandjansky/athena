# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

# Declare the package name:
atlas_subdir( TrkParticleCreator )

# Component(s) in the package:
atlas_add_library( TrkParticleCreatorLib
   TrkParticleCreator/*.h
   INTERFACE
   PUBLIC_HEADERS TrkParticleCreator
   LINK_LIBRARIES ITrackToVertex TrkToolInterfaces AthenaBaseComps GaudiKernel TrkParticleBase
   TrkParameters TrkTrackSummary TrkTrack TrkEventPrimitives MagFieldConditions MagFieldElements
   MuonRecToolInterfaces TrkExInterfaces TrkTrackSummary xAODTracking AthContainers
   EventPrimitives InDetIdentifier PixelGeoModelLib )

atlas_add_component( TrkParticleCreator
   src/*.cxx src/components/*.cxx
   LINK_LIBRARIES AthContainers AthenaBaseComps EventPrimitives xAODTracking
   GaudiKernel InDetIdentifier MuonRecToolInterfaces TrkEventPrimitives
   TrkParameters TrkParticleBase TrkTrack TrkTrackSummary TrkToolInterfaces
   AthContainers AtlasDetDescr GeoPrimitives IdDictDetDescr InDetPrepRawData
   InDetRIO_OnTrack Particle ITrackToVertex TrkSurfaces
   TrkPseudoMeasurementOnTrack VxVertex TrkExInterfaces PixelGeoModelLib
   TrkParticleCreatorLib MagFieldElements MagFieldConditions BeamSpotConditionsData )

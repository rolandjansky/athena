# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

# Declare the package name:
atlas_subdir( VP1TrackSystems )

# External dependencies:
find_package( CLHEP )
find_package( Coin3D )
find_package( Qt5 COMPONENTS Core Gui Widgets  )
find_package( GeoModelCore )

# Generate UI files automatically:
# Note: add the "Widgets" component to "find_package( Qt5 ...)" if you have UI files, otherwise UIC, even if CMAKE_AUTOUIC is set to ON, is not run
set( CMAKE_AUTOUIC TRUE )
# Generate MOC files automatically:
set( CMAKE_AUTOMOC TRUE )

# Component(s) in the package:
atlas_add_library( VP1TrackSystems VP1TrackSystems/*.h src/*.cxx
   PUBLIC_HEADERS VP1TrackSystems
   PRIVATE_INCLUDE_DIRS ${COIN3D_INCLUDE_DIRS} ${CLHEP_INCLUDE_DIRS} ${CMAKE_CURRENT_BINARY_DIR}
   LINK_LIBRARIES ${GEOMODELCORE_LIBRARIES} AtlasHepMCLib GaudiKernel GeneratorObjects GeoPrimitives TrackRecordLib TrkDetDescrInterfaces TrkEventPrimitives TrkParameters TrkSurfaces VP1Base VP1Utils xAODTracking Qt5::Core Qt5::Gui
   PRIVATE_LINK_LIBRARIES ${CLHEP_LIBRARIES} ${COIN3D_LIBRARIES} AthContainers AtlasDetDescr EventPrimitives ForwardRegion_SimEv InDetIdentifier InDetPrepRawData InDetRIO_OnTrack InDetReadoutGeometry InDetSimEvent MuonChamberT0s MuonIdHelpersLib MuonPrepRawData MuonRIO_OnTrack MuonReadoutGeometry MuonRecHelperToolsLib MuonSegment MuonSimEvent Particle PixelReadoutGeometry SCT_ReadoutGeometry StoreGateLib TRT_ReadoutGeometry TrkCompetingRIOsOnTrack TrkDetElementBase TrkExInterfaces TrkFitterInterfaces TrkMaterialOnTrack TrkMeasurementBase TrkPrepRawData TrkPseudoMeasurementOnTrack TrkRIO_OnTrack TrkSegment TrkTrack TrkTrackSummary TrkVolumes VP1GuideLineSystems VP1HEPVis VP1PRDSystems )

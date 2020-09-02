# $Id: CMakeLists.txt 728682 2016-03-09 15:17:26Z krasznaa $
################################################################################
# Package: VP1TrackSystems
################################################################################

# Declare the package name:
atlas_subdir( VP1TrackSystems )

# Declare the package's dependencies:
atlas_depends_on_subdirs(
   PUBLIC
   DetectorDescription/GeoPrimitives
   DetectorDescription/Identifier
   Event/xAOD/xAODTracking
   GaudiKernel
   Generators/GeneratorObjects
   Simulation/G4Sim/TrackRecord
   Tracking/TrkDetDescr/TrkDetDescrInterfaces
   Tracking/TrkDetDescr/TrkSurfaces
   Tracking/TrkEvent/TrkEventPrimitives
   Tracking/TrkEvent/TrkParameters
   graphics/VP1/VP1Base
   graphics/VP1/VP1Utils
   PRIVATE
   Control/AthContainers
   Control/StoreGate
   DetectorDescription/AtlasDetDescr
   Event/EventPrimitives
   ForwardDetectors/ForwardSimulation/ForwardRegion_SimEv
   InnerDetector/InDetDetDescr/InDetIdentifier
   InnerDetector/InDetDetDescr/InDetReadoutGeometry
   InnerDetector/InDetDetDescr/PixelReadoutGeometry
   InnerDetector/InDetDetDescr/SCT_ReadoutGeometry
   InnerDetector/InDetDetDescr/TRT_ReadoutGeometry
   InnerDetector/InDetRecEvent/InDetPrepRawData
   InnerDetector/InDetRecEvent/InDetRIO_OnTrack
   InnerDetector/InDetSimEvent
   MuonSpectrometer/MuonDetDescr/MuonReadoutGeometry
   MuonSpectrometer/MuonIdHelpers
   MuonSpectrometer/MuonReconstruction/MuonRecEvent/MuonChamberT0s
   MuonSpectrometer/MuonReconstruction/MuonRecEvent/MuonPrepRawData
   MuonSpectrometer/MuonReconstruction/MuonRecEvent/MuonRIO_OnTrack
   MuonSpectrometer/MuonReconstruction/MuonRecEvent/MuonSegment
   MuonSpectrometer/MuonReconstruction/MuonRecTools/MuonRecHelperTools
   MuonSpectrometer/MuonSimEvent
   Reconstruction/Particle
   Tracking/TrkDetDescr/TrkDetDescrUtils
   Tracking/TrkDetDescr/TrkDetElementBase
   Tracking/TrkDetDescr/TrkVolumes
   Tracking/TrkEvent/TrkCompetingRIOsOnTrack
   Tracking/TrkEvent/TrkMaterialOnTrack
   Tracking/TrkEvent/TrkMeasurementBase
   Tracking/TrkEvent/TrkPrepRawData
   Tracking/TrkEvent/TrkPseudoMeasurementOnTrack
   Tracking/TrkEvent/TrkRIO_OnTrack
   Tracking/TrkEvent/TrkSegment
   Tracking/TrkEvent/TrkTrack
   Tracking/TrkEvent/TrkTrackSummary
   Tracking/TrkExtrapolation/TrkExInterfaces
   Tracking/TrkFitter/TrkFitterInterfaces
   graphics/VP1/VP1HEPVis
   graphics/VP1/VP1Systems/VP1GuideLineSystems
   graphics/VP1/VP1Systems/VP1PRDSystems 
   Generators/AtlasHepMC
   )

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
   PRIVATE_INCLUDE_DIRS ${COIN3D_INCLUDE_DIRS} ${CLHEP_INCLUDE_DIRS}
    ${CMAKE_CURRENT_BINARY_DIR}
   LINK_LIBRARIES ${GEOMODELCORE_LIBRARIES} GeoPrimitives Identifier xAODTracking
   GaudiKernel GeneratorObjects TrkDetDescrInterfaces TrkSurfaces
   TrkEventPrimitives TrkParameters VP1Base VP1Utils StoreGateLib SGtests
   MuonIdHelpersLib MuonRecHelperToolsLib Qt5::Core Qt5::Gui
   PRIVATE_LINK_LIBRARIES ${COIN3D_LIBRARIES} ${CLHEP_LIBRARIES}
   AtlasHepMCLib AthContainers AtlasDetDescr EventPrimitives
   ForwardRegion_SimEv InDetIdentifier InDetReadoutGeometry PixelReadoutGeometry SCT_ReadoutGeometry TRT_ReadoutGeometry InDetPrepRawData
   InDetRIO_OnTrack InDetSimEvent MuonReadoutGeometry MuonChamberT0s
   MuonPrepRawData MuonRIO_OnTrack MuonSegment MuonSimEvent Particle
   TrkDetDescrUtils TrkDetElementBase TrkVolumes TrkCompetingRIOsOnTrack
   TrkMaterialOnTrack TrkMeasurementBase TrkPrepRawData
   TrkPseudoMeasurementOnTrack TrkRIO_OnTrack TrkSegment TrkTrack
   TrkTrackSummary TrkExInterfaces TrkFitterInterfaces VP1HEPVis
   VP1GuideLineSystems VP1PRDSystems )

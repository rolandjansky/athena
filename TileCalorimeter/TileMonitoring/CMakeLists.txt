################################################################################
# Package: TileMonitoring
################################################################################

# Declare the package name:
atlas_subdir( TileMonitoring )

# Declare the package's dependencies:
atlas_depends_on_subdirs(
  PUBLIC
  Control/AthToolSupport/AsgTools
  Control/AthenaMonitoring
  Event/xAOD/xAODJet
  GaudiKernel
  Reconstruction/Jet/JetInterface
  TileCalorimeter/TileIdentifier
  PRIVATE
  Calorimeter/CaloEvent
  Calorimeter/CaloGeoHelpers
  Calorimeter/CaloIdentifier
  Control/AthenaKernel
  Event/ByteStreamCnvSvcBase
  Event/xAOD/xAODCaloEvent
  Event/xAOD/xAODEventInfo
  Event/xAOD/xAODTrigger
  Event/xAOD/xAODMuon
  PhysicsAnalysis/JetMissingEtID/JetSelectorTools
  Reconstruction/Jet/JetMomentTools
  Reconstruction/Jet/JetUtils
  TileCalorimeter/TileCalib/TileCalibBlobObjs
  TileCalorimeter/TileConditions
  TileCalorimeter/TileEvent
  TileCalorimeter/TileRecUtils
  Tools/LWHists
  Trigger/TrigAnalysis/TrigDecisionTool
  Trigger/TrigConfiguration/TrigConfInterfaces
  Trigger/TrigConfiguration/TrigConfL1Data
  Trigger/TrigT1/TrigT1Result )

# External dependencies:
find_package( CLHEP )
find_package( ROOT COMPONENTS Graf Gpad Hist Spectrum Core Tree MathCore
  RIO pthread Graf3d Html Postscript Gui GX11TTF GX11 HistPainter )

# Component(s) in the package:
atlas_add_library( TileMonitoringLib
   TileMonitoring/I*.h
   INTERFACE
   PUBLIC_HEADERS TileMonitoring
   LINK_LIBRARIES GaudiKernel )

atlas_add_component( TileMonitoring
  TileMonitoring/*.h src/*.cxx src/components/*.cxx
  INCLUDE_DIRS ${ROOT_INCLUDE_DIRS} ${CLHEP_INCLUDE_DIRS}
  LINK_LIBRARIES ${ROOT_LIBRARIES} ${CLHEP_LIBRARIES} AsgTools
  AthenaMonitoringLib xAODJet GaudiKernel JetInterface TileIdentifier
  CaloEvent CaloGeoHelpers CaloIdentifier AthenaKernel
  ByteStreamCnvSvcBaseLib xAODCaloEvent xAODEventInfo xAODTrigger xAODMuon JetUtils
  TileCalibBlobObjs TileConditionsLib TileEvent TileRecUtilsLib LWHists
  TrigDecisionToolLib TrigConfL1Data TrigT1Result TileMonitoringLib )

# Install files from the package:
atlas_install_joboptions( share/*.py )

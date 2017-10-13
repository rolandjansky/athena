################################################################################
# Package: MuonCalibExtraTreeAlg
################################################################################

# Declare the package name:
atlas_subdir( MuonCalibExtraTreeAlg )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PUBLIC
                          Control/AthenaBaseComps
                          Control/StoreGate
                          GaudiKernel
                          MuonSpectrometer/MuonReconstruction/MuonRecEvent/MuonPattern
                          Tracking/TrkEvent/TrkTrack
                          PRIVATE
                          Calorimeter/CaloEvent
                          Calorimeter/CaloIdentifier
                          Control/CxxUtils
                          Control/AthContainers
                          Event/EventPrimitives
                          Event/xAOD/xAODMuon
                          Event/xAOD/xAODTracking
                          MuonSpectrometer/MuonCalib/MuonCalibEvent
                          MuonSpectrometer/MuonCalib/MuonCalibEventBase
                          MuonSpectrometer/MuonCalib/MuonCalibExtraTree/MuonCalibExtraTreeEvent
                          MuonSpectrometer/MuonCalib/MuonCalibITools
                          MuonSpectrometer/MuonCalib/MuonCalibIdentifier
                          MuonSpectrometer/MuonCalib/MuonCalibNtuple
                          MuonSpectrometer/MuonCalib/MuonCalibUtils/MuonCalibStl
                          MuonSpectrometer/MuonDetDescr/MuonReadoutGeometry
                          MuonSpectrometer/MuonIdHelpers
                          MuonSpectrometer/MuonReconstruction/MuonRecEvent/MuonRIO_OnTrack
                          MuonSpectrometer/MuonReconstruction/MuonRecEvent/MuonSegment
                          MuonSpectrometer/MuonReconstruction/MuonRecTools/MuonRecHelperTools
                          TileCalorimeter/TileEvent
                          Tracking/TrkDetDescr/TrkDetElementBase
                          Tracking/TrkDetDescr/TrkGeometry
                          Tracking/TrkDetDescr/TrkSurfaces
                          Tracking/TrkEvent/TrkCompetingRIOsOnTrack
                          Tracking/TrkEvent/TrkEventPrimitives
                          Tracking/TrkEvent/TrkMaterialOnTrack
                          Tracking/TrkEvent/TrkMeasurementBase
                          Tracking/TrkEvent/TrkParameters
                          Tracking/TrkEvent/TrkPrepRawData
                          Tracking/TrkEvent/TrkPseudoMeasurementOnTrack
                          Tracking/TrkEvent/TrkRIO_OnTrack
                          Tracking/TrkExtrapolation/TrkExInterfaces
                          Tracking/TrkTools/TrkToolInterfaces
                          Trigger/TrigConfiguration/TrigConfL1Data
                          Trigger/TrigT1/TrigT1Interfaces
                          Trigger/TrigT1/TrigT1Result )

# External dependencies:
find_package( CLHEP )
find_package( ROOT COMPONENTS Core Tree MathCore Hist RIO pthread )

# Component(s) in the package:
atlas_add_library( MuonCalibExtraTreeAlgLib
                   src/*.cxx
                   PUBLIC_HEADERS MuonCalibExtraTreeAlg
                   INCLUDE_DIRS ${ROOT_INCLUDE_DIRS}
                   PRIVATE_INCLUDE_DIRS ${CLHEP_INCLUDE_DIRS}
                   PRIVATE_DEFINITIONS ${CLHEP_DEFINITIONS}
                   LINK_LIBRARIES ${ROOT_LIBRARIES} AthenaBaseComps GaudiKernel MuonPattern TrkTrack StoreGateLib SGtests MuonIdHelpersLib MuonRecHelperToolsLib
                   PRIVATE_LINK_LIBRARIES ${CLHEP_LIBRARIES} CaloEvent CaloIdentifier CxxUtils AthContainers EventPrimitives xAODMuon xAODTracking MuonCalibEvent MuonCalibEventBase MuonCalibExtraTreeEvent MuonCalibITools MuonCalibIdentifier MuonCalibNtuple MuonReadoutGeometry MuonRIO_OnTrack MuonSegment TileEvent TrkDetElementBase TrkGeometry TrkSurfaces TrkCompetingRIOsOnTrack TrkEventPrimitives TrkMaterialOnTrack TrkMeasurementBase TrkParameters TrkPrepRawData TrkPseudoMeasurementOnTrack TrkRIO_OnTrack TrkExInterfaces TrkToolInterfaces TrigConfL1Data TrigT1Interfaces TrigT1Result )

atlas_add_component( MuonCalibExtraTreeAlg
                     src/components/*.cxx
                     INCLUDE_DIRS ${ROOT_INCLUDE_DIRS} ${CLHEP_INCLUDE_DIRS}
                     LINK_LIBRARIES ${ROOT_LIBRARIES} ${CLHEP_LIBRARIES} AthenaBaseComps StoreGateLib SGtests GaudiKernel MuonPattern TrkTrack CaloEvent CaloIdentifier CxxUtils AthContainers EventPrimitives xAODMuon xAODTracking MuonCalibEvent MuonCalibEventBase MuonCalibExtraTreeEvent MuonCalibITools MuonCalibIdentifier MuonCalibNtuple MuonReadoutGeometry MuonIdHelpersLib MuonRIO_OnTrack MuonSegment MuonRecHelperToolsLib TileEvent TrkDetElementBase TrkGeometry TrkSurfaces TrkCompetingRIOsOnTrack TrkEventPrimitives TrkMaterialOnTrack TrkMeasurementBase TrkParameters TrkPrepRawData TrkPseudoMeasurementOnTrack TrkRIO_OnTrack TrkExInterfaces TrkToolInterfaces TrigConfL1Data TrigT1Interfaces TrigT1Result MuonCalibExtraTreeAlgLib )

# Install files from the package:
atlas_install_joboptions( share/*.py )


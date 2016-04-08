################################################################################
# Package: MuonAlignGenTools
################################################################################

# Declare the package name:
atlas_subdir( MuonAlignGenTools )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PUBLIC
                          Control/AthenaBaseComps
                          Control/DataModel
                          DetectorDescription/Identifier
                          GaudiKernel
                          MuonSpectrometer/MuonGMdbObjects
                          MuonSpectrometer/MuonReconstruction/MuonRecTools/MuonRecToolInterfaces
                          Tracking/TrkAlignment/TrkAlignEvent
                          Tracking/TrkAlignment/TrkAlignGenTools
                          Tracking/TrkAlignment/TrkAlignInterfaces
                          Tracking/TrkDetDescr/TrkDetDescrInterfaces
                          Tracking/TrkEvent/TrkSegment
                          Tracking/TrkEvent/TrkTrack
                          PRIVATE
                          Control/StoreGate
                          Database/AthenaPOOL/RDBAccessSvc
                          DetectorDescription/AtlasDetDescr
                          DetectorDescription/GeoModel/GeoModelInterfaces
                          DetectorDescription/GeoPrimitives
                          Event/EventPrimitives
                          Event/xAOD/xAODEventInfo
                          Generators/GeneratorObjects
                          MuonSpectrometer/MuonAlignment/MuonAlignEvent
                          MuonSpectrometer/MuonCalib/MuonCalibITools
                          MuonSpectrometer/MuonCalib/MuonCalibIdentifier
                          MuonSpectrometer/MuonCalib/MuonCalibStandAlone/MuonCalibStandAloneBase
                          MuonSpectrometer/MuonConditions/MuonCondGeneral/MuonCondInterface
                          MuonSpectrometer/MuonDetDescr/MuonReadoutGeometry
                          MuonSpectrometer/MuonIdHelpers
                          MuonSpectrometer/MuonReconstruction/MuonRecEvent/MuonCompetingRIOsOnTrack
                          MuonSpectrometer/MuonReconstruction/MuonRecEvent/MuonPrepRawData
                          MuonSpectrometer/MuonReconstruction/MuonRecEvent/MuonRIO_OnTrack
                          MuonSpectrometer/MuonReconstruction/MuonRecEvent/MuonSegment
                          MuonSpectrometer/MuonReconstruction/MuonRecTools/MuonRecHelperTools
                          MuonSpectrometer/MuonReconstruction/MuonRecUtils/MuonLinearSegmentMakerUtilities
                          MuonSpectrometer/MuonReconstruction/MuonSegmentMakers/MuonSegmentMakerUtils
                          Reconstruction/MuonIdentification/MuidInterfaces
                          Reconstruction/MuonIdentification/MuonCombinedToolInterfaces
                          Reconstruction/MuonIdentification/muonEvent
                          Reconstruction/Particle
                          Simulation/G4Sim/TrackRecord
                          Tracking/TrkDetDescr/TrkGeometry
                          Tracking/TrkDetDescr/TrkSurfaces
                          Tracking/TrkDetDescr/TrkVolumes
                          Tracking/TrkEvent/TrkCompetingRIOsOnTrack
                          Tracking/TrkEvent/TrkEventPrimitives
                          Tracking/TrkEvent/TrkEventUtils
                          Tracking/TrkEvent/TrkMaterialOnTrack
                          Tracking/TrkEvent/TrkMeasurementBase
                          Tracking/TrkEvent/TrkParameters
                          Tracking/TrkEvent/TrkPrepRawData
                          Tracking/TrkEvent/TrkRIO_OnTrack
                          Tracking/TrkEvent/TrkTrackSummary
                          Tracking/TrkExtrapolation/TrkExInterfaces
                          Tracking/TrkFitter/TrkFitterInterfaces
                          Tracking/TrkTools/TrkToolInterfaces
                          Trigger/TrigEvent/TrigDecisionInterface )

# External dependencies:
find_package( Boost COMPONENTS filesystem thread system )
find_package( CLHEP )
find_package( CORAL COMPONENTS CoralBase CoralKernel RelationalAccess )
find_package( Eigen )
find_package( HepMC )
find_package( ROOT COMPONENTS Core Tree MathCore Hist RIO pthread )

# Component(s) in the package:
atlas_add_component( MuonAlignGenTools
                     src/*.cxx
                     src/components/*.cxx
                     INCLUDE_DIRS ${Boost_INCLUDE_DIRS} ${ROOT_INCLUDE_DIRS} ${CORAL_INCLUDE_DIRS} ${CLHEP_INCLUDE_DIRS} ${HEPMC_INCLUDE_DIRS} ${EIGEN_INCLUDE_DIRS}
                     LINK_LIBRARIES ${Boost_LIBRARIES} ${ROOT_LIBRARIES} ${CORAL_LIBRARIES} ${CLHEP_LIBRARIES} ${HEPMC_LIBRARIES} ${EIGEN_LIBRARIES} AthenaBaseComps DataModel Identifier GaudiKernel MuonGMdbObjects MuonRecToolInterfaces TrkAlignEvent TrkAlignGenToolsLib TrkDetDescrInterfaces TrkSegment TrkTrack StoreGateLib SGtests AtlasDetDescr GeoPrimitives EventPrimitives xAODEventInfo GeneratorObjects MuonAlignEvent MuonCalibITools MuonCalibIdentifier MuonCalibStandAloneBaseLib MuonCondInterface MuonReadoutGeometry MuonIdHelpersLib MuonCompetingRIOsOnTrack MuonPrepRawData MuonRIO_OnTrack MuonSegment MuonRecHelperToolsLib MuonLinearSegmentMakerUtilities MuonSegmentMakerUtils MuidInterfaces MuonCombinedToolInterfaces muonEvent Particle TrkGeometry TrkSurfaces TrkVolumes TrkCompetingRIOsOnTrack TrkEventPrimitives TrkEventUtils TrkMaterialOnTrack TrkMeasurementBase TrkParameters TrkPrepRawData TrkRIO_OnTrack TrkTrackSummary TrkExInterfaces TrkFitterInterfaces TrkToolInterfaces )

# Install files from the package:
atlas_install_headers( MuonAlignGenTools )
atlas_install_python_modules( python/*.py )
atlas_install_joboptions( share/*.py )


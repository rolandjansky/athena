################################################################################
# Package: MdtVsTgcRawDataMonitoring
################################################################################

# Declare the package name:
atlas_subdir( MdtVsTgcRawDataMonitoring )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PUBLIC
                          Control/AthenaMonitoring
                          Control/StoreGate
                          Event/xAOD/xAODMuon
                          GaudiKernel
                          MuonSpectrometer/MuonDetDescr/MuonReadoutGeometry
                          MuonSpectrometer/MuonReconstruction/MuonRecEvent/MuonPrepRawData
                          MuonSpectrometer/MuonReconstruction/MuonRecEvent/MuonSegment
                          MuonSpectrometer/MuonReconstruction/MuonRecEvent/MuonTrigCoinData
                          MuonSpectrometer/MuonValidation/MuonDQA/MuonDQAUtils
                          Reconstruction/MuonIdentification/muonEvent
                          Tracking/TrkEvent/TrkSegment
                          Tracking/TrkEvent/TrkTrack
                          PRIVATE
                          DetectorDescription/GeoPrimitives
                          DetectorDescription/Identifier
                          Event/EventPrimitives
                          MuonSpectrometer/MuonCalib/MuonCalibIdentifier
                          MuonSpectrometer/MuonRDO
                          MuonSpectrometer/MuonReconstruction/MuonRecEvent/MuonCompetingRIOsOnTrack
                          MuonSpectrometer/MuonReconstruction/MuonRecEvent/MuonRIO_OnTrack
                          Tracking/TrkDetDescr/TrkSurfaces
                          Tracking/TrkEvent/TrkEventPrimitives
                          Tracking/TrkEvent/TrkRIO_OnTrack )

# External dependencies:
find_package( Eigen )
find_package( ROOT COMPONENTS Graf Gpad Core Tree MathCore Hist RIO pthread Graf3d Html Postscript Gui GX11TTF GX11 )

# Component(s) in the package:
atlas_add_component( MdtVsTgcRawDataMonitoring
                     src/*.cxx
                     src/components/*.cxx
                     src/functions/*.cxx
                     INCLUDE_DIRS ${ROOT_INCLUDE_DIRS} ${EIGEN_INCLUDE_DIRS}
                     LINK_LIBRARIES ${ROOT_LIBRARIES} ${EIGEN_LIBRARIES} AthenaMonitoringLib StoreGateLib SGtests xAODMuon GaudiKernel MuonReadoutGeometry MuonPrepRawData MuonSegment MuonTrigCoinData MuonDQAUtilsLib muonEvent TrkSegment TrkTrack GeoPrimitives Identifier EventPrimitives MuonCalibIdentifier MuonRDO MuonCompetingRIOsOnTrack MuonRIO_OnTrack TrkSurfaces TrkEventPrimitives TrkRIO_OnTrack )

# Install files from the package:
atlas_install_headers( MdtVsTgcRawDataMonitoring )
atlas_install_joboptions( share/*.py )


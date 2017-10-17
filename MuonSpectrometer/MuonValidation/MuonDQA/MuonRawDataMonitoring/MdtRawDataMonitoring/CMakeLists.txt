################################################################################
# Package: MdtRawDataMonitoring
################################################################################

# Declare the package name:
atlas_subdir( MdtRawDataMonitoring )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PUBLIC
                          Control/AthenaBaseComps
                          Control/AthenaMonitoring
                          Event/EventInfo
                          MuonSpectrometer/MuonIdHelpers
                          MuonSpectrometer/MuonReconstruction/MuonRecEvent/MuonPrepRawData
                          MuonSpectrometer/MuonValidation/MuonDQA/MuonDQAUtils
                          Tracking/TrkEvent/TrkSegment
                          PRIVATE
                          DetectorDescription/Identifier
                          Event/xAOD/xAODEventInfo
                          Event/xAOD/xAODTrigger
                          GaudiKernel
                          MuonSpectrometer/MuonCalib/MdtCalib/MdtCalibFitters
                          MuonSpectrometer/MuonCalib/MuonCalibIdentifier
                          MuonSpectrometer/MuonDetDescr/MuonReadoutGeometry
                          MuonSpectrometer/MuonReconstruction/MuonRecEvent/MuonRIO_OnTrack
                          MuonSpectrometer/MuonReconstruction/MuonRecEvent/MuonSegment
			  PhysicsAnalysis/MuonID/MuonSelectorTools
                          PhysicsAnalysis/AnalysisTrigger/AnalysisTriggerEvent
                          Tools/LWHists
                          Tracking/TrkEvent/TrkEventPrimitives )

# External dependencies:
find_package( ROOT COMPONENTS Graf Core Tree MathCore Hist RIO pthread Graf3d Gpad Html Postscript Gui GX11TTF GX11 )

# Component(s) in the package:
atlas_add_component( MdtRawDataMonitoring
                     src/*.cxx
                     src/components/*.cxx
                     INCLUDE_DIRS ${ROOT_INCLUDE_DIRS}
                     LINK_LIBRARIES ${ROOT_LIBRARIES} AthenaBaseComps AthenaMonitoringLib EventInfo MuonIdHelpersLib MuonPrepRawData MuonDQAUtilsLib TrkSegment Identifier xAODEventInfo xAODTrigger GaudiKernel MdtCalibFitters MuonCalibIdentifier MuonReadoutGeometry MuonRIO_OnTrack MuonSegment AnalysisTriggerEvent LWHists TrkEventPrimitives )

# Install files from the package:
atlas_install_headers( MdtRawDataMonitoring )
atlas_install_joboptions( share/*.py )


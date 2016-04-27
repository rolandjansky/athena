################################################################################
# Package: MuonCombinedEvaluationTools
################################################################################

# Declare the package name:
atlas_subdir( MuonCombinedEvaluationTools )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PRIVATE
                          Control/AthLinks
                          Control/AthenaBaseComps
                          Event/xAOD/xAODEventInfo
                          Event/xAOD/xAODMuon
                          Event/xAOD/xAODTracking
                          Event/xAOD/xAODTruth
                          GaudiKernel
                          MuonSpectrometer/MuonCalib/MuonCalibEventBase
                          MuonSpectrometer/MuonCalib/MuonCalibITools
                          MuonSpectrometer/MuonIdHelpers
                          MuonSpectrometer/MuonReconstruction/MuonRecEvent/MuonPrepRawData
                          MuonSpectrometer/MuonReconstruction/MuonRecEvent/MuonRIO_OnTrack
                          MuonSpectrometer/MuonReconstruction/MuonRecEvent/MuonSegment
                          MuonSpectrometer/MuonReconstruction/MuonRecTools/MuonRecHelperTools
                          Reconstruction/MuonIdentification/MuonCombinedToolInterfaces
                          Tracking/TrkDetDescr/TrkDetDescrInterfaces
                          Tracking/TrkDetDescr/TrkGeometry
                          Tracking/TrkEvent/TrkEventPrimitives
                          Tracking/TrkEvent/TrkMaterialOnTrack
                          Tracking/TrkEvent/TrkMeasurementBase
                          Tracking/TrkEvent/TrkTrack
                          Tracking/TrkFitter/TrkFitterInterfaces )

# Component(s) in the package:
atlas_add_component( MuonCombinedEvaluationTools
                     src/*.cxx
                     src/components/*.cxx
                     LINK_LIBRARIES AthLinks AthenaBaseComps xAODEventInfo xAODMuon xAODTracking xAODTruth GaudiKernel MuonCalibEventBase MuonCalibITools MuonIdHelpersLib MuonPrepRawData MuonRIO_OnTrack MuonSegment MuonRecHelperToolsLib MuonCombinedToolInterfaces TrkDetDescrInterfaces TrkGeometry TrkEventPrimitives TrkMaterialOnTrack TrkMeasurementBase TrkTrack TrkFitterInterfaces )


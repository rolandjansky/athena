################################################################################
# Package: MdtSegmentT0Fitter
################################################################################

# Declare the package name:
atlas_subdir( MdtSegmentT0Fitter )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PUBLIC
                          Control/AthenaBaseComps
                          GaudiKernel
                          MuonSpectrometer/MuonCalib/MdtCalib/MdtCalibSvc
                          MuonSpectrometer/MuonReconstruction/MuonSegmentMakers/MuonSegmentMakerInterfaces
                          Tracking/TrkUtilityPackages/TrkDriftCircleMath
                          PRIVATE
                          MuonSpectrometer/MuonCalib/MdtCalib/MdtCalibData
                          MuonSpectrometer/MuonCalib/MuonCalibTools
                          MuonSpectrometer/MuonDetDescr/MuonReadoutGeometry
                          MuonSpectrometer/MuonIdHelpers
                          MuonSpectrometer/MuonReconstruction/MuonRecEvent/MuonPrepRawData
                          MuonSpectrometer/MuonReconstruction/MuonRecEvent/MuonRIO_OnTrack )

# External dependencies:
find_package( ROOT COMPONENTS Minuit Core Tree MathCore Hist RIO pthread MathMore Minuit2 Matrix Physics HistPainter Rint )

# Component(s) in the package:
atlas_add_component( MdtSegmentT0Fitter
                     src/*.cxx
                     src/components/*.cxx
                     INCLUDE_DIRS ${ROOT_INCLUDE_DIRS}
                     LINK_LIBRARIES ${ROOT_LIBRARIES} AthenaBaseComps GaudiKernel MdtCalibSvcLib TrkDriftCircleMath MdtCalibData MuonCalibToolsLib MuonReadoutGeometry MuonIdHelpersLib MuonPrepRawData MuonRIO_OnTrack )

# Install files from the package:
atlas_install_headers( MdtSegmentT0Fitter )


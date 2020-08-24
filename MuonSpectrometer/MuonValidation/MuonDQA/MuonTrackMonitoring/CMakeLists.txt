################################################################################
# Package: MuonTrackMonitoring
################################################################################

# Declare the package name:
atlas_subdir( MuonTrackMonitoring )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PUBLIC
                          Control/AthenaMonitoring
                          Control/StoreGate
                          DetectorDescription/GeoPrimitives
                          Event/EventPrimitives
                          Event/FourMomUtils
                          Event/xAOD/xAODEventInfo
                          Event/xAOD/xAODMuon
                          Event/xAOD/xAODTracking
                          GaudiKernel
                          MuonSpectrometer/MuonIdHelpers
                          MuonSpectrometer/MuonReconstruction/MuonRecEvent/MuonPrepRawData
                          MuonSpectrometer/MuonReconstruction/MuonRecTools/MuonRecHelperTools
                          MuonSpectrometer/MuonReconstruction/MuonRecTools/MuonRecToolInterfaces
                          MuonSpectrometer/MuonValidation/MuonHistogramming/MuonHistUtils
                          PhysicsAnalysis/MuonID/MuonPerformanceAnalysis/MuonResonanceTools
                          PhysicsAnalysis/MuonID/MuonSelectorTools
                          Tracking/TrkEvent/TrkEventPrimitives
                          Tracking/TrkEvent/TrkParameters
                          Tracking/TrkEvent/TrkTrack
                          Tracking/TrkTools/TrkToolInterfaces
                          Tracking/TrkValidation/TrkValHistUtils
                          Trigger/TrigConfiguration/TrigConfL1Data
                          PRIVATE
                          Tracking/TrkEvent/TrkMeasurementBase
                          Tracking/TrkEvent/TrkMultiComponentStateOnSurface )

# External dependencies:
find_package( Eigen )
find_package( ROOT COMPONENTS Core Tree MathCore Hist RIO pthread Graf Graf3d Gpad Html Postscript Gui GX11TTF GX11 )

# Component(s) in the package:
atlas_add_component( MuonTrackMonitoring
                     src/*.cxx
                     src/components/*.cxx
                     INCLUDE_DIRS ${ROOT_INCLUDE_DIRS} ${EIGEN_INCLUDE_DIRS}
                     LINK_LIBRARIES ${ROOT_LIBRARIES} ${EIGEN_LIBRARIES} AthenaMonitoringLib StoreGateLib SGtests GeoPrimitives EventPrimitives FourMomUtils xAODEventInfo xAODMuon xAODTracking GaudiKernel MuonIdHelpersLib MuonPrepRawData MuonRecHelperToolsLib MuonRecToolInterfaces MuonHistUtils MuonResonanceToolsLib MuonSelectorToolsLib TrkEventPrimitives TrkParameters TrkTrack TrkToolInterfaces TrkValHistUtils TrigConfL1Data TrkMeasurementBase TrkMultiComponentStateOnSurface )

# Install files from the package:
atlas_install_python_modules( python/*.py )


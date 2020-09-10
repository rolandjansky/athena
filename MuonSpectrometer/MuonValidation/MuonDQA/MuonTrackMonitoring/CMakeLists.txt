################################################################################
# Package: MuonTrackMonitoring
################################################################################

# Declare the package name:
atlas_subdir( MuonTrackMonitoring )

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


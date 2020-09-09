################################################################################
# Package: MuonCalibMonitoring
################################################################################

# Declare the package name:
atlas_subdir( MuonCalibMonitoring )

# External dependencies:
find_package( ROOT COMPONENTS Core Tree MathCore Hist RIO pthread Graf Graf3d Gpad Html Postscript Gui GX11TTF GX11 )

# Component(s) in the package:
atlas_add_component( MuonCalibMonitoring
                     src/*.cxx
                     src/components/*.cxx
                     INCLUDE_DIRS ${ROOT_INCLUDE_DIRS}
                     LINK_LIBRARIES ${ROOT_LIBRARIES} AthenaMonitoringLib StoreGateLib SGtests GaudiKernel CscCalibData MuonCondData MuonCondInterface MuonReadoutGeometry MuonIdHelpersLib )

################################################################################
# Package: MuonCalibMonitoring
################################################################################

# Declare the package name:
atlas_subdir( MuonCalibMonitoring )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PRIVATE
                          Control/AthenaMonitoring
                          Control/StoreGate
                          GaudiKernel
                          MuonSpectrometer/MuonCalib/CscCalib/CscCalibData
                          MuonSpectrometer/MuonConditions/MuonCondGeneral/MuonCondData
                          MuonSpectrometer/MuonConditions/MuonCondGeneral/MuonCondInterface
                          MuonSpectrometer/MuonDetDescr/MuonReadoutGeometry
                          MuonSpectrometer/MuonIdHelpers )

# External dependencies:
find_package( ROOT COMPONENTS Core Tree MathCore Hist RIO pthread Graf Graf3d Gpad Html Postscript Gui GX11TTF GX11 )

# Component(s) in the package:
atlas_add_component( MuonCalibMonitoring
                     src/*.cxx
                     src/components/*.cxx
                     INCLUDE_DIRS ${ROOT_INCLUDE_DIRS}
                     LINK_LIBRARIES ${ROOT_LIBRARIES} AthenaMonitoringLib StoreGateLib SGtests GaudiKernel CscCalibData MuonCondData MuonCondInterface MuonReadoutGeometry MuonIdHelpersLib )

################################################################################
# Package: MdtCalibValidation
################################################################################

# Declare the package name:
atlas_subdir( MdtCalibValidation )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PUBLIC
                          Control/AthenaBaseComps
                          Control/StoreGate
                          GaudiKernel
                          MuonSpectrometer/MuonCalib/MdtCalib/MdtCalibData
                          PRIVATE
                          MuonSpectrometer/MuonCalib/MuonCalibDbOperations
                          MuonSpectrometer/MuonCalib/MuonCalibStandAlone/MuonCalibStandAloneBase
                          MuonSpectrometer/MuonCalib/MuonCalibUtils/MuonCalibMath
                          MuonSpectrometer/MuonCalib/MuonCalibUtils/MuonCalibStl
                          MuonSpectrometer/MuonDetDescr/MuonReadoutGeometry
                          MuonSpectrometer/MuonIdHelpers )

# External dependencies:
find_package( ROOT COMPONENTS Core Tree MathCore Hist RIO pthread Graf Graf3d Gpad Html Postscript Gui GX11TTF GX11 )

# tag NEEDS_CORAL_BASE was not recognized in automatic conversion in cmt2cmake

# Component(s) in the package:
atlas_add_library( MdtCalibValidationLib
                   src/*.cxx
                   PUBLIC_HEADERS MdtCalibValidation
                   INCLUDE_DIRS ${ROOT_INCLUDE_DIRS}
                   LINK_LIBRARIES ${ROOT_LIBRARIES} AthenaBaseComps GaudiKernel MdtCalibData StoreGateLib SGtests MuonCalibDbOperationsLib MuonCalibStandAloneBaseLib MuonIdHelpersLib
                   PRIVATE_LINK_LIBRARIES MuonCalibMath MuonReadoutGeometry )

atlas_add_component( MdtCalibValidation
                     src/components/*.cxx
                     INCLUDE_DIRS ${ROOT_INCLUDE_DIRS}
                     LINK_LIBRARIES ${ROOT_LIBRARIES} AthenaBaseComps StoreGateLib SGtests GaudiKernel MdtCalibData MuonCalibDbOperationsLib MuonCalibStandAloneBaseLib MuonCalibMath MuonReadoutGeometry MuonIdHelpersLib MdtCalibValidationLib )


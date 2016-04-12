################################################################################
# Package: MuonCalibStandAloneExtraTools
################################################################################

# Declare the package name:
atlas_subdir( MuonCalibStandAloneExtraTools )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PUBLIC
                          Control/AthenaBaseComps
                          GaudiKernel
                          MuonSpectrometer/MuonCalib/MdtCalib/MdtCalibData
                          MuonSpectrometer/MuonCalib/MdtCalib/MdtCalibFitters
                          MuonSpectrometer/MuonCalib/MdtCalib/MdtCalibUtils
                          MuonSpectrometer/MuonCalib/MuonCalibEventBase
                          MuonSpectrometer/MuonCalib/MuonCalibExtraTree/MuonCalibExtraTreeEvent
                          MuonSpectrometer/MuonCalib/MuonCalibIdentifier
                          MuonSpectrometer/MuonCalib/MuonCalibStandAlone/MdtCalibIOSvc
                          MuonSpectrometer/MuonCalib/MuonCalibStandAlone/MuonCalibStandAloneBase
                          PRIVATE
                          Control/StoreGate
                          DetectorDescription/Identifier
                          MuonSpectrometer/MuonCalib/MdtCalib/MdtCalibInterfaces
                          MuonSpectrometer/MuonCalib/MuonCalibExtraTree/MuonCalibExtraNtuple
                          MuonSpectrometer/MuonCalib/MuonCalibExtraTree/MuonCalibExtraTreeAlg
                          MuonSpectrometer/MuonCalib/MuonCalibITools
                          MuonSpectrometer/MuonCalib/MuonCalibNtuple
                          MuonSpectrometer/MuonCalib/MuonCalibStandAlone/CalibNtupleUtils
                          MuonSpectrometer/MuonDetDescr/MuonReadoutGeometry
                          MuonSpectrometer/MuonIdHelpers )

# External dependencies:
find_package( ROOT COMPONENTS Core Tree MathCore Hist RIO pthread MathMore Minuit Minuit2 Matrix Physics HistPainter Rint Graf Graf3d Gpad Html Postscript Gui GX11TTF GX11 )

# Component(s) in the package:
atlas_add_library( MuonCalibStandAloneExtraToolsLib
                   src/*.cxx
                   PUBLIC_HEADERS MuonCalibStandAloneExtraTools
                   INCLUDE_DIRS ${ROOT_INCLUDE_DIRS}
                   LINK_LIBRARIES ${ROOT_LIBRARIES} AthenaBaseComps GaudiKernel MdtCalibData MdtCalibFitters MdtCalibUtils MuonCalibEventBase MuonCalibExtraTreeEvent MuonCalibIdentifier MdtCalibIOSvcLib MuonCalibStandAloneBaseLib StoreGateLib SGtests MuonCalibExtraTreeAlgLib MuonIdHelpersLib
                   PRIVATE_LINK_LIBRARIES Identifier MuonCalibExtraNtuple MuonCalibITools MuonCalibNtuple CalibNtupleUtils MuonReadoutGeometry )

atlas_add_component( MuonCalibStandAloneExtraTools
                     src/components/*.cxx
                     INCLUDE_DIRS ${ROOT_INCLUDE_DIRS}
                     LINK_LIBRARIES ${ROOT_LIBRARIES} AthenaBaseComps GaudiKernel MdtCalibData MdtCalibFitters MdtCalibUtils MuonCalibEventBase MuonCalibExtraTreeEvent MuonCalibIdentifier MdtCalibIOSvcLib MuonCalibStandAloneBaseLib StoreGateLib SGtests Identifier MuonCalibExtraNtuple MuonCalibExtraTreeAlgLib MuonCalibITools MuonCalibNtuple CalibNtupleUtils MuonReadoutGeometry MuonIdHelpersLib MuonCalibStandAloneExtraToolsLib )

# Install files from the package:
atlas_install_python_modules( python/*.py )


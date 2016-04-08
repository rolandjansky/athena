################################################################################
# Package: MuonCalibStandAloneTools
################################################################################

# Declare the package name:
atlas_subdir( MuonCalibStandAloneTools )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PUBLIC
                          Control/AthenaBaseComps
                          MuonSpectrometer/MuonCalib/MdtCalib/MdtCalibFitters
                          MuonSpectrometer/MuonCalib/MdtCalib/MdtCalibInterfaces
                          MuonSpectrometer/MuonCalib/MdtCalib/MdtCalibT0
                          MuonSpectrometer/MuonCalib/MdtCalib/MdtCalibUtils
                          MuonSpectrometer/MuonCalib/MuonCalibEventBase
                          MuonSpectrometer/MuonCalib/MuonCalibITools
                          MuonSpectrometer/MuonCalib/MuonCalibIdentifier
                          MuonSpectrometer/MuonCalib/MuonCalibStandAlone/MdtCalibIOSvc
                          MuonSpectrometer/MuonCalib/MuonCalibStandAlone/MuonCalibStandAloneBase
                          MuonSpectrometer/MuonIdHelpers
                          PRIVATE
                          DetectorDescription/Identifier
                          GaudiKernel
                          MuonSpectrometer/MuonCalib/MdtCalib/MdtCalibData
                          MuonSpectrometer/MuonCalib/MdtCalib/MdtCalibRt
                          MuonSpectrometer/MuonCalib/MuonCalibUtils/MuonCalibStl
                          MuonSpectrometer/MuonDetDescr/MuonReadoutGeometry )

# External dependencies:
find_package( CLHEP )
find_package( ROOT COMPONENTS Core Tree MathCore Hist RIO pthread MathMore Minuit Minuit2 Matrix Physics HistPainter Rint Graf Graf3d Gpad Html Postscript Gui GX11TTF GX11 )

# Component(s) in the package:
atlas_add_library( MuonCalibStandAloneToolsLib
                   src/*.cxx
                   PUBLIC_HEADERS MuonCalibStandAloneTools
                   INCLUDE_DIRS ${ROOT_INCLUDE_DIRS} ${CLHEP_INCLUDE_DIRS}
                   DEFINITIONS ${CLHEP_DEFINITIONS}
                   LINK_LIBRARIES ${ROOT_LIBRARIES} ${CLHEP_LIBRARIES} AthenaBaseComps MdtCalibFitters MdtCalibT0 MdtCalibUtils MuonCalibEventBase MuonCalibITools MuonCalibIdentifier MdtCalibIOSvcLib MuonCalibStandAloneBaseLib MuonIdHelpersLib
                   PRIVATE_LINK_LIBRARIES Identifier GaudiKernel MdtCalibData MdtCalibRt MuonReadoutGeometry )

atlas_add_component( MuonCalibStandAloneTools
                     src/components/*.cxx
                     INCLUDE_DIRS ${ROOT_INCLUDE_DIRS} ${CLHEP_INCLUDE_DIRS}
                     LINK_LIBRARIES ${ROOT_LIBRARIES} ${CLHEP_LIBRARIES} AthenaBaseComps MdtCalibFitters MdtCalibT0 MdtCalibUtils MuonCalibEventBase MuonCalibITools MuonCalibIdentifier MdtCalibIOSvcLib MuonCalibStandAloneBaseLib MuonIdHelpersLib Identifier GaudiKernel MdtCalibData MdtCalibRt MuonReadoutGeometry MuonCalibStandAloneToolsLib )

# Install files from the package:
atlas_install_python_modules( python/*.py )
atlas_install_joboptions( share/*.py )


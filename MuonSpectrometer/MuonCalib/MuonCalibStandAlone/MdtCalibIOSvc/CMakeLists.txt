################################################################################
# Package: MdtCalibIOSvc
################################################################################

# Declare the package name:
atlas_subdir( MdtCalibIOSvc )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PUBLIC
                          Control/AthenaBaseComps
                          GaudiKernel
                          MuonSpectrometer/MuonCalib/MdtCalib/MdtCalibUtils
                          MuonSpectrometer/MuonCalib/MuonCalibStandAlone/MuonCalibStandAloneBase
                          PRIVATE
                          Control/StoreGate
                          DetectorDescription/Identifier
                          MuonSpectrometer/MuonCalib/MdtCalib/MdtCalibData
                          MuonSpectrometer/MuonCalib/MdtCalib/MdtCalibInterfaces
                          MuonSpectrometer/MuonCalib/MdtCalib/MdtCalibRt
                          MuonSpectrometer/MuonCalib/MdtCalib/MdtCalibT0
                          MuonSpectrometer/MuonCalib/MuonCalibIdentifier
                          MuonSpectrometer/MuonCalib/MuonCalibUtils/MuonCalibMath
                          MuonSpectrometer/MuonDetDescr/MuonReadoutGeometry
                          MuonSpectrometer/MuonIdHelpers )

# Component(s) in the package:
atlas_add_library( MdtCalibIOSvcLib
                   src/*.cxx
                   PUBLIC_HEADERS MdtCalibIOSvc
                   LINK_LIBRARIES AthenaBaseComps GaudiKernel MdtCalibUtils MuonCalibStandAloneBaseLib StoreGateLib SGtests MuonIdHelpersLib
                   PRIVATE_LINK_LIBRARIES Identifier MdtCalibData MdtCalibRt MdtCalibT0 MuonCalibIdentifier MuonCalibMath MuonReadoutGeometry )

atlas_add_component( MdtCalibIOSvc
                     src/components/*.cxx
                     LINK_LIBRARIES AthenaBaseComps GaudiKernel MdtCalibUtils MuonCalibStandAloneBaseLib StoreGateLib SGtests Identifier MdtCalibData MdtCalibRt MdtCalibT0 MuonCalibIdentifier MuonCalibMath MuonReadoutGeometry MuonIdHelpersLib MdtCalibIOSvcLib )

# Install files from the package:
atlas_install_joboptions( share/*.py )


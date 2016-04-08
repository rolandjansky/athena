################################################################################
# Package: MdtCalibTools
################################################################################

# Declare the package name:
atlas_subdir( MdtCalibTools )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PUBLIC
                          Control/AthenaBaseComps
                          GaudiKernel
                          MuonSpectrometer/MuonCalib/MdtCalib/MdtCalibITools
                          MuonSpectrometer/MuonCalib/MdtCalib/MdtCalibInterfaces
                          MuonSpectrometer/MuonCalib/MuonCalibITools
                          PRIVATE
                          Control/StoreGate
                          MuonSpectrometer/MuonCalib/MdtCalib/MdtCalibEventLoop
                          MuonSpectrometer/MuonCalib/MdtCalib/MdtCalibFitters
                          MuonSpectrometer/MuonCalib/MdtCalib/MdtCalibUtils
                          MuonSpectrometer/MuonCalib/MuonCalibEventBase
                          MuonSpectrometer/MuonCalib/MuonCalibNtuple
                          MuonSpectrometer/MuonCalib/MuonCalibUtils/MuonCalibStl
                          MuonSpectrometer/MuonDetDescr/MuonReadoutGeometry
                          MuonSpectrometer/MuonIdHelpers )

# Component(s) in the package:
atlas_add_component( MdtCalibTools
                     src/*.cxx
                     src/components/*.cxx
                     LINK_LIBRARIES AthenaBaseComps GaudiKernel MuonCalibITools StoreGateLib SGtests MdtCalibEventLoop MdtCalibFitters MdtCalibUtils MuonCalibEventBase MuonCalibNtuple MuonReadoutGeometry MuonIdHelpersLib )

# Install files from the package:
atlas_install_headers( MdtCalibTools )
atlas_install_joboptions( share/*.py )


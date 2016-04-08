################################################################################
# Package: MdtCalibDbAsciiTool
################################################################################

# Declare the package name:
atlas_subdir( MdtCalibDbAsciiTool )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PUBLIC
                          Control/AthenaBaseComps
                          GaudiKernel
                          MuonSpectrometer/MuonCalib/MdtCalib/MdtCalibData
                          MuonSpectrometer/MuonCalib/MdtCalib/MdtCalibInterfaces
                          PRIVATE
                          Control/AthenaKernel
                          Control/SGTools
                          Control/StoreGate
                          DetectorDescription/Identifier
                          MuonSpectrometer/MuonCalib/MdtCalib/MdtCalibSvc
                          MuonSpectrometer/MuonCalib/MdtCalib/MdtCalibUtils
                          MuonSpectrometer/MuonCalib/MuonCalibUtils/MuonCalibStl
                          MuonSpectrometer/MuonDetDescr/MuonReadoutGeometry
                          MuonSpectrometer/MuonIdHelpers
                          Simulation/Tools/AtlasCLHEP_RandomGenerators
                          Tools/PathResolver )

# External dependencies:
find_package( CLHEP )

# Component(s) in the package:
atlas_add_component( MdtCalibDbAsciiTool
                     src/*.cxx
                     src/components/*.cxx
                     INCLUDE_DIRS ${CLHEP_INCLUDE_DIRS}
                     LINK_LIBRARIES ${CLHEP_LIBRARIES} AthenaBaseComps GaudiKernel MdtCalibData AthenaKernel SGTools StoreGateLib SGtests Identifier MdtCalibSvcLib MdtCalibUtils MuonReadoutGeometry MuonIdHelpersLib AtlasCLHEP_RandomGenerators PathResolver )

# Install files from the package:
atlas_install_headers( MdtCalibDbAsciiTool )
atlas_install_joboptions( share/*.py )
atlas_install_runtime( share/DC2_rt_asciiDb.dat )


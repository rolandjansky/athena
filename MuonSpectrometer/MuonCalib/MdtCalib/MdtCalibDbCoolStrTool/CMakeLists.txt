################################################################################
# Package: MdtCalibDbCoolStrTool
################################################################################

# Declare the package name:
atlas_subdir( MdtCalibDbCoolStrTool )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PUBLIC
                          Control/AthenaBaseComps
                          GaudiKernel
                          MuonSpectrometer/MuonCalib/MdtCalib/MdtCalibData
                          MuonSpectrometer/MuonCalib/MdtCalib/MdtCalibInterfaces
                          MuonSpectrometer/MuonCalib/MuonCalibITools
                          PRIVATE
                          Control/AthenaKernel
                          Control/SGTools
                          Control/StoreGate
                          Database/AthenaPOOL/AthenaPoolUtilities
                          DetectorDescription/Identifier
                          MuonSpectrometer/MuonCalib/MdtCalib/MdtCalibSvc
                          MuonSpectrometer/MuonCalib/MdtCalib/MdtCalibUtils
                          MuonSpectrometer/MuonCalib/MuonCalibIdentifier
                          MuonSpectrometer/MuonCalib/MuonCalibTools
                          MuonSpectrometer/MuonCalib/MuonCalibUtils/MuonCalibMath
                          MuonSpectrometer/MuonCalib/MuonCalibUtils/MuonCalibStl
                          MuonSpectrometer/MuonDetDescr/MuonReadoutGeometry
                          MuonSpectrometer/MuonIdHelpers
                          Simulation/Tools/AtlasCLHEP_RandomGenerators
                          Tools/PathResolver )

# External dependencies:
find_package( CLHEP )
find_package( CORAL COMPONENTS CoralBase CoralKernel RelationalAccess )
find_package( ROOT COMPONENTS Core Tree MathCore Hist RIO pthread )

# Component(s) in the package:
atlas_add_component( MdtCalibDbCoolStrTool
                     src/*.cxx
                     src/components/*.cxx
                     INCLUDE_DIRS ${ROOT_INCLUDE_DIRS} ${CORAL_INCLUDE_DIRS} ${CLHEP_INCLUDE_DIRS}
                     LINK_LIBRARIES ${ROOT_LIBRARIES} ${CORAL_LIBRARIES} ${CLHEP_LIBRARIES} z AthenaBaseComps GaudiKernel MdtCalibData MuonCalibITools AthenaKernel SGTools StoreGateLib SGtests AthenaPoolUtilities Identifier MdtCalibSvcLib MdtCalibUtils MuonCalibIdentifier MuonCalibToolsLib MuonCalibMath MuonReadoutGeometry MuonIdHelpersLib AtlasCLHEP_RandomGenerators PathResolver z )

# Install files from the package:
atlas_install_headers( MdtCalibDbCoolStrTool )
atlas_install_joboptions( share/*.py )
atlas_install_runtime( share/DC2_rt_default.dat share/MdtCalibDbCoolStrTool_RT_default_comm.dat )


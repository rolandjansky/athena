################################################################################
# Package: MuonCalibDbOperations
################################################################################

# Declare the package name:
atlas_subdir( MuonCalibDbOperations )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PUBLIC
                          Control/AthenaBaseComps
                          GaudiKernel
                          MuonSpectrometer/MuonCalib/MdtCalib/MdtCalibUtils
                          MuonSpectrometer/MuonCalib/MuonCalibStandAlone/MuonCalibStandAloneBase
                          PRIVATE
                          Control/AthenaKernel
                          Control/StoreGate
                          MuonSpectrometer/MuonCalib/MdtCalib/MdtCalibData
                          MuonSpectrometer/MuonCalib/MdtCalib/MdtCalibRt
                          MuonSpectrometer/MuonCalib/MuonCalibIdentifier
                          MuonSpectrometer/MuonCalib/MuonCalibUtils/MuonCalibMath
                          MuonSpectrometer/MuonDetDescr/MuonReadoutGeometry
                          MuonSpectrometer/MuonIdHelpers )

# External dependencies:
find_package( COOL COMPONENTS CoolKernel CoolApplication )
find_package( CORAL COMPONENTS CoralBase CoralKernel RelationalAccess )
find_package( ROOT COMPONENTS Core Tree MathCore Hist RIO pthread )
find_package( cx_Oracle )

# Component(s) in the package:
atlas_add_library( MuonCalibDbOperationsLib
                   src/*.cxx
                   PUBLIC_HEADERS MuonCalibDbOperations
                   PRIVATE_INCLUDE_DIRS ${COOL_INCLUDE_DIRS} ${ROOT_INCLUDE_DIRS} ${CORAL_INCLUDE_DIRS}
                   LINK_LIBRARIES AthenaBaseComps GaudiKernel MdtCalibUtils MuonCalibStandAloneBaseLib StoreGateLib SGtests MuonIdHelpersLib z
                   PRIVATE_LINK_LIBRARIES ${COOL_LIBRARIES} ${ROOT_LIBRARIES} ${CORAL_LIBRARIES} AthenaKernel MdtCalibData MdtCalibRt MuonCalibIdentifier MuonCalibMath MuonReadoutGeometry )

atlas_add_component( MuonCalibDbOperations
                     src/components/*.cxx
                     INCLUDE_DIRS ${COOL_INCLUDE_DIRS} ${ROOT_INCLUDE_DIRS} ${CORAL_INCLUDE_DIRS}
                     LINK_LIBRARIES ${COOL_LIBRARIES} ${ROOT_LIBRARIES} ${CORAL_LIBRARIES} AthenaBaseComps GaudiKernel MdtCalibUtils MuonCalibStandAloneBaseLib AthenaKernel StoreGateLib SGtests MdtCalibData MdtCalibRt MuonCalibIdentifier MuonCalibMath MuonReadoutGeometry MuonIdHelpersLib MuonCalibDbOperationsLib z )

# Install files from the package:
atlas_install_python_modules( python/*.py )
atlas_install_joboptions( share/*.py )
atlas_install_scripts( share/CalibDbCompareRT.py share/CalibDbCompareT0.py share/CalibDbDump.py share/CalibCoolCompareT0.py share/CalibCoolCompareRT.py share/CalibDbCool2Calib.py share/calibdata.py )


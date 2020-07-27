################################################################################
# Package: MuonCondAlg
################################################################################

# Declare the package name:
atlas_subdir( MuonCondAlg )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PUBLIC
                          Control/AthenaBaseComps
                          Control/AthenaKernel
                          Control/StoreGate
                          Database/AthenaPOOL/AthenaPoolUtilities
			  DetectorDescription/GeoModel/GeoModelInterfaces
                          DetectorDescription/Identifier
                          GaudiKernel
                          MuonSpectrometer/MuonAlignment/MuonAlignmentData
                          MuonSpectrometer/MuonConditions/MuonCondGeneral/MuonCondData
                          MuonSpectrometer/MuonConditions/MuonCondGeneral/MuonCondInterface
                          MuonSpectrometer/MuonConditions/MuonCondGeneral/MuonCondSvc
			  MuonSpectrometer/MuonCalib/MdtCalib/MdtCalibData
			  MuonSpectrometer/MuonCalib/MdtCalib/MdtCalibInterfaces
			  MuonSpectrometer/MuonCalib/MuonCalibITools
			  MuonSpectrometer/MuonCalib/MdtCalib/MdtCalibSvc
			  MuonSpectrometer/MuonCalib/MdtCalib/MdtCalibUtils
			  MuonSpectrometer/MuonCalib/MuonCalibIdentifier
			  MuonSpectrometer/MuonCalib/MuonCalibTools
			  MuonSpectrometer/MuonCalib/MuonCalibUtils/MuonCalibMath
			  MuonSpectrometer/MuonCalib/MuonCalibUtils/MuonCalibStl
                          PRIVATE
                          Event/EventInfo
                          MuonSpectrometer/MuonDetDescr/MuonReadoutGeometry
                          MuonSpectrometer/MuonIdHelpers 
			  Tools/PathResolver )

# External dependencies:
find_package( CORAL COMPONENTS CoralBase CoralKernel RelationalAccess )
find_package( ROOT COMPONENTS Core Tree MathCore Hist RIO pthread )

# Component(s) in the package:
atlas_add_library( MuonCondAlgLib
                   src/*.cxx
                   PUBLIC_HEADERS MuonCondAlg
                   INCLUDE_DIRS ${CORAL_INCLUDE_DIRS}
                   PRIVATE_INCLUDE_DIRS ${ROOT_INCLUDE_DIRS}
                   LINK_LIBRARIES ${CORAL_LIBRARIES} AthenaBaseComps AthenaKernel AthenaPoolUtilities Identifier GaudiKernel MuonCondData MuonCondInterface MuonCondSvcLib StoreGateLib MuonIdHelpersLib MuonReadoutGeometry MdtCalibSvcLib MdtCalibData MuonCalibITools MdtCalibUtils MuonCalibToolsLib PathResolver z 
                   PRIVATE_LINK_LIBRARIES ${ROOT_LIBRARIES} EventInfo )

atlas_add_component( MuonCondAlg
                     src/components/*.cxx
                     LINK_LIBRARIES GaudiKernel MuonCondAlgLib )

# Install files from the package:
atlas_install_python_modules( python/*.py )

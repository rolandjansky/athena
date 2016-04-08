################################################################################
# Package: CalibNtupleAnalysisAlg
################################################################################

# Declare the package name:
atlas_subdir( CalibNtupleAnalysisAlg )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PUBLIC
                          Control/AthenaBaseComps
                          DataQuality/GoodRunsLists
                          DetectorDescription/GeoPrimitives
                          GaudiKernel
                          MagneticField/MagFieldInterfaces
                          MuonSpectrometer/MuonCalib/MdtCalib/MdtCalibData
                          MuonSpectrometer/MuonCalib/MuonCalibStandAlone/MdtCalibIOSvc
                          MuonSpectrometer/MuonCalib/MuonCalibStandAlone/MuonCalibStandAloneBase
                          MuonSpectrometer/MuonIdHelpers
                          PRIVATE
                          Control/StoreGate
                          DetectorDescription/GeoModel/GeoModelInterfaces
                          Event/EventInfo
                          MuonSpectrometer/MuonCalib/MdtCalib/MdtCalibFitters
                          MuonSpectrometer/MuonCalib/MdtCalib/MdtCalibRt
                          MuonSpectrometer/MuonCalib/MuonCalibEventBase
                          MuonSpectrometer/MuonCalib/MuonCalibITools
                          MuonSpectrometer/MuonCalib/MuonCalibIdentifier
                          MuonSpectrometer/MuonCalib/MuonCalibStandAlone/CalibNtupleReader
                          MuonSpectrometer/MuonDetDescr/MuonReadoutGeometry )

# External dependencies:
find_package( CLHEP )
find_package( CORAL COMPONENTS CoralBase CoralKernel RelationalAccess )
find_package( Eigen )
find_package( ROOT COMPONENTS Core Tree MathCore Hist RIO pthread )

# Component(s) in the package:
atlas_add_library( CalibNtupleAnalysisAlgLib
                   src/*.cxx
                   PUBLIC_HEADERS CalibNtupleAnalysisAlg
                   INCLUDE_DIRS ${CLHEP_INCLUDE_DIRS} ${EIGEN_INCLUDE_DIRS}
                   PRIVATE_INCLUDE_DIRS ${ROOT_INCLUDE_DIRS} ${CORAL_INCLUDE_DIRS}
                   DEFINITIONS ${CLHEP_DEFINITIONS}
                   LINK_LIBRARIES ${CLHEP_LIBRARIES} ${EIGEN_LIBRARIES} AthenaBaseComps GeoPrimitives GaudiKernel MagFieldInterfaces MdtCalibData GoodRunsListsLib MdtCalibIOSvcLib MuonCalibStandAloneBaseLib MuonIdHelpersLib StoreGateLib SGtests
                   PRIVATE_LINK_LIBRARIES ${ROOT_LIBRARIES} ${CORAL_LIBRARIES} EventInfo MdtCalibFitters MdtCalibRt MuonCalibEventBase MuonCalibITools MuonCalibIdentifier CalibNtupleReader MuonReadoutGeometry )

atlas_add_component( CalibNtupleAnalysisAlg
                     src/components/*.cxx
                     INCLUDE_DIRS ${ROOT_INCLUDE_DIRS} ${CORAL_INCLUDE_DIRS} ${CLHEP_INCLUDE_DIRS} ${EIGEN_INCLUDE_DIRS}
                     LINK_LIBRARIES ${ROOT_LIBRARIES} ${CORAL_LIBRARIES} ${CLHEP_LIBRARIES} ${EIGEN_LIBRARIES} AthenaBaseComps GoodRunsListsLib GeoPrimitives GaudiKernel MagFieldInterfaces MdtCalibData MdtCalibIOSvcLib MuonCalibStandAloneBaseLib MuonIdHelpersLib StoreGateLib SGtests EventInfo MdtCalibFitters MdtCalibRt MuonCalibEventBase MuonCalibITools MuonCalibIdentifier CalibNtupleReader MuonReadoutGeometry CalibNtupleAnalysisAlgLib )

atlas_add_dictionary( CalibNtupleAnalysisAlgDict
                      CalibNtupleAnalysisAlg/GeoDraw.h
                      CalibNtupleAnalysisAlg/selection.xml
                      INCLUDE_DIRS ${ROOT_INCLUDE_DIRS} ${CORAL_INCLUDE_DIRS} ${CLHEP_INCLUDE_DIRS} ${EIGEN_INCLUDE_DIRS}
                      LINK_LIBRARIES ${ROOT_LIBRARIES} ${CORAL_LIBRARIES} ${CLHEP_LIBRARIES} ${EIGEN_LIBRARIES} AthenaBaseComps GoodRunsListsLib GeoPrimitives GaudiKernel MagFieldInterfaces MdtCalibData MdtCalibIOSvcLib MuonCalibStandAloneBaseLib MuonIdHelpersLib StoreGateLib SGtests EventInfo MdtCalibFitters MdtCalibRt MuonCalibEventBase MuonCalibITools MuonCalibIdentifier CalibNtupleReader MuonReadoutGeometry CalibNtupleAnalysisAlgLib )

# Install files from the package:
atlas_install_python_modules( python/*.py )
atlas_install_joboptions( share/*.py )


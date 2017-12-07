################################################################################
# Package: MuonEventAthenaPool
################################################################################

# Declare the package name:
atlas_subdir( MuonEventAthenaPool )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PUBLIC
                          Database/AthenaPOOL/AthenaPoolServices
                          Database/AthenaPOOL/AthenaPoolUtilities
                          Database/AtlasSealCLHEP
                          MuonSpectrometer/MuonRDO
                          MuonSpectrometer/MuonReconstruction/MuonRecEvent/MuonRIO_OnTrack
                          MuonSpectrometer/MuonReconstruction/MuonRecEvent/MuonSegment
                          PRIVATE
                          AtlasTest/TestTools
                          Control/CLIDSvc
                          Control/DataModel
                          Control/SGTools
                          Control/StoreGate
                          Database/AthenaPOOL/AthenaPoolCnvSvc
                          DetectorDescription/Identifier
                          GaudiKernel
                          MuonSpectrometer/MuonCablings/RPCcablingInterface
                          MuonSpectrometer/MuonCalib/CscCalib/CscCalibEvent
                          MuonSpectrometer/MuonCnv/MuonEventTPCnv
                          MuonSpectrometer/MuonDetDescr/MuonReadoutGeometry
                          MuonSpectrometer/MuonDigitContainer
                          MuonSpectrometer/MuonIdHelpers
                          MuonSpectrometer/MuonReconstruction/MuonRecEvent/MuonChamberT0s
                          MuonSpectrometer/MuonReconstruction/MuonRecEvent/MuonPrepRawData
                          MuonSpectrometer/MuonReconstruction/MuonRecEvent/MuonTrigCoinData
                          MuonSpectrometer/MuonSimData )

# External dependencies:
find_package( Boost COMPONENTS filesystem thread system )
find_package( ROOT COMPONENTS Core Tree MathCore Hist RIO pthread )

# Component(s) in the package:
atlas_add_poolcnv_library( MuonEventAthenaPoolPoolCnv
                           src/*.cxx
                           FILES MuonSimData/MuonSimDataCollection.h MuonSimData/CscSimDataCollection.h MuonRDO/MdtCsmContainer.h MuonRDO/RpcPadContainer.h MuonRDO/TgcRdoContainer.h MuonRDO/CscRawDataContainer.h MuonRDO/RpcSectorLogicContainer.h MuonRDO/STGC_RawDataContainer.h MuonRDO/MM_RawDataContainer.h MuonDigitContainer/MdtDigitContainer.h MuonDigitContainer/RpcDigitContainer.h MuonDigitContainer/TgcDigitContainer.h MuonDigitContainer/CscDigitContainer.h MuonDigitContainer/MmDigitContainer.h MuonDigitContainer/sTgcDigitContainer.h CscCalibEvent/CscCalibDataContainer.h MuonPrepRawData/CscPrepDataContainer.h MuonPrepRawData/CscStripPrepDataContainer.h MuonPrepRawData/RpcPrepDataContainer.h MuonPrepRawData/TgcPrepDataContainer.h MuonPrepRawData/MdtPrepDataContainer.h MuonPrepRawData/MMPrepDataContainer.h MuonPrepRawData/sTgcPrepDataContainer.h MuonTrigCoinData/TgcCoinDataContainer.h MuonTrigCoinData/RpcCoinDataContainer.h MuonChamberT0s/ChamberT0s.h src/MuonMeasurements.h
                           TYPES_WITH_NAMESPACE Muon::STGC_RawDataContainer Muon::MM_RawDataContainer Muon::CscPrepDataContainer Muon::CscStripPrepRawDataContainer Muon::RpcPrepDataContainer Muon::TgcPrepDataContainer Muon::MdtPrepDataContainer Muon::MMPrepDataContainer Muon::sTgcPrepDataContainer Muon::TgcCoinDataContainer Muon::ChamberT0s TPCnv::MuonMeasurements
                           INCLUDE_DIRS ${Boost_INCLUDE_DIRS} ${ROOT_INCLUDE_DIRS}
                           LINK_LIBRARIES ${Boost_LIBRARIES} ${ROOT_LIBRARIES} AthenaPoolUtilities AtlasSealCLHEP MuonRDO MuonRIO_OnTrack MuonSegment DataModel SGTools StoreGateLib SGtests AthenaPoolCnvSvcLib Identifier GaudiKernel RPCcablingInterfaceLib CscCalibEvent MuonEventTPCnv MuonReadoutGeometry MuonDigitContainer MuonIdHelpersLib MuonChamberT0s MuonPrepRawData MuonTrigCoinData MuonSimData )

atlas_add_dictionary( MuonRDOCnvDict
                      MuonEventAthenaPool/MuonRDOCnvDict.h
                      MuonEventAthenaPool/selection.xml
                      INCLUDE_DIRS ${Boost_INCLUDE_DIRS} ${ROOT_INCLUDE_DIRS}
                      LINK_LIBRARIES ${Boost_LIBRARIES} ${ROOT_LIBRARIES} AthenaPoolUtilities AtlasSealCLHEP MuonRDO MuonRIO_OnTrack MuonSegment DataModel SGTools StoreGateLib SGtests AthenaPoolCnvSvcLib Identifier GaudiKernel RPCcablingInterfaceLib CscCalibEvent MuonEventTPCnv MuonReadoutGeometry MuonDigitContainer MuonIdHelpersLib MuonChamberT0s MuonPrepRawData MuonTrigCoinData MuonSimData )

# Install files from the package:
atlas_install_headers( MuonEventAthenaPool )
atlas_install_joboptions( share/*.py )

# Set up (a) test(s) for the converter(s):
if( IS_DIRECTORY ${CMAKE_SOURCE_DIR}/Database/AthenaPOOL/AthenaPoolUtilities )
   set( AthenaPoolUtilitiesTest_DIR
      ${CMAKE_SOURCE_DIR}/Database/AthenaPOOL/AthenaPoolUtilities/cmake )
endif()
find_package( AthenaPoolUtilitiesTest )

if( ATHENAPOOLUTILITIESTEST_FOUND )
  set( MUONEVENTATHENAPOOL_REFERENCE_TAG
       MuonEventAthenaPoolReference-01-00-00 )
  run_tpcnv_test( MuonEventTPCnv_17.0.0   ESD-17.0.0
                   REFERENCE_TAG ${MUONEVENTATHENAPOOL_REFERENCE_TAG} )
  run_tpcnv_test( MuonEventTPCnv_20.1.7.2 ESD-20.1.7.2
                   REFERENCE_TAG ${MUONEVENTATHENAPOOL_REFERENCE_TAG} )
else()
   message( WARNING "Couldn't find AthenaPoolUtilitiesTest. No test(s) set up." )
endif()   

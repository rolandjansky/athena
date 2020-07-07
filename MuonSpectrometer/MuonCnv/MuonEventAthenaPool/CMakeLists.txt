# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

# Declare the package name:
atlas_subdir( MuonEventAthenaPool )

# External dependencies:
find_package( Boost )
find_package( ROOT COMPONENTS Core )

# Component(s) in the package:
atlas_add_library( MuonEventAthenaPoolLib
   MuonEventAthenaPool/*.h src/*Cnv_p*.h src/*Cnv_p*.cxx
   PUBLIC_HEADERS MuonEventAthenaPool
   INCLUDE_DIRS ${Boost_INCLUDE_DIRS} ${ROOT_INCLUDE_DIRS}
   LINK_LIBRARIES ${Boost_LIBRARIES} ${ROOT_LIBRARIES} AthenaPoolUtilities
   AtlasSealCLHEP MuonRDO MuonRIO_OnTrack MuonSegment AthAllocators
   AthContainers AthenaKernel StoreGateLib AthenaPoolCnvSvcLib
   Identifier GaudiKernel RPCcablingInterfaceLib CscCalibEvent MuonEventTPCnv
   MuonReadoutGeometry MuonDigitContainer MuonIdHelpersLib MuonChamberT0s
   MuonPrepRawData MuonTrigCoinData MuonSimData AthenaPoolServicesLib )

atlas_add_poolcnv_library( MuonEventAthenaPoolPoolCnv
   MuonEventAthenaPool/*.h src/*Cnv.h src/*Cnv.cxx
   FILES MuonSimData/MuonSimDataCollection.h MuonSimData/CscSimDataCollection.h
         MuonRDO/MdtCsmContainer.h MuonRDO/RpcPadContainer.h
         MuonRDO/TgcRdoContainer.h MuonRDO/CscRawDataContainer.h
         MuonRDO/RpcSectorLogicContainer.h MuonRDO/STGC_RawDataContainer.h
         MuonRDO/MM_RawDataContainer.h MuonDigitContainer/MdtDigitContainer.h
         MuonDigitContainer/RpcDigitContainer.h
         MuonDigitContainer/TgcDigitContainer.h
         MuonDigitContainer/CscDigitContainer.h
         MuonDigitContainer/MmDigitContainer.h
         MuonDigitContainer/sTgcDigitContainer.h
         CscCalibEvent/CscCalibDataContainer.h
         MuonPrepRawData/CscPrepDataContainer.h
         MuonPrepRawData/CscStripPrepDataContainer.h
         MuonPrepRawData/RpcPrepDataContainer.h
         MuonPrepRawData/TgcPrepDataContainer.h
         MuonPrepRawData/MdtPrepDataContainer.h
         MuonPrepRawData/MMPrepDataContainer.h
         MuonPrepRawData/sTgcPrepDataContainer.h
         MuonTrigCoinData/TgcCoinDataContainer.h
         MuonTrigCoinData/RpcCoinDataContainer.h
         MuonChamberT0s/ChamberT0s.h src/MuonMeasurements.h
   TYPES_WITH_NAMESPACE Muon::STGC_RawDataContainer Muon::MM_RawDataContainer
                        Muon::CscPrepDataContainer
                        Muon::CscStripPrepRawDataContainer
                        Muon::RpcPrepDataContainer Muon::TgcPrepDataContainer
                        Muon::MdtPrepDataContainer Muon::MMPrepDataContainer
                        Muon::sTgcPrepDataContainer Muon::TgcCoinDataContainer
                        Muon::ChamberT0s TPCnv::MuonMeasurements
   INCLUDE_DIRS ${Boost_INCLUDE_DIRS} ${ROOT_INCLUDE_DIRS}
   LINK_LIBRARIES ${Boost_LIBRARIES} ${ROOT_LIBRARIES} AthenaPoolUtilities
                  AtlasSealCLHEP MuonRDO MuonRIO_OnTrack MuonSegment
                  AthAllocators AthContainers AthenaKernel StoreGateLib
                  AthenaPoolCnvSvcLib Identifier GaudiKernel
                  RPCcablingInterfaceLib CscCalibEvent MuonEventTPCnv
                  MuonReadoutGeometry MuonDigitContainer MuonIdHelpersLib
                  MuonChamberT0s MuonPrepRawData MuonTrigCoinData MuonSimData
                  RPC_CondCablingLib MuonEventAthenaPoolLib )

atlas_add_dictionary( MuonRDOCnvDict
   MuonEventAthenaPool/MuonRDOCnvDict.h
   MuonEventAthenaPool/selection.xml
   LINK_LIBRARIES CxxUtils AthenaPoolServicesLib MuonRDO MuonRIO_OnTrack
   MuonSegment )

# Install files from the package:
atlas_install_joboptions( share/*.py )

# Set up (a) test(s) for the converter(s):
find_package( AthenaPoolUtilitiesTest )

if( ATHENAPOOLUTILITIESTEST_FOUND )
  set( MUONEVENTATHENAPOOL_REFERENCE_TAG
       MuonEventAthenaPoolReference-03-01-00 )
  run_tpcnv_legacy_test( MuonEventTPCnv_17.0.0   ESD-17.0.0
                   REFERENCE_TAG ${MUONEVENTATHENAPOOL_REFERENCE_TAG} )
  run_tpcnv_legacy_test( MuonEventTPCnv_20.1.7.2 ESD-20.1.7.2
                   REFERENCE_TAG ${MUONEVENTATHENAPOOL_REFERENCE_TAG} )
else()
   message( WARNING
      "Couldn't find AthenaPoolUtilitiesTest. No test(s) set up." )
endif()   

# Helper variable for running the tests:
set( _jobOPath "${CMAKE_CURRENT_SOURCE_DIR}/share" )
set( _jobOPath "${_jobOPath}:${CMAKE_JOBOPT_OUTPUT_DIRECTORY}" )
set( _jobOPath "${_jobOPath}:$ENV{JOBOPTSEARCHPATH}" )

# Set up the tests of the package:
foreach( name
    CscRawDataCnv_p1_test
    CscRawDataCnv_p2_test
    CscRawDataCnv_p3_test
    CscRawDataCollectionCnv_p1_test
    CscRawDataCollectionCnv_p2_test
    CscRawDataCollectionCnv_p3_test
    MdtAmtHitCnv_p1_test
    MdtCsmCnv_p1_test
    RpcCoinMatrixCnv_p1_test
    RpcFiredChannelCnv_p1_test
    RpcPadCnv_p1_test
    RpcPadContainerCnv_p1_test
    RpcPadContainerCnv_p2_test
    RpcSectorLogicContainerCnv_p1_test
    TgcRawDataCnv_p1_test
    TgcRawDataCnv_p2_test
    TgcRawDataCnv_p3_test
    TgcRdoCnv_p1_test
    TgcRdoCnv_p2_test
    TgcRdoCnv_p3_test )

   atlas_add_test( ${name}
      SOURCES test/${name}.cxx
      LINK_LIBRARIES IdDictParser MuonEventAthenaPoolLib
      PROPERTIES TIMEOUT 300
      ENVIRONMENT "JOBOPTSEARCHPATH=${_jobOPath}" )

endforeach()

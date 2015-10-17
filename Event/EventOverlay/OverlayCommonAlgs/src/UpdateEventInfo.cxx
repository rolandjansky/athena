/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// Ketevi A. Assamagan, October 2009

// Piyali Banerjee, March 2011

#include "UpdateEventInfo.h"

#include "EventInfo/EventInfo.h"
#include "EventInfo/EventType.h"

#include "GeneratorObjects/McEventCollection.h"
#include "TrackRecord/TrackRecordCollection.h"
#include "CaloSimEvent/CaloCalibrationHitContainer.h"

#include "TileEvent/TileDigitsContainer.h"
#include "TileEvent/TileRawChannelContainer.h"
#include "TileEvent/TileL2Container.h"
#include "TileEvent/TileHitContainer.h"
#include "TileEvent/TileCellIDC.h"
#include "TileEvent/TileBeamElemContainer.h"
#include "TileEvent/TileLaserObject.h"
#include "TileSimEvent/TileHitVector.h"

#include "LArRawEvent/LArDigitContainer.h"
#include "LArRawEvent/LArRawChannelContainer.h"
#include "LArRawEvent/LArFebHeaderContainer.h"
#include "LArSimEvent/LArHitContainer.h"
#include "LArSimEvent/LArHitFloatContainer.h"

#include "InDetSimData/InDetSimDataCollection.h"
#include "MuonSimData/MuonSimDataCollection.h"
#include "MuonSimData/CscSimDataCollection.h"
#include "InDetRawData/TRT_RDO_Container.h"
#include "InDetRawData/SCT_RDO_Container.h"
#include "InDetRawData/PixelRDO_Container.h"
#include "InDetSimEvent/TRTUncompressedHitCollection.h"
#include "InDetSimEvent/SiHitCollection.h"

#include "MuonDigitContainer/MdtDigitContainer.h"
#include "MuonDigitContainer/RpcDigitContainer.h"
#include "MuonDigitContainer/TgcDigitContainer.h"
#include "MuonDigitContainer/CscDigitContainer.h"
#include "MuonRDO/CscRawDataContainer.h"
#include "MuonRDO/MdtCsmContainer.h"
#include "MuonRDO/RpcPadContainer.h"
#include "MuonRDO/TgcRdoContainer.h"
#include "MuonSimEvent/RPCSimHitCollection.h"
#include "MuonSimEvent/TGCSimHitCollection.h"
#include "MuonSimEvent/CSCSimHitCollection.h"
#include "MuonSimEvent/MDTSimHitCollection.h"

#include "InDetBCM_RawData/BCM_RDO_Container.h"

#include "LArRawEvent/LArTTL1Container.h"
#include "TileEvent/TileTTL1Container.h"
#include "TrigT1Result/RoIBResult.h"
#include "TrigT1Result/RecRoIBResult.h"
#include "TrigT1Result/MuCTPI_RDO.h"
#include "TrigT1Result/MuCTPI_RIO.h"
#include "TrigT1Result/CTP_RDO.h"
#include "TrigT1Result/CTP_RIO.h"
#include "TrigT1CaloEvent/TriggerTower.h"
#include "TrigT1CaloEvent/JetElement.h"
#include "TrigT1CaloEvent/CPMHits.h"
#include "TrigT1CaloEvent/CPMRoI.h"
#include "TrigT1CaloEvent/CMMCPHits.h"
#include "TrigT1CaloEvent/CMMJetHits.h"
#include "TrigT1CaloEvent/CMMEtSums.h"
#include "TrigT1CaloEvent/JEMHits.h"
#include "TrigT1CaloEvent/JEMRoI.h"
#include "TrigT1CaloEvent/JEMEtSums.h"
#include "TrigT1CaloEvent/CMMRoI.h"
#include "TrigT1CaloEvent/CPMTower.h"
#include "TrigT1CaloEvent/EmTauROI.h"
#include "TrigT1CaloEvent/EmTauROI_ClassDEF.h"
#include "TrigT1CaloEvent/JetROI.h"
#include "TrigT1CaloEvent/JetROI_ClassDEF.h"
#include "TrigT1CaloEvent/RODHeader.h"
#include "TrigT1CaloEvent/EnergyRoI.h"
#include "TrigT1CaloEvent/JetEtRoI.h"
#include "TrigT1CaloEvent/JEPBSCollection.h"
#include "TrigT1CaloEvent/JEPRoIBSCollection.h"
#include "TrigT1CaloEvent/CPBSCollection.h"
#include "TrigT1Interfaces/JetCTP.h"
#include "TrigT1Interfaces/EmTauCTP.h"
#include "TrigT1Interfaces/EnergyCTP.h"
#include "TrigT1Interfaces/MuCTPICTP.h"
#include "TrigT1Interfaces/MbtsCTP.h"
#include "TrigT1Interfaces/BcmCTP.h"
#include "TrigT1Interfaces/CTPSLink.h"
#include "TrigT1Interfaces/SlinkWord.h"
#include "TrigT1Interfaces/Lvl1MuCTPIInput.h"
#include "TrigT1Interfaces/MuCTPIToRoIBSLink.h"
#include "PersistentDataModel/DataHeader.h"

#include <iostream>
#include <typeinfo>

//================================================================
UpdateEventInfo::UpdateEventInfo(const std::string &name, ISvcLocator *pSvcLocator) :
  OverlayAlgBase(name, pSvcLocator)
  , m_storeGateTemp("StoreGateSvc/TemporaryStore", name),m_count(0),m_infoType("MyEvent")

{
  declareProperty("TempStore", m_storeGateTemp, "help");
  declareProperty("InfoType", m_infoType);
}

//================================================================
StatusCode UpdateEventInfo::overlayInitialize()
{
  if (m_storeGateTemp.retrieve().isFailure())
    {
      ATH_MSG_ERROR ( "overlayInitialize(): StoreGate[temp] service not found !" );
      return StatusCode::FAILURE;
    }
  m_count = 0;
  return StatusCode::SUCCESS;
}

//================================================================
StatusCode UpdateEventInfo::overlayFinalize()
{
  return StatusCode::SUCCESS;
}

//================================================================
StatusCode UpdateEventInfo::overlayExecute()
{
  //std::cout << "UpdateEventInfo::execute() begin" << std::endl;
  //std::cout << m_storeGateMC->dump() << std::endl;
  //std::cout << m_storeGateTemp->dump() << std::endl;
  //std::cout << m_storeGateData->dump() << std::endl;
  //std::cout << m_storeGateOutput->dump() << std::endl;

  /** first, remove the EventInfo in the MC store */
  if (m_storeGateTemp->contains<EventInfo>(m_infoType) )
    {
      removeAllObjectsOfType<EventInfo>(&*m_storeGateTemp);
    }
  /** Copy the EventInfo from the data store to the MC store
      the out MC RDO must be reconstructed with the MC conditions */
  const EventInfo * oldEvtInfo = 0;
  if (m_storeGateOutput->retrieve(oldEvtInfo, m_infoType).isSuccess() )
    {
      EventInfo * newEvtInfo = new EventInfo ( *oldEvtInfo );
      if ( m_storeGateTemp->record( newEvtInfo, m_infoType ).isFailure() )
        {
          ATH_MSG_WARNING ( "overlayExecute(): Could not record EventInfo to MC output storeGate, key= " << m_infoType );
        }
    }
  else
    {
      ATH_MSG_WARNING ( "overlayExecute(): Could retrieve EventInfo from data store, key = " << m_infoType );
    }

  /** Copy MC information to temp */
  checkBeforeRemove<McEventCollection>(&*m_storeGateTemp);
  checkBeforeRemove<TrackRecordCollection>(&*m_storeGateTemp);
  checkBeforeRemove<CaloCalibrationHitContainer>(&*m_storeGateTemp);
  checkBeforeRemove<InDetSimDataCollection>(&*m_storeGateTemp);
  checkBeforeRemove<CscSimDataCollection>(&*m_storeGateTemp);
  checkBeforeRemove<MuonSimDataCollection>(&*m_storeGateTemp);
  checkBeforeRemove<DataHeader>(&*m_storeGateTemp);

  copyAllObjectsOfType<McEventCollection>(&*m_storeGateTemp, &*m_storeGateMC);
  copyAllObjectsOfType<TrackRecordCollection>(&*m_storeGateTemp, &*m_storeGateMC);
  copyAllObjectsOfType<CaloCalibrationHitContainer>(&*m_storeGateTemp, &*m_storeGateMC);
  copyAllObjectsOfType<InDetSimDataCollection>(&*m_storeGateTemp, &*m_storeGateMC);
  copyAllObjectsOfType<CscSimDataCollection>(&*m_storeGateTemp, &*m_storeGateMC);
  copyAllObjectsOfType<MuonSimDataCollection>(&*m_storeGateTemp, &*m_storeGateMC);
  copyAllObjectsOfType<DataHeader>(&*m_storeGateTemp, &*m_storeGateMC);

  /** Copy Inner Detector stuff */
  if ( m_count > 0 )
    {
      checkBeforeRemove<TRTUncompressedHitCollection>(&*m_storeGateTemp);
      checkBeforeRemove<TRT_RDO_Container>(&*m_storeGateTemp);
      checkBeforeRemove<SCT_RDO_Container>(&*m_storeGateTemp);
      checkBeforeRemove<PixelRDO_Container>(&*m_storeGateTemp);
      checkBeforeRemove<SiHitCollection>(&*m_storeGateTemp);
    }
  copyAllObjectsOfType<TRTUncompressedHitCollection>(&*m_storeGateTemp, &*m_storeGateMC);
  copyAllObjectsOfType<TRT_RDO_Container>(&*m_storeGateTemp, &*m_storeGateMC);
  copyAllObjectsOfType<SCT_RDO_Container>(&*m_storeGateTemp, &*m_storeGateMC);
  copyAllObjectsOfType<PixelRDO_Container>(&*m_storeGateTemp, &*m_storeGateMC);
  copyAllObjectsOfType<SiHitCollection>(&*m_storeGateTemp, &*m_storeGateMC);

  /** Copy calorimeter stuff */
  checkBeforeRemove<LArFebHeaderContainer>(&*m_storeGateTemp);
  checkBeforeRemove<LArHitContainer>(&*m_storeGateTemp);
  checkBeforeRemove<LArHitFloatContainer>(&*m_storeGateTemp);
  checkBeforeRemove<LArDigitContainer>(&*m_storeGateTemp);
  checkBeforeRemove<LArRawChannelContainer>(&*m_storeGateTemp);
  checkBeforeRemove<TileHitContainer>(&*m_storeGateTemp);
  checkBeforeRemove<TileDigitsContainer>(&*m_storeGateTemp);
  checkBeforeRemove<TileRawChannelContainer>(&*m_storeGateTemp);
  checkBeforeRemove<TileCellIDC>(&*m_storeGateTemp);
  checkBeforeRemove<TileL2Container>(&*m_storeGateTemp);
  checkBeforeRemove<TileBeamElemContainer>(&*m_storeGateTemp);
  checkBeforeRemove<TileLaserObject>(&*m_storeGateTemp);
  checkBeforeRemove<TileHitVector>(&*m_storeGateTemp);

  copyAllObjectsOfType<LArFebHeaderContainer>(&*m_storeGateTemp, &*m_storeGateMC);
  copyAllObjectsOfType<LArHitContainer>(&*m_storeGateTemp, &*m_storeGateMC);
  copyAllObjectsOfType<LArHitFloatContainer>(&*m_storeGateTemp, &*m_storeGateMC);
  copyAllObjectsOfType<LArDigitContainer>(&*m_storeGateTemp, &*m_storeGateMC);
  copyAllObjectsOfType<LArRawChannelContainer>(&*m_storeGateTemp, &*m_storeGateMC);
  copyAllObjectsOfType<TileHitContainer>(&*m_storeGateTemp, &*m_storeGateMC);
  copyAllObjectsOfType<TileDigitsContainer>(&*m_storeGateTemp, &*m_storeGateMC);
  copyAllObjectsOfType<TileRawChannelContainer>(&*m_storeGateTemp, &*m_storeGateMC);
  copyAllObjectsOfType<TileCellIDC>(&*m_storeGateTemp, &*m_storeGateMC);
  copyAllObjectsOfType<TileL2Container>(&*m_storeGateTemp, &*m_storeGateMC);
  copyAllObjectsOfType<TileBeamElemContainer>(&*m_storeGateTemp, &*m_storeGateMC);
  copyAllObjectsOfType<TileLaserObject>(&*m_storeGateTemp, &*m_storeGateMC);
  copyAllObjectsOfType<TileHitVector>(&*m_storeGateTemp, &*m_storeGateMC);

  /** Copy muon objects */
  if ( m_count > 0 )
    {
      checkBeforeRemove<RPCSimHitCollection>(&*m_storeGateTemp);
      checkBeforeRemove<TGCSimHitCollection>(&*m_storeGateTemp);
      checkBeforeRemove<CSCSimHitCollection>(&*m_storeGateTemp);
      checkBeforeRemove<MDTSimHitCollection>(&*m_storeGateTemp);
      checkBeforeRemove<CscDigitContainer>(&*m_storeGateTemp);
      checkBeforeRemove<MdtDigitContainer>(&*m_storeGateTemp);
      checkBeforeRemove<RpcDigitContainer>(&*m_storeGateTemp);
      checkBeforeRemove<TgcDigitContainer>(&*m_storeGateTemp);
      checkBeforeRemove<CscRawDataContainer>(&*m_storeGateTemp);
      checkBeforeRemove<MdtCsmContainer>(&*m_storeGateTemp);
      checkBeforeRemove<RpcPadContainer>(&*m_storeGateTemp);
      checkBeforeRemove<TgcRdoContainer>(&*m_storeGateTemp);
    }

  copyAllObjectsOfType<RPCSimHitCollection>(&*m_storeGateTemp, &*m_storeGateMC);
  copyAllObjectsOfType<TGCSimHitCollection>(&*m_storeGateTemp, &*m_storeGateMC);
  copyAllObjectsOfType<CSCSimHitCollection>(&*m_storeGateTemp, &*m_storeGateMC);
  copyAllObjectsOfType<MDTSimHitCollection>(&*m_storeGateTemp, &*m_storeGateMC);
  copyAllObjectsOfType<CscDigitContainer>(&*m_storeGateTemp, &*m_storeGateMC);
  copyAllObjectsOfType<MdtDigitContainer>(&*m_storeGateTemp, &*m_storeGateMC);
  copyAllObjectsOfType<RpcDigitContainer>(&*m_storeGateTemp, &*m_storeGateMC);
  copyAllObjectsOfType<TgcDigitContainer>(&*m_storeGateTemp, &*m_storeGateMC);
  copyAllObjectsOfType<CscRawDataContainer>(&*m_storeGateTemp, &*m_storeGateMC);
  copyAllObjectsOfType<MdtCsmContainer>(&*m_storeGateTemp, &*m_storeGateMC);
  copyAllObjectsOfType<RpcPadContainer>(&*m_storeGateTemp, &*m_storeGateMC);
  copyAllObjectsOfType<TgcRdoContainer>(&*m_storeGateTemp, &*m_storeGateMC);

  /** Copy beam related objects */
  if ( m_count > 0 )
    {
      checkBeforeRemove<BCM_RDO_Container>(&*m_storeGateTemp);
    }
  copyAllObjectsOfType<BCM_RDO_Container>(&*m_storeGateTemp, &*m_storeGateMC);

  /** Copy level 1 objects */
  checkBeforeRemove<LArTTL1Container>(&*m_storeGateTemp);
  checkBeforeRemove<TileTTL1Container>(&*m_storeGateTemp);
  checkBeforeRemove<ROIB::RoIBResult>(&*m_storeGateTemp);
  checkBeforeRemove<ROIB::RecRoIBResult>(&*m_storeGateTemp);
  checkBeforeRemove<MuCTPI_RDO>(&*m_storeGateTemp);
  checkBeforeRemove<MuCTPI_RIO>(&*m_storeGateTemp);
  checkBeforeRemove<CTP_RDO>(&*m_storeGateTemp);
  checkBeforeRemove<CTP_RIO>(&*m_storeGateTemp);
  checkBeforeRemove< DataVector<LVL1::TriggerTower> >(&*m_storeGateTemp);
  checkBeforeRemove< DataVector<LVL1::CPMTower> >(&*m_storeGateTemp);
  checkBeforeRemove< DataVector<LVL1::JetElement> >(&*m_storeGateTemp);
  checkBeforeRemove< DataVector<LVL1::CPMHits> >(&*m_storeGateTemp);
  checkBeforeRemove< DataVector<LVL1::CPMRoI> >(&*m_storeGateTemp);
  checkBeforeRemove< DataVector<LVL1::CMMCPHits> >(&*m_storeGateTemp);
  checkBeforeRemove< DataVector<LVL1::CMMJetHits> >(&*m_storeGateTemp);
  checkBeforeRemove< DataVector<LVL1::CMMEtSums> >(&*m_storeGateTemp);
  checkBeforeRemove< DataVector<LVL1::JEMHits> >(&*m_storeGateTemp);
  checkBeforeRemove< DataVector<LVL1::JEMRoI> >(&*m_storeGateTemp);
  checkBeforeRemove< DataVector<LVL1::JEMEtSums> >(&*m_storeGateTemp);
  checkBeforeRemove< DataVector<LVL1::EmTauROI> >(&*m_storeGateTemp);
  checkBeforeRemove< DataVector<LVL1::JetROI> >(&*m_storeGateTemp);
  checkBeforeRemove< DataVector<LVL1::RODHeader> >(&*m_storeGateTemp);
  // SlinkWord and vector<int> are commented out because their class
  // definitions have no CLASS_DEF and so sizeof() is unable to estimate
  // their size and so they cannot be retrieved from StoreGate
  // checkBeforeRemove< DataVector<LVL1CTP::SlinkWord> >(&*m_storeGateTemp);
  // checkBeforeRemove< std::vector<unsigned int> >(&*m_storeGateTemp);
  checkBeforeRemove<LVL1::JetCTP>(&*m_storeGateTemp);
  checkBeforeRemove<LVL1::EmTauCTP>(&*m_storeGateTemp);
  checkBeforeRemove<LVL1::EnergyCTP>(&*m_storeGateTemp);
  checkBeforeRemove<LVL1::EnergyRoI>(&*m_storeGateTemp);
  checkBeforeRemove<LVL1::CMMRoI>(&*m_storeGateTemp);
  checkBeforeRemove<LVL1::MuCTPICTP>(&*m_storeGateTemp);
  checkBeforeRemove<LVL1::MbtsCTP>(&*m_storeGateTemp);
  checkBeforeRemove<LVL1::BcmCTP>(&*m_storeGateTemp);
  checkBeforeRemove<LVL1::JetEtRoI>(&*m_storeGateTemp);
  checkBeforeRemove<LVL1::JEPBSCollection>(&*m_storeGateTemp);
  checkBeforeRemove<LVL1::JEPRoIBSCollection>(&*m_storeGateTemp);
  checkBeforeRemove<LVL1::CPBSCollection>(&*m_storeGateTemp);
  checkBeforeRemove<LVL1CTP::CTPSLink>(&*m_storeGateTemp);
  checkBeforeRemove<LVL1MUONIF::Lvl1MuCTPIInput>(&*m_storeGateTemp);
  checkBeforeRemove<L1MUINT::MuCTPIToRoIBSLink>(&*m_storeGateTemp);

  copyAllObjectsOfType<LArTTL1Container>(&*m_storeGateTemp, &*m_storeGateMC);
  copyAllObjectsOfType<TileTTL1Container>(&*m_storeGateTemp, &*m_storeGateMC);
  copyAllObjectsOfType<ROIB::RoIBResult>(&*m_storeGateTemp, &*m_storeGateMC);
  copyAllObjectsOfType<ROIB::RecRoIBResult>(&*m_storeGateTemp, &*m_storeGateMC);
  copyAllObjectsOfType<MuCTPI_RDO>(&*m_storeGateTemp, &*m_storeGateMC);
  copyAllObjectsOfType<MuCTPI_RIO>(&*m_storeGateTemp, &*m_storeGateMC);
  copyAllObjectsOfType<CTP_RDO>(&*m_storeGateTemp, &*m_storeGateMC);
  copyAllObjectsOfType<CTP_RIO>(&*m_storeGateTemp, &*m_storeGateMC);
  copyAllObjectsOfType< DataVector<LVL1::TriggerTower> >(&*m_storeGateTemp, &*m_storeGateMC);
  copyAllObjectsOfType< DataVector<LVL1::CPMTower> >(&*m_storeGateTemp, &*m_storeGateMC);
  copyAllObjectsOfType< DataVector<LVL1::JetElement> >(&*m_storeGateTemp, &*m_storeGateMC);
  copyAllObjectsOfType< DataVector<LVL1::CPMHits> >(&*m_storeGateTemp, &*m_storeGateMC);
  copyAllObjectsOfType< DataVector<LVL1::CPMRoI> >(&*m_storeGateTemp, &*m_storeGateMC);
  copyAllObjectsOfType< DataVector<LVL1::CMMCPHits> >(&*m_storeGateTemp, &*m_storeGateMC);
  copyAllObjectsOfType< DataVector<LVL1::CMMJetHits> >(&*m_storeGateTemp, &*m_storeGateMC);
  copyAllObjectsOfType< DataVector<LVL1::CMMEtSums> >(&*m_storeGateTemp, &*m_storeGateMC);
  copyAllObjectsOfType< DataVector<LVL1::JEMHits> >(&*m_storeGateTemp, &*m_storeGateMC);
  copyAllObjectsOfType< DataVector<LVL1::JEMRoI> >(&*m_storeGateTemp, &*m_storeGateMC);
  copyAllObjectsOfType< DataVector<LVL1::JEMEtSums> >(&*m_storeGateTemp, &*m_storeGateMC);
  copyAllObjectsOfType< DataVector<LVL1::EmTauROI> >(&*m_storeGateTemp, &*m_storeGateMC);
  copyAllObjectsOfType< DataVector<LVL1::JetROI> >(&*m_storeGateTemp, &*m_storeGateMC);
  copyAllObjectsOfType< DataVector<LVL1::RODHeader> >(&*m_storeGateTemp, &*m_storeGateMC);
  // SlinkWord and vector<int> are commented out because their class
  // definitions have no CLASS_DEF and so sizeof() is unable to estimate
  // their size and so they cannot be retrieved from StoreGate
  // copyAllObjectsOfType< DataVector<LVL1CTP::SlinkWord> >(&*m_storeGateTemp, &*m_storeGateMC);
  // copyAllObjectsOfType< std::vector<unsigned int> >(&*m_storeGateTemp, &*m_storeGateMC);
  copyAllObjectsOfType<LVL1::JetCTP>(&*m_storeGateTemp, &*m_storeGateMC);
  copyAllObjectsOfType<LVL1::EmTauCTP>(&*m_storeGateTemp, &*m_storeGateMC);
  copyAllObjectsOfType<LVL1::EnergyCTP>(&*m_storeGateTemp, &*m_storeGateMC);
  copyAllObjectsOfType<LVL1::EnergyRoI>(&*m_storeGateTemp, &*m_storeGateMC);
  copyAllObjectsOfType<LVL1::CMMRoI>(&*m_storeGateTemp, &*m_storeGateMC);
  copyAllObjectsOfType<LVL1::MuCTPICTP>(&*m_storeGateTemp, &*m_storeGateMC);
  copyAllObjectsOfType<LVL1::MbtsCTP>(&*m_storeGateTemp, &*m_storeGateMC);
  copyAllObjectsOfType<LVL1::BcmCTP>(&*m_storeGateTemp, &*m_storeGateMC);
  copyAllObjectsOfType<LVL1::JetEtRoI>(&*m_storeGateTemp, &*m_storeGateMC);
  copyAllObjectsOfType<LVL1::JEPBSCollection>(&*m_storeGateTemp, &*m_storeGateMC);
  copyAllObjectsOfType<LVL1::JEPRoIBSCollection>(&*m_storeGateTemp, &*m_storeGateMC);
  copyAllObjectsOfType<LVL1::CPBSCollection>(&*m_storeGateTemp, &*m_storeGateMC);
  copyAllObjectsOfType<LVL1CTP::CTPSLink>(&*m_storeGateTemp, &*m_storeGateMC);
  copyAllObjectsOfType<LVL1MUONIF::Lvl1MuCTPIInput>(&*m_storeGateTemp, &*m_storeGateMC);
  copyAllObjectsOfType<L1MUINT::MuCTPIToRoIBSLink>(&*m_storeGateTemp, &*m_storeGateMC);

  //std::cout << "UpdateEventInfo::execute() end" << std::endl;
  //std::cout << m_storeGateMC->dump() << std::endl;
  //std::cout << m_storeGateTemp->dump() << std::endl;
  //std::cout << m_storeGateData->dump() << std::endl;
  //std::cout << m_storeGateOutput->dump() << std::endl;
  m_count++;
  return StatusCode::SUCCESS;
}

//================================================================
//EOF

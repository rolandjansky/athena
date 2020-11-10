/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#include "FastCaloSimHit/FastHitConv.h"

#include "AthenaBaseComps/AthCheckMacros.h"
#include "CaloEvent/CaloCellContainer.h"
#include "CaloIdentifier/CaloIdManager.h"
#include "CaloIdentifier/LArEM_ID.h"
#include "CaloIdentifier/LArFCAL_ID.h"
#include "CaloIdentifier/LArHEC_ID.h"
#include "CaloIdentifier/TileID.h"
#include "EventInfo/PileUpEventInfo.h"
#include "EventInfo/EventType.h"
#include "LArSimEvent/LArHitFloat.h"
#include "LArSimEvent/LArHitFloatContainer.h"
#include "LArSimEvent/LArHit.h"
#include "LArSimEvent/LArHitContainer.h"
#include "PileUpTools/PileUpMergeSvc.h"
#include "StoreGate/StoreGateSvc.h"
#include "TileConditions/TileInfo.h"
#include "TileEvent/TileCellContainer.h"
#include "TileSimEvent/TileHit.h"
#include "TileSimEvent/TileHitVector.h"

//================ Constructor =================================================

FastHitConv::FastHitConv(const std::string& name, ISvcLocator* pSvcLocator ) :
  AthAlgorithm(name, pSvcLocator),
  m_embHitContainer("LArHitEMB"),
  m_emecHitContainer("LArHitEMEC"),
  m_fcalHitContainer("LArHitFCAL"),
  m_hecHitContainer("LArHitHEC"),
  m_tileHitVector("TileHitVec"),
  m_caloCellsOutputName("AllFastCalo"),
  m_storeGateFastCalo("StoreGateSvc/FastCalo", name),
  m_pMergeSvc(nullptr),
  m_tileInfo(nullptr),
  m_larEmID(nullptr),
  m_larFcalID(nullptr),
  m_larHecID(nullptr),
  m_tileID(nullptr),
  m_pileup(false)

{

  declareProperty("doPileup", m_pileup, "Pileup mode (default=false)");
  declareProperty("embHitContainername",m_embHitContainer,"Name of output FastSim LAr EM Barrel Hit Container");
  declareProperty("emecHitContainername",m_emecHitContainer,"Name of output FastSim LAr EM Endcap Hit Container");
  declareProperty("fcalHitContainername",m_fcalHitContainer,"Name of output FastSim LAr FCAL Hit Container");
  declareProperty("hecHitContainername",m_hecHitContainer,"Name of output FastSim LAr HEC Hit Container");
  declareProperty("tileHitContainername",m_tileHitVector,"Name of output FastSim Tile Hit Container");
  declareProperty("CaloCellsInputName",m_caloCellsOutputName) ;
}

StatusCode FastHitConv::initialize()
{

  // Services needed for Pileup
  if(m_pileup)
    {
      CHECK(service("PileUpMergeSvc", m_pMergeSvc));
      CHECK(m_storeGateFastCalo.retrieve());
    }
  ATH_MSG_DEBUG("StoreGateFastCalo Svc structure at Initialisation"<<(*m_storeGateFastCalo).dump());
  const DataHandle<CaloIdManager> caloIdManager;
  CHECK(detStore()->retrieve(caloIdManager));
  m_larEmID = caloIdManager->getEM_ID();
  if(m_larEmID==0)
    throw std::runtime_error("FastHitConv: Invalid LAr EM ID helper");
  m_larFcalID = caloIdManager->getFCAL_ID();
  if(m_larFcalID==0)
    throw std::runtime_error("FastHitConv: Invalid FCAL ID helper");
  m_larHecID = caloIdManager->getHEC_ID();
  if(m_larHecID==0)
    throw std::runtime_error("FastHitConv: Invalid HEC ID helper");
  m_tileID = caloIdManager->getTileID();
  if (m_tileID==0)
    throw std::runtime_error("FastHitConv: Invalid Tile ID helper");

  // LAr and Tile Sampling Fractions
  ATH_CHECK(m_fSamplKey.initialize());
  CHECK(detStore()->retrieve(m_tileInfo, "TileInfo"));

  return StatusCode::SUCCESS;
}

StatusCode FastHitConv::execute()
{

  ATH_MSG_DEBUG("execute()");

  // run hit conversion only if calo cell container exists on storegate
  if ( evtStore()->contains<CaloCellContainer>(m_caloCellsOutputName))
    {
      ATH_MSG_DEBUG("CaloCellContainer found on StoreGate. Will run fast hit conversion.");
      CHECK(this->initEvent());
      CHECK(this->hitConstruction());
      CHECK(this->finaliseEvent());
    }
  else
    {
      ATH_MSG_DEBUG("No CaloCellContainer found on Storegate. Skipping fast hit conversion.");
    }

  return StatusCode::SUCCESS;
}

StatusCode FastHitConv::initEvent()
{

  //MsgStream log(msgSvc(), name());
  //log << MSG::DEBUG << "initEvent()" << endmsg;
  ATH_MSG_DEBUG("initEvent()");
  if(!m_embHitContainer.isValid() ) { m_embHitContainer  = std::make_unique<LArHitContainer>(m_embHitContainer.name());}
  if(!m_emecHitContainer.isValid()) { m_emecHitContainer = std::make_unique<LArHitContainer>(m_emecHitContainer.name());}
  if(!m_fcalHitContainer.isValid()) { m_fcalHitContainer = std::make_unique<LArHitContainer>(m_fcalHitContainer.name());}
  if(!m_hecHitContainer.isValid() ) { m_hecHitContainer  = std::make_unique<LArHitContainer>(m_hecHitContainer.name());}
  if(!m_tileHitVector.isValid()        ) { m_tileHitVector         = std::make_unique<TileHitVector>(m_tileHitVector.name());}

  return StatusCode::SUCCESS;

}

StatusCode FastHitConv::finaliseEvent()
{
  ATH_MSG_DEBUG("finaliseEvent()");
  ATH_MSG_DEBUG(m_embHitContainer.name()<<" : "<<m_embHitContainer->size()<<" hits ");
  ATH_MSG_DEBUG(m_emecHitContainer.name()<<" : "<<m_emecHitContainer->size()<<" hits ");
  ATH_MSG_DEBUG(m_fcalHitContainer.name()<<" : "<<m_fcalHitContainer->size()<<" hits ");
  ATH_MSG_DEBUG(m_hecHitContainer.name()<<" : "<<m_hecHitContainer->size()<<" hits ");
  ATH_MSG_DEBUG(m_tileHitVector.name()<<" : "<<m_tileHitVector->size()<<" hits ");

  if(m_pileup)
    {
      CHECK((*m_storeGateFastCalo).clearStore(true));
      ATH_MSG_DEBUG (" StoreGateFastCalo structure at end of event (before recording collections): \n "
                     << (*m_storeGateFastCalo).dump() );
      //FIXME not sure if this is still required. If so, need to update syntax here.
      // CHECK((*m_storeGateFastCalo).record(m_embHitContainer,  m_embHitContainer.name()));
      // CHECK((*m_storeGateFastCalo).record(m_emecHitContainer, m_emecHitContainer.name()));
      // CHECK((*m_storeGateFastCalo).record(m_fcalHitContainer, m_fcalHitContainer.name()));
      // CHECK((*m_storeGateFastCalo).record(m_hecHitContainer,  m_hecHitContainer.name()));
      // CHECK((*m_storeGateFastCalo).record(m_tileHitVector,    m_tileHitVector.name()));
      ATH_MSG_DEBUG (" StoreGateFastCalo structure at end of event (after recording collections): \n "
                     << (*m_storeGateFastCalo).dump() );
      const EventInfo *evt(nullptr);
      CHECK(evtStore()->retrieve(evt,"MyEvent"));
      const EventInfo* newEvt=new EventInfo(*evt);
      PileUpEventInfo *pOverEvent(nullptr);
      CHECK(evtStore()->retrieve(pOverEvent,"OverlayEvent"));
      pOverEvent->addSubEvt(0, PileUpTimeEventIndex::Signal, newEvt, &(*m_storeGateFastCalo));
      ATH_MSG_DEBUG (" StoreGate structure at end of event (after recording collections): \n "
                     << evtStore()->dump() );
    }
  return StatusCode::SUCCESS;

}


StatusCode FastHitConv::hitConstruction()
{

  //
  // - Read in hits from FastCaloSim and convert them to HitCollections
  // - Need to convert energy scale using sampling fraction
  //        for LAr e = cellenergy * samplingfrac
  //        for tile e = cellenergy / samplingfrac
  //

  ATH_MSG_DEBUG("ATLFASTIIDigi "<<this->name()<<" hitConstruction");
  const double minEnergy = 1e-9;

  // Read back FastCaloSim cells
  const CaloCellContainer* cellContainer;
  CHECK(evtStore()->retrieve(cellContainer, m_caloCellsOutputName));

  double hitTime = 0.0;
  Identifier cellid( (unsigned int) 0);
  double energy = 0.0;
  double energyConv = 0.0;
  float SampFrac = 0.0;

  int countFastCell = 0;

  double eLArEMRead   = 0.0;
  double eLArHECRead  = 0.0;
  double eLArFCALRead = 0.0;
  double eTileRead    = 0.0;

  double eLArEMConv   = 0.0;
  double eLArHECConv  = 0.0;
  double eLArFCALConv = 0.0;
  double eTileConv    = 0.0;

  SG::ReadCondHandle<ILArfSampl> fSamplHdl(m_fSamplKey);
  const ILArfSampl* fSampl=*fSamplHdl;

  // First do the LArEM cells
  CaloCellContainer::const_iterator it1 = cellContainer->beginConstCalo(CaloCell_ID::LAREM);
  CaloCellContainer::const_iterator it2 = cellContainer->endConstCalo(CaloCell_ID::LAREM);

  for(;it1!=it2;it1++)
    {
      ATH_MSG_DEBUG("FastCell LArEM"<<countFastCell);

      cellid = (*it1)->ID();
      energy = (*it1)->energy();
      SampFrac = fSampl->FSAMPL(cellid);
      energyConv = energy*SampFrac;

      eLArEMRead += energy;
      eLArEMConv += energyConv;

      ATH_MSG_DEBUG("ReadFastCell id= "<<cellid<<"E= "<<energy<<" SampFrac= "<<SampFrac);
      if(energyConv > minEnergy)
        {
          if(m_larEmID->is_lar_em(cellid)) // Belt and Braces check that it's really an LArEM cell
            {
              if(m_larEmID->is_em_barrel(cellid)) // Both EM Barrel and EM EndCap in container -> separate them
                {
                  ATH_MSG_DEBUG(" Storing emb hit");
                  m_embHitContainer->push_back(new LArHit(cellid, energyConv, hitTime));
                }
              else if(m_larEmID->is_em_endcap(cellid))
                {
                  ATH_MSG_DEBUG(" Storing em endcap hit");
                  m_emecHitContainer->push_back(new LArHit(cellid, energyConv, hitTime));
                }
            }
        }
      countFastCell++;
    }


  // ... then the LArHEC cells

  it1 = cellContainer->beginConstCalo(CaloCell_ID::LARHEC);
  it2 = cellContainer->endConstCalo(CaloCell_ID::LARHEC);

  countFastCell = 0;

  for(;it1!=it2;it1++)
    {
      ATH_MSG_DEBUG("FastCell LArHEC"<<countFastCell);

      cellid = (*it1)->ID();
      energy = (*it1)->energy();
      SampFrac = fSampl->FSAMPL(cellid);
      energyConv = energy*SampFrac;

      eLArHECRead += energy;
      eLArHECConv += energyConv;

      ATH_MSG_DEBUG("ReadFastCell id= "<<cellid<<"E= "<<energy<<"SampFrac= "<<SampFrac);

      if(energyConv > minEnergy)
        {
          if(m_larHecID->is_lar_hec(cellid))
            {
              ATH_MSG_DEBUG(" Storing hec hit");
              m_hecHitContainer->push_back(new LArHit(cellid, energyConv, hitTime));
            }
        }
      countFastCell++;
    }


  // ... then the LArFCAL cells

  it1 = cellContainer->beginConstCalo(CaloCell_ID::LARFCAL);
  it2 = cellContainer->endConstCalo(CaloCell_ID::LARFCAL);
  countFastCell = 0;
  for(;it1!=it2;it1++)
    {
      ATH_MSG_DEBUG("FastCell LArFCAL"<<countFastCell);
      cellid = (*it1)->ID();
      energy = (*it1)->energy();
      SampFrac = fSampl->FSAMPL(cellid);
      energyConv = energy*SampFrac;

      eLArHECRead += energy;
      eLArHECConv += energyConv;

      ATH_MSG_DEBUG("ReadFastCell id= "<<cellid<<"E="<<energy<<"SampFrac="<<SampFrac);
      if(energyConv > minEnergy)
        {
          if(m_larFcalID->is_lar_fcal(cellid)) // again double check the cell position
            {
              ATH_MSG_DEBUG(" Storing fcal hit");
              m_fcalHitContainer->push_back(new LArHit(cellid, energyConv, hitTime));
            }
        }
      countFastCell++;
    }


  // ... and finally the Tile cells

  it1 = cellContainer->beginConstCalo(CaloCell_ID::TILE);
  it2 = cellContainer->endConstCalo(CaloCell_ID::TILE);

  countFastCell = 0;

  Identifier pmt_id0( (unsigned int) 0);
  Identifier pmt_id1( (unsigned int) 0);

  for(;it1!=it2;it1++)
    {
      ATH_MSG_DEBUG("FastCell Tile Cal"<<countFastCell);
      cellid = (*it1)->ID();
      energy = (*it1)->energy();
      SampFrac = m_tileInfo->HitCalib(cellid);
      energyConv = energy/SampFrac;

      eTileRead += energy;
      eTileConv += energyConv;

      pmt_id0 = m_tileID->pmt_id(cellid, 0);
      pmt_id1 = m_tileID->pmt_id(cellid, 1);

      ATH_MSG_DEBUG("ReadFastCell id= "<<cellid<<"E="<<energy<<"SampFrac"<<SampFrac);
      ATH_MSG_DEBUG("PMT_id0="<<pmt_id0<<"PMT_id1="<<pmt_id1);
      if(energyConv > minEnergy)
        {

          if(m_tileID->is_tile(cellid))
            {
              ATH_MSG_DEBUG(" Storing tile hit");
              if (m_tileID->is_tile_gapscin(cellid))
                {
                  // only one PMT per cell in Tile Gap region....
                  m_tileHitVector->Emplace(pmt_id0, energyConv, hitTime);
                }
              else
                {
                  // ... two PMTs per cell everywhere else.
                  m_tileHitVector->Emplace(pmt_id0, energyConv/2.0, hitTime);
                  m_tileHitVector->Emplace(pmt_id1, energyConv/2.0, hitTime);
                }
            }
        }
      countFastCell++;
    }
  ATH_MSG_DEBUG("eReadTot= "<<eLArEMRead+eLArHECRead+eLArFCALRead+eTileRead);
  ATH_MSG_DEBUG("eLArEMRead= "<<eLArEMRead<<" eLArHECRead= "<<eLArHECRead<<" eLArFCALRead= "<<eLArFCALRead<<" eTileRead= "<<eTileRead);
  ATH_MSG_DEBUG("eConvTot= "<<eLArEMConv+eLArHECConv+eLArFCALConv+eTileConv);
  ATH_MSG_DEBUG("eLArEMConv= "<<eLArEMConv<<" eLArHECConv= "<<eLArHECConv<<"eLArFCALConv"<<eLArFCALConv<<"eTileConv"<<eTileConv);
  return StatusCode::SUCCESS;
}

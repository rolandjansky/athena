/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// FastHitConvertTool.cxx, (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////

#include "FastCaloSimHit/FastHitConvertTool.h"

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
#include "TileIdentifier/TileHWID.h"
#include "TileEvent/TileCellContainer.h"
#include "TileSimEvent/TileHit.h"
#include "TileSimEvent/TileHitVector.h"

//================ Constructor =================================================

FastHitConvertTool::FastHitConvertTool(const std::string& type,
                                       const std::string& name,
                                       const IInterface*  parent )
  :
  base_class(type,name,parent),
  m_storeGateFastCalo("StoreGateSvc/FastCalo",name)
{
}

//================ Destructor =================================================

// FastHitConvertTool::~FastHitConvertTool()
//{}


//================ Initialisation =================================================

StatusCode FastHitConvertTool::initialize()
{

  //Service for Pileup
  if(m_pileup)
    {
      CHECK(service("PileUpMergeSvc", m_pMergeSvc));
      CHECK(m_storeGateFastCalo.retrieve());
    }
  ATH_MSG_DEBUG("StoreGateFastCalo Svc structure at Initialisation"<<(*m_storeGateFastCalo).dump());
  const DataHandle<CaloIdManager> caloIdManager;
  CHECK(detStore()->retrieve(caloIdManager));
  m_larEmID=caloIdManager->getEM_ID();
  if(m_larEmID==0)
    throw std::runtime_error("FastHitConvertTool: Invalid LAr EM ID helper");
  m_larFcalID=caloIdManager->getFCAL_ID();
  if(m_larFcalID==0)
    throw std::runtime_error("FastHitConvertTool: Invalid FCAL ID helper");
  m_larHecID=caloIdManager->getHEC_ID();
  if(m_larHecID==0)
    throw std::runtime_error("FastHitConvertTool: Invalid HEC ID helper");
  m_tileID=caloIdManager->getTileID();
  if(m_tileID==0)
    throw std::runtime_error("FastHitConvertTool: Invalid Tile ID helper");

  // LAr and Tile Sampling Fractions
  ATH_CHECK(m_fSamplKey.initialize());

  ATH_CHECK(detStore()->retrieve(m_tileHWID));
  ATH_CHECK( m_tileSamplingFractionKey.initialize() );

  ATH_CHECK( m_tileCablingSvc.retrieve() );
  m_tileCabling = m_tileCablingSvc->cablingService();

  // Output keys
  ATH_CHECK(m_embHitContainerKey.initialize());
  ATH_CHECK(m_emecHitContainerKey.initialize());
  ATH_CHECK(m_fcalHitContainerKey.initialize());
  ATH_CHECK(m_hecHitContainerKey.initialize());
  ATH_CHECK(m_tileHitVectorKey.initialize());

  // Inputs for pileup (only initialized if m_pileup==true)
  ATH_CHECK(m_pileup_evt.initialize(m_pileup));
  ATH_CHECK(m_pileup_pOverEvent.initialize(m_pileup));

  return StatusCode::SUCCESS;
}

StatusCode FastHitConvertTool::process(CaloCellContainer* theCellCont, const EventContext& ctx) const
{
  ATH_MSG_DEBUG("initEvent()");
  auto embHitContainer  = SG::makeHandle<LArHitContainer>( m_embHitContainerKey, ctx );
  auto emecHitContainer = SG::makeHandle<LArHitContainer>( m_emecHitContainerKey, ctx );
  auto fcalHitContainer = SG::makeHandle<LArHitContainer>( m_fcalHitContainerKey, ctx );
  auto hecHitContainer  = SG::makeHandle<LArHitContainer>( m_hecHitContainerKey, ctx );
  auto tileHitVector    = SG::makeHandle<TileHitVector>(   m_tileHitVectorKey, ctx );
  embHitContainer  = std::make_unique<LArHitContainer>( m_embHitContainerKey.key() );
  emecHitContainer = std::make_unique<LArHitContainer>( m_emecHitContainerKey.key() );
  fcalHitContainer = std::make_unique<LArHitContainer>( m_fcalHitContainerKey.key() );
  hecHitContainer  = std::make_unique<LArHitContainer>( m_hecHitContainerKey.key() );
  tileHitVector    = std::make_unique<TileHitVector>(   m_tileHitVectorKey.key() );

  ATH_MSG_DEBUG("ATLFASTIIDigi "<<this->name()<<" hitConstruction");
  const double minEnergy=1e-9;
  double hitTime=0.0;
  Identifier cellid((unsigned int) 0);
  double energy=0.0;
  double energyConv=0.0;
  float SampFrac=0.0;

  int countFastCell=0;

  double eLArEMRead=0.0;
  double eLArHECRead=0.0;
  double eLArFCALRead=0.0;
  double eTileRead=0.0;

  double eLArEMConv=0.0;
  double eLArHECConv=0.0;
  double eLArFCALConv=0.0;
  double eTileConv=0.0;

  SG::ReadCondHandle<ILArfSampl> fSamplHdl(m_fSamplKey, ctx);
  const ILArfSampl* fSampl=*fSamplHdl;

  CaloCellContainer::const_iterator it1=theCellCont->beginConstCalo(CaloCell_ID::LAREM);
  CaloCellContainer::const_iterator it2=theCellCont->endConstCalo(CaloCell_ID::LAREM);
  for(;it1!=it2;it1++)
    {
      ATH_MSG_DEBUG("FastCell LArEM"<<countFastCell);

      cellid=(*it1)->ID();
      energy=(*it1)->energy();
      SampFrac=fSampl->FSAMPL(cellid);
      energyConv=energy*SampFrac;

      eLArEMRead+=energy;
      eLArEMConv+=energyConv;

      ATH_MSG_DEBUG("ReadFastCell id= "<<cellid<<"E= "<<energy<<" SampFrac= "<<SampFrac);
      if(energyConv > minEnergy)
        {
          if(m_larEmID->is_lar_em(cellid))
            {
              if(m_larEmID->is_em_barrel(cellid))
                {
                  ATH_MSG_DEBUG("Storing em barrel hit");
                  embHitContainer->push_back(new LArHit(cellid,energyConv,hitTime));
                }
              else if(m_larEmID->is_em_endcap(cellid))
                {
                  ATH_MSG_DEBUG("Storing em endcap hit");
                  emecHitContainer->push_back(new LArHit(cellid,energyConv,hitTime));
                }
            }
        }
      countFastCell++;
    }

  it1=theCellCont->beginConstCalo(CaloCell_ID::LARHEC);
  it2=theCellCont->endConstCalo(CaloCell_ID::LARHEC);

  countFastCell=0;
  for(;it1!=it2;it1++)
    {
      ATH_MSG_DEBUG("FastCell LArHEC"<<countFastCell);

      cellid=(*it1)->ID();
      energy=(*it1)->energy();
      SampFrac=fSampl->FSAMPL(cellid);
      energyConv=energy*SampFrac;

      eLArHECRead+=energy;
      eLArHECConv+=energyConv;

      ATH_MSG_DEBUG("ReadFastCell id= "<<cellid<<"E= "<<energy<<"SampFrac= "<<SampFrac);

      if(energyConv>minEnergy)
        {
          if(m_larHecID->is_lar_hec(cellid))
            {
              ATH_MSG_DEBUG("Storing hec hit");
              hecHitContainer->push_back(new LArHit(cellid,energyConv,hitTime));
            }
        }
      countFastCell++;
    }

  it1=theCellCont->beginConstCalo(CaloCell_ID::LARFCAL);
  it2=theCellCont->endConstCalo(CaloCell_ID::LARFCAL);
  countFastCell=0;
  for(;it1!=it2;it1++)
    {
      ATH_MSG_DEBUG("FastCell LArFCAL"<<countFastCell);
      cellid=(*it1)->ID();
      energy=(*it1)->energy();
      SampFrac=fSampl->FSAMPL(cellid);
      energyConv=energy*SampFrac;

      eLArHECRead+=energy;
      eLArHECConv+=energyConv;

      ATH_MSG_DEBUG("ReadFastCell id= "<<cellid<<"E="<<energy<<"SampFrac="<<SampFrac);
      if(energyConv>minEnergy)
        {
          if(m_larFcalID->is_lar_fcal(cellid))
            {
              ATH_MSG_DEBUG("Storing fcal hit");
              fcalHitContainer->push_back(new LArHit(cellid,energyConv,hitTime));
            }
        }
      countFastCell++;
    }

  SG::ReadCondHandle<TileSamplingFraction> tileSamplingFraction(m_tileSamplingFractionKey, ctx);
  ATH_CHECK( tileSamplingFraction.isValid() );

  it1=theCellCont->beginConstCalo(CaloCell_ID::TILE);
  it2=theCellCont->endConstCalo(CaloCell_ID::TILE);

  countFastCell=0;
  Identifier pmt_id0((unsigned int) 0);
  Identifier pmt_id1((unsigned int) 0);

  for(;it1!=it2;it1++)
    {
      ATH_MSG_DEBUG("FastCell Tile Cal"<<countFastCell);
      cellid=(*it1)->ID();
      energy=(*it1)->energy();

      pmt_id0=m_tileID->pmt_id(cellid,0);
      pmt_id1=m_tileID->pmt_id(cellid,1);

      HWIdentifier channel_id = m_tileCabling->s2h_channel_id(pmt_id0);
      int channel = m_tileHWID->channel(channel_id);
      int drawerIdx = m_tileHWID->drawerIdx(channel_id);
      SampFrac = tileSamplingFraction->getSamplingFraction(drawerIdx, channel);
      energyConv=energy/SampFrac;

      eTileRead+=energy;
      eTileConv+=energyConv;

      ATH_MSG_DEBUG("ReadFastCell id= "<<cellid<<"E="<<energy<<"SampFrac"<<SampFrac);
      ATH_MSG_DEBUG("PMT_id0="<<pmt_id0<<"PMT_id1="<<pmt_id1);
      if(energyConv>minEnergy)
        {
          if(m_tileID->is_tile(cellid))
            {
              ATH_MSG_DEBUG("Storing tile hit");
              if(m_tileID->is_tile_gapscin(cellid))
                {
                  tileHitVector->push_back(TileHit(pmt_id0,energyConv,hitTime));
                }
              else
                {
                  tileHitVector->push_back(TileHit(pmt_id0,energyConv/2.0,hitTime));
                  tileHitVector->push_back(TileHit(pmt_id1,energyConv/2.0,hitTime));
                }
            }
        }
      countFastCell++;
    }
  ATH_MSG_DEBUG("eReadTot= "<<eLArEMRead+eLArHECRead+eLArFCALRead+eTileRead);
  ATH_MSG_DEBUG("eLArEMRead= "<<eLArEMRead<<" eLArHECRead= "<<eLArHECRead<<" eLArFCALRead= "<<eLArFCALRead<<" eTileRead= "<<eTileRead);
  ATH_MSG_DEBUG("eConvTot= "<<eLArEMConv+eLArHECConv+eLArFCALConv+eTileConv);
  ATH_MSG_DEBUG("eLArEMConv= "<<eLArEMConv<<" eLArHECConv= "<<eLArHECConv<<"eLArFCALConv"<<eLArFCALConv<<"eTileConv"<<eTileConv);

  ATH_MSG_DEBUG("finaliseEvent()");
  ATH_MSG_DEBUG(embHitContainer.name()<<" : "<<embHitContainer->size()<<" hits ");
  ATH_MSG_DEBUG(emecHitContainer.name()<<" : "<<emecHitContainer->size()<<" hits ");
  ATH_MSG_DEBUG(fcalHitContainer.name()<<" : "<<fcalHitContainer->size()<<" hits ");
  ATH_MSG_DEBUG(hecHitContainer.name()<<" : "<<hecHitContainer->size()<<" hits ");
  ATH_MSG_DEBUG(tileHitVector.name()<<" : "<<tileHitVector->size()<<" hits ");

  if(m_pileup)
    {
      CHECK((*m_storeGateFastCalo).clearStore(true));
      auto evt = SG::makeHandle<EventInfo>(m_pileup_evt, ctx);
      const EventInfo* newEvt=new EventInfo(*evt);

      // Migration note: this was an evtStore()->retrieve for a non-const pointer
      // Using a const_cast to preserve the old behaviour, but this is MT-unfriendly
      if (ctx.slot() > 1) {
         ATH_MSG_ERROR ("FastHitConvertTool doesn't support pileup in AthenaMT");
         return StatusCode::FAILURE;
      }
      auto pOverEventHandle = SG::makeHandle<PileUpEventInfo>(m_pileup_pOverEvent, ctx);
      auto pOverEvent = const_cast<PileUpEventInfo*>(pOverEventHandle.get());
      pOverEvent->addSubEvt(0,PileUpTimeEventIndex::Signal,newEvt,&(*m_storeGateFastCalo));
    }
  return StatusCode::SUCCESS;
}

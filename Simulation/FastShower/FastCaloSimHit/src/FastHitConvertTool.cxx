/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
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
#include "CxxUtils/make_unique.h"
#include "EventInfo/PileUpEventInfo.h"
#include "EventInfo/EventType.h"
#include "LArSimEvent/LArHitFloat.h"
#include "LArSimEvent/LArHitFloatContainer.h"
#include "LArSimEvent/LArHit.h"
#include "LArSimEvent/LArHitContainer.h"
#include "PileUpTools/PileUpMergeSvc.h"
#include "StoreGate/StoreGate.h"
#include "TileConditions/TileInfo.h"
#include "TileEvent/TileCellContainer.h"
#include "TileSimEvent/TileHit.h"
#include "TileSimEvent/TileHitVector.h"

//================ Constructor =================================================

FastHitConvertTool::FastHitConvertTool(const std::string& type,
                                       const std::string& name,
                                       const IInterface*  parent )
  :
  base_class(type,name,parent),
  m_fastHitContainer(nullptr),
  m_fastTileHits(nullptr),
  m_embHitContainer("LArHitEMB"),
  m_emecHitContainer("LArHitEMEC"),
  m_fcalHitContainer("LArHitFCAL"),
  m_hecHitContainer("LArHitHEC"),
  m_tileHitVector("TileHitVec"),
  m_storeGateFastCalo("StoreGateSvc/FastCalo",name),
  m_pMergeSvc(nullptr),
  m_tileInfo(nullptr),
  m_larEmID(nullptr),
  m_larFcalID(nullptr),
  m_larHecID(nullptr),
  m_tileID(nullptr),
  m_pileup(false)
{
  declareProperty("doPileup",m_pileup,"Pileup mode (default=false)");
  declareProperty("embHitContainername",m_embHitContainer,"Name of output FastSim LAr EM Barrel Hit Container");
  declareProperty("emecHitContainername",m_emecHitContainer,"Name of output FastSim LAr EM Endcap Hit Container");
  declareProperty("fcalHitContainername",m_fcalHitContainer,"Name of output FastSim LAr FCAL Hit Container");
  declareProperty("hecHitContainername",m_hecHitContainer,"Name of output FastSim LAr HEC Hit Container");
  declareProperty("tileHitContainername",m_tileHitVector,"Name of output FastSim Tile Hit Container");

  //  template for property decalration
  //declareProperty("PropertyName", m_propertyName);
}

//================ Destructor =================================================

// FastHitConvertTool::~FastHitConvertTool()
//{}


//================ Initialisation =================================================

StatusCode  FastHitConvertTool::initialize()
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
  CHECK(detStore()->regHandle(m_dd_fSampl,"LArfSampl"));
  CHECK(detStore()->retrieve(m_tileInfo, "TileInfo"));

  return StatusCode::SUCCESS;
}

StatusCode  FastHitConvertTool::process(CaloCellContainer *theCellContainer)
{
  const EventContext& ctx = Gaudi::Hive::currentContext();
  if (ctx.slot() > 1) {
    ATH_MSG_ERROR ("FastHitConvertTool doesn't work with MT.");
    return StatusCode::FAILURE;
  }

  CHECK(this->initEvent());
  CHECK(this->hitConstruction(theCellContainer));
  CHECK(this->finaliseEvent());
  return StatusCode::SUCCESS;
}

StatusCode  FastHitConvertTool::initEvent()
{
  ATH_MSG_DEBUG("initEvent()");
  //  m_embHitContainer=new LArHitContainer();
  //  m_emecHitContainer=new LArHitContainer();
  //  m_fcalHitContainer=new LArHitContainer();
  //  m_hecHitContainer=new LArHitContainer();
  //  m_tileHitVector=new TileHitVector(m_tileHitContainerName);

  if(!m_embHitContainer.isValid() ) { m_embHitContainer  = CxxUtils::make_unique<LArHitContainer>(m_embHitContainer.name());}
  if(!m_emecHitContainer.isValid()) { m_emecHitContainer = CxxUtils::make_unique<LArHitContainer>(m_emecHitContainer.name());}
  if(!m_fcalHitContainer.isValid()) { m_fcalHitContainer = CxxUtils::make_unique<LArHitContainer>(m_fcalHitContainer.name());}
  if(!m_hecHitContainer.isValid() ) { m_hecHitContainer  = CxxUtils::make_unique<LArHitContainer>(m_hecHitContainer.name());}
  if(!m_tileHitVector.isValid()        ) { m_tileHitVector         = CxxUtils::make_unique<TileHitVector>(m_tileHitVector.name());}

  return StatusCode::SUCCESS;
}
StatusCode  FastHitConvertTool::finaliseEvent()
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
      const EventInfo *evt(nullptr);
      CHECK(evtStore()->retrieve(evt,"MyEvent"));
      const EventInfo* newEvt=new EventInfo(*evt);
      PileUpEventInfo *pOverEvent(nullptr);
      CHECK(evtStore()->retrieve(pOverEvent,"OverlayEvent"));
      pOverEvent->addSubEvt(0,PileUpTimeEventIndex::Signal,newEvt,&(*m_storeGateFastCalo));
    }
  return StatusCode::SUCCESS;
}

StatusCode FastHitConvertTool::hitConstruction(CaloCellContainer *theCellCont)
{
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

  CaloCellContainer::const_iterator it1=theCellCont->beginConstCalo(CaloCell_ID::LAREM);
  CaloCellContainer::const_iterator it2=theCellCont->endConstCalo(CaloCell_ID::LAREM);
  for(;it1!=it2;it1++)
    {
      ATH_MSG_DEBUG("FastCell LArEM"<<countFastCell);

      cellid=(*it1)->ID();
      energy=(*it1)->energy();
      SampFrac=m_dd_fSampl->FSAMPL(cellid);
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
                  m_embHitContainer->push_back(new LArHit(cellid,energyConv,hitTime));
                }
              else if(m_larEmID->is_em_endcap(cellid))
                {
                  ATH_MSG_DEBUG("Storing em endcap hit");
                  m_emecHitContainer->push_back(new LArHit(cellid,energyConv,hitTime));
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
      SampFrac=m_dd_fSampl->FSAMPL(cellid);
      energyConv=energy*SampFrac;

      eLArHECRead+=energy;
      eLArHECConv+=energyConv;

      ATH_MSG_DEBUG("ReadFastCell id= "<<cellid<<"E= "<<energy<<"SampFrac= "<<SampFrac);

      if(energyConv>minEnergy)
        {
          if(m_larHecID->is_lar_hec(cellid))
            {
              ATH_MSG_DEBUG("Storing hec hit");
              m_hecHitContainer->push_back(new LArHit(cellid,energyConv,hitTime));
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
      SampFrac=m_dd_fSampl->FSAMPL(cellid);
      energyConv=energy*SampFrac;

      eLArHECRead+=energy;
      eLArHECConv+=energyConv;

      ATH_MSG_DEBUG("ReadFastCell id= "<<cellid<<"E="<<energy<<"SampFrac="<<SampFrac);
      if(energyConv>minEnergy)
        {
          if(m_larFcalID->is_lar_fcal(cellid))
            {
              ATH_MSG_DEBUG("Storing fcal hit");
              m_fcalHitContainer->push_back(new LArHit(cellid,energyConv,hitTime));
            }
        }
      countFastCell++;
    }

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
      SampFrac=m_tileInfo->HitCalib(cellid);
      energyConv=energy/SampFrac;

      eTileRead+=energy;
      eTileConv+=energyConv;

      pmt_id0=m_tileID->pmt_id(cellid,0);
      pmt_id1=m_tileID->pmt_id(cellid,1);

      ATH_MSG_DEBUG("ReadFastCell id= "<<cellid<<"E="<<energy<<"SampFrac"<<SampFrac);
      ATH_MSG_DEBUG("PMT_id0="<<pmt_id0<<"PMT_id1="<<pmt_id1);
      if(energyConv>minEnergy)
        {
          if(m_tileID->is_tile(cellid))
            {
              ATH_MSG_DEBUG("Storing tile hit");
              if(m_tileID->is_tile_gapscin(cellid))
                {
                  m_tileHitVector->push_back(TileHit(pmt_id0,energyConv,hitTime));
                }
              else
                {
                  m_tileHitVector->push_back(TileHit(pmt_id0,energyConv/2.0,hitTime));
                  m_tileHitVector->push_back(TileHit(pmt_id1,energyConv/2.0,hitTime));
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

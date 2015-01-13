/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// FastHitConvertTool.cxx, (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////

#include "FastCaloSimHit/FastHitConvertTool.h"
#include "GaudiKernel/AlgFactory.h"
#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/Property.h"
#include "GaudiKernel/IDataProviderSvc.h"
#include "GaudiKernel/IMessageSvc.h"
#include "GaudiKernel/ISvcLocator.h"
#include "StoreGate/StoreGateSvc.h"
#include "PileUpTools/PileUpMergeSvc.h"

#include "EventInfo/PileUpEventInfo.h"
#include "EventInfo/EventType.h"

#include "LArSimEvent/LArHitFloat.h"
#include "LArSimEvent/LArHitFloatContainer.h"
#include "LArSimEvent/LArHit.h"
#include "LArSimEvent/LArHitContainer.h"

#include "TileSimEvent/TileHit.h"
#include "TileSimEvent/TileHitVector.h"

#include "CaloIdentifier/CaloIdManager.h"
#include "CaloIdentifier/LArEM_ID.h"
#include "CaloIdentifier/LArFCAL_ID.h"
#include "CaloIdentifier/LArHEC_ID.h"
#include "CaloIdentifier/TileID.h"
#include "TileConditions/TileInfo.h"

#include "LArG4RunControl/LArG4GlobalOptions.h"

#include "CaloEvent/CaloCellContainer.h"
#include "TileEvent/TileCellContainer.h"

//================ Constructor =================================================

FastHitConvertTool::FastHitConvertTool(const std::string& type,
                                       const std::string& name,
                                       const IInterface*  parent )
  :
  AthAlgTool(type,name,parent),
  fastHitContainer(0),
  fastTileHits(0),
  embHitContainer(0),
  emecHitContainer(0),
  fcalHitContainer(0),
  hecHitContainer(0),
  m_tileHits(0),
  m_caloCellsOutputName(""),
  m_storeGate(0),
  m_detStore(0),
  m_storeGateFastCalo("StoreGateSvc/FastCalo",name),
  m_pMergeSvc(0),
  m_tileInfo(0),
  m_larEmID(0),
  m_larFcalID(0),
  m_larHecID(0),
  m_tileID(0)
{
  declareInterface<ICaloCellMakerTool>(this);
  m_pileup=false;
  m_embHitContainerName="LArHitEMB";
  m_emecHitContainerName="LArHitEMEC";
  m_fcalHitContainerName="LArHitFCAL";
  m_hecHitContainerName="LArHitHEC";
  m_tileHitContainerName="TileHitVec";

  declareProperty("doPileup",m_pileup,"Pileup mode (default=false)");
  declareProperty("embHitContainername",m_embHitContainerName,"Name of output FastSim LAr EM Barrel Hit Container");
  declareProperty("emecHitContainername",m_emecHitContainerName,"Name of output FastSim LAr EM Endcap Hit Container");
  declareProperty("fcalHitContainername",m_fcalHitContainerName,"Name of output FastSim LAr FCAL Hit Container");
  declareProperty("hecHitContainername",m_hecHitContainerName,"Name of output FastSim LAr HEC Hit Container");
  declareProperty("tileHitContainername",m_tileHitContainerName,"Name of output FastSim Tile Hit Container");
  declareProperty("CaloCellsInputName",m_caloCellsOutputName);

  //  template for property decalration
  //declareProperty("PropertyName", m_propertyName);
}

//================ Destructor =================================================

// FastHitConvertTool::~FastHitConvertTool()
//{}


//================ Initialisation =================================================

StatusCode  FastHitConvertTool::initialize()
{


  StatusCode sc=service("StoreGateSvc",m_storeGate);
  if (sc.isFailure())
    {
      ATH_MSG_FATAL("StoreGateSvc not found");
      return StatusCode::FAILURE;
    }

  sc=service("DetectorStore",m_detStore);
  if(sc.isFailure())
    {
      ATH_MSG_FATAL("Unable to get pointer to DetectorStore service");
      return StatusCode::FAILURE;
    }
  else ATH_MSG_INFO("DetectorStore Svc initialized");
  //  ATH_MSG_INFO("DetectorStore Svc structure at Initialisation:\n"<<m_detStore->dump());

  //Service for Pileup
  if(m_pileup)
    {
      sc=service("PileUpMergeSvc",m_pMergeSvc);
      if(sc.isFailure())
        {
          ATH_MSG_ERROR("initialize: Can not find PileUpMergeSvc");
          return StatusCode::FAILURE;
        }
      sc=m_storeGateFastCalo.retrieve();
      if(sc.isFailure())
        {
          ATH_MSG_FATAL("StoreGateFastCalo not found!");
          return StatusCode::FAILURE;
        }
    }
  ATH_MSG_DEBUG("StoreGateFastCalo Svc structure at Initialisation"<<(*m_storeGateFastCalo).dump());
  const DataHandle<CaloIdManager> caloIdManager;
  sc=m_detStore->retrieve(caloIdManager);
  if(sc.isSuccess())
    ATH_MSG_DEBUG("CaloIDManager retrieved.");
  else
    throw std::runtime_error("FastHitConvertTool: Unable to retrieve CaloIDManeger");
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
  sc=m_detStore->regHandle(m_dd_fSampl,"LArfSampl");
  if(sc.isFailure())
    {
      ATH_MSG_ERROR("Unable to register handle for LArfSampl");
      return StatusCode::FAILURE;
    }
  else ATH_MSG_DEBUG("Got the register handle for LArfSampl");
  sc=m_detStore->retrieve(m_tileInfo,"TileInfo");
  if(sc.isFailure())
    {
      ATH_MSG_ERROR("Unable to retrieve TileInfo from DetectorStore");
      return StatusCode::FAILURE;
    }
  ATH_MSG_INFO ("initialize() successful in " << name());

  sc = AlgTool::initialize();
  if (sc.isFailure()) return StatusCode::FAILURE;

  return StatusCode::SUCCESS;
}

StatusCode  FastHitConvertTool::process(CaloCellContainer *theCellContainer)
{
  if(initEvent().isFailure())
    {
      ATH_MSG_FATAL("initEvent() Failed");
      return StatusCode::FAILURE;
    }
  if(hitConstruction(theCellContainer).isFailure())
    {
      ATH_MSG_FATAL("hitConstruction() Failed");
      return StatusCode::FAILURE;
    }
  if(finaliseEvent().isFailure())
    {
      ATH_MSG_FATAL("finaliseEvent() Failed");
      return StatusCode::FAILURE;
    }
  return StatusCode::SUCCESS;
}

StatusCode  FastHitConvertTool::initEvent()
{
  ATH_MSG_DEBUG("initEvent()");
  //  embHitContainer=new LArHitContainer();
  //  emecHitContainer=new LArHitContainer();
  //  fcalHitContainer=new LArHitContainer();
  //  hecHitContainer=new LArHitContainer();
  //  m_tileHits=new TileHitVector(m_tileHitContainerName);

  if(!(m_storeGate->retrieve(embHitContainer,m_embHitContainerName)))
    {
      ATH_MSG_ERROR("Could not retrieve embHitContainer");
    }
  if(!(m_storeGate->retrieve(emecHitContainer,m_emecHitContainerName)))
    {
      ATH_MSG_ERROR("Could not retrieve emecHitContainer");
    }
  if(!(m_storeGate->retrieve(fcalHitContainer,m_fcalHitContainerName)))
    {
      ATH_MSG_ERROR("Could not retrieve fcalHitContainer");
    }
  if(!(m_storeGate->retrieve(hecHitContainer,m_hecHitContainerName)))
    {
      ATH_MSG_ERROR("Could not retrieve hecHitContainer");
    }
  if(!(m_storeGate->retrieve(m_tileHits,m_tileHitContainerName)))
    {
      ATH_MSG_ERROR("Could not retrieve tileHitContainer");
    }

  return StatusCode::SUCCESS;
}
StatusCode  FastHitConvertTool::finaliseEvent()
{
  ATH_MSG_DEBUG("finaliseEvent()");
  ATH_MSG_DEBUG(m_embHitContainerName<<" : "<<embHitContainer->size()<<" hits ");
  ATH_MSG_DEBUG(m_emecHitContainerName<<" : "<<emecHitContainer->size()<<" hits ");
  ATH_MSG_DEBUG(m_fcalHitContainerName<<" : "<<fcalHitContainer->size()<<" hits ");
  ATH_MSG_DEBUG(m_hecHitContainerName<<" : "<<hecHitContainer->size()<<" hits ");
  ATH_MSG_DEBUG(m_tileHitContainerName<<" : "<<m_tileHits->size()<<" hits ");

  if(m_pileup)
    {
      if((*m_storeGateFastCalo).clearStore(true).isFailure())
        {
          ATH_MSG_ERROR("Clear of FastCalo data store failed");
          return StatusCode::FAILURE;
        }
      const EventInfo *evt=0;
      if(m_storeGate->retrieve(evt,"MyEvent").isFailure())
        {
          ATH_MSG_ERROR("Could not get event info");
          return StatusCode::FAILURE;
        }
      else ATH_MSG_DEBUG("Got the event info");
      const EventInfo* newEvt=new EventInfo(*evt);
      PileUpEventInfo *pOverEvent(0);
      if(m_storeGate->retrieve(pOverEvent,"OverlayEvent").isFailure())
        {
          ATH_MSG_ERROR("Could not get OverlayEvent");
          return StatusCode::FAILURE;
        }
      else
        {
          ATH_MSG_DEBUG("Got the OverlayEvent");
        }
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
      //    ATH_MSG_INFO("cellid="<<cellid);
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
              hecHitContainer->push_back(new LArHit(cellid,energyConv,hitTime));
            }
        }
    }
  countFastCell++;

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
              fcalHitContainer->push_back(new LArHit(cellid,energyConv,hitTime));
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
                  m_tileHits->push_back(TileHit(pmt_id0,energyConv,hitTime));
                }
              else
                {
                  m_tileHits->push_back(TileHit(pmt_id0,energyConv/2.0,hitTime));
                  m_tileHits->push_back(TileHit(pmt_id1,energyConv/2.0,hitTime));
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
//================ Finalisation =================================================

StatusCode  FastHitConvertTool::finalize()
{
  StatusCode sc = AlgTool::finalize();
  return sc;
}

//============================================================================================

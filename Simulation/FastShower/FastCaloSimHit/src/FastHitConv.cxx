/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "FastCaloSimHit/FastHitConv.h"

#include "GaudiKernel/AlgFactory.h"
#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/Property.h"
#include "GaudiKernel/IDataProviderSvc.h"
#include "GaudiKernel/IMessageSvc.h"
#include "GaudiKernel/ISvcLocator.h"

#include "StoreGate/StoreGate.h"

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


// Constructor
FastHitConv::FastHitConv(const std::string& name, ISvcLocator* pSvcLocator ) :
  AthAlgorithm(name, pSvcLocator),
  fastHitCont(NULL),
  fastTileHits(NULL),
  embHitCont(NULL),
  emecHitCont(NULL),
  fcalHitCont(NULL),
  hecHitCont(NULL),
  m_tileHits(NULL),
  m_caloCellsOutputName("AllFastCalo"),
  m_msgSvc(NULL),
  //m_storeGate(NULL),
  //m_detStore(NULL),
  m_storeGateFastCalo("StoreGateSvc/FastCalo", name),
  m_pMergeSvc(NULL),
  m_tileInfo(NULL),
  m_larEmID(NULL),
  m_larFcalID(NULL),
  m_larHecID(NULL),
  m_tileID(NULL)
{

  m_pileup = false;

  m_embHitContName  = "LArHitEMB_Fast";
  m_emecHitContName = "LArHitEMEC_Fast";
  m_fcalHitContName = "LArHitFCAL_Fast";
  m_hecHitContName  = "LArHitHEC_Fast";
  m_tileHitContName = "TileHitVec_Fast";

  declareProperty("doPileup", m_pileup, "Pileup mode (default=false)");

  declareProperty("embHitContName",  m_embHitContName,  "Name of output FastSim LAr EM Barrel Hit Container");
  declareProperty("emecHitContName", m_emecHitContName, "Name of output FastSim LAr EM Endcap Hit Container");
  declareProperty("fcalHitContName", m_fcalHitContName, "Name of output FastSim LAr FCAL Hit Container");
  declareProperty("hecHitContName",  m_hecHitContName,  "Name of output FastSim LAr HEC Hit Container");
  declareProperty("tileHitContName", m_tileHitContName, "Name of output FastSim Tile Hit Container");
  declareProperty("CaloCellsInputName",m_caloCellsOutputName) ;
}

StatusCode FastHitConv::initialize(){

  //MsgStream log(msgSvc(), name());
  //log << MSG::INFO << "initialize()" << endreq;
  msg(MSG::INFO) <<"initialize()"<<endreq;
  
  /*
  StatusCode status = service("StoreGateSvc", m_storeGate);

  if(status.isFailure()){
    msg(MSG::FATAL) <<"StoreGateSvc not found!"<<endreq;
    //log << MSG::FATAL << "StoreGateSvc not found!" << endreq;
    return StatusCode::FAILURE;
  }
  */
  StatusCode status = service("MessageSvc", m_msgSvc);
  if (status.isFailure()) {
    msg(MSG::FATAL) <<"MessageSvc not found!" << endreq; 
    //log << MSG::FATAL << "MessageSvc not found!" << endreq;
    return status;
  }
  else {
    if (msgLvl(MSG::DEBUG)) msg(MSG::DEBUG) <<"MessageSvc initialized." << endreq; 
    //log << MSG::DEBUG << "MessageSvc initialized." << endreq;
  }
  /*
  status = service("DetectorStore", m_detStore);
  if (status.isFailure()) {
    msg(MSG::FATAL) << "Unable to get pointer to DetectorStore Service" << endreq; 
    //log << MSG::FATAL << "Unable to get pointer to DetectorStore Service" << endreq;
    return status;
  }
  else {
    if (msgLvl(MSG::DEBUG)) msg(MSG::DEBUG) << "DetectorStore Svc initialized." << endreq;
    //log << MSG::DEBUG << "DetectorStore Svc initialized." << endreq;
  }
  
  if (msgLvl(MSG::DEBUG)) msg(MSG::DEBUG) << " DetectorStore Svc structure at initilaisation: \n "
					  << m_detStore->dump() << endreq;
  //log << MSG::DEBUG << " DetectorStore Svc structure at initilaisation: \n "
  //    << m_detStore->dump() << endreq;
  */
  
  // Services needed for Pileup
  
  if(m_pileup){
    
    status = service("PileUpMergeSvc", m_pMergeSvc);
    if (status.isFailure()) {
      msg(MSG::ERROR) << "initialize: Can not find PileUpMergeSvc" << endreq;
      //log << MSG::ERROR << "initialize: Can not find PileUpMergeSvc" << endreq;
      return StatusCode::FAILURE;
    }
    
    status = m_storeGateFastCalo.retrieve();
    if(status.isFailure()){
      msg(MSG::FATAL) << "StoreGateFastCalo not found!" << endreq;
      //log << MSG::FATAL << "StoreGateFastCalo not found!" << endreq;
      return StatusCode::FAILURE;
    }
    
  }

  if (msgLvl(MSG::DEBUG)) msg(MSG::DEBUG) << " StoreGateFastCalo Svc structure at initilaisation: \n "
					  << (*m_storeGateFastCalo).dump() << endreq;
  //log << MSG::DEBUG << " StoreGateFastCalo Svc structure at initilaisation: \n "
  //    << (*m_storeGateFastCalo).dump() << endreq;
  
  
  // IDs for different calo components
  
  const DataHandle<CaloIdManager> caloIdManager;
  
  status = detStore()->retrieve(caloIdManager);
  if(status.isSuccess()) {
    if (msgLvl(MSG::DEBUG)) msg(MSG::DEBUG) << "CaloIDManager retrieved!" <<endreq; 
    //log << MSG::DEBUG << "CaloIDManager retrieved!" <<endreq;
  }  
  else {
    throw std::runtime_error("FastHitConv: Unable to retrieve CaloIDManager");
  }

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
  
  status = detStore()->regHandle(m_dd_fSampl,"LArfSampl");
  if (status.isFailure()) {
    msg(MSG::ERROR) << " Unable to register handle for LArfSampl" << endreq;
    //log << MSG::ERROR << " Unable to register handle for LArfSampl" << endreq;
    return StatusCode::FAILURE;
  } else {
    if (msgLvl(MSG::DEBUG)) msg(MSG::DEBUG) << " Got the register handle for LArfSampl" << endreq;
    //log << MSG::DEBUG << " Got the register handle for LArfSampl" << endreq;
  }
  
  status = detStore()->retrieve(m_tileInfo, "TileInfo");
  if (status.isFailure()) {
    msg(MSG::ERROR) << "Unable to retrieve TileInfo from DetectorStore" << endreq;
    //log << MSG::ERROR
    //  << "Unable to retrieve TileInfo from DetectorStore" << endreq;
    return status;
  }
    
  return StatusCode::SUCCESS;
}
  
StatusCode FastHitConv::execute(){

  //MsgStream log(msgSvc(), name());
  //log << MSG::DEBUG << "execute()" << endreq;
  ATH_MSG_DEBUG("execute()");

  // run hit conversion only if calo cell container exists on storegate
  if ( evtStore()->contains<CaloCellContainer>(m_caloCellsOutputName)) {
    //log << MSG::DEBUG << "CaloCellContainer found on StoreGate. Will run fast hit conversion." << endreq;
    ATH_MSG_DEBUG("CaloCellContainer found on StoreGate. Will run fast hit conversion.");
    if( initEvent().isFailure()){
      ATH_MSG_FATAL("initEvent() Failed");
      //log << MSG::FATAL << "initEvent() Failed" << endreq;
      return StatusCode::FAILURE;
    }

    if( hitConstruction().isFailure()){
      ATH_MSG_FATAL("hitConstruction() Failed");
      //log << MSG::FATAL << "hitConstruction() Failed" << endreq;
      return StatusCode::FAILURE;
    }

    if( finaliseEvent().isFailure()){
      ATH_MSG_FATAL("finaliseEvent() Failed");
      //log << MSG::FATAL << "finaliseEvent() Failed" << endreq;
      return StatusCode::FAILURE;
    }

  } else {
    ATH_MSG_DEBUG("No CaloCellContainer found on Storegate. Skipping fast hit conversion.");
    //log << MSG::DEBUG << "No CaloCellContainer found on Storegate. Skipping fast hit conversion." << endreq;
  }

  return StatusCode::SUCCESS;
}

StatusCode FastHitConv::initEvent(){

  //MsgStream log(msgSvc(), name());
  //log << MSG::DEBUG << "initEvent()" << endreq;
  ATH_MSG_DEBUG("initEvent()");
  embHitCont  = new LArHitContainer();
  emecHitCont = new LArHitContainer();
  fcalHitCont = new LArHitContainer();
  hecHitCont  = new LArHitContainer();

  m_tileHits = new TileHitVector(m_tileHitContName);

  return StatusCode::SUCCESS;

}

StatusCode FastHitConv::finaliseEvent() {
  /*
  MsgStream log(msgSvc(), name());
  log << MSG::DEBUG << "finaliseEvent()" << endreq;

  log << MSG::DEBUG << " StoreGate structure at end of event: \n "
      << evtStore()->dump() << endreq;

  log << MSG::DEBUG << " Storing HitContainers:" << endreq;
  log << MSG::DEBUG << m_embHitContName  << " : " << embHitCont->size()  << " hits " << endreq;
  log << MSG::DEBUG << m_emecHitContName << " : " << emecHitCont->size() << " hits " << endreq;
  log << MSG::DEBUG << m_fcalHitContName << " : " << fcalHitCont->size() << " hits " << endreq;
  log << MSG::DEBUG << m_hecHitContName  << " : " << hecHitCont->size()  << " hits " << endreq;
  log << MSG::DEBUG << m_tileHitContName << " : " << m_tileHits->Size()  << " hits " << endreq;
  */
  if (msgLvl(MSG::DEBUG)) msg(MSG::DEBUG) << "finaliseEvent()" << endreq
					  << " StoreGate structure at end of event: \n " << evtStore()->dump() << endreq
					  << " Storing HitContainers:" << endreq
					  << m_embHitContName  << " : " << embHitCont->size()  << " hits " << endreq
					  << m_emecHitContName << " : " << emecHitCont->size() << " hits " << endreq
					  << m_fcalHitContName << " : " << fcalHitCont->size() << " hits " << endreq
					  << m_hecHitContName  << " : " << hecHitCont->size()  << " hits " << endreq
					  << m_tileHitContName << " : " << m_tileHits->Size()  << " hits " << endreq;


  if(m_pileup){//pileup or not?

    // pileup case

    if ((*m_storeGateFastCalo).clearStore(true).isFailure()) {
      ATH_MSG_ERROR("Clear of FastCalo data store failed");
      //log << MSG::ERROR << "Clear of FastCalo data store failed" << endreq;
      return StatusCode::FAILURE;
    }

    if (msgLvl(MSG::DEBUG)) msg(MSG::DEBUG) << " StoreGateFastCalo structure at end of event (before recording collections): \n "
					    << (*m_storeGateFastCalo).dump() << endreq;
    //log << MSG::DEBUG << " StoreGateFastCalo structure at end of event (before recording collections): \n "
    //   << (*m_storeGateFastCalo).dump() << endreq;

    if((*m_storeGateFastCalo).record(embHitCont,  m_embHitContName).isFailure())  msg(MSG::ERROR) << "SG record failed: embHitCont"<< endreq;;
    if((*m_storeGateFastCalo).record(emecHitCont, m_emecHitContName).isFailure()) msg(MSG::ERROR) << "SG record failed: emecHitCont"<< endreq;;
    if((*m_storeGateFastCalo).record(fcalHitCont, m_fcalHitContName).isFailure()) msg(MSG::ERROR) << "SG record failed: fcalHitCont"<< endreq;;
    if((*m_storeGateFastCalo).record(hecHitCont,  m_hecHitContName).isFailure())  msg(MSG::ERROR) << "SG record failed: embHitCont"<< endreq;;
    if((*m_storeGateFastCalo).record(m_tileHits,  m_tileHitContName).isFailure()) msg(MSG::ERROR) << "SG record failed: m_tileHits"<< endreq;;
    if (msgLvl(MSG::DEBUG)) msg(MSG::DEBUG) << " StoreGateFastCalo structure at end of event (after recording collections): \n "
					    << (*m_storeGateFastCalo).dump() << endreq;
    //log << MSG::DEBUG << " StoreGateFastCalo structure at end of event (after recording collections): \n "
    //    << (*m_storeGateFastCalo).dump() << endreq;


    const EventInfo* evt = 0;
    if ( evtStore()->retrieve( evt, "MyEvent" ).isFailure() ) {
      ATH_MSG_ERROR("  Could not get event info");
      return StatusCode::FAILURE;
    }else{
      ATH_MSG_DEBUG("  Got the event info");
      //log << MSG::DEBUG << "  Got the event info" << endreq;
    }

    const EventInfo* newEvt = new EventInfo(*evt);


    PileUpEventInfo* pOverEvent(0);
    if ( evtStore()->retrieve( pOverEvent, "OverlayEvent" ).isFailure() ) {
      msg(MSG::ERROR) << "  Could not get OverlayEvent" << endreq;
      return StatusCode::FAILURE;
    }else {
      ATH_MSG_DEBUG("  Got the OverlayEvent");
      //log << MSG::DEBUG << "  Got the OverlayEvent" << endreq;
    }

    pOverEvent->addSubEvt(0, PileUpTimeEventIndex::Signal, newEvt, &(*m_storeGateFastCalo));
    if (msgLvl(MSG::DEBUG)) msg(MSG::DEBUG) << " StoreGate structure at end of event (after recording collections): \n "
					    << evtStore()->dump() << endreq;
    //log << MSG::DEBUG << " StoreGate structure at end of event (after recording collections): \n "
    //    << evtStore()->dump() << endreq;

  } else {

    // standard (non-pileup) case

    //     helper.ExportCollection< LArHitContainer >(embHitCont);
    //     helper.ExportCollection< LArHitContainer >(emecHitCont);
    //     helper.ExportCollection< LArHitContainer >(fcalHitCont);
    //     helper.ExportCollection< LArHitContainer >(hecHitCont);

    if(evtStore()->record(embHitCont,  m_embHitContName).isFailure()) msg(MSG::ERROR) << "SG record failed: embHitCont"<< endreq;
    if(evtStore()->record(emecHitCont, m_emecHitContName).isFailure()) msg(MSG::ERROR) << "SG record failed: emecHitCont"<< endreq;
    if(evtStore()->record(fcalHitCont, m_fcalHitContName).isFailure()) msg(MSG::ERROR) << "SG record failed: fcalHitCont"<< endreq;
    if(evtStore()->record(hecHitCont,  m_hecHitContName).isFailure()) msg(MSG::ERROR) << "SG record failed: hecHitCont"<< endreq;
    if(evtStore()->record(m_tileHits,  m_tileHitContName).isFailure()) msg(MSG::ERROR) << "SG record failed: m_tileHits"<< endreq;
    
    if (msgLvl(MSG::DEBUG)) msg(MSG::DEBUG) << " StoreGate structure at end of event (after recording collections): \n "
					    << evtStore()->dump() << endreq;
    //log << MSG::DEBUG << " StoreGate structure at end of event (after recording collections): \n "
    //    << evtStore()->dump() << endreq;

  }

  return StatusCode::SUCCESS;

}


StatusCode FastHitConv::hitConstruction(){

  //
  // - Read in hits from FastCaloSim and convert them to HitCollections
  // - Need to convert energy scale using sampling fraction
  //        for LAr e = cellenergy * samplingfrac
  //        for tile e = cellenergy / samplingfrac
  //

  //MsgStream log( this->messageService(), this->name() );
  //log << MSG::DEBUG << " ATLFAST2Digi " << this->name() << " hitConstrution" << endreq ;
  if (msgLvl(MSG::DEBUG)) msg(MSG::DEBUG) << " ATLFAST2Digi " << this->name() << " hitConstrution" << endreq ; 

  const double minEnergy = 1e-9;


  // Read back FastCaloSim cells

  const CaloCellContainer* cellContainer;
  if (!evtStore()->retrieve(cellContainer, m_caloCellsOutputName)) {
    ATH_MSG_WARNING("Could not retrieve Calorimeter Cells ");
    //log << MSG::WARNING  << "Could not retrieve Calorimeter Cells " << endreq;
    return StatusCode::FAILURE;
  }

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


  // First do the LArEM cells

  CaloCellContainer::const_iterator it1 = cellContainer->beginConstCalo(CaloCell_ID::LAREM);
  CaloCellContainer::const_iterator it2 = cellContainer->endConstCalo(CaloCell_ID::LAREM);

  for(;it1!=it2;it1++){
    if (msgLvl(MSG::DEBUG)) msg(MSG::DEBUG) << " FastCell LArEM " << countFastCell << endreq ; 
    //log << MSG::DEBUG << " FastCell LArEM " << countFastCell << endreq ;

    cellid = (*it1)->ID();
    energy = (*it1)->energy();
    SampFrac = m_dd_fSampl->FSAMPL(cellid);
    energyConv = energy*SampFrac;

    eLArEMRead += energy;
    eLArEMConv += energyConv;
    if (msgLvl(MSG::DEBUG)) msg(MSG::DEBUG) << " ReadFastCell id =  " << cellid << " E = " << energy << " SampFrac = " << SampFrac << endreq ;
    //log << MSG::DEBUG << " ReadFastCell id =  " << cellid << " E = " << energy << " SampFrac = " << SampFrac << endreq ;

    if(energyConv > minEnergy){

      if(m_larEmID->is_lar_em(cellid)) // Belt and Braces check that it's really an LArEM cell
        {
          if(m_larEmID->is_em_barrel(cellid)) // Both EM Barrel and EM EndCap in container -> separate them
            {
	      ATH_MSG_DEBUG(" Storing emb hit");
	      //log << MSG::DEBUG << " Storing emb hit" << endreq;
              embHitCont->push_back(new LArHit(cellid, energyConv, hitTime));
            }
          else if(m_larEmID->is_em_endcap(cellid))
            {
	      ATH_MSG_DEBUG(" Storing em endcap hit");
	      //log << MSG::DEBUG << " Storing em endcap hit" << endreq;
              emecHitCont->push_back(new LArHit(cellid, energyConv, hitTime));
            }
        }
    }

    countFastCell++;

  }


  // ... then the LArHEC cells

  it1 = cellContainer->beginConstCalo(CaloCell_ID::LARHEC);
  it2 = cellContainer->endConstCalo(CaloCell_ID::LARHEC);

  countFastCell = 0;

  for(;it1!=it2;it1++){
    if (msgLvl(MSG::DEBUG)) msg(MSG::DEBUG) << " FastCell LArHEC " << countFastCell << endreq ;
    //log << MSG::DEBUG << " FastCell LArHEC " << countFastCell << endreq ;

    cellid = (*it1)->ID();
    energy = (*it1)->energy();
    SampFrac = m_dd_fSampl->FSAMPL(cellid);
    energyConv = energy*SampFrac;

    eLArHECRead += energy;
    eLArHECConv += energyConv;


    if (msgLvl(MSG::DEBUG)) msg(MSG::DEBUG) << " ReadFastCell id =  " << cellid << " E = " << energy << " SampFrac = " << SampFrac << endreq ;
    //    log << MSG::DEBUG << " ReadFastCell id =  " << cellid << " E = " << energy << " SampFrac = " << SampFrac << endreq ;

    if(energyConv > minEnergy){

      if(m_larHecID->is_lar_hec(cellid))
        {
	  ATH_MSG_DEBUG(" Storing hec hit");
	  //log << MSG::DEBUG << " Storing hec hit" << endreq;
          hecHitCont->push_back(new LArHit(cellid, energyConv, hitTime));
        }
    }

    countFastCell++;

  }


  // ... then the LArFCAL cells

  it1 = cellContainer->beginConstCalo(CaloCell_ID::LARFCAL);
  it2 = cellContainer->endConstCalo(CaloCell_ID::LARFCAL);

  countFastCell = 0;

  for(;it1!=it2;it1++){
    if (msgLvl(MSG::DEBUG)) msg(MSG::DEBUG)<< " FastCell LArFCAL " << countFastCell << endreq ;
    //log << MSG::DEBUG << " FastCell LArFCAL " << countFastCell << endreq ;

    cellid = (*it1)->ID();
    energy = (*it1)->energy();
    SampFrac = m_dd_fSampl->FSAMPL(cellid);
    energyConv = energy*SampFrac;

    eLArHECRead += energy;
    eLArHECConv += energyConv;
    if (msgLvl(MSG::DEBUG)) msg(MSG::DEBUG)<< " ReadFastCell id =  " << cellid << " E = " << energy << " SampFrac = " << SampFrac << endreq ;
    //log << MSG::DEBUG << " ReadFastCell id =  " << cellid << " E = " << energy << " SampFrac = " << SampFrac << endreq ;

    if(energyConv > minEnergy){

      if(m_larFcalID->is_lar_fcal(cellid)) // again double check the cell position
        {
	  ATH_MSG_DEBUG(" Storing fcal hit");
	  //log << MSG::DEBUG << " Storing fcal hit" << endreq ;
          fcalHitCont->push_back(new LArHit(cellid, energyConv, hitTime));
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

  for(;it1!=it2;it1++){
    if (msgLvl(MSG::DEBUG)) msg(MSG::DEBUG)<< " FastCell Tile Cal " << countFastCell << endreq ;
    //log << MSG::DEBUG << " FastCell Tile Cal " << countFastCell << endreq ;

    cellid = (*it1)->ID();
    energy = (*it1)->energy();
    SampFrac = m_tileInfo->HitCalib(cellid);
    energyConv = energy/SampFrac;

    eTileRead += energy;
    eTileConv += energyConv;

    pmt_id0 = m_tileID->pmt_id(cellid, 0);
    pmt_id1 = m_tileID->pmt_id(cellid, 1);

    if (msgLvl(MSG::DEBUG)) msg(MSG::DEBUG)<< " ReadFastCell id =  " << cellid << " E = " << energy << " SampFrac = " << SampFrac<< endreq << " PMT_id0 = " << pmt_id0 << " PMT_id1 = " << pmt_id1 << endreq;
    //log << MSG::DEBUG << " ReadFastCell id =  " << cellid << " E = " << energy << " SampFrac = " << SampFrac<< endreq ;
    //log << MSG::DEBUG << " PMT_id0 = " << pmt_id0 << " PMT_id1 = " << pmt_id1 << endreq;


    if(energyConv > minEnergy){

      if(m_tileID->is_tile(cellid))
        {
	  ATH_MSG_DEBUG(" Storing tile hit");
	  //log << MSG::DEBUG << " Storing tile hit" << endreq ;

          if (m_tileID->is_tile_gapscin(cellid)){
            // only one PMT per cell in Tile Gap region....
            m_tileHits->Emplace(pmt_id0, energyConv, hitTime);
          }else{
            // ... two PMTs per cell everywhere else.
            m_tileHits->Emplace(pmt_id0, energyConv/2.0, hitTime);
            m_tileHits->Emplace(pmt_id1, energyConv/2.0, hitTime);
          }

        }

    }

    countFastCell++;

  }


  //      //
  //      // Test with known hits
  //      // First do the LAr hits....
  //      //

  //      Identifier id0( (unsigned int) 740294662 ); // eta = -2.55861, phi = 0.347702 LAr EM End Cap
  //      Identifier id1( (unsigned int) 757105170 ); // eta = -0.204688, phi = 0.935728 LAr EM Barrel
  //      Identifier id2( (unsigned int) 912261120 ); // eta = 912261120, phi = 0.375011 LAr FCal
  //      Identifier id3( (unsigned int) 847708160 ); // eta = 1.6573, phi = -1.52171 LAr Had End Cap
  //      double cellEnergy = 100.0e3;

  //      float SampFrac0 = m_dd_fSampl->FSAMPL(id0);
  //      float SampFrac1 = m_dd_fSampl->FSAMPL(id1);
  //      float SampFrac2 = m_dd_fSampl->FSAMPL(id2);
  //      float SampFrac3 = m_dd_fSampl->FSAMPL(id3);

  //      double hitEnergy0 = cellEnergy * SampFrac0;
  //      double hitEnergy1 = cellEnergy * SampFrac1;
  //      double hitEnergy2 = cellEnergy * SampFrac2;
  //      double hitEnergy3 = cellEnergy * SampFrac3;

  //      log << MSG::DEBUG << " FastHitConv::readFastEvent()" << endreq ;
  //      log << MSG::DEBUG << " LArHits:" << endreq ;
  //      log << MSG::DEBUG << " CellId = " << id0 << " CellE = " << cellEnergy << " SampFrac = " << SampFrac0 << " HitEnergy = " << hitEnergy0 << endreq;
  //      log << MSG::DEBUG << " CellId = " << id1 << " CellE = " << cellEnergy << " SampFrac = " << SampFrac1 << " HitEnergy = " << hitEnergy1 << endreq;
  //      log << MSG::DEBUG << " CellId = " << id2 << " CellE = " << cellEnergy << " SampFrac = " << SampFrac2 << " HitEnergy = " << hitEnergy2 << endreq;
  //      log << MSG::DEBUG << " CellId = " << id3 << " CellE = " << cellEnergy << " SampFrac = " << SampFrac3 << " HitEnergy = " << hitEnergy3 << endreq;


  //      emecHitCont->push_back(new LArHit(id0, hitEnergy0, hitTime));
  //      embHitCont->push_back(new LArHit(id1, hitEnergy1, hitTime));
  //      hecHitCont->push_back(new LArHit(id3, hitEnergy3, hitTime));
  //      fcalHitCont->push_back(new LArHit(id2, hitEnergy2, hitTime));

  //      eLArHECRead += cellEnergy;
  //      eLArHECConv += hitEnergy0;

  //      eLArEMRead += cellEnergy;
  //      eLArEMConv += hitEnergy1;

  //      eLArFCALRead += cellEnergy;
  //      eLArFCALConv += hitEnergy2;

  //      eLArEMRead += cellEnergy;
  //      eLArEMConv += hitEnergy3;

  //      // ... and then the Tile Cal hits

  //      Identifier idTile0( (unsigned int) 1149420048 ); // eta = 0.25, phi = 1.12901 Tile Barrel
  //      Identifier pmt_id0 = m_tileID->pmt_id(idTile0, 0);
  //      Identifier pmt_id1 = m_tileID->pmt_id(idTile0, 1);

  //      double SampFracTile0 = m_tileInfo->HitCalib(idTile0);
  //      //double SampFracTile0 = 1.0;

  //      double hitEnergyTile0 = cellEnergy / SampFracTile0;

  //      eTileRead += cellEnergy;
  //      eTileConv += hitEnergyTile0;

  //      log << MSG::DEBUG << " TileHits:" << endreq ;
  //      log << MSG::DEBUG << " CellId = " << idTile0 << " CellE = " << cellEnergy << " SamplFrac = " << SampFracTile0 << " HitEnergy = " << hitEnergyTile0 << endreq;

  //      m_tileHits->push_back(new TileHit(pmt_id0, hitEnergyTile0/2.0, hitTime));
  //      m_tileHits->push_back(new TileHit(pmt_id1, hitEnergyTile0/2.0, hitTime));

  if (msgLvl(MSG::DEBUG)) msg(MSG::DEBUG) << " eReadTot = " << eLArEMRead + eLArHECRead + eLArFCALRead + eTileRead << endreq
					  << " eLArEMRead = " << eLArEMRead << " eLArHECRead = " << eLArHECRead << " eLArFCALRead = " << eLArFCALRead << " eTileRead = " << eTileRead << endreq
					  << " eConvTot = " << eLArEMConv + eLArHECConv + eLArFCALConv + eTileConv << endreq
					  << " eLArEMConv = " << eLArEMConv << " eLArHECConv = " << eLArHECConv << " eLArFCALConv = " << eLArFCALConv << " eTile\
Conv = " << eTileConv << endreq;
  //log << MSG::DEBUG << " eReadTot = " << eLArEMRead + eLArHECRead + eLArFCALRead + eTileRead << endreq;
  //log << MSG::DEBUG << " eLArEMRead = " << eLArEMRead << " eLArHECRead = " << eLArHECRead << " eLArFCALRead = " << eLArFCALRead << " eTileRead = " << eTileRead << endreq;
  //log << MSG::DEBUG << " eConvTot = " << eLArEMConv + eLArHECConv + eLArFCALConv + eTileConv << endreq;
  //log << MSG::DEBUG << " eLArEMConv = " << eLArEMConv << " eLArHECConv = " << eLArHECConv << " eLArFCALConv = " << eLArFCALConv << " eTileConv = " << eTileConv << endreq;


  return StatusCode::SUCCESS;
}

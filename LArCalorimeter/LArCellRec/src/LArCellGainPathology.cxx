/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/********************************************************************

NAME:     LArCellGainPathology
PACKAGE:  offline/Calorimeter/LArCellRec

AUTHORS: G.Unal
CREATION:  23 april 2009

PURPOSE:

********************************************************************/

#include "LArCellRec/LArCellGainPathology.h"

#include "GaudiKernel/Service.h"
#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/Property.h"
#include "GaudiKernel/ListItem.h"
#include "GaudiKernel/StatusCode.h"
#include "StoreGate/StoreGateSvc.h" 
#include "GeoModelInterfaces/IGeoModelSvc.h"
#include "CaloEvent/CaloCellContainer.h"
#include "Identifier/Identifier.h"
#include "Identifier/IdentifierHash.h"
#include "CaloIdentifier/CaloIdManager.h"
#include "CaloIdentifier/CaloCell_ID.h"
#include "LArRawEvent/LArFebErrorSummary.h" 
#include "LArIdentifier/LArOnlineID.h" 
#include "LArTools/LArCablingService.h"


/////////////////////////////////////////////////////////////////////
// CONSTRUCTOR:
/////////////////////////////////////////////////////////////////////

LArCellGainPathology::LArCellGainPathology(
			     const std::string& type, 
			     const std::string& name, 
			     const IInterface* parent)
  :AlgTool(type, name, parent)

{ 
  declareInterface<ICaloCellMakerTool>(this); 

}


/////////////////////////////////////////////////////////////////////
// INITIALIZE:
// The initialize method will create all the required algorithm objects
/////////////////////////////////////////////////////////////////////

StatusCode LArCellGainPathology::initialize()
{
  MsgStream  log(msgSvc(),name());

  //initialize the StoreGateSvc ptr
  StatusCode sc = service("StoreGateSvc", m_storeGate); 
  if (sc.isFailure()) {
    log << MSG::ERROR << " Cannot retrieve StoreGateSvc" << endreq;
    return sc;
  } 

  //initialize the detectorStore ptr
  sc = service("DetectorStore", m_detStore); 
  if (sc.isFailure()) {
    log << MSG::ERROR << " Cannot retrieve detectorStore" << endreq;
    return sc;
  } 

  // callback to GeoModel to retrieve identifier helpers, etc..
  const IGeoModelSvc *geoModel=0;
  sc = service("GeoModelSvc", geoModel);
  if(sc.isFailure())
  {
    log << MSG::ERROR << "Could not locate GeoModelSvc" << endreq;
    return sc;
  }

  // dummy parameters for the callback:
  int dummyInt=0;
  std::list<std::string> dummyList;

  if (geoModel->geoInitialized())
  {
    return geoInit(dummyInt,dummyList);
  }
  else
  {
    sc = m_detStore->regFcn(&IGeoModelSvc::geoInit,
                          geoModel,
                          &LArCellGainPathology::geoInit,this);
    if(sc.isFailure())
    {
      log << MSG::ERROR << "Could not register geoInit callback" << endreq;
      return sc;
    }
  }
  return StatusCode::SUCCESS;
}

StatusCode LArCellGainPathology::geoInit(IOVSVC_CALLBACK_ARGS)
{

  MsgStream  log(msgSvc(),name());

  const  CaloIdManager* caloIdMgr;
  StatusCode sc = m_detStore->retrieve( caloIdMgr );
  if (sc.isFailure()) {
   log << MSG::ERROR << "Unable to retrieve CaloIdMgr " << endreq;
   return sc;
  }
  m_calo_id = caloIdMgr->getCaloCell_ID();


  // translate offline ID into online ID
  sc = m_cablingService.retrieve();
  if(sc.isFailure()){
    log << MSG::ERROR << "Could not retrieve LArCablingService Tool" << endreq;
    return sc;
  }
  
  sc = m_detStore->retrieve(m_onlineID, "LArOnlineID");
  if (sc.isFailure()) {
    log << MSG::ERROR << "Could not get LArOnlineID helper !" << endreq; 
    return sc;
  }

  return StatusCode::SUCCESS;

}

StatusCode LArCellGainPathology::finalize()
{

   return StatusCode::SUCCESS;
}

StatusCode LArCellGainPathology::process(CaloCellContainer * theCont )
{
	
  MsgStream  log(msgSvc(),name());
  bool dump=false;
  if (log.level()<=MSG::DEBUG) dump=true;


  if(dump) log << MSG::DEBUG << " in  LArCellGainPathology::process " << endreq;

// loop over all Febs

  std::vector<HWIdentifier>::const_iterator feb = m_onlineID->feb_begin();
  std::vector<HWIdentifier>::const_iterator feb_end = m_onlineID->feb_end();


  for ( ; feb != feb_end; feb++) {


// for debug
      HWIdentifier febId = (*feb);
      unsigned int ifeb = febId.get_identifier32().get_compact();
      if(dump) log << MSG::DEBUG << " process Feb: " << ifeb << " ";

// get information for channel 0-63  64-127
   
      for (int i=0;i<2;i++) {
       int cha1 = i*64;
       int cha2 = i*64+63;
       HWIdentifier hwid1 = m_onlineID->channel_Id(febId,cha1);
       HWIdentifier hwid2 = m_onlineID->channel_Id(febId,cha2);
       this->ApplyPathology(theCont,hwid1,hwid2);
      }
       

  }
 

  return StatusCode::SUCCESS;
}

void LArCellGainPathology::ApplyPathology(CaloCellContainer* theCont, HWIdentifier id1, HWIdentifier id2)
{

    CaloCell* cell1 = this->GetCell(theCont, id1);
    CaloCell* cell2 = this->GetCell(theCont, id2);

    if (!cell1 || !cell2) return;

    //std::cout << " cell pair " << m_onlineID->show_to_string(id1) << " " << m_onlineID->show_to_string(id2) << " " <<
    //  cell1->gain() << " " << cell2->gain() <<  " " << cell1->energy() << " " << cell2->energy() << std::endl;

    if (cell1->gain() == cell2->gain()) return;


    CaloGain::CaloGain newGain = cell2->gain();
    double eratio=1;
    if (newGain==CaloGain::LARHIGHGAIN) {
       if (cell1->gain()==CaloGain::LARMEDIUMGAIN) eratio=0.1;
       if (cell1->gain()==CaloGain::LARLOWGAIN) eratio=0.01;
    }
    if (newGain==CaloGain::LARMEDIUMGAIN) {
       if (cell1->gain()==CaloGain::LARHIGHGAIN) eratio=10.;
       if (cell1->gain()==CaloGain::LARLOWGAIN) eratio=0.1;
    }
    if (newGain==CaloGain::LARLOWGAIN) {
       if (cell1->gain()==CaloGain::LARHIGHGAIN) eratio=100.;
       if (cell1->gain()==CaloGain::LARMEDIUMGAIN) eratio=10.;
    }
    double eold = cell1->energy();
    cell1->setEnergy(eratio*eold);
    cell1->setGain(newGain);
    //std::cout << "   --- new gain/energy of cell1 " << cell1->gain() << " " << cell1->energy() << std::endl;

}

CaloCell* LArCellGainPathology::GetCell(CaloCellContainer* theCont, HWIdentifier id)
{
  CaloCell* aCell =0;
  if (m_cablingService->isOnlineConnected(id)) {
    Identifier id_off = m_cablingService->cnvToIdentifier(id);
    IdentifierHash theCellHashID = m_calo_id->calo_cell_hash(id_off);
    int index = theCont->findIndex(theCellHashID);
    if (index>=0) {
       aCell = theCont->at(index);
    }
  }
  return aCell;

}

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "LArConditionsTest/TestLArConditionsTools.h"
#include "GaudiKernel/Algorithm.h"
#include "GaudiKernel/IToolSvc.h"
#include "GaudiKernel/IAlgTool.h"

#include "StoreGate/StoreGateSvc.h"
#include "CaloIdentifier/CaloCell_ID.h"
#include "CaloDetDescr/CaloDetDescrManager.h"
#include "CaloDetDescr/CaloDetDescrElement.h"
#include "CaloEvent/CaloCellContainer.h"
#include "EventInfo/EventInfo.h"

#include "LArElecCalib/ILArOFCTool.h"
#include "LArElecCalib/ILArADC2MeVTool.h"
#include "LArElecCalib/ILArAutoCorrNoiseTool.h"
#include "LArElecCalib/ILArAutoCorrTotalTool.h"

TestLArConditionsTools::TestLArConditionsTools(const std::string& name, ISvcLocator* pSvcLocator) : 
  //Algorithm(name,pSvcLocator),m_noiseTool(0),m_detStore(0),m_evtStore(0)
  Algorithm(name,pSvcLocator),
  m_ofcTool("LArOFCTool"),
  m_acTotalTool("LArAutoCorrTotalTool"),
  m_acNoiseTool("LArAutoCorrNoiseTool"),
  m_adc2MeVTool("LArADC2MeVTool"),
  m_noiseTool("CaloNoiseTool/calonoisetool"),
  m_roiMap("LArRoI_Map"),
  m_idHelper(0),
  m_detStore(0),m_evtStore(0)
  { 
    declareProperty ("RetrieveToolInInit", m_retrieveInInit=true);
    //std::cout<<" in TestLArConditionsTools c-tor"<<std::endl;
  }

TestLArConditionsTools::~TestLArConditionsTools()
{ 
  //std::cout<<" in TestLArConditionsTools d-tor"<<std::endl;
} 

StatusCode TestLArConditionsTools::retrieveTools() {
    MsgStream  log(messageService(),name());

    StatusCode sc = m_noiseTool.retrieve();
    if(!sc.isSuccess()) {
      log<<MSG::ERROR<<" Failed to retrieve ICaloNoiseTool " <<endreq;
      return sc;
    }

    log<<MSG::DEBUG<<" CaloNoiseTool typename "<<m_noiseTool.typeAndName()<<endreq;

    sc = m_acNoiseTool.retrieve();
    if(!sc.isSuccess()) {
      log<<MSG::ERROR<<" Failed to retrieve ILArAutoCorrNoiseTool " <<endreq;
      return sc;
    }

    // ILArADC2MeVTool
    sc = m_adc2MeVTool.retrieve();
    if(!sc.isSuccess()) {
      log<<MSG::ERROR<<" Failed to retrieve ILArADC2MeVTool " <<endreq;
      return sc;
    }

    // ILArAutoCorrTotalTool
    sc = m_acTotalTool.retrieve();
    if(!sc.isSuccess()) {
      log<<MSG::ERROR<<" Failed to retrieve ILArAutoCorrTotalTool " <<endreq;
      return sc;
    }

    sc = m_ofcTool.retrieve();
    if(!sc.isSuccess()) {
      log<<MSG::ERROR<<" Failed to retrieve ILArOFCTool " <<endreq;
      return sc;
    }

    sc = m_roiMap.retrieve();
    if(!sc.isSuccess()) {
      log<<MSG::ERROR<<" Failed to retrieve LArRoI_Map " <<endreq;
      return sc;
    }

    return sc;
}


StatusCode TestLArConditionsTools::initialize() {
  MsgStream  log(messageService(),name());

  log << MSG::DEBUG 
      << " in initialize  "<<endreq;

  StatusCode sc = retrieveTools() ;
  if (sc.isFailure()) {
    log << MSG::ERROR
	<< "Unable to retrieve tools "
	<< endreq;
	return sc;
  }
   

  // Pointer to StoreGate (cached)
  sc = service("DetectorStore", m_detStore);
  if (sc.isFailure()) {
    log << MSG::ERROR
	<< "Unable to retrieve pointer to DetectorStore "
	<< endreq;
	return sc;
  }
  log << MSG::DEBUG << "Retrieved DetectorStore" << endreq;


  // Pointer to StoreGateSvc (cached)
  sc = service("StoreGateSvc", m_evtStore);
  if (sc.isFailure()) {
    log << MSG::ERROR
	<< "Unable to retrieve pointer to StoreGateSvc  "
	<< endreq;
	return sc;
  }
  log << MSG::DEBUG << "Retrieved StoreGateSvc "<< endreq;


  sc = m_detStore->retrieve( m_idHelper);
  if(sc!=StatusCode::SUCCESS){ 
    log << MSG::ERROR << " Can not retrieve CaloCell_ID" << endreq;
    return sc;
  }

  const EventInfo* evtInfo = 0;
  sc = m_evtStore->retrieve( evtInfo);
  if(sc!=StatusCode::SUCCESS){ 
    log << MSG::WARNING << " Can not retrieve EventInfo " << endreq;
    //  return sc;
  }else
    {

      log << MSG::INFO << " EventInfo retrieved " << endreq;
    }
  

  /*
  log<<MSG::INFO<<" testing LArRoI_Map in init "<<endreq;
  sc = testLArRoI_Map(); 
  if(!sc.isSuccess()) {
    log<<MSG::ERROR<<" failed LArRoI_Map test "<<endreq;
  }

  */

  log << MSG::INFO << " done initialize " << endreq;
  
  return StatusCode::SUCCESS;

}

StatusCode TestLArConditionsTools::testCaloCellNoise() {
  MsgStream  log(messageService(),name());

  log << MSG::INFO << " in testCaloCellNoise" << endreq;

  const CaloCellContainer* cont;
  StatusCode sc = m_evtStore->retrieve(cont,"AllCalo");
  if(!sc.isSuccess()){
	log << MSG::ERROR << " Can not retrieve CaloCellContainer" << endreq;
	return sc; 
  }

  CaloCellContainer::const_iterator cell_it = cont->begin();
  CaloCellContainer::const_iterator cell_it_e = cont->end();

    int n=0;
    for( ; cell_it!=cell_it_e ; ++cell_it){

      const CaloCell* cell = *cell_it;
      Identifier id = cell->ID() ;
      const CaloDetDescrElement* dde=cell->caloDDE();

      if(dde==0) {
	log << MSG::ERROR << " DDE null " <<  m_idHelper->print_to_string(id)<<endreq;
	continue;
      }

      log << MSG::DEBUG<< " channel " << m_idHelper->print_to_string(id)<<endreq;

      float f2 = m_noiseTool->elecNoiseRMSHighestGain(dde);
      float f1 = m_noiseTool->elecNoiseRMS(cell);
      // float f2 = m_noiseTool->elecNoiseRMSHighestGain(dde,1);
      // float f1 = m_noiseTool->elecNoiseRMS(dde,CaloGain::LARHIGHGAIN,-1);
      //      float f2 = m_noiseTool->elecNoiseRMS(dde,CaloGain::LARHIGHGAIN,1);
      // float f2 = 0;

      /*
      std::vector<float> totalNoises = m_noiseTool->elecNoiseRMS3gains(dde,1);
      if(totalNoises.size()==0){
	log << MSG::ERROR << " fail to get noise for this channel" << 
	  m_idHelper->print_to_string(id)<<endreq;
      }
      */
      log << MSG::DEBUG<< " channel " << m_idHelper->print_to_string(id)
	  <<" noise "<<f1<<" "<<f2<<endreq;

      ++n;

    }

    log<<MSG::INFO<<" number of cells tested "<<n<<endreq;
    return StatusCode::SUCCESS;

}

StatusCode TestLArConditionsTools::testCaloNoiseDDE() {

    MsgStream  log(messageService(),name());

    const CaloDetDescrManager*  dd_man ; 
    StatusCode sc = m_detStore->retrieve( dd_man);
    if(sc!=StatusCode::SUCCESS){ 
	log << MSG::ERROR << " Can not retrieve CaloDetDescrManager" << endreq;
	return sc;
    }

    std::vector<Identifier>::const_iterator cell_it  = m_idHelper->cell_begin();
    std::vector<Identifier>::const_iterator cell_it_e  = m_idHelper->cell_end();

    int n=0;
    for( ; cell_it!=cell_it_e ; ++cell_it){

      Identifier id = *cell_it;
      bool isLAr = m_idHelper->is_lar(id);
      const CaloDetDescrElement* dde=dd_man->get_element(id); 
      if(dde==0) {
	log << MSG::ERROR << " DDE null " <<  m_idHelper->print_to_string(id)<<endreq;
	continue;
      }

      log << MSG::DEBUG<< " channel " << m_idHelper->print_to_string(id)<<endreq;

      float f1 = m_noiseTool->elecNoiseRMS(dde,CaloGain::LARHIGHGAIN,-1);
      float f2 = m_noiseTool->elecNoiseRMS(dde,CaloGain::LARHIGHGAIN,1);

      std::vector<float> totalNoises = m_noiseTool->elecNoiseRMS3gains(dde,1);
      if(totalNoises.size()==0){
	log << MSG::ERROR << " fail to get noise for this channel" << 
	  m_idHelper->print_to_string(id)<<endreq;
      }

      log << MSG::DEBUG<< " noise =  " << f1<<" "<<f2<<endreq;


      if(isLAr){
        ILArOFCTool::OFCRef_t ofc_a = m_ofcTool->OFC_a(id,0); 
	if(ofc_a.size()==0){
	  log << MSG::ERROR << " fail to get OFC " << 
	    m_idHelper->print_to_string(id)<<endreq;
      }
	
	const std::vector<float>& adc2mev  =  m_adc2MeVTool->ADC2MEV(id,0); 
	if(adc2mev.size()==0){
	log << MSG::ERROR << " fail to get ADC2MEV " << 
	  m_idHelper->print_to_string(id)<<endreq;
	}
	
	const std::vector<double> acTotal  =  m_acTotalTool->autoCorrTotal(id,0); 
	if(acTotal.size()==0){
	  log << MSG::ERROR << " fail to get AutoCorrTotal " << 
	    m_idHelper->print_to_string(id)<<endreq;
	}
      }

      ++n;

    }

    log<<MSG::INFO<<" number of cells tested "<<n<<endreq;
    return StatusCode::SUCCESS;

}

StatusCode TestLArConditionsTools::testLArRoI_Map() {

    MsgStream  log(messageService(),name());

    std::vector<Identifier>::const_iterator cell_it  = m_idHelper->cell_begin();
    std::vector<Identifier>::const_iterator cell_it_e  = m_idHelper->cell_end();

    int n=0;
    for( ; cell_it!=cell_it_e ; ++cell_it){

      Identifier id = *cell_it;
      bool isLAr = m_idHelper->is_lar(id);
      int ttid = m_roiMap->TrigTowerID(id);
      if(log.level()<=MSG::DEBUG) {
	log << MSG::DEBUG
	    << " n " << n
	    << " id " << id.get_compact()
	    << " isLAr " << isLAr
	    << " ttid " << ttid
	    << endreq;
      }
      ++n;
    }

    log<<MSG::INFO<<" number of cells tested for LArRoI_Map "<<n<<endreq;
    return StatusCode::SUCCESS;

}
StatusCode TestLArConditionsTools::execute() {
  
  MsgStream  log(messageService(),name());

  log<<MSG::DEBUG<<" in execute"<<endreq;
  //StatusCode sc = testCaloCellNoise(); 
  StatusCode sc = testCaloNoiseDDE(); 
  if(!sc.isSuccess()) {
    log<<MSG::ERROR<<" failed CaloNoise Test"<<endreq;
  }


  sc = testLArRoI_Map(); 
  if(!sc.isSuccess()) {
    log<<MSG::ERROR<<" failed LArRoI_Map test "<<endreq;
  }
  return StatusCode::SUCCESS;
}

StatusCode TestLArConditionsTools::finalize() {
  return StatusCode::SUCCESS;
}


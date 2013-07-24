/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "LArCalibUtils/LArAccumulatedCalibDigitContSplitter.h"

#include "GaudiKernel/IIncidentSvc.h"
#include "GaudiKernel/MsgStream.h"

#include "CaloIdentifier/CaloIdManager.h"
#include "CaloIdentifier/LArEM_ID.h"
#include "LArIdentifier/LArOnlineID.h"
#include "LArTools/LArCablingService.h"

#include "LArRawEvent/LArAccumulatedCalibDigitContainer.h"
#include "LArRawEvent/LArCalibDigitContainer.h"

#include "LArRawConditions/LArWaveHelper.h"

#include <fstream>

LArAccumulatedCalibDigitContSplitter::LArAccumulatedCalibDigitContSplitter(const std::string& name, ISvcLocator* pSvcLocator) 
  : Algorithm(name, pSvcLocator),m_recAll(false), m_numLine(4), m_useDacAndIsPulsedIndex(false),
    m_storeGateSvc(0),
    m_event_counter(0)
{
 declareProperty("KeyList",     m_keylist);
 declareProperty("RecAllCells", m_recAll);
 declareProperty("NumberSplitted", m_numLine);

 declareProperty("UseDacAndIsPulsedIndex",  m_useDacAndIsPulsedIndex); 
 declareProperty("KeyOutputList",    m_OutputList); 
}

LArAccumulatedCalibDigitContSplitter::~LArAccumulatedCalibDigitContSplitter() 
{}

StatusCode LArAccumulatedCalibDigitContSplitter::initialize()
{ 

  MsgStream log(msgSvc(), name());
  
  StatusCode sc = service("StoreGateSvc", m_storeGateSvc);
  if (sc.isFailure()) {
      log << MSG::FATAL << " Cannot locate StoreGateSvc " << std::endl;
      return StatusCode::FAILURE;
  }   

  // Default list
  if (!m_keylist.size()){ // Not key list given
    m_keylist.push_back("HIGH");
    m_keylist.push_back("MEDIUM");
    m_keylist.push_back("LOW");
   }

  m_event_counter = 0;

  return StatusCode::SUCCESS;
}


StatusCode LArAccumulatedCalibDigitContSplitter::execute() 
{
 MsgStream log(msgSvc(), name());
 StatusCode sc;

 if ( m_event_counter < 100 || ( m_event_counter < 1000 && m_event_counter%100==0 ) || m_event_counter%1000==0 ) 
    log << MSG::INFO << "Processing event " << m_event_counter << endreq;
 ++m_event_counter;
 
 if (m_keylist.size()==0) {
   log << MSG::ERROR << "Key list is empty! No containers to process!" << endreq;
   return StatusCode::FAILURE;
 } 
 
 // execute() method...
 return executeWithAccumulatedDigits();
}

StatusCode LArAccumulatedCalibDigitContSplitter::executeWithAccumulatedDigits()
{

 MsgStream log(msgSvc(), name());
 StatusCode sc;

 const LArAccumulatedCalibDigitContainer* larAccumulatedCalibDigitContainer;
 std::vector<LArAccumulatedCalibDigitContainer*> calibDigitCont (m_numLine);

 for (unsigned int i=0;i<m_numLine;++i)    
   calibDigitCont[i]= new LArAccumulatedCalibDigitContainer(SG::VIEW_ELEMENTS);
 
 std::vector<std::string>::const_iterator key_it=m_keylist.begin();
 std::vector<std::string>::const_iterator key_it_e=m_keylist.end();

 for (;key_it!=key_it_e; ++key_it) { //Loop over all containers that are to be processed (e.g. different gains)

   sc = m_storeGateSvc->retrieve(larAccumulatedCalibDigitContainer,*key_it);
   if (sc.isFailure()) {
     log << MSG::WARNING << "Cannot read LArAccumulatedCalibDigitContainer from StoreGate! key=" << *key_it << endreq;
     continue; // Try next container
   }

   LArAccumulatedCalibDigitContainer::const_iterator it    =larAccumulatedCalibDigitContainer->begin();
   LArAccumulatedCalibDigitContainer::const_iterator it_end=larAccumulatedCalibDigitContainer->end();
   if (it == it_end) {
     log << MSG::DEBUG << "LArAccumulatedCalibDigitContainer with key=" << *key_it << " is empty " << endreq;
     continue; // at this event LArAccumulatedCalibDigitContainer is empty, do not even try to loop on it...
   }

   const float delayScale = larAccumulatedCalibDigitContainer->getDelayScale();
   for (unsigned int iCont=0;iCont<m_numLine;iCont++) calibDigitCont[iCont]->setDelayScale(delayScale); 

   for (;it!=it_end; ++it) { // Loop over all cells

     //HWIdentifier chid=(*it)->hardwareID(); 
     CaloGain::CaloGain gain=(*it)->gain();

     if (gain<0 || gain>CaloGain::LARNGAIN) {
       log << MSG::ERROR << "Found not-matching gain number ("<< (int)gain <<")" << endreq;
       return StatusCode::FAILURE;
     }

     if ( (!m_recAll) && (!(*it)->isPulsed())  ) continue ; // If not all cells and not pulsed, skip cell
     
     for(unsigned int iLine=1;iLine<=m_numLine;++iLine){
       if((*it)->isPulsed(iLine)){
	 calibDigitCont[iLine-1]->push_back(*it);
       }
     }
   } //End loop over all cells

 } //End loop over all containers

 // Record splitted containers
 for (unsigned int i=0;i<m_OutputList.size();++i) {
     std::string key = m_OutputList[i];
     m_storeGateSvc->record(calibDigitCont[i],key);
 }
 
 return StatusCode::SUCCESS;
}



StatusCode LArAccumulatedCalibDigitContSplitter::stop()
{
//  MsgStream log(msgSvc(), name());
//  log << MSG::INFO << " finished." << endreq;
  return StatusCode::SUCCESS;
}

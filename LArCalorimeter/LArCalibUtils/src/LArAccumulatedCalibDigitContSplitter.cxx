/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "LArCalibUtils/LArAccumulatedCalibDigitContSplitter.h"

#include "GaudiKernel/IIncidentSvc.h"
#include "GaudiKernel/MsgStream.h"

#include "CaloIdentifier/CaloIdManager.h"
#include "CaloIdentifier/LArEM_ID.h"
#include "LArIdentifier/LArOnlineID.h"
#include "LArCabling/LArCablingService.h"

#include "LArRawEvent/LArAccumulatedCalibDigitContainer.h"
#include "LArRawEvent/LArCalibDigitContainer.h"

#include "LArRawConditions/LArWaveHelper.h"

#include <fstream>

LArAccumulatedCalibDigitContSplitter::LArAccumulatedCalibDigitContSplitter(const std::string& name, ISvcLocator* pSvcLocator) 
  : AthAlgorithm(name, pSvcLocator),m_recAll(false), m_numLine(4), m_useDacAndIsPulsedIndex(false),
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
 if ( m_event_counter < 100 || ( m_event_counter < 1000 && m_event_counter%100==0 ) || m_event_counter%1000==0 ) 
   ATH_MSG_INFO ( "Processing event " << m_event_counter );
 ++m_event_counter;
 
 if (m_keylist.size()==0) {
   ATH_MSG_ERROR ( "Key list is empty! No containers to process!" );
   return StatusCode::FAILURE;
 } 
 
 // execute() method...
 ATH_CHECK( executeWithAccumulatedDigits() );
 return StatusCode::SUCCESS;

}

StatusCode LArAccumulatedCalibDigitContSplitter::executeWithAccumulatedDigits()
{
 const LArAccumulatedCalibDigitContainer* larAccumulatedCalibDigitContainer;
 std::vector<LArAccumulatedCalibDigitContainer*> calibDigitCont (m_numLine);

 for (unsigned int i=0;i<m_numLine;++i)    
   calibDigitCont[i]= new LArAccumulatedCalibDigitContainer(SG::VIEW_ELEMENTS);
 
 std::vector<std::string>::const_iterator key_it=m_keylist.begin();
 std::vector<std::string>::const_iterator key_it_e=m_keylist.end();

 for (;key_it!=key_it_e; ++key_it) { //Loop over all containers that are to be processed (e.g. different gains)

   StatusCode sc = evtStore()->retrieve(larAccumulatedCalibDigitContainer,*key_it);
   if (sc.isFailure()) {
     ATH_MSG_WARNING ( "Cannot read LArAccumulatedCalibDigitContainer from StoreGate! key=" << *key_it );
     continue; // Try next container
   }

   LArAccumulatedCalibDigitContainer::const_iterator it    =larAccumulatedCalibDigitContainer->begin();
   LArAccumulatedCalibDigitContainer::const_iterator it_end=larAccumulatedCalibDigitContainer->end();
   if (it == it_end) {
     ATH_MSG_DEBUG ( "LArAccumulatedCalibDigitContainer with key=" << *key_it << " is empty " );
     continue; // at this event LArAccumulatedCalibDigitContainer is empty, do not even try to loop on it...
   }

   const float delayScale = larAccumulatedCalibDigitContainer->getDelayScale();
   for (unsigned int iCont=0;iCont<m_numLine;iCont++) calibDigitCont[iCont]->setDelayScale(delayScale); 

   for (;it!=it_end; ++it) { // Loop over all cells

     //HWIdentifier chid=(*it)->hardwareID(); 
     CaloGain::CaloGain gain=(*it)->gain();

     if (gain<0 || gain>CaloGain::LARNGAIN) {
       ATH_MSG_ERROR ( "Found not-matching gain number ("<< (int)gain <<")" );
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
     ATH_CHECK( evtStore()->record(calibDigitCont[i],key) );
 }
 
 return StatusCode::SUCCESS;
}



StatusCode LArAccumulatedCalibDigitContSplitter::stop()
{
//  MsgStream log(msgSvc(), name());
//  log << MSG::INFO << " finished." << endmsg;
  return StatusCode::SUCCESS;
}

/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#include "LArCalibUtils/LArAccumulatedCalibDigitContSplitter.h"

#include "GaudiKernel/IIncidentSvc.h"
#include "GaudiKernel/MsgStream.h"

#include "CaloIdentifier/CaloIdManager.h"
#include "CaloIdentifier/LArEM_ID.h"
#include "LArIdentifier/LArOnlineID.h"

#include "LArRawEvent/LArAccumulatedCalibDigitContainer.h"
#include "LArRawEvent/LArCalibDigitContainer.h"

#include "LArRawConditions/LArWaveHelper.h"
#include "AthContainers/ConstDataVector.h"

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
  typedef ConstDataVector<LArAccumulatedCalibDigitContainer> ConstAccumContainer;
 const LArAccumulatedCalibDigitContainer* larAccumulatedCalibDigitContainer;

 // First loop over containers to get delayScale.
 float delayScale = 1*CLHEP::ns;
 for (const std::string& key : m_keylist) {
   if (evtStore()->retrieve(larAccumulatedCalibDigitContainer,key).isSuccess()) {
     if (!larAccumulatedCalibDigitContainer->empty()) {
       delayScale = larAccumulatedCalibDigitContainer->getDelayScale();
     }
   }
 }
   
 std::vector<std::unique_ptr<ConstAccumContainer> > calibDigitCont (m_numLine);
 for (unsigned int i=0;i<m_numLine;++i) {
   calibDigitCont[i]= std::make_unique<ConstAccumContainer>(SG::VIEW_ELEMENTS,
                                                            delayScale);
 }

 //Loop over all containers that are to be processed (e.g. different gains)
 for (const std::string& key : m_keylist) {
   StatusCode sc = evtStore()->retrieve(larAccumulatedCalibDigitContainer,key);
   if (sc.isFailure()) {
     ATH_MSG_WARNING ( "Cannot read LArAccumulatedCalibDigitContainer from StoreGate! key=" << key );
     continue; // Try next container
   }

   if (larAccumulatedCalibDigitContainer->empty()) {
     ATH_MSG_DEBUG ( "LArAccumulatedCalibDigitContainer with key=" << key << " is empty " );
     continue; // at this event LArAccumulatedCalibDigitContainer is empty, do not even try to loop on it...
   }

   for (const LArAccumulatedCalibDigit* digit : *larAccumulatedCalibDigitContainer) {
     //HWIdentifier chid=digit->hardwareID(); 
     CaloGain::CaloGain gain=digit->gain();

     if (gain<0 || gain>CaloGain::LARNGAIN) {
       ATH_MSG_ERROR ( "Found not-matching gain number ("<< (int)gain <<")" );
       return StatusCode::FAILURE;
     }

     if ( (!m_recAll) && (!digit->isPulsed())  ) continue ; // If not all cells and not pulsed, skip cell
     
     for(unsigned int iLine=1;iLine<=m_numLine;++iLine){
       if(digit->isPulsed(iLine)){
	 calibDigitCont[iLine-1]->push_back(digit);
       }
     }
   } //End loop over all cells

 } //End loop over all containers

 // Record splitted containers
 for (unsigned int i=0;i<m_OutputList.size();++i) {
     std::string key = m_OutputList[i];
     ATH_CHECK( evtStore()->record(std::move(calibDigitCont[i]),key) );
 }
 
 return StatusCode::SUCCESS;
}



StatusCode LArAccumulatedCalibDigitContSplitter::stop()
{
//  MsgStream log(msgSvc(), name());
//  log << MSG::INFO << " finished." << endmsg;
  return StatusCode::SUCCESS;
}

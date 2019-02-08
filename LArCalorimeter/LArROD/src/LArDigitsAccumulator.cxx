/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#include "LArROD/LArDigitsAccumulator.h"

#include "CLHEP/Units/SystemOfUnits.h"
#include <math.h>
#include "stdint.h"

LArDigitsAccumulator::LArDigitsAccumulator (const std::string& name, ISvcLocator* pSvcLocator):
  AthAlgorithm(name, pSvcLocator),
  m_onlineHelper(0),
  m_AccuDigitContainerName("LArAccumulatedDigits"),
  m_NtriggersPerStep(100),
  m_nStepTrigger(30)
{
  declareProperty("LArAccuDigitContainerName",m_AccuDigitContainerName);
  declareProperty("KeyList",m_keylist);
  declareProperty("NTriggersPerStep",m_NtriggersPerStep);
  declareProperty("StepOfTriggers",m_nStepTrigger);

  m_event_counter=0;
}


StatusCode LArDigitsAccumulator::initialize(){
  
  MsgStream log(msgSvc(), name());

  // retrieve online ID helper
  StatusCode sc = detStore()->retrieve(m_onlineHelper, "LArOnlineID");
  if (sc.isFailure()) {
    log << MSG::ERROR << "Could not get LArOnlineID helper !" << endmsg;
    return StatusCode::FAILURE;
  }


  m_Accumulated.resize(m_onlineHelper->channelHashMax());

  return StatusCode::SUCCESS;
}



StatusCode LArDigitsAccumulator::execute() 
{
  
  StatusCode sc;
  MsgStream log(msgSvc(), name());

  if ( m_event_counter < 100 || m_event_counter%100==0 )
    log << MSG::INFO << "Processing event " << m_event_counter << endmsg;
  ++m_event_counter;

  
  // pointer to input container
  const LArDigitContainer* DigitContainer=NULL;


  std::vector<std::string>::const_iterator key_it=m_keylist.begin();
  std::vector<std::string>::const_iterator key_it_e=m_keylist.end();

  // retrieve input Digits
  
  //Loop over all containers that are to be processed (e.g. different gains)
  for (;key_it!=key_it_e;key_it++) { 
     
    sc=evtStore()->retrieve(DigitContainer,*key_it);
    if(sc.isFailure()) {
      log << MSG::ERROR << "Can't retrieve LArDigitContainer with key " << *key_it << "from StoreGate." << endmsg;
      return StatusCode::SUCCESS;
    }else{
      log << MSG::DEBUG << "Retrieved LArDigitContainer with key " << *key_it << " from StoreGate." << endmsg;
    }

    // store LArAccumulatedDigits
    ACCUMDIGIT_VEC& vAccum = m_my_vec;
    if(vAccum.size()==0) vAccum.resize(m_onlineHelper->channelHashMax());

    // Loop over DigitContainer
    LArDigitContainer::const_iterator it=DigitContainer->begin();
    LArDigitContainer::const_iterator it_end=DigitContainer->end();

    if(it == it_end) {
      log << MSG::DEBUG << "LArDigitContainer with key=" << *key_it << " is empty " << endmsg;
    }else{
      log << MSG::DEBUG << "LArDigitContainer with key=" << *key_it << " has size =  " << DigitContainer->size() <<  endmsg;
    }

    //unsigned int iStepTrigger=0;

    // output container
    LArAccumulatedDigitContainer* larAccuDigitContainer = new LArAccumulatedDigitContainer(SG::VIEW_ELEMENTS);
    //Loop over all cells
    for (;it!=it_end;it++) {  

      // identificators
      HWIdentifier chid=(*it)->hardwareID();      
      const IdentifierHash hashid = m_onlineHelper->channel_Hash(chid);

      CaloGain::CaloGain gain=(*it)->gain();
      if (gain<0 || gain>CaloGain::LARNGAIN)
	{log << MSG::ERROR << "Found not-matching gain number ("<< (int)gain <<")" << endmsg;
          delete larAccuDigitContainer;
	  return StatusCode::FAILURE;
	}

      // object to be filled for each cell
      LArAccumulated& cellAccumulated = m_Accumulated[hashid];
      
      // trigger counter for each cell
      cellAccumulated.m_ntrigger++;

      //log << MSG::DEBUG << "chid = " << chid << ", trigger = " << cellAccumulated.m_ntrigger << ", sample 0 = "<< (*it)->samples()[0] << endmsg;

      // at first trigger, initialize vectors
      unsigned int sizeSamples = (*it)->samples().size();
      //log << MSG::DEBUG << "# of samples = " << sizeSamples << endmsg;

      LArAccumulatedDigit* accuDigit;

      if(m_event_counter==1){
	accuDigit = new LArAccumulatedDigit(chid);
	vAccum[hashid]=accuDigit;	
      }
      
      if(cellAccumulated.m_ntrigger==1){
	//	std::cout << "resizing to 0 "<< std::endl;
	cellAccumulated.m_samplesum.clear();
	cellAccumulated.m_samplesum.resize(sizeSamples,0);
	cellAccumulated.m_matrix.clear();
	cellAccumulated.m_matrix.resize(sizeSamples,0);
      }
      accuDigit = m_my_vec[hashid];
      
      unsigned int l= 0;
      for(unsigned int j=0;j<sizeSamples;j++){
	cellAccumulated.m_samplesum[j] += (*it)->samples()[j];
	for(unsigned int k=j;k<sizeSamples;k++)
	  {
	    l=k-j;
	    cellAccumulated.m_matrix[l] += (*it)->samples()[j]*(*it)->samples()[k];
	    //	    std::cout << "accumulation, l =  "<< l << ", j = "<< j << ", k = "<< k << ", matrix = "<< cellAccumulated.m_matrix[l] <<", sum = "<< cellAccumulated.m_samplesum <<  std::endl;
	    l++;
	  }
      }

      //log << MSG::DEBUG << "Sum = " << cellAccumulated.m_samplesum << endmsg;
      //log << MSG::DEBUG << "Matrix = " << cellAccumulated.m_matrix[0] << endmsg;

      // when reached total number of triggers for this step, fill LArAccumulatedDigit and reset number of triggers

      if(cellAccumulated.m_ntrigger==m_NtriggersPerStep){

	//log << MSG::DEBUG << "filling LArAccumulatedCalibDigit " << endmsg;
	//log << MSG::DEBUG << "chid = " << chid << ", gain = " << gain << ", trigPerStep = " << m_NtriggersPerStep << endmsg;
	
	accuDigit->setAddSubStep(gain,cellAccumulated.m_samplesum,cellAccumulated.m_matrix,m_NtriggersPerStep);
      
	larAccuDigitContainer->push_back(accuDigit);

	cellAccumulated.m_ntrigger = 0;

      }
      
    }// loop over cells in container

    log << MSG::DEBUG << "Finished loop over channels " << endmsg;

    const std::string SGkey=(*key_it)+m_AccuDigitContainerName;
    sc = evtStore()->record(larAccuDigitContainer,SGkey);
    if (sc!=StatusCode::SUCCESS)
      {log << MSG::WARNING << "Unable to record LArAccumulatedDigitContainer with key " << SGkey << " from DetectorStore. " << endmsg;
      } 
    else
      log << MSG::INFO << "Recorded succesfully LArAccumulatedDigitContainer with key " << SGkey  << endmsg;
    
    sc = evtStore()->setConst(larAccuDigitContainer);
    if (sc.isFailure()) {
      log << MSG::ERROR << " Cannot lock LArAccumulatedDigitContainer " << endmsg;
      return(StatusCode::FAILURE);
    }


  } // loop over key container
  return StatusCode::SUCCESS;
}
  


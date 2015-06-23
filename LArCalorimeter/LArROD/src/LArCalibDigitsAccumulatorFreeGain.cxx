/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "LArROD/LArCalibDigitsAccumulatorFreeGain.h"

#include "CLHEP/Units/SystemOfUnits.h"
#include <math.h>
#include "stdint.h"

using CLHEP::ns;

LArCalibDigitsAccumulatorFreeGain::LArCalibDigitsAccumulatorFreeGain (const std::string& name, ISvcLocator* pSvcLocator):
  AthAlgorithm(name, pSvcLocator),
  m_larCablingSvc("LArCablingService"),
  m_onlineHelper(0),
  m_delay(0),
  m_isPulsed(false),
  m_calibAccuDigitContainerName("LArAccumulatedCalibDigits"),
  m_nStepTrigger(1),
  m_delayScale(1*ns),
  m_keepPulsed(false)
{
  declareProperty("LArAccuCalibDigitContainerName",m_calibAccuDigitContainerName);
  declareProperty("KeyList",m_keylist);
  declareProperty("StepOfTriggers",m_nStepTrigger);
  declareProperty("DelayScale",m_delayScale);
  declareProperty("KeepOnlyPulsed",m_keepPulsed);

  m_delay=-1;
  m_event_counter=0;
}


StatusCode LArCalibDigitsAccumulatorFreeGain::initialize(){
  
  MsgStream log(msgSvc(), name());
  StatusCode sc;

  // retrieve online ID helper
  sc = detStore()->retrieve(m_onlineHelper, "LArOnlineID");
  if (sc.isFailure()) {
    log << MSG::ERROR << "Could not get LArOnlineID helper !" << endreq;
    return StatusCode::FAILURE;
  }

  // retrieve cabling service
  IToolSvc* toolSvc;
  sc=service( "ToolSvc",toolSvc  );
  if (sc.isFailure()) {
    log << MSG::ERROR << "Unable to retrieve ToolSvc" << endreq;
    return StatusCode::FAILURE;
  }
  
  if (m_larCablingSvc.retrieve().isFailure()) {
    log << MSG::ERROR << "Unable to retrieve LArCablingService" << endreq;
    return StatusCode::FAILURE;
  }

  m_AccumulatedHG.resize(m_onlineHelper->channelHashMax());
  m_AccumulatedMG.resize(m_onlineHelper->channelHashMax());
  m_AccumulatedLG.resize(m_onlineHelper->channelHashMax());

  return StatusCode::SUCCESS;
}



StatusCode LArCalibDigitsAccumulatorFreeGain::execute() 
{
  
  StatusCode sc;
  MsgStream log(msgSvc(), name());

  if ( m_event_counter < 100 || m_event_counter%100==0 )
    log << MSG::INFO << "Processing event " << m_event_counter << endreq;
  ++m_event_counter;

  
  // pointer to input container
  const LArCalibDigitContainer* calibDigitContainer=NULL;

  // retrieve calibration settings
  const LArCalibParams* calibParams;
  sc=detStore()->retrieve(calibParams,"LArCalibParams");
  if (sc.isFailure())
    {log << MSG::ERROR << "Cannot load LArCalibParams from DetStore." << endreq;
      return StatusCode::FAILURE;
    }

  unsigned int sizeSteps = (m_nStepTrigger>1 ? (m_nStepTrigger+1):1);

  std::vector<std::string>::const_iterator key_it=m_keylist.begin();
  std::vector<std::string>::const_iterator key_it_e=m_keylist.end();

  // retrieve input calibDigits
  
  //Loop over all containers that are to be processed (e.g. different gains)
  for (;key_it!=key_it_e;key_it++) { 
    
    sc=evtStore()->retrieve(calibDigitContainer,*key_it);
    if(sc.isFailure()) {
      log << MSG::ERROR << "Can't retrieve LArCalibDigitContainer with key " << *key_it << "from StoreGate." << endreq;
      return StatusCode::SUCCESS;
    }else{
      log << MSG::DEBUG << "Retrieved LArCalibDigitContainer with key " << *key_it << " from StoreGate." << endreq;
    }

    // Loop over CalibDigitContainer
    LArCalibDigitContainer::const_iterator it=calibDigitContainer->begin();
    LArCalibDigitContainer::const_iterator it_end=calibDigitContainer->end();

    if(it == it_end) {
      log << MSG::DEBUG << "LArCalibDigitContainer with key=" << *key_it << " is empty " << endreq;
    }else{
      log << MSG::DEBUG << "LArCalibDigitContainer with key=" << *key_it << " has size =  " << calibDigitContainer->size() <<  endreq;
    }

    // counter of triggers 
    std::vector<unsigned int> ntrigger, nTriggerPerStep, nStepTrigger, iStepTrigger;
    // effective number of triggers: per channel
    ntrigger.resize(calibDigitContainer->size(),0);
    // asked number of triggers (from calib settings): per FEB
    nTriggerPerStep.resize(m_onlineHelper->febHashMax(),0);
    nStepTrigger.resize(m_onlineHelper->febHashMax(),0);
    iStepTrigger.resize(m_onlineHelper->febHashMax(),0);

    // output container
    LArAccumulatedCalibDigitContainer* larAccuCalibDigitContainerHG = new LArAccumulatedCalibDigitContainer();

    LArAccumulatedCalibDigitContainer* larAccuCalibDigitContainerMG = new LArAccumulatedCalibDigitContainer();

    LArAccumulatedCalibDigitContainer* larAccuCalibDigitContainerLG = new LArAccumulatedCalibDigitContainer();

    //Loop over all cells
    for (;it!=it_end;it++) {  

      if(m_keepPulsed && !(*it)->isPulsed()) continue;
      
      // identificators
      HWIdentifier chid=(*it)->hardwareID();      
      const HWIdentifier febid=m_onlineHelper->feb_Id(chid);
      const IdentifierHash febhash = m_onlineHelper->feb_Hash(febid);
      const IdentifierHash hashid = m_onlineHelper->channel_Hash(chid);
      
      // BELOW: DIRTY HACK BECAUSE THERE SEEMS TO BE A BUG IN THE CABLINGSVC CONCERNING THE CALIBLINES.

      // get calibration settings
      const std::vector<HWIdentifier>& calibLineID=m_larCablingSvc->calibSlotLine(chid);
      HWIdentifier calibModuleID;
      if(calibLineID.size()>0){
        calibModuleID=m_onlineHelper->calib_module_Id(calibLineID[0]);
        nTriggerPerStep[febhash] = calibParams->NTrigger(calibModuleID);
        log << MSG::DEBUG << "Ntrigger per step = " << nTriggerPerStep[febhash] << endreq;
        if(nTriggerPerStep[febhash] > 1000) nTriggerPerStep[febhash]=100; // very dirty !!! 
      } else {
        nTriggerPerStep[febhash] = 100; // very dirty !! 
      }

      // cell is pulsed ? 
      m_isPulsed = (*it)->isPulsed();
      
      //First cell to be processed, set delay
      if (m_delay==-1) { 
        m_delay=(*it)->delay();
      } else {
        // next cells: should be the same delay
        if (m_delay!=(*it)->delay()) {
          log << MSG::DEBUG << "Delay is changing to " << (*it)->delay() << " from " << m_delay << ": book a new LArAccumulatedCalibDigitContainer" << endreq;
          m_delay=(*it)->delay();
        }
      }
      
      CaloGain::CaloGain gain=(*it)->gain();
      if (gain<0 || gain>CaloGain::LARNGAIN) {
	log << MSG::ERROR << "Found not-matching gain number ("<< (int)gain <<")" << endreq;
        delete larAccuCalibDigitContainerHG;
        delete larAccuCalibDigitContainerMG;
        delete larAccuCalibDigitContainerLG;
	return StatusCode::FAILURE;
      }

      /***************************** High Gain ***************************/
      // object to be filled for each cell
      LArAccumulated& cellAccumulatedHG = m_AccumulatedHG[hashid];
      if (gain==CaloGain::LARHIGHGAIN) {
	// trigger counter for each cell
	cellAccumulatedHG.m_ntrigger++;
	log << MSG::DEBUG << "HG chid = " << chid << ", trigger = " << cellAccumulatedHG.m_ntrigger << ", DAC = " << (*it)->DAC() << endreq;
	// at first trigger, initialize vectors
	unsigned int sizeSamples = (*it)->samples().size();
	log << MSG::DEBUG << "sizeSteps = " << sizeSteps << ", # of samples = " << sizeSamples << endreq;
	if(cellAccumulatedHG.m_ntrigger==1){
	  cellAccumulatedHG.m_sum.clear();
	  cellAccumulatedHG.m_sum2.clear();
	  cellAccumulatedHG.m_sum.resize(sizeSamples,0);
	  cellAccumulatedHG.m_sum2.resize(sizeSamples,0);
	}
	for(unsigned int j=0;j<sizeSamples;j++) {
	  cellAccumulatedHG.m_sum[j] += (*it)->samples()[j];
	  cellAccumulatedHG.m_sum2[j] += (*it)->samples()[j]*(*it)->samples()[j];
	}
	log << MSG::DEBUG << "Sum = " << cellAccumulatedHG.m_sum[2] << endreq;
	log << MSG::DEBUG << "Sum2 = " << cellAccumulatedHG.m_sum2[2] << endreq;
      }	// end High Gain

      /***************************** Medium Gain ***************************/
      // object to be filled for each cell
      LArAccumulated& cellAccumulatedMG = m_AccumulatedMG[hashid];
      if (gain==CaloGain::LARMEDIUMGAIN) {
	// trigger counter for each cell
	cellAccumulatedMG.m_ntrigger++;
	log << MSG::DEBUG << "MG chid = " << chid << ", trigger = " << cellAccumulatedMG.m_ntrigger << ", DAC = " << (*it)->DAC() << endreq;
	// at first trigger, initialize vectors
	unsigned int sizeSamples = (*it)->samples().size();
	log << MSG::DEBUG << "sizeSteps = " << sizeSteps << ", # of samples = " << sizeSamples << endreq;
	if(cellAccumulatedMG.m_ntrigger==1){
	  cellAccumulatedMG.m_sum.clear();
	  cellAccumulatedMG.m_sum2.clear();
	  cellAccumulatedMG.m_sum.resize(sizeSamples,0);
	  cellAccumulatedMG.m_sum2.resize(sizeSamples,0);
	}
	for(unsigned int j=0;j<sizeSamples;j++){
	  cellAccumulatedMG.m_sum[j] += (*it)->samples()[j];
	  cellAccumulatedMG.m_sum2[j] += (*it)->samples()[j]*(*it)->samples()[j];
	}
	log << MSG::DEBUG << "Sum = " << cellAccumulatedMG.m_sum[2] << endreq;
	log << MSG::DEBUG << "Sum2 = " << cellAccumulatedMG.m_sum2[2] << endreq;
      }	// end High Gain

      /***************************** Low Gain ***************************/
      // object to be filled for each cell
      LArAccumulated& cellAccumulatedLG = m_AccumulatedLG[hashid];
      if (gain==CaloGain::LARLOWGAIN) {
	if (chid==959628800) std::cout << "---> in loop LG - DAC = " << (*it)->DAC() << std::endl;
	cellAccumulatedLG.m_ntrigger++;
	unsigned int sizeSamples = (*it)->samples().size();
	if(cellAccumulatedLG.m_ntrigger==1){
	  cellAccumulatedLG.m_sum.clear();
	  cellAccumulatedLG.m_sum2.clear();
	  cellAccumulatedLG.m_sum.resize(sizeSamples,0);
	  cellAccumulatedLG.m_sum2.resize(sizeSamples,0);
	}
	for(unsigned int j=0;j<sizeSamples;j++){
	  cellAccumulatedLG.m_sum[j] += (*it)->samples()[j];
	  cellAccumulatedLG.m_sum2[j] += (*it)->samples()[j]*(*it)->samples()[j];
	}
	log << MSG::DEBUG << "Sum = " << cellAccumulatedLG.m_sum[2] << endreq;
	log << MSG::DEBUG << "Sum2 = " << cellAccumulatedLG.m_sum2[2] << endreq;
      }	// end High Gain


      // when reached total number of triggers for this step, fill LArAccumulatedCalibDigit and reset number of triggers
      if( (cellAccumulatedHG.m_ntrigger+cellAccumulatedMG.m_ntrigger+cellAccumulatedLG.m_ntrigger)==nTriggerPerStep[febhash]){
	log << MSG::DEBUG << "filling LArAccumulatedCalibDigit " << endreq;
	log << MSG::DEBUG << "chid = " << chid << ", gain = " << gain << ", DAC = " << (*it)->DAC() << ", isPulsed = " << m_isPulsed << ", delay = " << m_delay << ", trigPerStep = " << nTriggerPerStep[febhash] << ", istep = " << iStepTrigger[febhash] << endreq;
	iStepTrigger[febhash]++;
	unsigned int sizeSamples = (*it)->samples().size();

	if (cellAccumulatedHG.m_ntrigger>0) {
	  LArAccumulatedCalibDigit* accuCalibDigitHG;
	  std::vector < uint32_t > sampleSumHG;
	  std::vector < uint32_t > sample2SumHG;
	  sampleSumHG.resize(sizeSamples,0);
	  sample2SumHG.resize(sizeSamples,0);
	  accuCalibDigitHG = new LArAccumulatedCalibDigit(chid, gain, sampleSumHG, sample2SumHG, 0, (uint16_t)(*it)->DAC(), (uint16_t)m_delay, m_isPulsed, 0, 0);
	  accuCalibDigitHG->setAddSubStep(cellAccumulatedHG.m_sum,cellAccumulatedHG.m_sum2,cellAccumulatedHG.m_ntrigger);
	  larAccuCalibDigitContainerHG->push_back(accuCalibDigitHG);
	}

	if (cellAccumulatedMG.m_ntrigger>0) {
	  LArAccumulatedCalibDigit* accuCalibDigitMG;
	  std::vector < uint32_t > sampleSumMG;
	  std::vector < uint32_t > sample2SumMG;
	  sampleSumMG.resize(sizeSamples,0);
	  sample2SumMG.resize(sizeSamples,0);
	  accuCalibDigitMG = new LArAccumulatedCalibDigit(chid, gain, sampleSumMG, sample2SumMG, 0, (uint16_t)(*it)->DAC(), (uint16_t)m_delay, m_isPulsed, 0, 0);
	  accuCalibDigitMG->setAddSubStep(cellAccumulatedMG.m_sum,cellAccumulatedMG.m_sum2,cellAccumulatedMG.m_ntrigger);
	  larAccuCalibDigitContainerMG->push_back(accuCalibDigitMG);
	}

	if (cellAccumulatedLG.m_ntrigger>0) {
	  LArAccumulatedCalibDigit* accuCalibDigitLG;
	  std::vector < uint32_t > sampleSumLG;
	  std::vector < uint32_t > sample2SumLG;
	  sampleSumLG.resize(sizeSamples,0);
	  sample2SumLG.resize(sizeSamples,0);
	  accuCalibDigitLG = new LArAccumulatedCalibDigit(chid, gain, sampleSumLG, sample2SumLG, 0, (uint16_t)(*it)->DAC(), (uint16_t)m_delay, m_isPulsed, 0, 0);
	  accuCalibDigitLG->setAddSubStep(cellAccumulatedLG.m_sum,cellAccumulatedLG.m_sum2,cellAccumulatedLG.m_ntrigger);
	  larAccuCalibDigitContainerLG->push_back(accuCalibDigitLG);
	}

	cellAccumulatedHG.m_ntrigger = 0;
	cellAccumulatedMG.m_ntrigger = 0;
	cellAccumulatedLG.m_ntrigger = 0;	
      }


      
    }// loop over cells in container


    larAccuCalibDigitContainerHG->setDelayScale(m_delayScale);
    larAccuCalibDigitContainerMG->setDelayScale(m_delayScale);
    larAccuCalibDigitContainerLG->setDelayScale(m_delayScale);

    sc = evtStore()->record(larAccuCalibDigitContainerHG,"HIGH");
    if (sc!=StatusCode::SUCCESS)
      log << MSG::WARNING << "Unable to record LArAccumulatedCalibDigitContainer with key HIGH from DetectorStore. " << endreq;
    else
      log << MSG::DEBUG << "Recorded succesfully LArAccumulatedCalibDigitContainer with key HIGH" << endreq;
    sc = evtStore()->setConst(larAccuCalibDigitContainerHG);
    if (sc.isFailure()) {
      log << MSG::ERROR << " Cannot lock LArAccumulatedCalibDigitContainerHG " << endreq;
      delete larAccuCalibDigitContainerMG;
      delete larAccuCalibDigitContainerLG;
      return(StatusCode::FAILURE);
    }

    sc = evtStore()->record(larAccuCalibDigitContainerMG,"MEDIUM");
    if (sc!=StatusCode::SUCCESS) 
      log << MSG::WARNING << "Unable to record LArAccumulatedCalibDigitContainer with key MEDIUM from DetectorStore. " << endreq;
    else
      log << MSG::DEBUG << "Recorded succesfully LArAccumulatedCalibDigitContainer with key MEDIUM" << endreq;
    sc = evtStore()->setConst(larAccuCalibDigitContainerMG);
    if (sc.isFailure()) {
      log << MSG::ERROR << " Cannot lock LArAccumulatedCalibDigitContainerMG " << endreq;
      delete larAccuCalibDigitContainerLG;
      return(StatusCode::FAILURE);
    }

    sc = evtStore()->record(larAccuCalibDigitContainerLG,"LOW");
    if (sc!=StatusCode::SUCCESS)
      log << MSG::WARNING << "Unable to record LArAccumulatedCalibDigitContainer with key LOW from DetectorStore. " << endreq;
    else
      log << MSG::DEBUG << "Recorded succesfully LArAccumulatedCalibDigitContainer with key LOW" << endreq;
    sc = evtStore()->setConst(larAccuCalibDigitContainerLG);
    if (sc.isFailure()) {
      log << MSG::ERROR << " Cannot lock LArAccumulatedCalibDigitContainerLG " << endreq;
      return(StatusCode::FAILURE);
    }
  } // loop over key container
  return StatusCode::SUCCESS;
}
  


/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#include "LArROD/LArCalibDigitsAccumulator.h"

#include "CLHEP/Units/SystemOfUnits.h"
#include <math.h>
#include "stdint.h"

using CLHEP::ns;

LArCalibDigitsAccumulator::LArCalibDigitsAccumulator (const std::string& name, ISvcLocator* pSvcLocator):
  AthAlgorithm(name, pSvcLocator),
  m_onlineHelper(0),
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


StatusCode LArCalibDigitsAccumulator::initialize(){
  
  // retrieve online ID helper
  ATH_CHECK( detStore()->retrieve(m_onlineHelper, "LArOnlineID") );

  ATH_CHECK( m_calibMapKey.initialize() );

  m_Accumulated.resize(m_onlineHelper->channelHashMax());

  return StatusCode::SUCCESS;
}



StatusCode LArCalibDigitsAccumulator::execute() 
{
  
  StatusCode sc;

  if ( m_event_counter < 100 || m_event_counter%100==0 )
    ATH_MSG_INFO( "Processing event " << m_event_counter );
  ++m_event_counter;

   SG::ReadCondHandle<LArCalibLineMapping> clHdl{m_calibMapKey};
   const LArCalibLineMapping *clcabling {*clHdl};
   if(!clcabling) {
      ATH_MSG_WARNING( "Do not have calib line mapping from key " << m_calibMapKey.key() );
      return StatusCode::FAILURE;
   }
  
  // pointer to input container
  const LArCalibDigitContainer* calibDigitContainer=NULL;

  // retrieve calibration settings
  const LArCalibParams* calibParams;
  sc=detStore()->retrieve(calibParams,"LArCalibParams");
  if (sc.isFailure())
    {ATH_MSG_ERROR( "Cannot load LArCalibParams from DetStore." );
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
      ATH_MSG_ERROR( "Can't retrieve LArCalibDigitContainer with key " << *key_it << "from StoreGate." );
      return StatusCode::SUCCESS;
    }else{
      ATH_MSG_DEBUG( "Retrieved LArCalibDigitContainer with key " << *key_it << " from StoreGate." );
    }

    // store LArAccumulatedDigits
    ACCUMDIGIT_VEC& vAccum = m_my_vec;
    if(vAccum.size()==0) vAccum.resize(m_onlineHelper->channelHashMax());

    // Loop over CalibDigitContainer
    LArCalibDigitContainer::const_iterator it=calibDigitContainer->begin();
    LArCalibDigitContainer::const_iterator it_end=calibDigitContainer->end();

    if(it == it_end) {
      ATH_MSG_DEBUG( "LArCalibDigitContainer with key=" << *key_it << " is empty " );
    }else{
      ATH_MSG_DEBUG( "LArCalibDigitContainer with key=" << *key_it << " has size =  " << calibDigitContainer->size() );
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
    LArAccumulatedCalibDigitContainer* larAccuCalibDigitContainer = new LArAccumulatedCalibDigitContainer();
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
      const std::vector<HWIdentifier>& calibLineID=clcabling->calibSlotLine(chid);
      HWIdentifier calibModuleID;
      if(calibLineID.size()>0){
	calibModuleID=m_onlineHelper->calib_module_Id(calibLineID[0]);
	nTriggerPerStep[febhash] = calibParams->NTrigger(calibModuleID);
	ATH_MSG_DEBUG( "Ntrigger per step = " << nTriggerPerStep[febhash] );
	if(nTriggerPerStep[febhash] > 1000) nTriggerPerStep[febhash]=100; // very dirty !!! 
      }else{

	nTriggerPerStep[febhash] = 100; // very dirty !! 
      }

      // cell is pulsed ? 
      bool isPulsed = (*it)->isPulsed();
      
      //First cell to be processed, set delay
      if (m_delay==-1) { 
	m_delay=(*it)->delay();
      }
      else
	// next cells: should be the same delay
	if (m_delay!=(*it)->delay()) {
	  ATH_MSG_DEBUG( "Delay is changing to " << (*it)->delay() << " from " << m_delay << ": book a new LArAccumulatedCalibDigitContainer" );
	  m_delay=(*it)->delay();
	}

      CaloGain::CaloGain gain=(*it)->gain();
      if (gain<0 || gain>CaloGain::LARNGAIN)
	{ATH_MSG_ERROR( "Found not-matching gain number ("<< (int)gain <<")" );
          delete larAccuCalibDigitContainer;
	  return StatusCode::FAILURE;
	}

      // object to be filled for each cell
      LArAccumulated& cellAccumulated = m_Accumulated[hashid];
      
      // trigger counter for each cell
      cellAccumulated.m_ntrigger++;

      ATH_MSG_DEBUG( "chid = " << chid << ", trigger = " << cellAccumulated.m_ntrigger << ", DAC = " << (*it)->DAC() );

      // at first trigger, initialize vectors
      unsigned int sizeSamples = (*it)->samples().size();
      ATH_MSG_DEBUG( "sizeSteps = " << sizeSteps << ", # of samples = " << sizeSamples );

      LArAccumulatedCalibDigit* accuCalibDigit;

      if(cellAccumulated.m_ntrigger==1){
	cellAccumulated.m_sum.clear();
	cellAccumulated.m_sum2.clear();
	cellAccumulated.m_sum.resize(sizeSamples,0);
	cellAccumulated.m_sum2.resize(sizeSamples,0);

	// first time, put new LArAccumulatedCalibDigit	
	// Modif J. Labbe from JF. Marchand - Nov. 2009
	//	accuCalibDigit = new LArAccumulatedCalibDigit(chid,gain,sizeSamples,0,m_nStepTrigger,(uint16_t)(*it)->DAC(),(uint16_t)m_delay,isPulsed);
	accuCalibDigit = new LArAccumulatedCalibDigit(chid,gain,sizeSamples,(uint16_t)(*it)->DAC(),(uint16_t)m_delay,isPulsed,0,0);
	vAccum[hashid]=accuCalibDigit;

      }else{

	// other times, retrieve the pointer to LArAccumulatedCalibDigit
	accuCalibDigit = m_my_vec[hashid];

      }
      

      for(unsigned int j=0;j<sizeSamples;j++){
	cellAccumulated.m_sum[j] += (*it)->samples()[j];
	cellAccumulated.m_sum2[j] += (*it)->samples()[j]*(*it)->samples()[j];
      }

      ATH_MSG_DEBUG( "Sum = " << cellAccumulated.m_sum[0] );
      ATH_MSG_DEBUG( "Sum2 = " << cellAccumulated.m_sum2[0] );

      // when reached total number of triggers for this step, fill LArAccumulatedCalibDigit and reset number of triggers

      if(cellAccumulated.m_ntrigger==nTriggerPerStep[febhash]){
	ATH_MSG_DEBUG( "filling LArAccumulatedCalibDigit " );
	ATH_MSG_DEBUG( "chid = " << chid << ", gain = " << gain << ", DAC = " << (*it)->DAC() << ", isPulsed = " << isPulsed << ", delay = " << m_delay << ", trigPerStep = " << nTriggerPerStep[febhash] << ", istep = " << iStepTrigger[febhash] );
	
	accuCalibDigit->setAddSubStep(cellAccumulated.m_sum,cellAccumulated.m_sum2,nTriggerPerStep[febhash]);
	iStepTrigger[febhash]++;
	
	//	std::cout << "DAC after = " << accuCalibDigit->DAC() << std::endl;

	std::vector<float> mean =  accuCalibDigit->mean();
	std::vector<float> RMS =  accuCalibDigit->RMS();

	for(unsigned int i=0;i<mean.size();i++){
	  ATH_MSG_DEBUG( "mean["<<i<<"] = " << mean[i] );
	  ATH_MSG_DEBUG( "RMS["<<i<<"] = " << RMS[i] );
	}

	larAccuCalibDigitContainer->push_back(accuCalibDigit);

	cellAccumulated.m_ntrigger = 0;

      }
      
    }// loop over cells in container


    larAccuCalibDigitContainer->setDelayScale(m_delayScale);
    sc = evtStore()->record(larAccuCalibDigitContainer,*key_it);
    if (sc!=StatusCode::SUCCESS)
      {ATH_MSG_WARNING( "Unable to record LArAccumulatedCalibDigitContainer with key " << *key_it << " from DetectorStore. " );
      } 
    else
      ATH_MSG_DEBUG( "Recorded succesfully LArAccumulatedCalibDigitContainer with key " << *key_it  );
    
    sc = evtStore()->setConst(larAccuCalibDigitContainer);
    if (sc.isFailure()) {
      ATH_MSG_ERROR( " Cannot lock LArAccumulatedCalibDigitContainer " );
      return(StatusCode::FAILURE);
    }


  } // loop over key container
  return StatusCode::SUCCESS;
}
  


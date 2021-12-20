/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#include "LArROD/LArCalibDigitsAccumulator.h"
#include "LArIdentifier/LArOnlineID.h"
#include "LArIdentifier/LArOnline_SuperCellID.h"
#include "xAODEventInfo/EventInfo.h"
#include "CLHEP/Units/SystemOfUnits.h"
#include <math.h>
#include "stdint.h"

using CLHEP::ns;

LArCalibDigitsAccumulator::LArCalibDigitsAccumulator (const std::string& name, ISvcLocator* pSvcLocator):
  AthAlgorithm(name, pSvcLocator),
  m_sc2ccMappingTool("CaloSuperCellIDTool"), 
  m_onlineHelper(0),
  m_sampleShift(0)
{
  declareProperty("LArAccuCalibDigitContainerName",m_calibAccuDigitContainerName, "LArAccumulatedCalibDigits");
  declareProperty("KeyList",m_keylist);
  declareProperty("StepOfTriggers",m_nStepTrigger=1);
  declareProperty("DelayScale",m_delayScale=1*ns);
  declareProperty("KeepOnlyPulsed",m_keepPulsed=false);
  declareProperty("KeepFullyPulsedSC",m_keepFullyPulsedSC=false);
  declareProperty("isSC",m_isSC=false);
  declareProperty("SampleShift",m_sampleShift=0);
  m_delay=-1;
  m_event_counter=0;
}


StatusCode LArCalibDigitsAccumulator::initialize(){
  
  m_eventNb=0;
  // retrieve online ID helper
  StatusCode sc;
  ATH_CHECK( m_cablingKey.initialize() );
  ATH_CHECK( m_calibMapKey.initialize() );

  if(m_isSC){
     const LArOnline_SuperCellID *scid;
     sc = detStore()->retrieve(scid, "LArOnline_SuperCellID");
     if (sc.isFailure()) {
        ATH_MSG_ERROR( "Could not get LArOnline_SuperCellID helper !" );
        return sc;
     } else {
        m_onlineHelper = (const LArOnlineID_Base*)scid;
        ATH_MSG_DEBUG("Found the LArOnlineID helper");
     }
     ATH_CHECK( m_sc2ccMappingTool.retrieve() );
     ATH_CHECK( m_cablingKeySC.initialize() );
     ATH_CHECK( m_calibMapSCKey.initialize() );
  } else { 
     const LArOnlineID* ll;
     sc = detStore()->retrieve(ll, "LArOnlineID");
     if (sc.isFailure()) {
        ATH_MSG_ERROR( "Could not get LArOnlineID helper !" );
        return sc;
     } else {
        m_onlineHelper = (const LArOnlineID_Base*)ll;
        ATH_MSG_DEBUG(" Found the LArOnlineID helper. ");
     }
  } //m_isSC

  

  m_Accumulated.resize(m_onlineHelper->channelHashMax());

  for (const std::string& key : m_keylist) {
    std::map<int,std::vector<int>*>* map_forKey = new std::map<int,std::vector<int>*>();
    m_readingsMap[key] = map_forKey;
  }


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
  
  // new here ====
  const LArOnOffIdMapping* cabling(0);
  const LArOnOffIdMapping* cablingLeg(0);
  if( m_isSC ){
    SG::ReadCondHandle<LArOnOffIdMapping> cablingHdl{m_cablingKeySC};
    cabling = {*cablingHdl};
    if(!cabling) {
      ATH_MSG_ERROR("Do not have mapping object " << m_cablingKeySC.key());
      return StatusCode::FAILURE;
    }
    
    SG::ReadCondHandle<LArOnOffIdMapping> cablingHdlLeg{m_cablingKey};
    cablingLeg = {*cablingHdlLeg};
    if(!cablingLeg) {
      ATH_MSG_ERROR("Do not have mapping object " << m_cablingKey.key());
      return StatusCode::FAILURE;
    }
  }

  // =============

   

  // pointer to input container
  const LArCalibDigitContainer* calibDigitContainer=NULL;

  // Event info 
  const DataHandle<xAOD::EventInfo> thisEventInfo;
  ATH_CHECK( evtStore()->retrieve(thisEventInfo) );
  unsigned eventNb = thisEventInfo->eventNumber();
  eventNb=m_eventNb;
  ++m_eventNb;
  // retrieve calibration settings
  const LArCalibParams* calibParams;
  sc=detStore()->retrieve(calibParams,"LArCalibParams");
  if (sc.isFailure())
    {ATH_MSG_ERROR( "Cannot load LArCalibParams from DetStore." );
      return StatusCode::FAILURE;
    }

  unsigned int sizeSteps = (m_nStepTrigger>1 ? (m_nStepTrigger+1):1);

  // retrieve input calibDigits
  
  //Loop over all containers that are to be processed (e.g. different gains)
  for (const std::string& key : m_keylist) {

    std::map<int,std::vector<int>*>* readingsMap_pointer = 0;
    m_readingsMap_it = m_readingsMap.find(key);
    if (m_readingsMap_it == m_readingsMap.end()) {
      ATH_MSG_INFO("Did not find corresponding map for key " << key);
    } else {
      readingsMap_pointer = m_readingsMap_it->second;
    }


    sc=evtStore()->retrieve(calibDigitContainer,key);
    if(sc.isFailure()) {
      ATH_MSG_ERROR( "Can't retrieve LArCalibDigitContainer with key " << key << "from StoreGate." );
      return StatusCode::SUCCESS;
    }else{
      ATH_MSG_DEBUG( "Retrieved LArCalibDigitContainer with key " << key << " from StoreGate." );
    }

    // store LArAccumulatedDigits
    ACCUMDIGIT_VEC& vAccum = m_my_vec;
    if(vAccum.size()==0) vAccum.resize(m_onlineHelper->channelHashMax());

    // Loop over CalibDigitContainer

    if(calibDigitContainer->empty()) {
      ATH_MSG_DEBUG( "LArCalibDigitContainer with key=" << key << " is empty " );
    }else{
      ATH_MSG_DEBUG( "LArCalibDigitContainer with key=" << key << " has size =  " << calibDigitContainer->size() );
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
    for (const LArCalibDigit* digit : *calibDigitContainer) {
      if(m_keepPulsed && !digit->isPulsed()) continue;
      
      // identificators
      HWIdentifier chid=digit->hardwareID();      
      const HWIdentifier febid=m_onlineHelper->feb_Id(chid);
      const IdentifierHash febhash = m_onlineHelper->feb_Hash(febid);
      const IdentifierHash hashid = m_onlineHelper->channel_Hash(chid);


      
      //// ############################

      std::vector<Identifier> ccellIds(0);
      unsigned numPulsedLeg = 0;
      unsigned numCL = 0;
      if( m_isSC ){
	Identifier myofflineID = cabling->cnvToIdentifier(digit->hardwareID()) ;
	ccellIds = m_sc2ccMappingTool->superCellToOfflineID( myofflineID );
        for (Identifier id : ccellIds) {// loop cells in sc
	  HWIdentifier cellLegHWID  = cablingLeg->createSignalChannelID(id);
	  //ATH_MSG_WARNING( "oi " << id << cellLegHWID);
	  const std::vector<HWIdentifier>& calibLineLeg = clcabling->calibSlotLine(cellLegHWID);
	  numCL += calibLineLeg.size();
          for (HWIdentifier calibLineHWID : calibLineLeg) {// loop legacy calib lines
	    if ( calibParams->isPulsed(eventNb,calibLineHWID) ){
	      numPulsedLeg += 1;
	    }else{
	      if ( digit->isPulsed() ) ATH_MSG_WARNING("SC "<< chid << " constituent cell "<< cellLegHWID << " calib line "<< calibLineHWID<< " not pulsed");}
	  }//end calib line Leg loop
	}//end legacy cell loop
	if ( digit->isPulsed() && numPulsedLeg != numCL ){ //(int)(ccellIds.size()) ){
	  ATH_MSG_WARNING("Number of pulsed legacy cells does not equal number of calibration lines "<<chid<<"!! LArParams counter = " << numPulsedLeg << ", SC2CCMappingTool = " << ccellIds.size() << ", num CLs = "<< numCL);

	  if(m_keepFullyPulsedSC){ // && numPulsedLeg < (int)(ccellIds.size())){
	    ATH_MSG_WARNING("Discarding this SC ("<<chid<<") as it is not fully pulsed");
	    continue;
	  }
	}

	ATH_MSG_DEBUG("SC "<<chid<<" pulsed cells "<< numPulsedLeg <<" or "<< ccellIds.size()<<", "<<numCL<<" calibration lines");
      } // end m_isSC

      //// ############################
      

      m_channelMap_it = readingsMap_pointer->find(chid.get_identifier32().get_compact());
      std::vector<int>* cellStorage;
      if (m_channelMap_it == readingsMap_pointer->end()) {
	std::vector<int>* cellVector = new std::vector<int>();
        (*readingsMap_pointer)[chid.get_identifier32().get_compact()] = cellVector;
        
        cellStorage = cellVector;
      } else {
        cellStorage = m_channelMap_it->second;
      }


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
      bool isPulsed = digit->isPulsed();
      
      //First cell to be processed, set delay
      if (m_delay==-1) { 
	m_delay=digit->delay();
      }
      else
	// next cells: should be the same delay
	if (m_delay!=digit->delay()) {
	  ATH_MSG_DEBUG( "Delay is changing to " << digit->delay() << " from " << m_delay << ": book a new LArAccumulatedCalibDigitContainer" );
	  m_delay=digit->delay();
	}
      
      CaloGain::CaloGain gain=digit->gain();
      if (gain<0 || gain>CaloGain::LARNGAIN)
	{ATH_MSG_ERROR( "Found not-matching gain number ("<< (int)gain <<")" );
          delete larAccuCalibDigitContainer;
	  return StatusCode::FAILURE;
	}
      
      // object to be filled for each cell
      LArAccumulated& cellAccumulated = m_Accumulated[hashid];
      
      // trigger counter for each cell
      cellAccumulated.m_ntrigger++;
      ATH_MSG_INFO( "chid = " << chid << ", trigger = " << cellAccumulated.m_ntrigger << ", DAC = " << digit->DAC()<<", Delay = "<<digit->delay()<<", isPulsed? "<<digit->isPulsed() );
            
      // at first trigger, initialize vectors
      unsigned int sizeSamples = digit->samples().size();
      ATH_MSG_DEBUG( "sizeSteps = " << sizeSteps << ", # of samples = " << sizeSamples );

      LArAccumulatedCalibDigit* accuCalibDigit;

      if(cellAccumulated.m_ntrigger==1){
	cellAccumulated.m_sum.clear();
	cellAccumulated.m_sum2.clear();
	cellAccumulated.m_sum.resize(sizeSamples,0);
	cellAccumulated.m_sum2.resize(sizeSamples,0);

	// first time, put new LArAccumulatedCalibDigit	
	// Modif J. Labbe from JF. Marchand - Nov. 2009
	//	accuCalibDigit = new LArAccumulatedCalibDigit(chid,gain,sizeSamples,0,m_nStepTrigger,(uint16_t)digit->DAC(),(uint16_t)m_delay,isPulsed);
	
	if ( m_isSC ){	  
	  ATH_MSG_DEBUG("Channel "<<chid<<" DAC "<< digit->DAC()<< " will multiply by "<<numPulsedLeg<< " = "<<digit->DAC()*numPulsedLeg<<" is pulsed??? "<<isPulsed);
	  accuCalibDigit = new LArAccumulatedCalibDigit(chid,gain,sizeSamples,(int32_t)(digit->DAC()*numPulsedLeg),(uint16_t)m_delay,isPulsed,0,0);
	  //}
	}else{
	  accuCalibDigit = new LArAccumulatedCalibDigit(chid,gain,sizeSamples,(uint16_t)digit->DAC(),(uint16_t)m_delay,isPulsed,0,0);
	}
	vAccum[hashid]=accuCalibDigit;

      }else{

	// other times, retrieve the pointer to LArAccumulatedCalibDigit
	accuCalibDigit = m_my_vec[hashid];

      }
      

      for(unsigned int j=0;j<sizeSamples;j++){
	cellStorage->push_back(digit->samples()[j]);
	cellAccumulated.m_sum[j] += digit->samples()[j];
	cellAccumulated.m_sum2[j] += digit->samples()[j]*digit->samples()[j];
      }


      // when reached total number of triggers for this step, fill LArAccumulatedCalibDigit and reset number of triggers

      if(cellAccumulated.m_ntrigger==nTriggerPerStep[febhash]){
	ATH_MSG_DEBUG( "filling LArAccumulatedCalibDigit " );
	ATH_MSG_DEBUG( "chid = " << chid << ", gain = " << gain << ", DAC = " << digit->DAC() << ", isPulsed = " << isPulsed << ", delay = " << m_delay << ", trigPerStep = " << nTriggerPerStep[febhash] << ", istep = " << iStepTrigger[febhash] );



	if ( m_sampleShift != 0 ){
	  
	  // First loop to find "allGood" samplings and their mean values
	  for (unsigned int sample = 0; sample < sizeSamples; ++sample) {
	    std::vector<int> sampleVector;
	    uint64_t sum  = 0;
	    uint64_t sum2 = 0;
	    
	    for (unsigned int trig = 0; trig < nTriggerPerStep[febhash]; ++trig) {
              if ( m_sampleShift < 0 ){
		if (sample >= sizeSamples+m_sampleShift){
		  sampleVector.push_back(cellStorage->at(trig*sizeSamples+sizeSamples-1)); 
		}else{
		  sampleVector.push_back(cellStorage->at(trig*sizeSamples-m_sampleShift+sample));
		}
	      }else{
		if ( sample < static_cast<unsigned>(std::abs(m_sampleShift)) ){
		  sampleVector.push_back(cellStorage->at(trig*sizeSamples));
		}else{		  
		  sampleVector.push_back(cellStorage->at(trig*sizeSamples-m_sampleShift+sample));
		}
	      }
	    }// trig

	    sort(sampleVector.begin(), sampleVector.end());
	    
	    for (unsigned int trig = 0; trig < nTriggerPerStep[febhash]; ++trig) {
              sum  += sampleVector.at(trig);
	      sum2 += sampleVector.at(trig) * sampleVector.at(trig);
	    }

            cellAccumulated.m_sum[sample]  = sum;
	    cellAccumulated.m_sum2[sample] = sum2;

	  }//sample
	}//m_sampleShift
	
	accuCalibDigit->setAddSubStep(cellAccumulated.m_sum,cellAccumulated.m_sum2,nTriggerPerStep[febhash]);
	iStepTrigger[febhash]++;
	
	std::vector<float> mean =  accuCalibDigit->mean();
	std::vector<float> RMS =  accuCalibDigit->RMS();

	for(unsigned int i=0;i<mean.size();i++){
	  ATH_MSG_DEBUG( "mean["<<i<<"] = " << mean[i] );
	  ATH_MSG_DEBUG( "RMS["<<i<<"] = " << RMS[i] );
	}

	larAccuCalibDigitContainer->push_back(accuCalibDigit);

	cellAccumulated.m_ntrigger = 0;

	delete cellStorage;
	(*readingsMap_pointer).erase(chid.get_identifier32().get_compact());
	
      }
      
    }// loop over cells in container


    larAccuCalibDigitContainer->setDelayScale(m_delayScale);
    sc = evtStore()->record(larAccuCalibDigitContainer,key);
    if (sc!=StatusCode::SUCCESS)
      {ATH_MSG_WARNING( "Unable to record LArAccumulatedCalibDigitContainer with key " << key << " from DetectorStore. " );
      } 
    else
      ATH_MSG_DEBUG( "Recorded succesfully LArAccumulatedCalibDigitContainer with key " << key  << " with size " << larAccuCalibDigitContainer->size());
    
    sc = evtStore()->setConst(larAccuCalibDigitContainer);
    if (sc.isFailure()) {
      ATH_MSG_ERROR( " Cannot lock LArAccumulatedCalibDigitContainer " );
      return(StatusCode::FAILURE);
    }


  } // loop over key container
  return StatusCode::SUCCESS;
}
  


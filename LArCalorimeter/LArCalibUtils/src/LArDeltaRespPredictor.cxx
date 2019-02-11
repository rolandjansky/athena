/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#include "LArCalibUtils/LArDeltaRespPredictor.h"

#include "GaudiKernel/ToolHandle.h"

#include "LArRawConditions/LArCaliWaveContainer.h"
#include "LArRawConditions/LArWFParamsContainer.h"

#include <iostream>
#include <fstream>

LArDeltaRespPredictor::LArDeltaRespPredictor (const std::string& name, ISvcLocator* pSvcLocator) : 
  AthAlgorithm(name, pSvcLocator)
{
  // list of gains to be processed
  m_keylist.clear() ;
  m_keylist.push_back("HIGH");
  m_keylist.push_back("MEDIUM");
  m_keylist.push_back("LOW");
  declareProperty("KeyList",m_keylist);  
}

LArDeltaRespPredictor::~LArDeltaRespPredictor() 
{}

StatusCode LArDeltaRespPredictor::initialize() 
{
  ATH_CHECK(m_cablingKey.initialize());

  return StatusCode::SUCCESS ;
}

StatusCode LArDeltaRespPredictor::stop() 
{
  ToolHandle<LArWFParamTool> larWFParamTool("LArWFParamTool");
  ATH_CHECK( larWFParamTool.retrieve() );

  ToolHandle<LArDeltaRespTool> larDeltaRespTool("LArDeltaRespTool");
  ATH_CHECK( larDeltaRespTool.retrieve() );
    
  const LArCaliWaveContainer* caliWaveContainer = nullptr;
  std::string keyCali = "CaliWave" ;
  ATH_CHECK( detStore()->retrieve(caliWaveContainer,keyCali) );
  ATH_MSG_INFO ( "Processing LArCaliWaveContainer from StoreGate, key='CaliWave'" );

  SG::ReadCondHandle<LArOnOffIdMapping> cablingHdl{m_cablingKey};
  const LArOnOffIdMapping* cabling{*cablingHdl};
  if(!cabling) {
     ATH_MSG_ERROR("Do not have mapping object " << m_cablingKey.key() );
     return StatusCode::FAILURE;
  }

  // Create new LArCaliWaveContainer for DeltaResp(s)
  LArCaliWaveContainer* larDeltaRespContainer = 0;

  // Only record if keylist is not empty
  if (m_keylist.empty())
    return StatusCode::SUCCESS;

  // Create new LArCaliWaveContainer for DeltaResp(s)
  larDeltaRespContainer = new LArCaliWaveContainer();
    
  // Record LArDeltaRespContainer to DetectorStore
  keyCali += "_delta" ;
  ATH_CHECK( detStore()->record(larDeltaRespContainer,keyCali) );
  
  // get the waveforms from the detector store 
  std::vector<std::string>::const_iterator key_it=m_keylist.begin();
  std::vector<std::string>::const_iterator key_it_e=m_keylist.end();
  
  for (;key_it!=key_it_e;key_it++) { //Loop over all containers that are to be processed (e.g. different gains)
    
    // Set gain from key value
    int gain = CaloGain::LARHIGHGAIN;
    if      ((*key_it) == "MEDIUM") gain = CaloGain::LARMEDIUMGAIN;
    else if ((*key_it) == "LOW")    gain = CaloGain::LARLOWGAIN;
    
    // Create new LArWFParamsContainer
    //LArWFParamsContainer* larWFParamsContainer=new LArWFParamsContainer();
    
    typedef LArCaliWaveContainer::ConstConditionsMapIterator const_iterator;
    const_iterator itVec   = caliWaveContainer->begin(gain);
    const_iterator itVec_e = caliWaveContainer->end(gain);

    // Loop over caliWaveContainer (e.g. different cells and DAC values)
      
    for (; itVec != itVec_e; ++itVec) {
		
      LArCaliWaveContainer::LArCaliWaves::const_iterator cont_it   = (*itVec).begin();
      LArCaliWaveContainer::LArCaliWaves::const_iterator cont_it_e = (*itVec).end();

      // Get the vector of waves for this chid,gain
      LArCaliWaveContainer::LArCaliWaves& dacWaves = 
	  larDeltaRespContainer->get(itVec.channelId(), gain);

      for (;cont_it!=cont_it_e;cont_it++) { 
        const LArCaliWave& larCaliWave=*cont_it;
	if ( larCaliWave.getFlag() != LArWave::dac0 ) { // skip dac0 waves
	  // Get the waveform parameters
          LArWFParams wfParams;
          StatusCode sc = larWFParamTool->getLArWaveParams(larCaliWave,
                                                           itVec.channelId(),
                                                           (CaloGain::CaloGain)gain,
                                                           wfParams, cabling);
          if (sc.isFailure()) { // bad parameters
            ATH_MSG_INFO ( "Bad parameters for channel " << 
                           (itVec.channelId()) );
	  } else { // Compute Delta Response
  	    LArCaliWave lardeltaresp = larDeltaRespTool->makeLArDeltaResp(wfParams, 
									  larCaliWave);
	    lardeltaresp.setFlag( LArWave::predCali ) ;
	    dacWaves.push_back(lardeltaresp);
	  }
	}   // end if on dac0
      }  // end loop over cells
    }  // end loop over cell vecs
  }  // End loop over all CaliWave containers

  ATH_MSG_DEBUG ( "LArDeltaRespPredictor stopped!" );
  return StatusCode::SUCCESS;
}

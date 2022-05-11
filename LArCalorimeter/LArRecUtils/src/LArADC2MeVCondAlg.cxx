/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/


#include "LArADC2MeVCondAlg.h"
#include "LArIdentifier/LArOnline_SuperCellID.h"
#include "LArIdentifier/LArOnlineID.h"
#include "CaloIdentifier/CaloGain.h"
#include <memory>
#include "GaudiKernel/EventIDRange.h"

LArADC2MeVCondAlg::LArADC2MeVCondAlg(const std::string& name, ISvcLocator* pSvcLocator) :
  AthReentrantAlgorithm(name, pSvcLocator)
{}

LArADC2MeVCondAlg::~LArADC2MeVCondAlg() {}


StatusCode LArADC2MeVCondAlg::initialize() {

  //Identifier helper:
  if (m_isSuperCell) {
    m_nGains=1;
    const LArOnline_SuperCellID* scidhelper;
    ATH_CHECK(detStore()->retrieve(scidhelper,"LArOnline_SuperCellID"));
    m_larOnlineID=scidhelper; //cast to base-class
  }
  else {//regular cells
    m_nGains=3;
    const LArOnlineID* idhelper;
    ATH_CHECK(detStore()->retrieve(idhelper,"LArOnlineID"));
    m_larOnlineID=idhelper; //cast to base-class
  }

  if (!m_larOnlineID) {
    ATH_MSG_ERROR("Failed to obtain LArOnlineID");
    return StatusCode::FAILURE;
  }

  // Read handle for cabling
  ATH_CHECK(m_cablingKey.initialize());

 // Read handles for input conditions
  ATH_CHECK(m_lAruA2MeVKey.initialize());	   
  ATH_CHECK(m_lArDAC2uAKey.initialize());	   
  ATH_CHECK(m_lArRampKey.initialize());

  //The following two are optional (not used for MC and/or SuperCells)
  if (m_lArMphysOverMcalKey.key().size()) {
    ATH_CHECK(m_lArMphysOverMcalKey.initialize()); 
  }
  if (m_lArHVScaleCorrKey.key().size()) {
    ATH_CHECK(m_lArHVScaleCorrKey.initialize());   
  }

  //Write handle
  ATH_CHECK( m_ADC2MeVKey.initialize() );

  ATH_CHECK(m_febConfigKey.initialize(m_useFEBGainThresholds));

  return StatusCode::SUCCESS;
}


StatusCode LArADC2MeVCondAlg::execute(const EventContext& ctx) const{

  //Set up write handle
  SG::WriteCondHandle<LArADC2MeV> writeHandle{m_ADC2MeVKey,ctx};
  
  if (writeHandle.isValid()) {
    ATH_MSG_DEBUG("Found valid write handle");
    return StatusCode::SUCCESS;
  }  

  //Cabling (should have an ~infinte IOV)
  SG::ReadCondHandle<LArOnOffIdMapping> cablingHdl{m_cablingKey,ctx};
  const LArOnOffIdMapping* cabling{*cablingHdl};
  writeHandle.addDependency(cablingHdl);

  //Get pointers to input data and determine validity range
  SG::ReadCondHandle<ILAruA2MeV> uA2MeVHdl{m_lAruA2MeVKey,ctx};
  const ILAruA2MeV* laruA2MeV{*uA2MeVHdl};
  writeHandle.addDependency(uA2MeVHdl);
  
  SG::ReadCondHandle<ILArDAC2uA> DAC2uAHdl{m_lArDAC2uAKey,ctx};
  const ILArDAC2uA* larDAC2uA{*DAC2uAHdl};
  writeHandle.addDependency(DAC2uAHdl);

  SG::ReadCondHandle<ILArRamp> rampHdl{m_lArRampKey,ctx};
  const ILArRamp* larRamp{*rampHdl};
  writeHandle.addDependency(rampHdl);

  // retrieve LArFebConfig if needed
  const LArFebConfig *febConfig=nullptr;
  if(m_useFEBGainThresholds) {
    SG::ReadCondHandle<LArFebConfig> configHdl{m_febConfigKey,ctx};
    febConfig = *configHdl;
    if (febConfig==nullptr) {
      ATH_MSG_ERROR( "Unable to retrieve LArFebConfig with key " << m_febConfigKey.key());
      return StatusCode::FAILURE;
    }
    writeHandle.addDependency(configHdl);
  }
  //The following two are optional (not used for MC and/or SuperCells)
  const ILArMphysOverMcal* larmPhysOverMCal=nullptr;
  if (m_lArMphysOverMcalKey.key().size()) {
    SG::ReadCondHandle<ILArMphysOverMcal> mphysOverMcalHdl{m_lArMphysOverMcalKey,ctx};
    larmPhysOverMCal=*mphysOverMcalHdl;
    writeHandle.addDependency(mphysOverMcalHdl);
  }//end if have MphysOverMcal

  const ILArHVScaleCorr* larHVScaleCorr=nullptr;
  if (m_lArHVScaleCorrKey.key().size()) {
    SG::ReadCondHandle<ILArHVScaleCorr> HVScaleCorrHdl{m_lArHVScaleCorrKey,ctx};
    larHVScaleCorr=*HVScaleCorrHdl;
    writeHandle.addDependency(HVScaleCorrHdl);
  }//end if have HVScaleCorr
  
  
  ATH_MSG_INFO("IOV of ADC2MeV object is " << writeHandle.getRange());

  //Sanity & debugging conters:
  unsigned nNouA2MeV=0;
  unsigned nNoDAC2uA=0;
  unsigned nNoMphysOverMcal=0;
  unsigned nNoRamp=0;
  unsigned nNoHVScaleCorr=0;

  //'Guess' the degree of the ramp polynom (should be all the same):
  unsigned rampPolyDeg=0;
  std::vector<HWIdentifier>::const_iterator it  = m_larOnlineID->channel_begin();
  std::vector<HWIdentifier>::const_iterator it_e= m_larOnlineID->channel_end();
  while (rampPolyDeg==0 && it!=it_e) {
    rampPolyDeg=larRamp->ADC2DAC(*it,0).size();
    ++it;
  }

  ATH_MSG_INFO("Working with a ramp polynom of degree " << rampPolyDeg);
  //Create output object
  std::unique_ptr<LArADC2MeV> lArADC2MeVObj=std::make_unique<LArADC2MeV>(m_larOnlineID,cabling,m_nGains,rampPolyDeg);

  it  = m_larOnlineID->channel_begin();
  it_e= m_larOnlineID->channel_end();
  for (;it!=it_e;++it) {
    const HWIdentifier chid=*it;
    const IdentifierHash hid=m_larOnlineID->channel_Hash(chid);

    //Check if connected:
    if (cabling->isOnlineConnectedFromHash(hid)) {
      //uA2MeV and DAC2uA are gain-less and always needed:
      const float& uA2MeV=laruA2MeV->UA2MEV(chid);
      if (uA2MeV == (float)ILAruA2MeV::ERRORCODE) {
	++nNouA2MeV;
	continue;
      }

      const float& DAC2uA=larDAC2uA->DAC2UA(chid);
      if (DAC2uA == (float)ILArDAC2uA::ERRORCODE) {
	ATH_MSG_ERROR( "No DAC2uA value for for channel " << m_larOnlineID->channel_name(chid) );
	++nNoDAC2uA;
	continue;
      }

      //First intermediate result
      float factor=uA2MeV*DAC2uA;
    
      if (larHVScaleCorr) { //Have HVScaleCorr (remember, it's optional)
	const float&  HVScaleCorr = larHVScaleCorr->HVScaleCorr(chid);
	if (HVScaleCorr == (float)ILAruA2MeV::ERRORCODE) {
	  //That's a bit unusual but not a disaster, go ahead w/o HV Scale correction
	  ATH_MSG_DEBUG("No HVScaleCorr value for for channel " << m_larOnlineID->channel_name(chid));
	  ++nNoHVScaleCorr;
	}
	else {
	  factor*=HVScaleCorr;
	}
      }//end if have HVScaleCorr
    

      //The following factors are gain-dependent:
      std::vector<float> factorGain(m_nGains,factor);
    
      if (larmPhysOverMCal) {//Have mPhysOverMcal obj  (remember, it's optional)
	for (size_t igain=0;igain<m_nGains;++igain) {
	  const float& mPhysOverMCal=larmPhysOverMCal->MphysOverMcal(chid,igain);
	  if ( mPhysOverMCal==(float)ILArMphysOverMcal::ERRORCODE) {
	    //That's ok, not all channels actually have an MphysOverMcal value
	    ++nNoMphysOverMcal;
	  }
	  else {
	    factorGain[igain]/=mPhysOverMCal;
	  }
	}//end loop over gains
      }//end if have MPhysOverMcal

   
      for(unsigned int igain=0;igain<m_nGains;igain++) {

	// ADC2DAC is a vector (polynomial fit of ramps)
	const ILArRamp::RampRef_t adc2dac = larRamp->ADC2DAC(chid,igain);
	if (adc2dac.size()<2) {
          // Some channels can be missing in some configurations.
          ATH_MSG_VERBOSE("No Ramp found for channel " << m_larOnlineID->channel_name(chid) << ", gain " << igain);
	  continue;
	}
      
	std::vector<float> ADC2MeV; //output data

	//Determine if the intercept is to be used:
	if (igain==0 || (igain==1 && febConfig && febConfig->lowerGainThreshold(chid)<5)) { 
	  //Don't use ramp intercept in high gain and in medium gain if the no high gain is used
	  //(eg lowerGainThreshold is ~zero)
	  ADC2MeV.push_back(0.);
	}
	else {
	  ADC2MeV.push_back(factorGain[igain]*adc2dac[0]);
	}

	//Fill remaining polynom terms (usualy only one left)
	for (size_t i=1;i<adc2dac.size();++i) {
	  ADC2MeV.push_back(factorGain[igain]*adc2dac[i]);
	}
      
	//Now we are done with this channel and gain. Add it to the output container
	bool stat=lArADC2MeVObj->set(hid,igain,ADC2MeV);
	if (!stat) { //fails only if hash or gain are out-of-range
	  ATH_MSG_ERROR( "LArADC2MeV::set fails for gain " << igain << ", hash " << hid );
	}
	assert(stat); 

      }//end loop over gains
    }//end if connected
  }//end loop over readout channels


  ATH_CHECK(writeHandle.record(std::move(lArADC2MeVObj)));
  
  if (nNouA2MeV) ATH_MSG_ERROR( "No uA2MeV values for " << nNouA2MeV << " channels" );
  if (nNoDAC2uA) ATH_MSG_ERROR( "No DAC2uA values for " << nNouA2MeV << " channels" );
  if (nNoRamp) ATH_MSG_ERROR( "No Ramp values for " << nNoRamp << " channels * gains " );
  
  if (nNoMphysOverMcal) ATH_MSG_INFO("No MphysOverMcal values for " << nNoMphysOverMcal << " channels * gains");
  if (nNoHVScaleCorr) ATH_MSG_WARNING("No HVScaleCorr values for " << nNoHVScaleCorr << " channels");

  if (nNouA2MeV || nNoDAC2uA || nNoRamp) 
    return StatusCode::FAILURE;
  else
    return StatusCode::SUCCESS;
}

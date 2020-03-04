/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
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

  //CondSvc
  ATH_CHECK( m_condSvc.retrieve() );

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

  // Register write handle
  if (m_condSvc->regHandle(this, m_ADC2MeVKey).isFailure()) {
    ATH_MSG_ERROR("unable to register WriteCondHandle " << m_ADC2MeVKey.fullKey() << " with CondSvc");
    return StatusCode::FAILURE;
  }

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


  //To determine the output range:
  EventIDRange rangeIn, rangeOut;

  //Cabling (should have an ~infinte IOV)
  SG::ReadCondHandle<LArOnOffIdMapping> cablingHdl{m_cablingKey,ctx};
  const LArOnOffIdMapping* cabling{*cablingHdl};
  if(!cablingHdl.range(rangeOut)) { //use RangeOut for the first one
    ATH_MSG_ERROR("Failed to retrieve validity range for LArOnOffIdMapping object with " <<  cablingHdl.key());
    return StatusCode::FAILURE;
  }

  //Get pointers to input data and determine validity range
  SG::ReadCondHandle<ILAruA2MeV> uA2MeVHdl{m_lAruA2MeVKey,ctx};
  const ILAruA2MeV* laruA2MeV{*uA2MeVHdl};
  if (!uA2MeVHdl.range(rangeIn)){ 
    ATH_MSG_ERROR("Failed to retrieve validity range for uA2MeV object with " << uA2MeVHdl.key());
    return StatusCode::FAILURE;
  }

  rangeOut=EventIDRange::intersect(rangeOut,rangeIn); 
  
  SG::ReadCondHandle<ILArDAC2uA> DAC2uAHdl{m_lArDAC2uAKey,ctx};
  const ILArDAC2uA* larDAC2uA{*DAC2uAHdl};
  
  if (!DAC2uAHdl.range(rangeIn)){
    ATH_MSG_ERROR("Failed to retrieve validity range for DAC2uA object with key " << DAC2uAHdl.key());
    return StatusCode::FAILURE;
  }

  rangeOut=EventIDRange::intersect(rangeOut,rangeIn); 
  

  SG::ReadCondHandle<ILArRamp> rampHdl{m_lArRampKey,ctx};
  const ILArRamp* larRamp{*rampHdl};
  if (!rampHdl.range(rangeIn)){
    ATH_MSG_ERROR("Failed to retrieve validity range for ramp object with key " << rampHdl.key());
    return StatusCode::FAILURE;
  }
  rangeOut=EventIDRange::intersect(rangeOut,rangeIn);

  // retrieve LArFebConfig if needed
  const LArFebConfig *febConfig=nullptr;
  if(m_useFEBGainThresholds) {
    SG::ReadCondHandle<LArFebConfig> configHdl{m_febConfigKey,ctx};
     febConfig = *configHdl;
     if (febConfig==nullptr) {
        ATH_MSG_ERROR( "Unable to retrieve LArFebConfig with key " << m_febConfigKey.key());
        return StatusCode::FAILURE;
     }
     rangeOut=EventIDRange::intersect(rangeOut,rangeIn);
  }
  //The following two are optional (not used for MC and/or SuperCells)
  const ILArMphysOverMcal* larmPhysOverMCal=nullptr;
  if (m_lArMphysOverMcalKey.key().size()) {
    SG::ReadCondHandle<ILArMphysOverMcal> mphysOverMcalHdl{m_lArMphysOverMcalKey,ctx};
    larmPhysOverMCal=*mphysOverMcalHdl;
    if (!mphysOverMcalHdl.range(rangeIn)){
      ATH_MSG_ERROR("Failed to retrieve validity range for MphysOverMcal object with key " << mphysOverMcalHdl.key());
      return StatusCode::FAILURE;
    }
    rangeOut=EventIDRange::intersect(rangeOut,rangeIn);
  }//end if have MphysOverMcal

  const ILArHVScaleCorr* larHVScaleCorr=nullptr;
  if (m_lArHVScaleCorrKey.key().size()) {
    SG::ReadCondHandle<ILArHVScaleCorr> HVScaleCorrHdl{m_lArHVScaleCorrKey,ctx};
    larHVScaleCorr=*HVScaleCorrHdl;
    if (!HVScaleCorrHdl.range(rangeIn)){
      ATH_MSG_ERROR("Failed to retrieve validity range for HVScaleCorr object with key " << HVScaleCorrHdl.key());
      return StatusCode::FAILURE;
    }
    rangeOut=EventIDRange::intersect(rangeOut,rangeIn);
  }//end if have HVScaleCorr
  
  
  ATH_MSG_INFO("IOV of ADC2MeV object is " << rangeOut);

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
	//msg(MSG::ERROR) << "No uA2MeV value found for channel " << m_larOnlineID->channel_name(chid) << endmsg;
	++nNouA2MeV;
	continue;
      }

      const float& DAC2uA=larDAC2uA->DAC2UA(chid);
      if (DAC2uA == (float)ILArDAC2uA::ERRORCODE) {
	msg(MSG::ERROR) << "No DAC2uA value for for channel " << m_larOnlineID->channel_name(chid) << endmsg;
	++nNoDAC2uA;
	continue;
      }

      //First intermediate result
      float factor=uA2MeV*DAC2uA;
    
      if (larHVScaleCorr) { //Have HVScaleCorr (remember, it's optional)
	const float&  HVScaleCorr = larHVScaleCorr->HVScaleCorr(chid);
	if (HVScaleCorr == (float)ILAruA2MeV::ERRORCODE) {
	  //That's a bit unusual but not a desaster, go ahead w/o HV Scale correction
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
	  msg(MSG::ERROR) << "LArADC2MeV::set fails for gain " << igain << ", hash " << hid << endmsg;
	}
	assert(stat); 

      }//end loop over gains
    }//end if connected
  }//end loop over readout channels


  ATH_CHECK(writeHandle.record(rangeOut,std::move(lArADC2MeVObj)));
  
  if (nNouA2MeV) msg(MSG::ERROR) << "No uA2MeV values for " << nNouA2MeV << " channels" << endmsg;
  if (nNoDAC2uA) msg(MSG::ERROR) << "No DAC2uA values for " << nNouA2MeV << " channels" << endmsg;
  if (nNoRamp) msg(MSG::ERROR) << "No Ramp values for " << nNoRamp << " channels * gains " << endmsg;
  
  if (nNoMphysOverMcal) ATH_MSG_INFO("No MphysOverMcal values for " << nNoMphysOverMcal << " channels * gains");
  if (nNoHVScaleCorr) ATH_MSG_WARNING("No HVScaleCorr values for " << nNoHVScaleCorr << " channels");

  if (nNouA2MeV || nNoDAC2uA || nNoRamp) 
    return StatusCode::FAILURE;
  else
    return StatusCode::SUCCESS;
}

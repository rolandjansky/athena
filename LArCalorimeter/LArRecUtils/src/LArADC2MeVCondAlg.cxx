/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/


#include "LArRecUtils/LArADC2MeVCondAlg.h"

#include "LArRawConditions/LArADC2MeV.h"
#include "LArElecCalib/ILArRamp.h"
#include "LArElecCalib/ILArDAC2uA.h"
#include "LArElecCalib/ILAruA2MeV.h"
#include "LArElecCalib/ILArMphysOverMcal.h"
#include "LArElecCalib/ILArHVScaleCorr.h"
#include "LArIdentifier/LArOnlineID.h"
#include "LArIdentifier/LArOnline_SuperCellID.h"

#include <memory>

LArADC2MeVCondAlg::LArADC2MeVCondAlg(const std::string& name, ISvcLocator* pSvcLocator) :
  AthAlgorithm(name, pSvcLocator),
  //m_larOnlineIDKey("LArOnlineID"),
  //m_larSCOnlineIDKey("LArOnline_SuperCellID"),
  m_lAruA2MeVKey("LAruA2MeV"),
  m_lArDAC2uAKey("LArDAC2uAKey"),
  m_lArRampKey("LArRamp"),
  m_lArMphysOverMcalKey("LArMphysOverMcal"),
  m_lArHVScaleCorrKey("LArHVScaleCorr"),
  m_ADC2MeVKey("LArADC2MeV","LArADC2MeV"),
  m_condSvc("CondSvc",name),
  m_isSuperCell(false) {
  

  //declareProperty("LArOnlineIDKey",m_larOnlineIDKey,"SG key of LArOnlineID helper obj for regular cells");
  //declareProperty("LArSCOnlineIDKey",m_larSCOnlineIDKey,"SG key of LArOnlineID helper obj for SuperCells");
  declareProperty("LAruA2MeVKey",m_lAruA2MeVKey,"SG key of uA2MeV object");
  declareProperty("LArDAC2uAKey",m_lArDAC2uAKey,"SG key of DAC2uA object");
  declareProperty("LArRampKey",m_lArRampKey,"SG key of Ramp object");
  declareProperty("LArMphysOverMcalKey",m_lArMphysOverMcalKey,"SG key of MpysOverMcal object (or empty string if no MphysOverMcal)");
  declareProperty("LArHVScaleCorrKey",m_lArHVScaleCorrKey,"SG key of HVScaleCorr object (or empty string if no HVScaleCorr)");
  declareProperty("LArADC2MeVKey",m_ADC2MeVKey,"SG key of the resulting LArADC2MeV object");
  declareProperty("isSuperCell",m_isSuperCell,"switch to true to use the SuperCell Identfier helper");

}

LArADC2MeVCondAlg::~LArADC2MeVCondAlg() {}


StatusCode LArADC2MeVCondAlg::initialize() {

  //CondSvc
  ATH_CHECK( m_condSvc.retrieve() );

  if (m_isSuperCell) {
    m_nGains=1;
    //ATH_CHECK(m_larSCOnlineIDKey.initialize());
  }
  else {
    m_nGains=3;
    //ATH_CHECK(m_larOnlineIDKey.initialize());
  }

 // Read Handles for input conditions
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
  return StatusCode::SUCCESS;
}


StatusCode LArADC2MeVCondAlg::execute() {

  //Set up write handle
  SG::WriteCondHandle<LArADC2MeV> writeHandle{m_ADC2MeVKey};
  
  if (writeHandle.isValid()) {
    writeHandle.updateStore(); //????
    msg(MSG::WARNING) << "Found valid write handle" << endmsg;
    return StatusCode::SUCCESS;
  }  


  const LArOnlineID_Base* larOnlineID=nullptr;
  //Identifier helper:
  if (m_isSuperCell) {
    //SG::ReadCondHandle<LArOnline_SuperCellID> larOnlineHdl{m_larSCOnlineIDKey}
    const LArOnline_SuperCellID* scidhelper;
    ATH_CHECK(detStore()->retrieve(scidhelper,"LArOnline_SuperCellID"));
    larOnlineID=scidhelper; //cast to base-class
  }
  else {//regular cells
    //SG::ReadCondHandle<LArOnlineID> larOnlineHdl{m_larOnlineIDKey};
    const LArOnlineID* idhelper;
    ATH_CHECK(detStore()->retrieve(idhelper,"LArOnlineID"));
    larOnlineID=idhelper; //cast to base-class
  }

  //To determine the output range:
  EventIDRange rangeIn, rangeOut;

  //Get pointers to input data and determine validity range
  SG::ReadCondHandle<ILAruA2MeV> uA2MeVHdl{m_lAruA2MeVKey};
  const ILAruA2MeV* laruA2MeV{*uA2MeVHdl};
  if (!uA2MeVHdl.range(rangeOut)){
    ATH_MSG_ERROR("Failed to retrieve validity range for uA2MeV object with " << uA2MeVHdl.key());
    return StatusCode::FAILURE;
  }

  
  SG::ReadCondHandle<ILArDAC2uA> DAC2uAHdl{m_lArDAC2uAKey};
  const ILArDAC2uA* larDAC2uA{*DAC2uAHdl};
  
  if (!DAC2uAHdl.range(rangeIn)){
    ATH_MSG_ERROR("Failed to retrieve validity range for DAC2uA object with " << DAC2uAHdl.key());
    return StatusCode::FAILURE;
  }

  rangeOut.intersect(rangeOut,rangeIn);
  

  SG::ReadCondHandle<ILArRamp> rampHdl{m_lArRampKey};
  const ILArRamp* larRamp{*rampHdl};
  if (!rampHdl.range(rangeIn)){
    ATH_MSG_ERROR("Failed to retrieve validity range for ramp object with " << rampHdl.key());
    return StatusCode::FAILURE;
  }

  rangeOut.intersect(rangeOut,rangeIn);

  //The following two are optional (not used for MC and/or SuperCells)
  const ILArMphysOverMcal* larmPhysOverMCal=nullptr;
  if (m_lArHVScaleCorrKey.key().size()) {
    SG::ReadCondHandle<ILArMphysOverMcal> mphysOverMcalHdl{m_lArMphysOverMcalKey};
    larmPhysOverMCal=*mphysOverMcalHdl;
    if (mphysOverMcalHdl.range(rangeIn)){
      ATH_MSG_ERROR("Failed to retrieve validity range for MphysOverMcal object with " << mphysOverMcalHdl.key());
      return StatusCode::FAILURE;
    }
  rangeOut.intersect(rangeOut,rangeIn);
  }//end if have HVScaleCorr

  const ILArHVScaleCorr* larHVScaleCorr=nullptr;
  if (m_lArHVScaleCorrKey.key().size()) {
    SG::ReadCondHandle<ILArHVScaleCorr> HVScaleCorrHdl{m_lArHVScaleCorrKey};
    larHVScaleCorr=*HVScaleCorrHdl;
    if (HVScaleCorrHdl.range(rangeIn)){
      ATH_MSG_ERROR("Failed to retrieve validity range for HVScaleCorr object with " << HVScaleCorrHdl.key());
      return StatusCode::FAILURE;
    }
    rangeOut.intersect(rangeOut,rangeIn);
  }//end if have HVScaleCorr
  
  
  ATH_MSG_INFO("IOV of ADC2MeV object is " << rangeOut);

  //Sanity & debugging conters:
  unsigned nNouA2MeV=0;
  unsigned nNoDAC2uA=0;
  unsigned nNoMphysOverMcal=0;
  unsigned nNoRamp=0;
  unsigned nNoHVScaleCorr=0;


  //Create output object
  std::unique_ptr<LArADC2MeV> lArADC2MeVObj=std::make_unique<LArADC2MeV>(larOnlineID,m_nGains);


  
  //Start loop over channels
  std::vector<HWIdentifier>::const_iterator it  = larOnlineID->channel_begin();
  std::vector<HWIdentifier>::const_iterator it_e= larOnlineID->channel_end();
  for (;it!=it_e;++it) {
    const HWIdentifier chid=*it;

    //uA2MeV and DAC2uA are gain-less and always needed:
    const float& uA2MeV=laruA2MeV->UA2MEV(chid);
    if (uA2MeV == (float)ILAruA2MeV::ERRORCODE) {
      msg(MSG::ERROR) << "No uA2MeV value found for channel " << larOnlineID->channel_name(chid) << endmsg;
      ++nNouA2MeV;
      continue;
    }

    const float& DAC2uA=larDAC2uA->DAC2UA(chid);
    if (DAC2uA == (float)ILArDAC2uA::ERRORCODE) {
      msg(MSG::ERROR) << "No DAC2uA value for for channel " << larOnlineID->channel_name(chid) << endmsg;
      ++nNoDAC2uA;
      continue;
    }

    //First intermediate result
    float factor=uA2MeV*DAC2uA;

    
    if (larHVScaleCorr) { //Have HVScaleCorr (remember, it's optional)
      const float&  HVScaleCorr = larHVScaleCorr->HVScaleCorr(chid);
      if (HVScaleCorr == (float)ILAruA2MeV::ERRORCODE) {
	//That's a bit unusual but not a desaster, go ahead w/o HV Scale correction
	ATH_MSG_DEBUG("No HVScaleCorr value for for channel " << larOnlineID->channel_name(chid));
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

      std::vector<float> ADC2MeV;
      // ADC2DAC is a vector (polynomial fit of ramps)
      const ILArRamp::RampRef_t adc2dac = larRamp->ADC2DAC(chid,igain);
      if (adc2dac.size()<2) {
	msg(MSG::ERROR) << "No Ramp found for channel " << larOnlineID->channel_name(chid) << ", gain " << igain << endmsg;
	++nNoRamp;
	continue;
      }

      //Determine if the intercept is to be used:
      bool useRampIntercept=(igain>0); //FIXME: Check FebConfigReader
      if (useRampIntercept)
	ADC2MeV.push_back(factorGain[igain]*adc2dac[0]);
      else 
	ADC2MeV.push_back(0.);
      
      //Fill remaining polynom terms (usualy only one left)
      for (size_t i=1;i<adc2dac.size();++i) {
	ADC2MeV.push_back(factorGain[igain]*adc2dac[i]);
      }
      
      //Now we are done with this channel and gain. Add it to the output container
      const IdentifierHash hid=larOnlineID->channel_Hash(chid);
      bool stat=lArADC2MeVObj->set(hid,igain,ADC2MeV);
      if (!stat) msg(MSG::ERROR) << "LArADC2MeV::set fails" << endmsg;
      assert(stat); //fails only if hash or gain are out-of-range

    }//end loop over gains
  }//end loop over readout channels


  ATH_CHECK(writeHandle.record(rangeOut,lArADC2MeVObj.release()));
  
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

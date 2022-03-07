/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#include "LArROD/LArRawChannelBuilderAlg.h" 
#include "GaudiKernel/SystemOfUnits.h"
#include "LArRawEvent/LArRawChannelContainer.h"
#include "CaloEvent/CaloCellContainer.h"
#include "CaloDetDescr/CaloDetDescrManager.h"
#include "LArRawEvent/LArDigitContainer.h"
#include "LArIdentifier/LArOnline_SuperCellID.h"
#include "LArIdentifier/LArOnlineID.h"
#include "LArCOOLConditions/LArDSPThresholdsFlat.h"
#include "AthAllocators/DataPool.h"
#include <cmath>

LArRawChannelBuilderAlg::LArRawChannelBuilderAlg(const std::string& name, ISvcLocator* pSvcLocator):
  AthReentrantAlgorithm(name, pSvcLocator), m_sem_mgr(0) {}
  
StatusCode LArRawChannelBuilderAlg::initialize() {
  ATH_CHECK(m_digitKey.initialize());	 
  ATH_CHECK(m_cellKey.initialize(m_isSC));
  ATH_CHECK(m_rawChannelKey.initialize(!m_isSC));
  ATH_CHECK(m_pedestalKey.initialize());	 
  ATH_CHECK(m_adc2MeVKey.initialize());	 
  ATH_CHECK(m_ofcKey.initialize());	 
  ATH_CHECK(m_shapeKey.initialize());
  ATH_CHECK(m_cablingKey.initialize() );
  ATH_CHECK(m_run1DSPThresholdsKey.initialize(SG::AllowEmpty) );
  ATH_CHECK(m_run2DSPThresholdsKey.initialize(SG::AllowEmpty) );
  if (m_useDBFortQ) {
    if (m_run1DSPThresholdsKey.empty() && m_run2DSPThresholdsKey.empty()) {
      ATH_MSG_ERROR ("useDB requested but neither Run1DSPThresholdsKey nor Run2DSPThresholdsKey initialized.");
      return StatusCode::FAILURE;
    }
  }

  if ( m_isSC ) {
	const LArOnline_SuperCellID* ll;
	ATH_CHECK(detStore()->retrieve(ll,"LArOnline_SuperCellID"));
	m_onlineId = (const LArOnlineID_Base*)ll;
	ATH_CHECK( detStore()->retrieve (m_sem_mgr, "CaloSuperCellMgr") );
  }
  else {
	const LArOnlineID* ll;
	ATH_CHECK(detStore()->retrieve(ll,"LArOnlineID"));
	m_onlineId = (const LArOnlineID_Base*)ll;
  }

  const std::string cutmsg = m_absECutFortQ.value() ? " fabs(E) < " : " E < "; 
  ATH_MSG_INFO("Energy cut for time and quality computation: " << cutmsg << 
               " taken from COOL folder "<<
               m_run1DSPThresholdsKey.key() << " (run1) " <<
               m_run2DSPThresholdsKey.key() << " (run2) ");
  return StatusCode::SUCCESS;
}     

StatusCode LArRawChannelBuilderAlg::finalize() {
  return StatusCode::SUCCESS;
} 

StatusCode LArRawChannelBuilderAlg::execute(const EventContext& ctx) const {

  //Get event inputs from read handles:
  SG::ReadHandle<LArDigitContainer> inputContainer(m_digitKey,ctx);

  //Write output via write handle

  auto  outputContainerCellPtr = std::make_unique<CaloCellContainer>(SG::VIEW_ELEMENTS);
  auto  outputContainerLRPtr = std::make_unique<LArRawChannelContainer>();

  DataPool<CaloCell> dataPool;
  if ( m_isSC ) {
    unsigned int hash_max = m_onlineId->channelHashMax();
    if (dataPool.allocated()==0){
      dataPool.reserve (hash_max);
    }
    outputContainerCellPtr->reserve( hash_max );
  }
	    
  //Get Conditions input
  SG::ReadCondHandle<ILArPedestal> pedHdl(m_pedestalKey,ctx);
  const ILArPedestal* peds=*pedHdl;

  SG::ReadCondHandle<LArADC2MeV> adc2mevHdl(m_adc2MeVKey,ctx);
  const LArADC2MeV* adc2MeVs=*adc2mevHdl;

  SG::ReadCondHandle<ILArOFC> ofcHdl(m_ofcKey,ctx);
  const ILArOFC* ofcs=*ofcHdl;

  SG::ReadCondHandle<ILArShape> shapeHdl(m_shapeKey,ctx);
  const ILArShape* shapes=*shapeHdl;

  SG::ReadCondHandle<LArOnOffIdMapping> cabling(m_cablingKey,ctx);
  
  std::unique_ptr<LArDSPThresholdsFlat> run2DSPThresh;
  const LArDSPThresholdsComplete* run1DSPThresh = nullptr;
  if (m_useDBFortQ) {
    if (!m_run2DSPThresholdsKey.empty()) {
      SG::ReadCondHandle<AthenaAttributeList> dspThrshAttr (m_run2DSPThresholdsKey, ctx);
      run2DSPThresh = std::unique_ptr<LArDSPThresholdsFlat>(new LArDSPThresholdsFlat(*dspThrshAttr));
      if (ATH_UNLIKELY(!run2DSPThresh->good())) {
        ATH_MSG_ERROR( "Failed to initialize LArDSPThresholdFlat from attribute list loaded from " << m_run2DSPThresholdsKey.key()
                       << ". Aborting." ); 
        return StatusCode::FAILURE;
      }
    }
    else if (!m_run1DSPThresholdsKey.empty()) {
      SG::ReadCondHandle<LArDSPThresholdsComplete> dspThresh (m_run1DSPThresholdsKey, ctx);
      run1DSPThresh = dspThresh.cptr();
    }
    else {
      ATH_MSG_ERROR( "No DSP threshold configured.");
      return StatusCode::FAILURE;
    }
  }

  //Loop over digits:
  for (const LArDigit* digit : *inputContainer) {

    const HWIdentifier id=digit->hardwareID();
    const bool connected=(*cabling)->isOnlineConnected(id);
    

    ATH_MSG_VERBOSE("Working on channel " << m_onlineId->channel_name(id));

    const std::vector<short>& samples=digit->samples();
    const size_t nSamples=samples.size();
    const int gain=digit->gain();
    const float p=peds->pedestal(id,gain);
   


    //The following autos will resolve either into vectors or vector-proxies
    const auto& ofca=ofcs->OFC_a(id,gain);
    const auto& adc2mev=adc2MeVs->ADC2MEV(id,gain);
    
    //Sanity check on input conditions data:
    // FIXME: fix to get splash test running, should implement the iterations later
    size_t len=nSamples;
    if(!m_isSC && ATH_UNLIKELY(ofca.size()<nSamples)) {
      if (!connected) continue; //No conditions for disconencted channel, who cares?
      ATH_MSG_DEBUG("Number of OFC a's doesn't match number of samples for conencted channel " << m_onlineId->channel_name(id) 
		    << " gain " << gain << ". OFC size=" << ofca.size() << ", nbr ADC samples=" << nSamples);
      len=ofca.size();
    }
    if (m_isSC && !connected ) continue;
    if (m_isSC ) len = nSamples <= ofca.size() ? nSamples : ofca.size();
    
    if (ATH_UNLIKELY(p==ILArPedestal::ERRORCODE)) {
      if (!connected) continue; //No conditions for disconencted channel, who cares?
      ATH_MSG_ERROR("No valid pedestal for connected channel " << m_onlineId->channel_name(id) 
		    << " gain " << gain);
      return StatusCode::FAILURE;
    }

    if(ATH_UNLIKELY(adc2mev.size()<2)) {
      if (!connected) continue; //No conditions for disconencted channel, who cares?
      ATH_MSG_ERROR("No valid ADC2MeV for connected channel " << m_onlineId->channel_name(id) 
		    << " gain " << gain);
      return StatusCode::FAILURE;
    }


    //Apply OFCs to get amplitude
    float A=0;
    bool saturated=false;
    unsigned int init=0;
    bool passBCIDmax=false;
    // Check saturation AND discount pedestal
    std::vector<float> samp_no_ped(nSamples,0.0);
    for (size_t i=0;i<nSamples;++i) {
      if (samples[i]==4096 || samples[i]==0) saturated=true;
      samp_no_ped[i]=samples[i]-p;
    }
    if (!m_isSC){
      for (size_t i=0;i<len;++i) {
        A+=(samp_no_ped[i])*ofca[i];
      }
    } else {
      init=1;
      for (size_t i=0;i<len;++i) {
        A+=(samp_no_ped[i+init])*ofca[i];
      }
      float Am1=0.;
      for (size_t i=0;i<len;++i) {
        Am1+=(samp_no_ped[i])*ofca[i];
      }
      float AM1=0.;
      for (size_t i=0;i<len;++i) {
        AM1+=(samp_no_ped[i+2])*ofca[i];
      }
      if ( (A>Am1) && (A>AM1) ) passBCIDmax=true;
    } // end of m_isSC 
    
    //Apply Ramp
    const float E=adc2mev[0]+A*adc2mev[1];
    
    uint16_t iquaShort=0;
    float tau=0;


    uint16_t prov=0xa5; //Means all constants from DB
    if (saturated) prov|=0x0400;

    const float E1=m_absECutFortQ.value() ? std::fabs(E) : E;
    float ecut(0.);
    if (m_useDBFortQ) {
      if (run2DSPThresh) {
        ecut = run2DSPThresh->tQThr(id);
      }
      else if (run1DSPThresh) {
        ecut = run1DSPThresh->tQThr(id);
      }
      else {
        ATH_MSG_ERROR ("DSP threshold problem");
        return StatusCode::FAILURE;
      }
    }
    else {
      ecut = m_eCutFortQ;
    }
    if (E1 > ecut) {
      ATH_MSG_VERBOSE("Channel " << m_onlineId->channel_name(id) << " gain " << gain << " above threshold for tQ computation");
      prov|=0x2000; //  fill bit in provenance that time+quality information are available

      //Get time by applying OFC-b coefficients:
      const auto& ofcb=ofcs->OFC_b(id,gain);
      float At=0;
      for (size_t i=0;i<len;++i) {
	At+=(samp_no_ped[i+init])*ofcb[i];
      }
      //Divide A*t/A to get time
      tau=(std::fabs(A)>0.1) ? At/A : 0.0;
      const auto& fullShape=shapes->Shape(id,gain);
      
      //Get Q-factor
      size_t firstSample=m_firstSample;
      // fixing HEC to move +1 in case of 4 samples and firstSample 0 (copied from old LArRawChannelBuilder)
      if (fullShape.size()>nSamples && nSamples==4 && m_firstSample==0) {
	if (m_onlineId->isHECchannel(id)) {
	  firstSample=1;
	}
      }

      if (ATH_UNLIKELY(fullShape.size()<nSamples+firstSample)) {
	if (!connected) continue; //No conditions for disconnected channel, who cares?
	ATH_MSG_DEBUG("No valid shape for channel " <<  m_onlineId->channel_name(id) 
		      << " gain " << gain);
	ATH_MSG_DEBUG("Got size " << fullShape.size() << ", expected at least " << nSamples+firstSample);
	//return StatusCode::FAILURE;
        len=fullShape.size()-firstSample;
      }

      const float* shape=&*fullShape.begin()+firstSample;

      float q=0;
      if (m_useShapeDer) {
	const auto& fullshapeDer=shapes->ShapeDer(id,gain);
	if (ATH_UNLIKELY(fullshapeDer.size()<nSamples+firstSample)) {
	  ATH_MSG_DEBUG("No valid shape derivative for channel " <<  m_onlineId->channel_name(id) 
			<< " gain " << gain);
	  ATH_MSG_DEBUG("Got size " << fullshapeDer.size() << ", expected at least " << nSamples+firstSample);
	  //return StatusCode::FAILURE;
	}

	const float* shapeDer=&*fullshapeDer.begin()+firstSample;
	for (size_t i=0;i<len;++i) {
	  q += std::pow((A*(shape[i]-tau*shapeDer[i])-(samp_no_ped[i+init])),2);
	}
      }//end if useShapeDer
      else {
	//Q-factor w/o shape derivative
	for (size_t i=0;i<len;++i) {
	  q += std::pow((A*shape[i]-(samp_no_ped[i+init])),2);
	}
      }

      int iqua = static_cast<int>(q);
      if (iqua > 0xFFFF) iqua=0xFFFF;
      iquaShort = static_cast<uint16_t>(iqua & 0xFFFF);

      tau-=ofcs->timeOffset(id,gain);
      tau*=(Gaudi::Units::nanosecond/Gaudi::Units::picosecond); //Convert time to ps
    }//end if above cut

   
    if ( m_isSC ){
    CaloCell* ss = dataPool.nextElementPtr();
    Identifier offId = cabling->cnvToIdentifier(id);
    
    const CaloDetDescrElement* dde = m_sem_mgr->get_element (offId);
    ss->setCaloDDE(dde);
    ss->setEnergy(E);
    tau*=1e-3; // time in ns
    ss->setTime(tau);
    ss->setGain((CaloGain::CaloGain)0);
    float et = ss->et()*1e-3; // et in GeV
    // for super-cells provenance and time are slightly different
    uint16_t prov = 0x2000;
    if(et>10e3 && tau>-8 && tau<16) prov |= 0x200;
    else if(et<=10e3 && fabs(tau)<8) prov |= 0x200; 
    if ( passBCIDmax ) prov |=0x40;
    ss->setProvenance(prov);
    
    ss->setQuality(iquaShort);
    outputContainerCellPtr->push_back(ss);
    }
    else{
    outputContainerLRPtr->emplace_back(id,static_cast<int>(std::floor(E+0.5)),
				  static_cast<int>(std::floor(tau+0.5)),
				  iquaShort,prov,(CaloGain::CaloGain)gain);
    }
  }
  if ( m_isSC ) {
  SG::WriteHandle<CaloCellContainer>outputContainer(m_cellKey,ctx);
  ATH_CHECK(outputContainer.record(std::move(outputContainerCellPtr) ) );
  } else {
  SG::WriteHandle<LArRawChannelContainer>outputContainer(m_rawChannelKey,ctx);
  ATH_CHECK(outputContainer.record(std::move(outputContainerLRPtr) ) );
  }

  return StatusCode::SUCCESS;
}



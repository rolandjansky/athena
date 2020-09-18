/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#include "LArROD/LArRawChannelBuilderAlg.h" 
#include "GaudiKernel/SystemOfUnits.h"
#include "LArRawEvent/LArRawChannelContainer.h"
#include "LArRawEvent/LArDigitContainer.h"
#include "LArIdentifier/LArOnlineID.h"
#include "LArCOOLConditions/LArDSPThresholdsFlat.h"
#include <cmath>

LArRawChannelBuilderAlg::LArRawChannelBuilderAlg(const std::string& name, ISvcLocator* pSvcLocator):
  AthReentrantAlgorithm(name, pSvcLocator) {}
  
StatusCode LArRawChannelBuilderAlg::initialize() {
  ATH_CHECK(m_digitKey.initialize());	 
  ATH_CHECK(m_rawChannelKey.initialize());
  ATH_CHECK(m_pedestalKey.initialize());	 
  ATH_CHECK(m_adc2MeVKey.initialize());	 
  ATH_CHECK(m_ofcKey.initialize());	 
  ATH_CHECK(m_shapeKey.initialize());
  ATH_CHECK(m_cablingKey.initialize() );
  ATH_CHECK(m_thresholdsKey.initialize(m_useDBFortQ) );

  ATH_CHECK(detStore()->retrieve(m_onlineId,"LArOnlineID"));

  const std::string cutmsg = m_absECutFortQ.value() ? " fabs(E) < " : " E < "; 
  ATH_MSG_INFO("Energy cut for time and quality computation: " << cutmsg << " taken from COOL folder "<<m_thresholdsKey.key());
  return StatusCode::SUCCESS;
}     

StatusCode LArRawChannelBuilderAlg::finalize() {
  return StatusCode::SUCCESS;
} 

StatusCode LArRawChannelBuilderAlg::execute(const EventContext& ctx) const {

  //Get event inputs from read handles:
  SG::ReadHandle<LArDigitContainer> inputContainer(m_digitKey,ctx);

  //Write output via write handle
  SG::WriteHandle<LArRawChannelContainer>outputContainer(m_rawChannelKey,ctx);
  ATH_CHECK(outputContainer.record(std::make_unique<LArRawChannelContainer>()));
	    
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
  
  std::unique_ptr<LArDSPThresholdsFlat> dspThreshFlat;
  if (m_useDBFortQ) {
     SG::ReadCondHandle<AthenaAttributeList> dspThrshAttr (m_thresholdsKey, ctx);
     dspThreshFlat = std::unique_ptr<LArDSPThresholdsFlat>(new LArDSPThresholdsFlat(*dspThrshAttr));
     if (ATH_UNLIKELY(!dspThreshFlat->good())) {
         ATH_MSG_ERROR( "Failed to initialize LArDSPThresholdFlat from attribute list loaded from " << m_thresholdsKey.key()
                   << ". Aborting." ); 
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
    if(ATH_UNLIKELY(ofca.size()!=nSamples)) {
      if (!connected) continue; //No conditions for disconencted channel, who cares?
      ATH_MSG_ERROR("Number of OFC a's doesn't match number of samples for conencted channel " << m_onlineId->channel_name(id) 
		    << " gain " << gain << ". OFC size=" << ofca.size() << ", nbr ADC samples=" << nSamples);
      return StatusCode::FAILURE;
    }
    
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
    for (size_t i=0;i<nSamples;++i) {
      A+=(samples[i]-p)*ofca[i];
      if (samples[i]==4096 || samples[i]==0) saturated=true;
    }
    
    //Apply Ramp
    const float E=adc2mev[0]+A*adc2mev[1];
    
    uint16_t iquaShort=0;
    float tau=0;


    uint16_t prov=0xa5; //Means all constants from DB
    if (saturated) prov|=0x0400;

    const float E1=m_absECutFortQ.value() ? std::fabs(E) : E;
    float ecut(0.);
    if (m_useDBFortQ) { ecut = dspThreshFlat->tQThr(id);} else { ecut = m_eCutFortQ;}
    if (E1 > ecut) {
      ATH_MSG_VERBOSE("Channel " << m_onlineId->channel_name(id) << " gain " << gain << " above threshold for tQ computation");
      prov|=0x2000; //  fill bit in provenance that time+quality information are available

      //Get time by applying OFC-b coefficients:
      const auto& ofcb=ofcs->OFC_b(id,gain);
      float At=0;
      for (size_t i=0;i<nSamples;++i) {
	At+=(samples[i]-p)*ofcb[i];
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
	ATH_MSG_ERROR("No valid shape for channel " <<  m_onlineId->channel_name(id) 
		      << " gain " << gain);
	ATH_MSG_ERROR("Got size " << fullShape.size() << ", expected at least " << nSamples+firstSample);
	return StatusCode::FAILURE;
      }

      const float* shape=&*fullShape.begin()+firstSample;

      float q=0;
      if (m_useShapeDer) {
	const auto& fullshapeDer=shapes->ShapeDer(id,gain);
	if (ATH_UNLIKELY(fullshapeDer.size()<nSamples+firstSample)) {
	  ATH_MSG_ERROR("No valid shape derivative for channel " <<  m_onlineId->channel_name(id) 
			<< " gain " << gain);
	  ATH_MSG_ERROR("Got size " << fullshapeDer.size() << ", expected at least " << nSamples+firstSample);
	  return StatusCode::FAILURE;
	}

	const float* shapeDer=&*fullshapeDer.begin()+firstSample;
	for (size_t i=0;i<nSamples;++i) {
	  q += std::pow((A*(shape[i]-tau*shapeDer[i])-(samples[i]-p)),2);
	}
      }//end if useShapeDer
      else {
	//Q-factor w/o shape derivative
	for (size_t i=0;i<nSamples;++i) {
	  q += std::pow((A*shape[i]-(samples[i]-p)),2);
	}
      }

      int iqua = static_cast<int>(q);
      if (iqua > 0xFFFF) iqua=0xFFFF;
      iquaShort = static_cast<uint16_t>(iqua & 0xFFFF);

      tau-=ofcs->timeOffset(id,gain);
      tau*=(Gaudi::Units::nanosecond/Gaudi::Units::picosecond); //Convert time to ps
    }//end if above cut

   
    outputContainer->emplace_back(id,static_cast<int>(std::floor(E+0.5)),
				  static_cast<int>(std::floor(tau+0.5)),
				  iquaShort,prov,(CaloGain::CaloGain)gain);
  }

  return StatusCode::SUCCESS;
}



/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

// ********************************************************************
// NAME:     LArHVCorrectionMonAlg.cxx
// PACKAGE:  LArMonitoring
//
// AUTHOR:   Pavol Strizenec, based on tool by Jessica Leveque
//
// Class for monitoring offline HV corrections based in DCS infos
// ********************************************************************

#include "LArHVCorrectionMonAlg.h"

/*---------------------------------------------------------*/
LArHVCorrectionMonAlg::LArHVCorrectionMonAlg(const std::string& name, ISvcLocator* pSvcLocator)
   : AthMonitorAlgorithm(name, pSvcLocator),
    m_LArOnlineIDHelper(0),
    m_caloIdMgr(0),
    m_filledLB()
{ }

/*---------------------------------------------------------*/
StatusCode LArHVCorrectionMonAlg::initialize()
{
  
  ATH_MSG_INFO( "Initialize LArHVCorrectionMonAlg" );
  
  ATH_CHECK( detStore()->retrieve(m_LArOnlineIDHelper, "LArOnlineID") );
  
  // Retrieve ID helpers
  const CaloCell_ID* idHelper = nullptr;
  ATH_CHECK( detStore()->retrieve( idHelper, "CaloCell_ID" ) );
  
  ATH_CHECK( m_cablingKey.initialize() );

  ATH_CHECK( m_scaleCorrKey.initialize() );
  ATH_CHECK( m_onlineScaleCorrKey.initialize() );

  ATH_CHECK( m_caloMgrKey.initialize() );
  
  ATH_MSG_DEBUG( "Successful Initialize LArHVCorrection " );
  return AthMonitorAlgorithm::initialize();
}

/*---------------------------------------------------------*/
StatusCode LArHVCorrectionMonAlg::fillHistograms(const EventContext& ctx) const
{
  ATH_MSG_DEBUG( "in fillHists()" );

  unsigned int thisLB=ctx.eventID().lumi_block();
  bool doMonitor=false;
  {
    std::lock_guard<std::mutex> lock(m_mut);
    if(!m_filledLB.count(thisLB)) {
      m_filledLB.insert(thisLB);
      doMonitor=true;
    }
  }

  if(doMonitor) { //LB not yet monitored, so do the monitoring now

    SG::ReadCondHandle<CaloDetDescrManager> caloMgrHandle{m_caloMgrKey,ctx};
    ATH_CHECK(caloMgrHandle.isValid());
    const CaloDetDescrManager* ddman = *caloMgrHandle;
    
    // Retrieve event information
    int lumi_block = ctx.eventID().lumi_block();
    
    // Counter for deviating channels in each partition
    float nonNominal[] = {0.,0.,0.,0.,0.,0.,0.,0.};
    
    SG::ReadCondHandle<ILArHVScaleCorr> scaleCorr (m_scaleCorrKey, ctx);
    SG::ReadCondHandle<ILArHVScaleCorr> onlineScaleCorr (m_onlineScaleCorrKey, ctx);
    SG::ReadCondHandle<LArOnOffIdMapping> larCabling (m_cablingKey, ctx);
    
    std::vector<HWIdentifier>::const_iterator idptr = m_LArOnlineIDHelper->channel_begin();
    std::vector<HWIdentifier>::const_iterator idEndptr = m_LArOnlineIDHelper->channel_end();
    for (;idptr != idEndptr; ++idptr) {
      HWIdentifier id  = *idptr;
      Identifier offlineID = larCabling->cnvToIdentifier(id);
      
      // Skip disconnected channels
      if(!larCabling->isOnlineConnected(id)) continue;
      
      // Get Physical Coordinates
      float etaChan = 0; float phiChan = 0.;
      const CaloDetDescrElement* caloDetElement = ddman->get_element(offlineID);
      if(caloDetElement == 0 ){
	ATH_MSG_ERROR( "Cannot retrieve (eta,phi) coordinates for raw channels" );
	continue; 
      }else{
	etaChan = caloDetElement->eta_raw();
	phiChan = caloDetElement->phi_raw();
      }
      
      // Fix phi range in HEC
      if (m_LArOnlineIDHelper->isHECchannel(id)) phiChan = CaloPhiRange::fix(phiChan);
      
      // Retrieve HV correction info
      float hvdev = 0;
      float hvonline = onlineScaleCorr->HVScaleCorr(id);
      if (hvonline<=0) continue; //No valid online correction
      // Correction in scaleCorr has already been divided by hvonline.
      float hvcorr = scaleCorr->HVScaleCorr(id) * hvonline;
      if (hvcorr>hvonline) hvdev = hvonline-hvcorr; //Monitor only channels that get a higher correction from DCS (eg are at a lower voltage). 

    if (fabs(hvdev/hvonline)> m_threshold){
	// A-Side
	if(etaChan >= 0){
	  if(m_LArOnlineIDHelper->isEMBchannel(id)){
	    nonNominal[0]++;
            ATH_MSG_VERBOSE( "hvdev EMBA" << hvdev );
            auto meta = Monitored::Scalar<float>("etaEMBA",etaChan);
            auto mphi = Monitored::Scalar<float>("phiEMBA",phiChan);
            auto hvmon = Monitored::Scalar<float>("hvcorrEMBA",hvdev);
            fill(m_MonGroupName,meta,mphi,hvmon);
	  }
	  if(m_LArOnlineIDHelper->isEMECchannel(id)){
	    nonNominal[1]++;
            ATH_MSG_VERBOSE( "hvdev EMECA" << hvdev );
            auto meta = Monitored::Scalar<float>("etaEMECA",etaChan);
            auto mphi = Monitored::Scalar<float>("phiEMECA",phiChan);
            auto hvmon = Monitored::Scalar<float>("hvcorrEMECA",hvdev);
            fill(m_MonGroupName,meta,mphi,hvmon);
	  }
	  if(m_LArOnlineIDHelper->isHECchannel(id)){
	    nonNominal[2]++;
            ATH_MSG_VERBOSE( "hvdev HECA" << hvdev );
            auto meta = Monitored::Scalar<float>("etaHECA",etaChan);
            auto mphi = Monitored::Scalar<float>("phiHECA",phiChan);
            auto hvmon = Monitored::Scalar<float>("hvcorrHECA",hvdev);
            fill(m_MonGroupName,meta,mphi,hvmon);
	  }
	  if(m_LArOnlineIDHelper->isFCALchannel(id)){
	    nonNominal[3]++;
            ATH_MSG_VERBOSE( "hvdev FCALA" << hvdev );
            auto meta = Monitored::Scalar<float>("etaFCALA",etaChan);
            auto mphi = Monitored::Scalar<float>("phiFCALA",phiChan);
            auto hvmon = Monitored::Scalar<float>("hvcorrFCALA",hvdev);
            fill(m_MonGroupName,meta,mphi,hvmon);
	  }
	  
	  // C-side
	} else {
	  if(m_LArOnlineIDHelper->isEMBchannel(id)){
	    nonNominal[4]++;
            ATH_MSG_VERBOSE( "hvdev EMBC" << hvdev );
            auto meta = Monitored::Scalar<float>("etaEMBC",etaChan);
            auto mphi = Monitored::Scalar<float>("phiEMBC",phiChan);
            auto hvmon = Monitored::Scalar<float>("hvcorrEMBC",hvdev);
            fill(m_MonGroupName,meta,mphi,hvmon);
	  }
	  if(m_LArOnlineIDHelper->isEMECchannel(id)){
	    nonNominal[5]++;
            ATH_MSG_VERBOSE( "hvdev EMECC" << hvdev );
            auto meta = Monitored::Scalar<float>("etaEMECC",etaChan);
            auto mphi = Monitored::Scalar<float>("phiEMECC",phiChan);
            auto hvmon = Monitored::Scalar<float>("hvcorrEMECC",hvdev);
            fill(m_MonGroupName,meta,mphi,hvmon);
	  }
	  if(m_LArOnlineIDHelper->isHECchannel(id)){
	    nonNominal[6]++;
            ATH_MSG_VERBOSE( "hvdev HECC" << hvdev );
            auto meta = Monitored::Scalar<float>("etaHECC",etaChan);
            auto mphi = Monitored::Scalar<float>("phiHECC",phiChan);
            auto hvmon = Monitored::Scalar<float>("hvcorrHECC",hvdev);
            fill(m_MonGroupName,meta,mphi,hvmon);
	  }
	  if(m_LArOnlineIDHelper->isFCALchannel(id)){
	    nonNominal[7]++;
            ATH_MSG_VERBOSE( "hvdev FCALC" << hvdev );
            auto meta = Monitored::Scalar<float>("etaFCALC",etaChan);
            auto mphi = Monitored::Scalar<float>("phiFCALC",phiChan);
            auto hvmon = Monitored::Scalar<float>("hvcorrFCALC",hvdev);
            fill(m_MonGroupName,meta,mphi,hvmon);
	  }
	}
      }
    }// end Channels Loop
    
    // Fill number of problematic cell per LUMI_BLOCK
    auto monLB = Monitored::Scalar<int>("LB",lumi_block);
    auto nnEMBA = Monitored::Scalar<int>("nonnominalEMBA",nonNominal[0]);
    auto nnEMECA = Monitored::Scalar<int>("nonnominalEMECA",nonNominal[1]);
    auto nnHECA = Monitored::Scalar<int>("nonnominalHECA",nonNominal[2]);
    auto nnFCALA = Monitored::Scalar<int>("nonnominalFCALA",nonNominal[3]);
    auto nnEMBC = Monitored::Scalar<int>("nonnominalEMBC",nonNominal[4]);
    auto nnEMECC = Monitored::Scalar<int>("nonnominalEMECC",nonNominal[5]);
    auto nnHECC = Monitored::Scalar<int>("nonnominalHECC",nonNominal[6]);
    auto nnFCALC = Monitored::Scalar<int>("nonnominalFCALC",nonNominal[7]);
    fill(m_MonGroupName,monLB,nnEMBA,nnEMBC,nnEMECA,nnEMECC,nnHECA,nnFCALA,nnHECC,nnFCALC);
    
  } else {
    
    return StatusCode::SUCCESS;
    
  }// End of event treatment
  
  return StatusCode::SUCCESS;
}



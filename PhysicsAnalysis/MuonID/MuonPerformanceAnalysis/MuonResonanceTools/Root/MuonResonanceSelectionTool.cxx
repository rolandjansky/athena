/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// MuonResonanceSelectionTool.cxx

#include "MuonResonanceTools/MuonResonanceSelectionTool.h"

//**********************************************************************

MuonResonanceSelectionTool::MuonResonanceSelectionTool(std::string myname)
  : AsgTool(myname),
    m_highMassWindow(110000.0),
    m_lowMassWindow(90000.0),
#ifndef ROOTCORE
    m_seliTool("CP::MuonSelectionTool/MuonSelectionTool", this), 
    m_sfTool("CP::MuonEfficiencyScaleFactors/MuonEfficiencyScaleFactors", this ),
    m_calibTool("CP::MuonCalibrationAndSmearingTool/MuonCalibrationAndSmearingTool", this ),
    m_matchTool("Trig::TrigMuonMatching/TrigMuonMatching", this),
    m_trigTool("Trig::TrigDecisionTool/TrigDecisionTool", this)
#else 
    m_seliTool("CP::MuonSelectionTool/MuonSelectionTool"), 
    m_sfTool("CP::MuonEfficiencyScaleFactors/MuonEfficiencyScaleFactors" ),
    m_calibTool("CP::MuonCalibrationAndSmearingTool/MuonCalibrationAndSmearingTool" ),
    m_matchTool("Trig::TrigMuonMatching/TrigMuonMatching"),
    m_trigTool("Trig::TrigDecisionTool/TrigDecisionTool")
#endif
{
  
  declareProperty("PtCut",          m_ptCut = 20000.0);
  declareProperty("EtaCut",         m_etaCut = 2.5);
  declareProperty("IsoCaloCut",     m_isoCaloCut = 0.12);
  declareProperty("IsoTrkCut",      m_isoTrkCut = 0.12);
  declareProperty("Sigd0Cut",       m_d0Cut = 3.0);
  declareProperty("z0Cut",          m_z0Cut = 0.4);
  declareProperty("Max_d0",         m_Abs_d0Cut = 2.0);
  declareProperty("Max_z0",         m_Abs_z0Cut = 10.0);
  declareProperty("UseIDCuts",      m_IDCuts = true);
  declareProperty("Calibrate",      m_doCalib = true);
  declareProperty("EfficiencyCorr", m_doEff = true);
  declareProperty("TriggerList",    m_triggerList );

  declareProperty("MuonSelectionTool",   m_seliTool );
  declareProperty("ScaleFactorTool",     m_sfTool );
  declareProperty("MuonCalibrationTool", m_calibTool );
  declareProperty("TriggerDecisionTool", m_trigTool ); 
  declareProperty("TriggerMatchingTool", m_matchTool ); 
  
}

StatusCode MuonResonanceSelectionTool::initialize()
{
  
  ATH_CHECK( m_seliTool.retrieve() );
  if (m_doEff) ATH_CHECK( m_sfTool.retrieve() );
  if (m_doCalib) ATH_CHECK( m_calibTool.retrieve() );
  ATH_CHECK( m_trigTool.retrieve() );
  ATH_CHECK( m_matchTool.retrieve() ); 

  ATH_MSG_INFO("Tools initialized:" );
  ATH_MSG_INFO("SelectionTool   :: \t" << m_seliTool );
  ATH_MSG_INFO("ScaleFactorTool :: \t" << m_sfTool );
  ATH_MSG_INFO("CalibrationTool :: \t" << m_calibTool );
  ATH_MSG_INFO("TriggerTool     :: \t" << m_trigTool );
  ATH_MSG_INFO("TrigMatchingTool:: \t" << m_matchTool );

  ATH_MSG_INFO("Selection summary :  ");
  ATH_MSG_INFO("PtCut        = " <<  m_ptCut );
  ATH_MSG_INFO("EtaCut       = " <<  m_etaCut );
  ATH_MSG_INFO("IsoCaloCut   = " <<  m_isoCaloCut );
  ATH_MSG_INFO("IsoTrkCut    = " <<  m_isoTrkCut );
  ATH_MSG_INFO("Sigd0Cut     = " <<  m_d0Cut );
  ATH_MSG_INFO("z0Cut        = " <<  m_z0Cut );
  ATH_MSG_INFO("Max_d0       = " <<  m_Abs_d0Cut );
  ATH_MSG_INFO("Max_z0       = " <<  m_Abs_z0Cut );
  ATH_MSG_INFO("UseIDCuts    = " <<  m_IDCuts );
  ATH_MSG_INFO("Calibrate    = " <<  m_doCalib );
  ATH_MSG_INFO("Eff. Corr    = " <<  m_doEff ); 
  
  return StatusCode::SUCCESS;
}


//**********************************************************************


std::pair<std::vector<const xAOD::Muon*>,std::vector<const xAOD::Muon*> > MuonResonanceSelectionTool::selectMuons(const xAOD::MuonContainer* tags, bool isMC, CP::SystematicSet sys) const{

  ATH_MSG_DEBUG("Number of found Muons :"<< tags->size() ); 
  std::pair<std::vector<const xAOD::Muon*>,std::vector<const xAOD::Muon*> > goodMuons = std::make_pair(std::vector<const xAOD::Muon*>(), std::vector<const xAOD::Muon*>());

  if( !isTriggered() ){ 
    ATH_MSG_DEBUG("No trigger pass - rejecting event");
    return goodMuons;
  }

  if (m_doCalib) {      
    if( m_calibTool->applySystematicVariation( sys ) != CP::SystematicCode::Ok ) 
      ATH_MSG_WARNING( "Cannot configure muon calibration tool for systematic " << sys.name() ); 
  }
  if (m_doEff) {
    if( m_sfTool->applySystematicVariation( sys ) != CP::SystematicCode::Ok ) 
      ATH_MSG_WARNING( "Cannot configure muon efficiency corrections for systematic " << sys.name() );
  }
  
  // loop over muon container
  for(auto tag : *tags) {
  
    // select good muons (combined for the time being)
    if(tag->muonType() != xAOD::Muon::MuonType::Combined) continue;

    //correct muon
    xAOD::Muon* mu = 0;
    if(m_doCalib){    
      try{ m_calibTool->correctedCopy( *tag, mu );}
      catch(SG::ExcBadAuxVar&){
	ATH_MSG_WARNING( "Cannot retrieve aux-item - rejecting muon" );
	delete mu;
	continue;
      }
    }
    else{ 
      try{ mu = copy(*tag);}
      catch(SG::ExcBadAuxVar&){    
      	ATH_MSG_WARNING( "Cannot retrieve aux-item - rejecting muon" );
	delete mu;
	continue;
      }
    }
   
    ATH_MSG_DEBUG("Selected muon TLV Pt|Eta|Phi|E :: " 
		  << mu->p4().Pt() << " | " <<  mu->p4().Eta() << " | " << mu->p4().Phi() << " | " << mu->p4().E() );
    ATH_MSG_DEBUG("pT(corr)/pT : " << mu->pt()/tag->pt() );

    //apply efficiency SF
    if(m_doEff) applySF(*mu, isMC);
    
    //pass MuonSelectionTool
    if(!m_seliTool->accept(*mu)){ 
      ATH_MSG_DEBUG("Muon rejected by " << m_seliTool );
      delete mu;
      continue;
    }

    // Cut on mu eta, pT, iso, IP
    if(mu->pt() < m_ptCut){delete mu; continue;}
    if(TMath::Abs(mu->eta()) > m_etaCut){delete mu; continue;}
    float caloiso = m_isoCaloCut * mu->pt();
    float trkiso  = m_isoTrkCut * mu->pt();
    if( !(mu->isolation(caloiso, xAOD::Iso::etcone30)) ||
	!(mu->isolation(trkiso,  xAOD::Iso::ptcone30)) ){delete mu; continue;}
    if( !IPCut(*mu, m_z0Cut, m_d0Cut) ||
	!IPCutAbs(*mu, m_Abs_z0Cut, m_Abs_d0Cut) ){delete mu; continue;}
    if( m_IDCuts && !IDTrk(*mu) ){delete mu; continue;}

    //TriggerMatching
    applyTriggerMatch(*mu);

    if( mu->trackParticle(xAOD::Muon::Primary)->charge()>0) goodMuons.first.push_back(mu);   
    else goodMuons.second.push_back(mu);
  }
									  
  ATH_MSG_DEBUG("Number of selected Muons   : " << goodMuons.first.size()<< " "<< goodMuons.second.size() );

  return goodMuons;
}


//apply efficiency scale factors
void MuonResonanceSelectionTool::applySF(const xAOD::Muon& mu, bool isMC) const{
  
  float sf = 1.;
  if(isMC){
    if( m_sfTool->applyEfficiencyScaleFactor(mu) == CP::CorrectionCode::Error ){
      ATH_MSG_WARNING( "Failed to apply scale factor to the muon" );
      return;
    }
  }
  if(mu.isAvailable< float >("EfficiencyScaleFactor") ) sf = mu.auxdata< float >("EfficiencyScaleFactor");
  ATH_MSG_DEBUG("Muon efficiency SF :  " << sf );
  return;
}


// apply impact parameter cuts
bool  MuonResonanceSelectionTool::IPCut(const xAOD::Muon& mu, float z0cut, float d0cut) const{
  
  const xAOD::TrackParticle* tp  = const_cast<xAOD::TrackParticle*>(mu.primaryTrackParticle());
  if( mu.muonType() == xAOD::Muon::Combined && !tp )
    tp = const_cast<xAOD::TrackParticle*>((*mu.inDetTrackParticleLink()));
  
  if(tp){
    float err_d0 = sqrt(tp->definingParametersCovMatrix()(0,0));
    if(err_d0==0) err_d0 = 1e-9;

    //correct z0 wrt to PV
    const xAOD::Vertex *vx = const_cast<xAOD::Vertex*>(tp->vertex());
    float z0_corr=-99.;
    if(vx) z0_corr = tp->z0() + tp->vz() - vx->z();
        
    float IP_sigd0 = TMath::Abs(tp->d0()/err_d0);
    float IP_z0 = TMath::Abs(z0_corr*TMath::Sin(tp->theta()));

    ATH_MSG_DEBUG("IP:  d0 " << tp->d0() << " z0 "<< z0_corr << " theta " << tp->theta() << " sig(d0) "<< err_d0 );
        
    if(IP_sigd0<d0cut && IP_z0<z0cut) return true;
    else return false;
  }
  else{ 
    ATH_MSG_DEBUG("no associated track found, rejecting muon" );
    return false;
  }
}


// apply cut on total values of IPs
bool  MuonResonanceSelectionTool::IPCutAbs(const xAOD::Muon& mu, float Abs_z0, float Abs_d0) const{
  
  const xAOD::TrackParticle* tp  = const_cast<xAOD::TrackParticle*>(mu.primaryTrackParticle());
  if( mu.muonType() == xAOD::Muon::Combined && !tp )
    tp = const_cast<xAOD::TrackParticle*>((*mu.inDetTrackParticleLink()));
  
  if(tp){
    //correct z0 wrt to PV
    const xAOD::Vertex *vx = const_cast<xAOD::Vertex*>(tp->vertex());
    float z0_corr=-99.;
    if(vx) z0_corr = tp->z0() + tp->vz() - vx->z();
      
    if(tp->d0()<Abs_d0 && z0_corr<Abs_z0) return true;
    else return false;
  }
  else{ 
    ATH_MSG_DEBUG("no associated track found, rejecting muon" );
    return false;
  }
}


//apply cuts on ID track quality (obsolete if MuonSelectorTool is called)
bool MuonResonanceSelectionTool::IDTrk(const xAOD::Muon& mu) const{

  const xAOD::TrackParticle* id = const_cast<xAOD::TrackParticle*>((*mu.inDetTrackParticleLink()));

  if (id){
    uint8_t n_BHits=0, n_PixHits=0, n_PixDS=0, n_PixHoles=0, n_SCTHits=0, n_SCTDS=0, n_SCTHoles=0, n_TRTHits=0, n_TRTOut=0;
    if(!id->summaryValue(n_BHits,   xAOD::numberOfBLayerHits))       ATH_MSG_DEBUG("Could not retrieve number of BLayer hits" );
    if(!id->summaryValue(n_PixHits, xAOD::numberOfPixelHits))        ATH_MSG_DEBUG("Could not retrieve number of Pixel hits" );
    if(!id->summaryValue(n_PixDS,   xAOD::numberOfPixelDeadSensors)) ATH_MSG_DEBUG("Could not retrieve number of Pixel dead sensors" );
    if(!id->summaryValue(n_PixHoles,xAOD::numberOfPixelHoles))       ATH_MSG_DEBUG("Could not retrieve number of Pixel holes" );
    if(!id->summaryValue(n_SCTHits, xAOD::numberOfSCTHits))          ATH_MSG_DEBUG("Could not retrieve number of SCT hits" );
    if(!id->summaryValue(n_SCTDS,   xAOD::numberOfSCTDeadSensors))   ATH_MSG_DEBUG("Could not retrieve number of SCT dead ensors" );
    if(!id->summaryValue(n_SCTHoles,xAOD::numberOfSCTHoles))         ATH_MSG_DEBUG("Could not retrieve number of SCT holes" );
    if(!id->summaryValue(n_TRTHits, xAOD::numberOfTRTHits))          ATH_MSG_DEBUG("Could not retrieve number of TRT hits" );
    if(!id->summaryValue(n_TRTOut,  xAOD::numberOfTRTOutliers ))     ATH_MSG_DEBUG("Could not retrieve number of TRT outliners" );

    int nBHits=int(n_BHits),nPixHits=int(n_PixHits),nPixDS=int(n_PixDS),nPixHoles=int(n_PixHoles),nSCTHits=int(n_SCTHits),nSCTDS=int(n_SCTDS),nSCTHoles=int(n_SCTHoles),nTRTHits=int(n_TRTHits),nTRTOut=int(n_TRTOut),nTRT = nTRTHits + nTRTOut;
    ATH_MSG_DEBUG("Summary values: BLayerHits|PixelHits|PixelDeadSensors|PixelHoles|SCTHits|SCTDeadSensors|SCTHoles|TRTHits|TRTOutliners|  " 
		  << nBHits << "|" << nPixHits << "|" << nPixDS << "|" <<nPixHoles << "|" <<nSCTHits<< "|" <<nSCTDS<< "|" <<nSCTHoles<< "|" <<nTRTHits<< "|" <<nTRTOut<< "|" <<nTRT );   

    // if (nBHits < 1) return false;
    if (nPixHits + nPixDS < 1) return false;
    if (nSCTHits + nSCTDS < 5) return false;
    if (nPixHoles + nSCTHoles >= 3) return false;
    if( fabs(id->eta())>0.1 && fabs(id->eta())<1.9 ){
      if(nTRT<=5 || nTRTOut >= ((float)nTRT)*0.9) return false;
    }
    return true;
  }
  else{
    ATH_MSG_DEBUG("no ID track found, rejecting muon" );
    return false;
  }
}

// creates uncorrected copy of muon
xAOD::Muon* MuonResonanceSelectionTool::copy(const xAOD::Muon& mu) const{
  
  xAOD::Muon *mu_c = 0;
  if(mu.m()<=0) return mu_c;
  mu_c = new xAOD::Muon();
  mu_c->makePrivateStore(mu);
  return mu_c;
}

//Check if event passed trigger
bool MuonResonanceSelectionTool::isTriggered (void) const{

  ATH_MSG_DEBUG("Selected triggers " << (int)m_triggerList.size() << "\t Triggers found by tool " << (int)m_trigTool->getListOfTriggers().size() );
  if (m_triggerList.size() == 0 || m_trigTool->getListOfTriggers().size() == 0) return true; 

  for (auto trigger : m_triggerList){
    ATH_MSG_DEBUG("Asking trigger "<< trigger); 
    if ( m_trigTool->isPassed(trigger) ) return true; 
  } 
  return false; 
}

void MuonResonanceSelectionTool::applyTriggerMatch(xAOD::Muon& mu) const{

  if(m_triggerList.size() == 0 || m_trigTool->getListOfTriggers().size() == 0 ){ 
    mu.auxdata< bool >("isTriggered") = true;
    return;
  }   
  bool isTriggered = false;
  for (auto trigger :  m_triggerList) {
    ATH_MSG_DEBUG("Matching on " << trigger << " : \t" << m_matchTool->match(mu.eta(), mu.phi(), trigger) );       
    if( m_matchTool->match(mu.eta(), mu.phi(), trigger) ) isTriggered = true;
  }
  mu.auxdata< bool >("isTriggered") = isTriggered;
  return;
}



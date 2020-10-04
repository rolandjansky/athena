/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#include "MuonResonanceTools/MuonResonanceSelectionTool.h"
#include "xAODEventInfo/EventInfo.h"
#include "xAODTracking/TrackParticlexAODHelpers.h"
#include "xAODTracking/VertexContainer.h"
#include "xAODTracking/Vertex.h"

// **********************************************************************

MuonResonanceSelectionTool::MuonResonanceSelectionTool(std::string myname)
  : AsgTool(myname),
    m_highMassWindow(110000.0),
    m_lowMassWindow(90000.0),
#ifndef ROOTCORE
    m_seliTool("CP::MuonSelectionTool/MuonSelectionTool", this),
    m_sfTool("CP::MuonEfficiencyScaleFactors/MuonEfficiencyScaleFactors", this ),
    m_calibTool("CP::MuonCalibrationAndSmearingTool/MuonCalibrationAndSmearingTool", this ),
    m_matchTool(""),
    m_trigTool("")
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
  declareProperty("d0Cut",          m_d0Cut = 3.0);
  declareProperty("z0Cut",          m_z0Cut = 0.4);
  declareProperty("Max_d0",         m_Abs_d0Cut = 2.0);
  declareProperty("Max_z0",         m_Abs_z0Cut = 10.0);
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
  if (!m_trigTool.empty()) ATH_CHECK( m_trigTool.retrieve() );
  if (!m_matchTool.empty()) ATH_CHECK( m_matchTool.retrieve() );

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
  ATH_MSG_INFO("d0Cut        = " <<  m_d0Cut );
  ATH_MSG_INFO("z0Cut        = " <<  m_z0Cut );
  ATH_MSG_INFO("Max_d0       = " <<  m_Abs_d0Cut );
  ATH_MSG_INFO("Max_z0       = " <<  m_Abs_z0Cut );
  ATH_MSG_INFO("Calibrate    = " <<  m_doCalib );
  ATH_MSG_INFO("Eff. Corr    = " <<  m_doEff );

  return StatusCode::SUCCESS;
}

//**********************************************************************

std::pair<std::vector<const xAOD::Muon*>,std::vector<const xAOD::Muon*> > MuonResonanceSelectionTool::selectMuons(const xAOD::MuonContainer* tags, bool isMC, CP::SystematicSet sys) {

  ATH_MSG_DEBUG("Number of found Muons :"<< tags->size() );
  std::pair<std::vector<const xAOD::Muon*>,std::vector<const xAOD::Muon*> > goodMuons = std::make_pair(std::vector<const xAOD::Muon*>(), std::vector<const xAOD::Muon*>());

  const xAOD::EventInfo* info = 0;
  ATH_MSG_DEBUG(""<<evtStore());
  if (evtStore()->retrieve(info, "EventInfo").isFailure()){
      ATH_MSG_FATAL( "Unable to retrieve Event Info" );
  }

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

    // select muon type (Combined = 0)
    if(tag->muonType() != xAOD::Muon::MuonType::Combined) continue;

    // select muon author (MuidCo = 1)
    if(tag->author() != xAOD::Muon::Author::MuidCo) continue;

    // correct muon
    xAOD::Muon* mu = 0;
    if(m_doCalib){
      try{ m_calibTool->correctedCopy( *tag, mu ).ignore();}
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

    // apply efficiency SF
    if(m_doEff) applySF(*mu, isMC);

    // pass MuonSelectionTool
    if(!m_seliTool->accept(*mu)){
      ATH_MSG_DEBUG("Muon rejected by " << m_seliTool );
      delete mu;
      continue;
    }

    // Cut on muon eta, pT, iso, IP - values defined in the JobOptions
    if(mu->pt() < m_ptCut){delete mu; continue;}
    if(std::abs(mu->eta()) > m_etaCut){delete mu; continue;}
    float caloiso = m_isoCaloCut * mu->pt();
    float trkiso  = m_isoTrkCut * mu->pt();
    if( !(mu->isolation(caloiso, xAOD::Iso::etcone30)) ||
	!(mu->isolation(trkiso,  xAOD::Iso::ptcone30)) ){delete mu; continue;}
    if( !IPCut(*mu, m_z0Cut, m_d0Cut) ||
        !IPCutAbs(*mu, m_Abs_z0Cut, m_Abs_d0Cut) ){delete mu; continue;}

    // TriggerMatching
    applyTriggerMatch(*mu);

    if( mu->trackParticle(xAOD::Muon::Primary)->charge()>0) goodMuons.first.push_back(mu);
    else goodMuons.second.push_back(mu);

  }

  ATH_MSG_DEBUG("Number of selected Muons   : " << goodMuons.first.size()<< " "<< goodMuons.second.size() );

  return goodMuons;
}

// apply efficiency scale factors
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

bool MuonResonanceSelectionTool::IPCut(const xAOD::Muon& mu, float z0cut, float d0cut) const{

  const xAOD::EventInfo* info = 0;
  ATH_MSG_DEBUG(""<<evtStore());
  if (evtStore()->retrieve(info, "EventInfo").isFailure()){
    ATH_MSG_FATAL( "Unable to retrieve Event Info" );
  }

  const xAOD::TrackParticle* tp  = mu.primaryTrackParticle();
  if( mu.muonType() == xAOD::Muon::Combined && !tp )
    tp = *mu.inDetTrackParticleLink();

  if(tp){
    float d0 = tp->d0();
    float z0 = tp->z0();

    const xAOD::VertexContainer* primVertices = 0;
    const xAOD::Vertex* vx = 0;
    if(evtStore()->retrieve(primVertices,"PrimaryVertices").isFailure()){
    ATH_MSG_ERROR("Found no PV candidate for IP computation!");
    }
    else {
      vx = primVertices->at(0);
    }

    // delta z0
    float delta_z0 = 100.0;
    if(vx) delta_z0 = std::abs(tp->z0() + tp->vz() - vx->z());

    // d0 significance
    double d0_sig = xAOD::TrackingHelpers::d0significance( tp, info->beamPosSigmaX(), info->beamPosSigmaY(), info->beamPosSigmaXY() );  // d0 significance can be positive or negative!

    float IP_d0 = std::abs(d0_sig);
    float IP_z0 = std::abs(delta_z0*std::sin(tp->theta()));

    ATH_MSG_DEBUG("IP:  d0 " << d0 << " z0 "<< z0 << " theta " << tp->theta() << " d0_significance "<< d0_sig );

    if (IP_d0<d0cut && IP_z0<z0cut) return true;
    else return false;
  }
  else{
    ATH_MSG_DEBUG("no associated track found, rejecting muon" );
    return false;
  }
}

// apply cut on total values of IPs
bool MuonResonanceSelectionTool::IPCutAbs(const xAOD::Muon& mu, float Abs_z0, float Abs_d0) const{

  const xAOD::TrackParticle* tp  = mu.primaryTrackParticle();
  if( mu.muonType() == xAOD::Muon::Combined && !tp )
    tp = *mu.inDetTrackParticleLink();

  if(tp){
    const xAOD::VertexContainer* primVertices = 0;
    const xAOD::Vertex* vx = 0;
    if(evtStore()->retrieve(primVertices,"PrimaryVertices").isFailure()){
    ATH_MSG_ERROR("Found no PV candidate for IP computation!");
    }
    else {
      vx = primVertices->at(0);
    }
    float delta_z0 = 100.0;
    if(vx) delta_z0 = std::abs(tp->z0() + tp->vz() - vx->z());

    if(tp->d0()<Abs_d0 && delta_z0<Abs_z0) return true;
    else return false;
  }
  else{
    ATH_MSG_DEBUG("no associated track found, rejecting muon" );
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

// check if event pass GRL
bool MuonResonanceSelectionTool::hasPassedGRL (void) const{

  const xAOD::EventInfo* info = 0;
  ATH_MSG_DEBUG(""<<evtStore());
  if (evtStore()->retrieve(info, "EventInfo").isFailure()){
    ATH_MSG_FATAL( "Unable to retrieve Event Info" );
  }

  if(!info->eventType(xAOD::EventInfo::IS_SIMULATION)){

  int runNb = info->runNumber();
  int lumiBL = info->lumiBlock();

if (runNb == 279764){
  if ((lumiBL >=39 && lumiBL <=59) || (lumiBL >=66 && lumiBL <=113)) return true;
}
if (runNb == 279813){
  if ((lumiBL >=336 && lumiBL <=369) || (lumiBL >=371 && lumiBL <=380) || (lumiBL >=382 && lumiBL <=459) || (lumiBL >=462 && lumiBL <=549) || (lumiBL >=562 && lumiBL <=640) || (lumiBL >=642 && lumiBL <=859)) return true;
}
if (runNb == 279867){
  if ((lumiBL >=175 && lumiBL <=200) || (lumiBL >=202 && lumiBL <=228) || (lumiBL >=237 && lumiBL <=435)) return true;
}
if (runNb == 279928){
  if ((lumiBL >=448 && lumiBL <=455)) return true;
}
if (runNb == 279932){
  if ((lumiBL >=325 && lumiBL <=659) || (lumiBL >=661 && lumiBL <=732)) return true;
}
if (runNb == 279984){
  if ((lumiBL >=276 && lumiBL <=434) || (lumiBL >=436 && lumiBL <=506) || (lumiBL >=508 && lumiBL <=829)) return true;
}
if (runNb == 280319){
  if ((lumiBL >=100 && lumiBL <=148) || (lumiBL >=150 && lumiBL <=165) || (lumiBL >=167 && lumiBL <=680) || (lumiBL >=691 && lumiBL <=807)) return true;
}
if (runNb == 280368){
  if ((lumiBL >=163 && lumiBL <=165) || (lumiBL >=167 && lumiBL <=225)) return true;
}
if (runNb == 280423){
  if ((lumiBL >=293 && lumiBL <=313) || (lumiBL >=315 && lumiBL <=350) || (lumiBL >=352 && lumiBL <=369) || (lumiBL >=403 && lumiBL <=820) || (lumiBL >=828 && lumiBL <=838)) return true;
}
if (runNb == 280500){
  if ((lumiBL >=116 && lumiBL <=191)) return true;
}
if (runNb == 280520){
  if ((lumiBL >=166 && lumiBL <=167) || (lumiBL >=238 && lumiBL <=247) || (lumiBL >=251 && lumiBL <=251) || (lumiBL >=261 && lumiBL <=303) || (lumiBL >=308 && lumiBL <=321) || (lumiBL >=391 && lumiBL <=402) || (lumiBL >=461 && lumiBL <=493) || (lumiBL >=516 && lumiBL <=523)) return true;
}
if (runNb == 280614){
  if ((lumiBL >=381 && lumiBL <=417) || (lumiBL >=419 && lumiBL <=508) || (lumiBL >=510 && lumiBL <=550)) return true;
}
if (runNb == 280673){
  if ((lumiBL >=131 && lumiBL <=433) || (lumiBL >=437 && lumiBL <=648) || (lumiBL >=651 && lumiBL <=790) || (lumiBL >=792 && lumiBL <=989) || (lumiBL >=991 && lumiBL <=1198)) return true;
}
if (runNb == 280753){
  if ((lumiBL >=394 && lumiBL <=402) || (lumiBL >=410 && lumiBL <=429) || (lumiBL >=431 && lumiBL <=435) || (lumiBL >=457 && lumiBL <=459) || (lumiBL >=468 && lumiBL <=574) || (lumiBL >=576 && lumiBL <=699) || (lumiBL >=702 && lumiBL <=738) || (lumiBL >=762 && lumiBL <=772)) return true;
}
if (runNb == 280853){
  if ((lumiBL >=171 && lumiBL <=191) || (lumiBL >=205 && lumiBL <=245)) return true;
}
if (runNb == 280862){
  if ((lumiBL >=77 && lumiBL <=167) || (lumiBL >=169 && lumiBL <=523) || (lumiBL >=526 && lumiBL <=649) || (lumiBL >=655 && lumiBL <=724) || (lumiBL >=726 && lumiBL <=735) || (lumiBL >=737 && lumiBL <=770) || (lumiBL >=772 && lumiBL <=945) || (lumiBL >=947 && lumiBL <=987) || (lumiBL >=991 && lumiBL <=996) || (lumiBL >=998 && lumiBL <=1045) || (lumiBL >=1047 && lumiBL <=1048)) return true;
}
if (runNb == 280977){
  if ((lumiBL >=317 && lumiBL <=465) || (lumiBL >=468 && lumiBL <=515) || (lumiBL >=518 && lumiBL <=524) || (lumiBL >=526 && lumiBL <=530)) return true;
}
if (runNb == 281070){
  if ((lumiBL >=170 && lumiBL <=225) || (lumiBL >=227 && lumiBL <=238) || (lumiBL >=241 && lumiBL <=246)) return true;
}
if (runNb == 281074){
  if ((lumiBL >=163 && lumiBL <=434) || (lumiBL >=436 && lumiBL <=452)) return true;
}
if (runNb == 281075){
  if ((lumiBL >=240 && lumiBL <=247)) return true;
}
if (runNb == 281317){
  if ((lumiBL >=116 && lumiBL <=142) || (lumiBL >=144 && lumiBL <=251)) return true;
}
if (runNb == 281385){
  if ((lumiBL >=375 && lumiBL <=395) || (lumiBL >=401 && lumiBL <=516) || (lumiBL >=518 && lumiBL <=590) || (lumiBL >=592 && lumiBL <=778)) return true;
}
if (runNb == 282625){
  if ((lumiBL >=37 && lumiBL <=43) || (lumiBL >=46 && lumiBL <=48) || (lumiBL >=52 && lumiBL <=177)) return true;
}
if (runNb == 282631){
  if ((lumiBL >=102 && lumiBL <=145) || (lumiBL >=147 && lumiBL <=169) || (lumiBL >=171 && lumiBL <=281)) return true;
}
if (runNb == 282712){
  if ((lumiBL >=330 && lumiBL <=474) || (lumiBL >=476 && lumiBL <=519) || (lumiBL >=521 && lumiBL <=639) || (lumiBL >=641 && lumiBL <=653) || (lumiBL >=659 && lumiBL <=668) || (lumiBL >=671 && lumiBL <=788) || (lumiBL >=790 && lumiBL <=794) || (lumiBL >=798 && lumiBL <=801) || (lumiBL >=808 && lumiBL <=834)) return true;
}
if (runNb == 282992){
  if ((lumiBL >=131 && lumiBL <=165) || (lumiBL >=167 && lumiBL <=245) || (lumiBL >=247 && lumiBL <=255) || (lumiBL >=258 && lumiBL <=330) || (lumiBL >=341 && lumiBL <=359) || (lumiBL >=361 && lumiBL <=373) || (lumiBL >=378 && lumiBL <=408) || (lumiBL >=410 && lumiBL <=480) || (lumiBL >=482 && lumiBL <=500) || (lumiBL >=502 && lumiBL <=630) || (lumiBL >=632 && lumiBL <=640) || (lumiBL >=642 && lumiBL <=723)) return true;
}
if (runNb == 283074){
  if ((lumiBL >=123 && lumiBL <=163) || (lumiBL >=167 && lumiBL <=213) || (lumiBL >=215 && lumiBL <=325) || (lumiBL >=334 && lumiBL <=418)) return true;
}
if (runNb == 283155){
  if ((lumiBL >=145 && lumiBL <=211) || (lumiBL >=214 && lumiBL <=290)) return true;
}
if (runNb == 283270){
  if ((lumiBL >=11 && lumiBL <=75)) return true;
}
if (runNb == 283429){
  if ((lumiBL >=154 && lumiBL <=209) || (lumiBL >=211 && lumiBL <=221) || (lumiBL >=224 && lumiBL <=244) || (lumiBL >=246 && lumiBL <=289) || (lumiBL >=292 && lumiBL <=708) || (lumiBL >=711 && lumiBL <=879) || (lumiBL >=881 && lumiBL <=1009) || (lumiBL >=1011 && lumiBL <=1090) || (lumiBL >=1092 && lumiBL <=1105) || (lumiBL >=1107 && lumiBL <=1203) || (lumiBL >=1221 && lumiBL <=1246) || (lumiBL >=1248 && lumiBL <=1383) || (lumiBL >=1385 && lumiBL <=1439) || (lumiBL >=1441 && lumiBL <=1455)) return true;
}
if (runNb == 283608){
  if ((lumiBL >=8 && lumiBL <=104) || (lumiBL >=106 && lumiBL <=131) || (lumiBL >=133 && lumiBL <=135) || (lumiBL >=138 && lumiBL <=160) || (lumiBL >=162 && lumiBL <=179)) return true;
}
if (runNb == 283780){
  if ((lumiBL >=282 && lumiBL <=285) || (lumiBL >=290 && lumiBL <=379) || (lumiBL >=381 && lumiBL <=505) || (lumiBL >=507 && lumiBL <=544) || (lumiBL >=546 && lumiBL <=732) || (lumiBL >=734 && lumiBL <=1039) || (lumiBL >=1042 && lumiBL <=1066)) return true;
}
if (runNb == 284006){
  if ((lumiBL >=166 && lumiBL <=304) || (lumiBL >=311 && lumiBL <=347)) return true;
}
if (runNb == 284154){
  if ((lumiBL >=103 && lumiBL <=243)) return true;
}
if (runNb == 284213){
  if ((lumiBL >=43 && lumiBL <=164) || (lumiBL >=166 && lumiBL <=303) || (lumiBL >=306 && lumiBL <=433) || (lumiBL >=438 && lumiBL <=490) || (lumiBL >=492 && lumiBL <=776) || (lumiBL >=779 && lumiBL <=930) || (lumiBL >=932 && lumiBL <=962) || (lumiBL >=965 && lumiBL <=1033)) return true;
}
if (runNb == 284420){
 if ((lumiBL >=126 && lumiBL <=131) || (lumiBL >=133 && lumiBL <=165) || (lumiBL >=167 && lumiBL <=189) || (lumiBL >=191 && lumiBL <=230) || (lumiBL >=232 && lumiBL <=274) || (lumiBL >=277 && lumiBL <=277) || (lumiBL >=279 && lumiBL <=290) || (lumiBL >=292 && lumiBL <=348) || (lumiBL >=352 && lumiBL <=364) || (lumiBL >=367 && lumiBL <=377)) return true;
}
if (runNb == 284427){
  if ((lumiBL >=128 && lumiBL <=201) || (lumiBL >=203 && lumiBL <=265)) return true;
}
if (runNb == 284484){
  if ((lumiBL >=5 && lumiBL <=27) || (lumiBL >=34 && lumiBL <=86) || (lumiBL >=88 && lumiBL <=89) || (lumiBL >=91 && lumiBL <=95) || (lumiBL >=98 && lumiBL <=125) || (lumiBL >=127 && lumiBL <=149) || (lumiBL >=151 && lumiBL <=173) || (lumiBL >=179 && lumiBL <=280) || (lumiBL >=282 && lumiBL <=345) || (lumiBL >=347 && lumiBL <=370) || (lumiBL >=376 && lumiBL <=506)) return true;
}
  return false;
  }
  return true;
}

// check if event passed trigger
bool MuonResonanceSelectionTool::isTriggered (void) const{

  ATH_MSG_DEBUG("Selected triggers " << (int)m_triggerList.size() << "\t Triggers found by tool " << (int)m_trigTool->getListOfTriggers().size() );
  if (m_triggerList.size() == 0 || m_trigTool->getListOfTriggers().size() == 0) return true;

  for (auto trigger : m_triggerList){
    ATH_MSG_DEBUG("Asking trigger "<< trigger);
    if ( m_trigTool->isPassed(trigger) ) return true;
  }
  return false;
}

// trigger matching
void MuonResonanceSelectionTool::applyTriggerMatch(xAOD::Muon& mu) {

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

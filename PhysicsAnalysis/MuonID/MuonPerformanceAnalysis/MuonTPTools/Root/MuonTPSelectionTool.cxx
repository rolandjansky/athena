/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// MuonTPSelectionTool.cxx

#include "MuonTPTools/MuonTPSelectionTool.h"
#include "xAODTracking/VertexContainer.h"
#include "MCTruthClassifier/MCTruthClassifierDefs.h"
// #include "xAODPrimitives/tools/getIsolationCorrectionAccessor.h"
#include "xAODTruth/TruthParticleContainer.h"
#include "MuonTPTools/TrackParticlexAODHelpers.h"

//**********************************************************************

//========================================================================================================================
MuonTPSelectionTool::MuonTPSelectionTool(std::string myname) : 
    AsgTool(myname),
    m_tag_Triggers(0), 
    m_tag_Triggers_RerunMode(0), 
    m_selection_tool(),
    m_grlTool("GoodRunsListSelectionTool/GRLTool")
{
    declareProperty("TagPtCut",       m_tagPtCut = 20000.0);
    declareProperty("TagEtaCut",      m_tagEtaCut = 2.5);
    declareProperty("ProbePtCut",     m_probePtCut = 20000.0);
    declareProperty("ProbeEtaCut",    m_probeEtaCut = 3.0);
    declareProperty("HighMassWindow", m_highMassWindow = 101000.0);
    declareProperty("LowMassWindow",  m_lowMassWindow = 81000.0);
    declareProperty("IsNominal",              m_isNominal=true);
    declareProperty("IsNotIncludedInNominal", m_isNotPartOfNominal=false);

    declareProperty("EfficiencyFlag",          m_efficiencyFlag = "IDProbes");
    declareProperty("TagTriggerList",          m_tag_Triggers);
    declareProperty("TagTriggerListRerunMode", m_tag_Triggers_RerunMode);
    declareProperty("ObserverTriggerListL1",   m_observer_Triggers_L1);
    declareProperty("ObserverTriggerListHLT",  m_observer_Triggers_HLT);

    declareProperty("SelectionTool",           m_selection_tool);
    declareProperty("TriggerUtils",     m_trigUtils);
    declareProperty("GRLTool",                 m_grlTool, "The private GoodRunsListSelectionTool");
    declareProperty("IDTrackIsoDecorator",                 m_trk_iso_tool);
    declareProperty("IDTrackCaloDepositsDecorator",        m_trk_calodep_tool);
    declareProperty("DecorateProbeWithCaloDeposits",       m_addCaloDeposits = false);
    declareProperty("IsRunOnDAOD",    m_is_on_DAOD=false);

}

//========================================================================================================================
StatusCode MuonTPSelectionTool::initialize()
{
    ATH_CHECK(m_selection_tool.retrieve());
    ATH_CHECK(m_trigUtils.retrieve());
    ATH_CHECK(m_grlTool.retrieve());
    ATH_CHECK(m_trk_iso_tool.retrieve());
    ATH_MSG_INFO("m_is_on_DAOD: "<<m_is_on_DAOD);
    if (!m_is_on_DAOD) ATH_CHECK(m_trk_calodep_tool.retrieve());
    return StatusCode::SUCCESS;
}

//========================================================================================================================
ProbeContainer* MuonTPSelectionTool::selectProbes(const xAOD::MuonContainer* tags, const xAOD::IParticleContainer* probes) const
{
  // use m_muonSelectionTool to select muons
  ProbeContainer* probeCont = new ProbeContainer();
  
    // check GRL
    const xAOD::EventInfo* info = 0;
    ATH_MSG_DEBUG(""<<evtStore());
    if (evtStore()->retrieve(info, "EventInfo").isFailure()){
        ATH_MSG_FATAL( "Unable to retrieve Event Info" );
    }
    if (!passGRL(info)) return probeCont;

  // loop over tag container
  for(auto tag : *tags) {

    // select good muons (combined for the time being)
    // A muon selection tool should be used, but the tool
    // should be ASG dual-tool
    if(tag->muonType() != xAOD::Muon::MuonType::Combined) continue;
    // Could also cut on tag author, likely the same author of the probed efficiency
    // if(m_muonTagAuthor!=21 && tag->author()!=m_muonTagAuthor) continue;

    // Cut on tag eta, pT
    if(tag->pt() < m_tagPtCut) continue;
    if(fabs(tag->eta()) > m_tagEtaCut) continue;

    // for each selected tag, loop over probes
    for(auto probe : *probes) {

      // remove the probe track matched to the tag
      if(isTag(tag, probe)) continue;

      // select good probe tracks,
      // again a selecton ASG duel-tool could be used
      // tba

      // Cut on probe eta, pT
      if(probe->pt() < m_probePtCut) continue;
      if(fabs(probe->eta()) > m_probeEtaCut) continue;

      // Cut on tag-probe invariant mass
      double mtp = (tag->p4()+probe->p4()).M();
      if(mtp<m_lowMassWindow || mtp>m_highMassWindow) continue;

      // Charge cut
      //if(m_oppositeCharge &&
      //      tag->trackParticle(xAOD::Muon::Primary)->charge()

      // for each selected probe build a Probe object
      probeCont->push_back( new Probe(*tag, *probe) );
    }
  }

  ATH_MSG_DEBUG("Number of selected probes   : " << probeCont->size() );

  return probeCont;
}

//========================================================================================================================
bool MuonTPSelectionTool::passGRL(const xAOD::EventInfo* info) const
{
    if(!info->eventType(xAOD::EventInfo::IS_SIMULATION)){
        if(!m_grlTool->passRunLB(*info)) return false; //checks the GRL and skips to next event if not passing
    }
    return true;
}

//========================================================================================================================
bool MuonTPSelectionTool::isTag(const xAOD::Muon* tag, const xAOD::IParticle* probe) const
{
  return ( tag->p4().DeltaR(probe->p4()) < 0.01);
}

//========================================================================================================================
int MuonTPSelectionTool::ChargeProd (const xAOD::Muon* tag, const xAOD::IParticle* probe) const 
{
  // case of a muon probe
  const xAOD::Muon* muprobe = dynamic_cast<const xAOD::Muon*>(probe);
  if (muprobe) {
    return muprobe->trackParticle(xAOD::Muon::Primary)->charge() * tag->trackParticle(xAOD::Muon::Primary)->charge();
  }
  // case of an ID probe
  const xAOD::TrackParticle* trkprobe = dynamic_cast<const xAOD::TrackParticle*>(probe);
  if (trkprobe){
    return trkprobe->charge() * tag->trackParticle(xAOD::Muon::Primary)->charge();
  }
  return 0;
}

//========================================================================================================================
double MuonTPSelectionTool::DeltaPhiTP (const xAOD::Muon* tag, const xAOD::IParticle* probe) const
{
  return fabs(tag->p4().DeltaPhi(probe->p4()));
}

//========================================================================================================================
bool MuonTPSelectionTool::isFinalStateTruthMuon(const xAOD::IParticle* part) const
{
  // first, make sure we are actually looping over truth particles!
  const xAOD::TruthParticle* truthmu = dynamic_cast <const xAOD::TruthParticle*>(part);
  if (!truthmu) return false;

  // const xAOD::TruthParticle* muparent = truthmu->parent(0);
  if (fabs(truthmu->pdgId()) != 13) return false;
  // check the status code
  if (truthmu->status() != 1) return false;
  
  if (truthmu->isAvailable<int>("truthType")){
      try{
        // check the muon type
        if (truthmu->auxdata<int>("truthType") != MCTruthPartClassifier::IsoMuon ) return false; // IsoMuon
        // check the muon parent
        if (truthmu->auxdata<int>("truthOrigin") != MCTruthPartClassifier::ZBoson && truthmu->auxdata<int>("truthOrigin") != MCTruthPartClassifier::CCbarMeson && truthmu->auxdata<int>("truthOrigin") != MCTruthPartClassifier::JPsi ) return false;// ZBoson or CCbarMeson
      }
      catch(SG::ExcBadAuxVar failed){
        ATH_MSG_WARNING("Missing Truth decorations!");
      }  
    }
  
  return true;
}

//========================================================================================================================
bool MuonTPSelectionTool::isTruthMatched(const xAOD::IParticle* part) const
{
  // it's an xAOD::Muon
  const xAOD::Muon* muon = dynamic_cast<const xAOD::Muon*>(part);
  if(muon) {
    if(muon->isAvailable<ElementLink<xAOD::TruthParticleContainer> >("truthParticleLink")) {
      ElementLink<xAOD::TruthParticleContainer> link = muon->auxdata<ElementLink<xAOD::TruthParticleContainer> >("truthParticleLink");
      try{
          if(link.isValid() && (*link)->auxdata<int>("truthType")==MCTruthPartClassifier::IsoMuon)
          return true;
      }
      catch (SG::ExcBadAuxVar &){
          ATH_MSG_INFO("Unable to access truth matching decoration for a muon!");
      }
    }
  } 
  // it's an xAOD::TrackParticle 
  else{
    const xAOD::TrackParticle* track = dynamic_cast<const xAOD::TrackParticle*>(part);
    try{
        if(track && track->auxdata<int>("truthType")==MCTruthPartClassifier::IsoMuon)
          return true;    
        }
    catch (SG::ExcBadAuxVar &){
        ATH_MSG_INFO("Unable to access truth matching decoration for a track!");
    }
  }  

  return false;
}

//========================================================================================================================
bool MuonTPSelectionTool::passDummyTrigger(const xAOD::Muon* tag) const
{
  // until we have real trigger info, we select muons that went into the endcaps and are well above the pt threshold...
  //     (void)passTagTrigger(tag);
  return (fabs(tag->eta()) > 1.1 && tag->pt() > 25000.);
}

//========================================================================================================================
bool MuonTPSelectionTool::TagTriggerMatch (const xAOD::Muon* tag) const
{


    //  nothing requested -> pass through
    if (m_tag_Triggers.size() == 0) return true;
    //  require the tag to match one of our triggers
    bool pass = false;
    for (auto trig: m_tag_Triggers) {
         pass |= m_trigUtils->Trig_Match(*tag, trig);
    }
    return pass;
}

//========================================================================================================================
bool MuonTPSelectionTool::TagTriggerMatch_RerunMode (const xAOD::Muon* tag) const
{
    //  nothing requested -> pass through
    if (m_tag_Triggers_RerunMode.size() == 0) return true;
    //  require the tag to match one of our triggers
    bool pass = false;
    for (auto trig: m_tag_Triggers_RerunMode) {
         pass |= m_trigUtils->Trig_Match_RM(*tag, trig);
    }
    return pass;
}

//========================================================================================================================
void MuonTPSelectionTool::AddCutFlowHist(MuonTPCutFlowBase* hist)
{
  m_cutFlows.push_back(hist);
}

//========================================================================================================================
void MuonTPSelectionTool::FillCutFlows(std::string step, double weight) const
{
  for (auto cf : m_cutFlows){
    cf->fill(step.c_str(),weight);
  }
}

//========================================================================================================================
const std::vector<std::string>& MuonTPSelectionTool::observerTriggerList(const std::string &key) const
{
  if(key == "L1")  return m_observer_Triggers_L1;
  if(key == "HLT") return m_observer_Triggers_HLT;
  
  ATH_MSG_WARNING("observerTriggerList - unknown key: " << key);
  
  static std::vector<std::string> empty;

  return empty;
}

//========================================================================================================================
bool MuonTPSelectionTool::PassIPCuts(const xAOD::TrackParticle* tp, double d0cut, double d0signcut, double z0cut) const
{
  if (!tp) {
      ATH_MSG_ERROR("TrackParticle is null!");
      return false;
  }
  // find the PV
  const xAOD::VertexContainer* primVertices = 0 ;
  const xAOD::Vertex* pv = 0;
  if(evtStore()->retrieve(primVertices,"PrimaryVertices").isFailure()){
    ATH_MSG_ERROR("Found no PV candidate for IP computation!");
  }
  else {
    if (primVertices->size()) pv = primVertices->at(0);
    else ATH_MSG_ERROR("PrimaryVertices has size 0!");
  }

  const xAOD::EventInfo* info = 0;
  ATH_MSG_DEBUG(""<<evtStore());
  if (evtStore()->retrieve(info, "EventInfo").isFailure()){
    ATH_MSG_FATAL( "Unable to retrieve Event Info" );
  }
  
  float d0     = tp->d0();
  float d0sign = 0.;
    try {
        d0sign = xAOD::MuonTP_TrackingHelpers::d0significance(tp, info->beamPosSigmaX(), info->beamPosSigmaY(), info->beamPosSigmaXY());
    } catch (std::runtime_error&) {
        ATH_MSG_WARNING("Could not calculate d0significance of TrackParticle, set it to 1.e10.");
        d0sign = 1.e10;
    }
  float z0     = tp->z0();
  if (pv) z0   += tp->vz() - pv->z();

  if (d0cut >= 0 && fabs(d0) > d0cut) return false;
  if (d0signcut >= 0 && fabs(d0sign) > d0signcut) return false;
  if (z0cut >= 0 && fabs(z0) > z0cut) return false;

  return true;
}

//========================================================================================================================
bool MuonTPSelectionTool::Event_Trigger (void) const
{
    // if we are not requesting a trigger, or if the DS has no trigger info, accept the event
    if (m_tag_Triggers.size() == 0) return true;
    // otherwise see if one of our triggers is passed.

    for (auto trig: m_tag_Triggers){
        if (m_trigUtils->TrigDecision(trig)) return true;
    }
    return false;
}

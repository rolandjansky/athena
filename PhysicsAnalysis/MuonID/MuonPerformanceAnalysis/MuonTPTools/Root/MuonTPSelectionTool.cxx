/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// MuonTPSelectionTool.cxx

#include "MuonTPTools/MuonTPSelectionTool.h"
#include "xAODTracking/VertexContainer.h"
// #include "xAODPrimitives/​tools/​getIsolationCorrectionAccessor.h"

//**********************************************************************

MuonTPSelectionTool::MuonTPSelectionTool(std::string myname) : AsgTool(myname),m_tag_Triggers(0), m_selection_tool(),m_trigTool("Trig::TrigDecisionTool/TrigDecisionTool"),  m_matchTool("Trig::ITrigMuonMatching/TrigMuonMatching"){
  declareProperty("TagPtCut",       m_tagPtCut = 20000.0);
  declareProperty("TagEtaCut",      m_tagEtaCut = 2.5);
  declareProperty("ProbePtCut",     m_probePtCut = 20000.0);
  declareProperty("ProbeEtaCut",    m_probeEtaCut = 2.5);
  declareProperty("HighMassWindow", m_highMassWindow = 101000.0);
  declareProperty("LowMassWindow",  m_lowMassWindow = 81000.0);
  declareProperty("EfficiencyFlag", m_efficiencyFlag = "IDProbes");
  declareProperty("TagTriggerList", m_tag_Triggers );
  declareProperty("SelectionTool", m_selection_tool);
  declareProperty("TriggerMatchTool", m_matchTool);
  declareProperty("TriggerDecisionTool", m_trigTool);
  
  
  //  track iso recomputation for the full athena release
# if defined(ASGTOOL_ATHENA) && !defined(XAOD_ANALYSIS)
  declareProperty("TrackIsolationTool",        m_track_iso_tool);
  declareProperty("CaloIsolationTool",        m_calo_iso_tool);
  m_track_iso_to_run.push_back(xAOD::Iso::ptcone40);
  m_calo_iso_to_run.push_back(xAOD::Iso::topoetcone40);
# endif
}

StatusCode MuonTPSelectionTool::initialize()
{
# if defined(ASGTOOL_ATHENA) && !defined(XAOD_ANALYSIS)
  ATH_CHECK(m_track_iso_tool.retrieve());
  ATH_CHECK(m_calo_iso_tool.retrieve());
# endif
  ATH_CHECK(m_selection_tool.retrieve());
  ATH_CHECK(m_trigTool.retrieve());
  ATH_CHECK(m_matchTool.retrieve());
  return StatusCode::SUCCESS;

}


//**********************************************************************

ProbeContainer* MuonTPSelectionTool::selectProbes(const xAOD::MuonContainer* tags, const xAOD::IParticleContainer* probes) const
{
  // use m_muonSelectionTool to select muons
  ProbeContainer* probeCont = new ProbeContainer();

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
      //	 tag->trackParticle(xAOD::Muon::Primary)->charge()

      // for each selected probe build a Probe object
      probeCont->push_back( new Probe(*tag, *probe) );
    }
  }

  ATH_MSG_DEBUG("Number of selected probes   : " << probeCont->size() );

  return probeCont;
}
//**********************************************************************

bool MuonTPSelectionTool::isTag(const xAOD::Muon* tag, const xAOD::IParticle* probe) const
{
  return ( tag->p4().DeltaR(probe->p4()) < 0.01);
}
int MuonTPSelectionTool::ChargeProd (const xAOD::Muon* tag, const xAOD::IParticle* probe) const {

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
double MuonTPSelectionTool::DeltaPhiTP (const xAOD::Muon* tag, const xAOD::IParticle* probe) const{
    return fabs(tag->p4().DeltaPhi(probe->p4()));
}

double MuonTPSelectionTool::ptcone_trk (const xAOD::IParticle* part,  double ) const{
    
    //  this is present in the MUON1 derivation or if our tool already wrote it in a previous iteration
    if (part->isAvailable<float>("MUON1_ptcone40")){
        return part->auxdataConst<float>("MUON1_ptcone40");
    }
    else {  
        //  Here,  we can manually compute the isolation *if* we are running in full athena.
        //  In the other cases,  we just return a dummy result. 
        # if defined(ASGTOOL_ATHENA) && !defined(XAOD_ANALYSIS)
        xAOD::TrackCorrection corrlist;
        corrlist.trackbitset.set(static_cast<unsigned int>(xAOD::Iso::coreTrackPtr));
        xAOD::TrackIsolation resultTrack;
        const xAOD::TrackParticle* tp = dynamic_cast<const xAOD::TrackParticle*>(part);
        if (!tp || ! m_track_iso_tool->trackIsolation(resultTrack, *tp, m_track_iso_to_run, corrlist,tp->vertex(),0)){
            ATH_MSG_WARNING("Failed to apply the isolation for a particle");
        }
        static SG::AuxElement::Decorator< float > ptcone40("MUON1_ptcone40");
        ptcone40(*part) = resultTrack.ptcones.at(0);
        return resultTrack.ptcones.at(0);
        # else
        return -1;
        # endif
    }
}
double MuonTPSelectionTool::etcone_trk (const xAOD::IParticle* part,  double ) const{
    
    if (part->isAvailable<float>("MUON1_topoetcone40")){
        return part->auxdataConst<float>("MUON1_topoetcone40");
    }
    else {  
        //  See above - similar to track isolation
        # if defined(ASGTOOL_ATHENA) && !defined(XAOD_ANALYSIS)
        xAOD::CaloCorrection Calocorrlist;
        Calocorrlist.calobitset.set(static_cast<unsigned int>(xAOD::Iso::coreCone) | static_cast<unsigned int>(xAOD::Iso::pileupCorrection));
        xAOD::CaloIsolation resultCalo; 
        const xAOD::TrackParticle* tp = dynamic_cast<const xAOD::TrackParticle*>(part);
        if (!tp || ! m_calo_iso_tool->caloTopoClusterIsolation(resultCalo, *tp, m_calo_iso_to_run, Calocorrlist)){
            ATH_MSG_WARNING("Failed to apply the isolation for a particle");
        }
        static SG::AuxElement::Decorator< float > etcone40("MUON1_topoetcone40");
        etcone40(*part) = resultCalo.etcones.at(0);
        return resultCalo.etcones.at(0);
        # else
        return -1;
        # endif
    }
}
bool MuonTPSelectionTool::isFinalStateTruthMuon(const xAOD::IParticle* part) const{

    // first, make sure we are actually looping over truth particles!
    const xAOD::TruthParticle* truthmu = dynamic_cast <const xAOD::TruthParticle*>(part);
    if (!truthmu) return false;

    // check PDG ID
//    ATH_MSG_INFO("PDG ID");
//    if (!truthmu->isMuon()) return false;

    // check the status code
    if (truthmu->status() != 1) return false;
    return true;
}

bool MuonTPSelectionTool::passDummyTrigger(const xAOD::Muon* tag) const{
    // until we have real trigger info, we select muons that went into the endcaps and are well above the pt threshold...
//     (void)passTagTrigger(tag);
    return (fabs(tag->eta()) > 1.1 && tag->pt() > 25000.);
}
bool MuonTPSelectionTool::TagTriggerMatch (const xAOD::Muon* tag) const{

    //  nothing requested -> pass through
    if (m_tag_Triggers.size() == 0) return true;
    //  no trigger info --> dummy trigger
    if (m_trigTool->getListOfTriggers().size() == 0) {
        ATH_MSG_DEBUG("Running with dummy trigger - no triggers found");
        return passDummyTrigger(tag);
    }
    //  require the tag to match one of our triggers
    for (auto trig: m_tag_Triggers) {
        ATH_MSG_DEBUG("Matching on "<<trig <<": \t"<<m_matchTool->match(tag, trig));
        if (m_matchTool->match(tag, trig)) return true;
    }
    return false;
}

void MuonTPSelectionTool::AddCutFlowHist(MuonTPCutFlowBase* hist){
    m_cutFlows.push_back(hist);
}
void MuonTPSelectionTool::FillCutFlows(std::string step, double weight) const{
    for (auto cf : m_cutFlows){
        cf->fill(step.c_str(),weight);
    }
}

bool MuonTPSelectionTool::PassIPCuts(const xAOD::TrackParticle* tp, double d0cut, double d0signcut, double z0cut) const{

    // find the PV
    const xAOD::VertexContainer* primVertices = 0 ;
    const xAOD::Vertex* pv = 0;
    if(evtStore()->retrieve(primVertices,"PrimaryVertices").isFailure()){
        ATH_MSG_ERROR("Found no PV candidate for IP computation!");
    }
    else {
        pv = primVertices->at(0);
    }

    float d0 = 0;
    float d0sign = 0;
    float z0 = 0;

    d0 = tp->d0();
    float d0err = sqrt(tp->definingParametersCovMatrix()(0,0));
    d0sign = fabs(d0) / (d0err != 0 ? d0err : 1e-9);
    z0 = tp->z0();
    if (pv) z0 += tp->vz() - pv->z();

    if (d0cut >= 0 && fabs(d0) > d0cut) return false;
    if (d0signcut >= 0 && fabs(d0sign) > d0signcut) return false;
    if (z0cut >= 0 && fabs(z0) > z0cut) return false;

    return true;
}


bool MuonTPSelectionTool::Event_Trigger (void) const{
    // if we are not requesting a trigger, or if the DS has no trigger info, accept the event
    ATH_MSG_DEBUG("Have "<<m_trigTool->getListOfTriggers().size() <<" Triggers");
    if (m_tag_Triggers.size() == 0 || m_trigTool->getListOfTriggers().size() == 0) return true;
    // otherwise see if one of our triggers is passed.
    for (auto trig: m_tag_Triggers){
        ATH_MSG_DEBUG("Test "<<trig <<": \t"<<m_trigTool->isPassed(trig));
        if (m_trigTool->isPassed(trig)) return true;
    }
    return false;
}

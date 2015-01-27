/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// MuonTPSelectionTool.cxx

#include "MuonTPTools/MuonTPSelectionTool.h"

//**********************************************************************

MuonTPSelectionTool::MuonTPSelectionTool(std::string myname)
: AsgTool(myname), m_selection_tool() {
  
  declareProperty("TagPtCut",       m_tagPtCut = 20000.0);
  declareProperty("TagEtaCut",      m_tagEtaCut = 2.5);
  declareProperty("ProbePtCut",     m_probePtCut = 20000.0);
  declareProperty("ProbeEtaCut",    m_probeEtaCut = 2.5);
  declareProperty("HighMassWindow", m_highMassWindow = 101000.0);
  declareProperty("LowMassWindow",  m_lowMassWindow = 81000.0);
  declareProperty("EfficiencyFlag", m_efficiencyFlag = "IDProbes");  
  declareProperty("SelectionTool", m_selection_tool);
}

StatusCode MuonTPSelectionTool::initialize()
{
  ATH_CHECK(m_selection_tool.retrieve());
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

double MuonTPSelectionTool::ptcone_trk (const xAOD::IParticle* part, const xAOD::IParticleContainer* , double ) const{
    if (part->isAvailable<float>("PtCone40")){
        return part->auxdataConst<float>("PtCone40");
    }
    else {
        ATH_MSG_WARNING("Unable to check ID track isolation. Make sure the track decoration alg is being run upstream.");
        return -1;
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
    return (fabs(tag->eta()) > 1.4 && tag->pt() > 30000.);
}

void MuonTPSelectionTool::AddCutFlowHist(MuonTPCutFlowBase* hist){
    m_cutFlows.push_back(hist);
}
void MuonTPSelectionTool::FillCutFlows(std::string step, double weight) const{
    for (auto cf : m_cutFlows){
        cf->fill(step.c_str(),weight);
    }
}
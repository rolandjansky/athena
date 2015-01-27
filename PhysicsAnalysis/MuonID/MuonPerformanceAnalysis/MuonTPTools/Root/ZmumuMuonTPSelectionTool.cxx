/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/*
 * ZmumuMuonTPSelectionTool.cxx
 *
 *  Created on: Aug 23, 2014
 *      Author: goblirsc
 */

#include "MuonTPTools/ZmumuMuonTPSelectionTool.h"

ZmumuMuonTPSelectionTool::ZmumuMuonTPSelectionTool(std::string myname)
: AsgTool(myname), MuonTPSelectionTool(myname) {

  declareProperty("UseIDProbes",  m_IDProbe = false);
  declareProperty("UseMSProbes",  m_MSProbe = false);
  declareProperty("UseCBProbes",  m_CBProbe = false);
  declareProperty("UseCaloProbes",  m_CaloProbe = false);
  declareProperty("UseTruthProbes",  m_TruthProbe = false);

  declareProperty("TagIsoCut",  m_tagIsolation = 0.2);
  declareProperty("ProbeIsoCut",  m_probeIsolation = -1);

  declareProperty("AcceptSameCharge",  m_accept_sameCharge = false);
  declareProperty("AcceptOppCharge",  m_accept_oppCharge = true);

  declareProperty("DeltaPhiCut",      m_deltaPhiCut = -1.00);

  declareProperty("EfficiencyFlag", m_efficiencyFlag = "IDProbes");

  declareProperty("DoOnlyAside",      m_only_A_side = false);
  declareProperty("DoOnlyCside",      m_only_C_side = false);
  declareProperty("ProbeAbsEtaMin",      m_probe_abseta_min = -1.00);
  declareProperty("ProbeAbsEtaMax",      m_probe_abseta_max = 100.00);


  declareProperty("ProbeIDHitCut", m_probe_ID_hits = false);
}

StatusCode ZmumuMuonTPSelectionTool::initialize()
{
  // ATH_CHECK(m_muonSelectionTool.retrieve());
  ATH_CHECK(m_selection_tool.retrieve());
  return StatusCode::SUCCESS;
}


//**********************************************************************

ProbeContainer* ZmumuMuonTPSelectionTool::selectProbes(const xAOD::MuonContainer* tags, const xAOD::IParticleContainer* probes) const
{
  // use m_muonSelectionTool to select muons
  ProbeContainer* probeCont = new ProbeContainer();
  FillCutFlows("Processed");
  bool have_tag = false;
  bool have_probe = false;
  // truth probes are selected separately
  if (m_TruthProbe){
      have_tag = true;
      for(auto probe : *probes) {

          FillCutFlows("TagCandidates");
          FillCutFlows("TagQuality");
          FillCutFlows("TagPt");
          FillCutFlows("TagEta");
          FillCutFlows("TagTrigger");
          FillCutFlows("TagIsolation");
          if (!isFinalStateTruthMuon(probe)) {
              continue;
          }


            // Here, we only need to apply the kinematic selection
          if (!PassProbeKinematics(probe)) continue;

          FillCutFlows("ProbeKinematics");
          FillCutFlows("ProbeQuality");
          FillCutFlows("ProbeQuality");
          FillCutFlows("ProbeIsolation");
          FillCutFlows("ProbeInvMass");
          FillCutFlows("ProbeCharge");
          FillCutFlows("ProbeDeltaPhi");
            // in this case, fill the 'tag' slot of the probe with the probe - there is no tag for the truth efficiencies.
          probeCont->push_back( new Probe(*probe, *probe) );
          have_probe = true;
      }
  }
  // "Real" Tag&Probe pairs
  else {
      // loop over tag container
      for(auto tag : *tags) {


        FillCutFlows("TagCandidates");
        // select good muons (combined for the time being)
        // A muon selection tool should be used, but the tool
        // should be ASG dual-tool
        if(tag->muonType() != xAOD::Muon::MuonType::Combined) continue;
        // Could also cut on tag author, likely the same author of the probed efficiency
        // if(m_muonTagAuthor!=21 && tag->author()!=m_muonTagAuthor) continue;

        Root::TAccept tagResult = m_selection_tool->accept(*tag);		// we only use the ID cuts for now

        if (!m_selection_tool->passedIDCuts(*tag)) continue;
        // Cut on tag eta, pT


        FillCutFlows("TagQuality");
        if(tag->pt() < m_tagPtCut) continue;
        FillCutFlows("TagPt");
        if(fabs(tag->eta()) > m_tagEtaCut) continue;
        FillCutFlows("TagEta");
        if(!this->passDummyTrigger(tag)) continue;
        FillCutFlows("TagTrigger");

        // tag iso cut
        float tagiso = 0.;
        bool haveIso = tag->isolation(tagiso,xAOD::Iso::ptcone40);
        if (m_tagIsolation > 0 && (!haveIso || tagiso > m_tagIsolation * tag->p4().Pt())) continue;
        FillCutFlows("TagIsolation");
        have_tag = true;
        // for each selected tag, loop over probes
        for(auto probe : *probes) {
          // check the probe type
          if (!isRightType(probe)) continue;          

          // remove the probe track matched to the tag
          if(isTag(tag, probe)) continue;

          FillCutFlows("ProbeCandidates");
          // select good probe tracks,
          // again a selecton ASG duel-tool could be used
          // tba

          // Cut on probe eta, pT
          if (!PassProbeKinematics(probe)) continue;
          FillCutFlows("ProbeKinematics");
    

          // ID hits
          xAOD::Muon* probemu = dynamic_cast<xAOD::Muon*> (probe);
          if (m_probe_ID_hits && probemu) {
              if (!m_selection_tool->passedIDCuts(*probemu)) continue;
          }
          if (m_IDProbe){
              xAOD::TrackParticle *trk = dynamic_cast<xAOD::TrackParticle*>(probe);
              if (!m_selection_tool->passedIDCuts(*trk)) continue;
          }
          FillCutFlows("ProbeQuality");

          // probe iso cut
          if (m_probeIsolation > 0){
              float probeiso = -1.;
              bool probe_have_iso = false;
              if (probemu){
                  probe_have_iso = probemu->isolation(probeiso,xAOD::Iso::ptcone40);
              }
              else{
                  probe_have_iso = true;
                  probeiso = ptcone_trk(probe,probes,0.40);
              }
              if (! probe_have_iso || probeiso > m_probeIsolation * probe->p4().Pt()) continue;

          }
          FillCutFlows("ProbeIsolation");
          // Cut on tag-probe invariant mass
          double mtp = (tag->p4()+probe->p4()).M();
          if(mtp<m_lowMassWindow || mtp>m_highMassWindow) continue;
          FillCutFlows("ProbeInvMass");

          // Charge cut
          //if(m_oppositeCharge &&
          //	 tag->trackParticle(xAOD::Muon::Primary)->charge()
          int cp = ChargeProd(tag,probe);
          if (cp == 1 && !m_accept_sameCharge) continue;
          if (cp == -1 && !m_accept_oppCharge) continue;
          FillCutFlows("ProbeCharge");    // does this work?

          // Delta Phi cut
          if (DeltaPhiTP(tag,probe) < m_deltaPhiCut) continue;
          FillCutFlows("ProbeDeltaPhi");
          // for each selected probe build a Probe object
          probeCont->push_back( new Probe(*tag, *probe) );
          have_probe = true;
        }
      }
  }
  ATH_MSG_DEBUG("Number of selected probes   : " << probeCont->size() );

  if (have_tag) FillCutFlows("HaveTag");
  if (have_probe) FillCutFlows("HaveProbe");
  return probeCont;
}
//**********************************************************************

bool ZmumuMuonTPSelectionTool::isTag(const xAOD::Muon* tag, const xAOD::IParticle* probe) const
{
  return (tag->p4().DeltaR(probe->p4()) < 0.01);
}

bool ZmumuMuonTPSelectionTool::isRightType(const xAOD::IParticle* probe) const{

    const xAOD::Muon* muprobe = dynamic_cast <const xAOD::Muon*> (probe);

    // if the particle is not a muon, assume that the user knows what TrackParticleContainer he is providing!
    if (! muprobe) return true;

    // otherwise, we have to manually pick the right probe

    // ID Probe
    if (m_IDProbe){
        return (muprobe->trackParticle(xAOD::Muon::InnerDetectorTrackParticle) != NULL);
    }
    // CT Probe

    if (m_CaloProbe){
        return (muprobe && m_selection_tool->passedCaloTagQuality(*muprobe) &&  (muprobe->isAuthor(xAOD::Muon::CaloTag) || muprobe->isAuthor(xAOD::Muon::CaloLikelihood)));
    }
    // MS Probe
    if (m_MSProbe){
        return ((muprobe->muonType() == xAOD::Muon::MuonStandAlone || muprobe->muonType() == xAOD::Muon::Combined) && muprobe->trackParticle(xAOD::Muon::MuonSpectrometerTrackParticle)!= NULL);
    }
    // CB probe
    if (m_CBProbe){
        return (muprobe->muonType() == xAOD::Muon::Combined && muprobe->trackParticle(xAOD::Muon::CombinedTrackParticle) != NULL);
    }
    return false;

}

bool ZmumuMuonTPSelectionTool::PassProbeKinematics (const xAOD::IParticle* probe) const{

    if(probe->pt() < m_probePtCut) return false;
    if(fabs(probe->eta()) > m_probeEtaCut) return false;
    if (fabs(probe->eta()) < m_probe_abseta_min) return false;
    if (fabs(probe->eta()) > m_probe_abseta_max) return false;
    if (m_only_A_side && probe->eta() < 0) return false;
    if (m_only_C_side && probe->eta() > 0) return false;
    return true;
}

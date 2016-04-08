/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/*
 * DiMuonTPSelectionTool.cxx
 *
 *  Created on: Jun 18, 2015
 *      Author: goblirsc
 */

#include "MuonTPTools/DiMuonTPSelectionTool.h"

DiMuonTPSelectionTool::DiMuonTPSelectionTool(std::string myname)
:  MuonTPSelectionTool(myname) {

  declareProperty("UseIDProbes",      m_IDProbe = false);
  declareProperty("UseMSProbes",      m_MSProbe = false);
  declareProperty("UseCBProbes",      m_CBProbe = false);
  declareProperty("UseCaloProbes",    m_CaloProbe = false);
  declareProperty("UseTruthProbes",   m_TruthProbe = false);
  declareProperty("UseTruthMatchedProbes", m_TruthMatchedProbe = false);

  declareProperty("TagTrackIsoCut",   m_tagPtConeIso = 0.2);
  declareProperty("ProbeTrackIsoCut", m_probePtConeIso = -1.00);
  declareProperty("TagCaloIsoCut",    m_tagEtConeIso = 0.2);
  declareProperty("ProbeCaloIsoCut",  m_probeEtConeIso = -1.00);

  // for background studies

  declareProperty("TagTrackAntiIsoCut",   m_tag_antiPtConeIso = -1.00);
  declareProperty("ProbeTrackAntiIsoCut", m_probe_antiPtConeIso = -1.00);
  declareProperty("TagCaloAntiIsoCut",    m_tag_antiEtConeIso = -1.00);
  declareProperty("ProbeCaloAntiIsoCut",  m_probe_antiEtConeIso = -1.00);

  declareProperty("AcceptSameCharge",  m_accept_sameCharge = false);
  declareProperty("AcceptOppCharge",   m_accept_oppCharge = true);

  declareProperty("DeltaPhiCut",       m_deltaPhiCut = -1.00);
  declareProperty("DeltaEtaCut",       m_deltaEtaCut = -1.00);

  declareProperty("EfficiencyFlag",    m_efficiencyFlag = "IDProbes");

  declareProperty("DoOnlyAside",       m_only_A_side = false);
  declareProperty("DoOnlyCside",       m_only_C_side = false);
  declareProperty("ProbeAbsEtaMin",    m_probe_abseta_min = -1.00);
  declareProperty("ProbeAbsEtaMax",    m_probe_abseta_max = 100.00);

  declareProperty("TagD0Cut",          m_tag_d0 = -1.00);
  declareProperty("TagD0SignCut",      m_tag_d0Sign = -1.00);
  declareProperty("TagZ0Cut",          m_tag_z0 = -1.00);
  declareProperty("ProbeD0Cut",        m_probe_d0 = -1.00);
  declareProperty("ProbeD0SignCut",    m_probe_d0Sign = -1.00);
  declareProperty("ProbeZ0Cut",        m_probe_z0 = -1.00);

  declareProperty("ProbeIDHitCut",     m_probe_ID_hits = false);
  
  declareProperty("doProbeChargeSys",  m_doProbeChargeSys = false); // Next time you turn this on by default for *EVERYONE*,
  declareProperty("ProbeCharge",       m_ProbeCharge = "positive"); // at least write an email :)

  declareProperty("UseVeryLooseProbes",m_VeryLooseProbe = false);
  declareProperty("UseLooseProbes",    m_LooseProbe     = false);
  declareProperty("UseMediumProbes",   m_MediumProbe    = false);
  declareProperty("UseTightProbes",    m_TightProbe     = false);
  declareProperty("UseHighPtProbes",   m_HighPtProbe    = false);
}

StatusCode DiMuonTPSelectionTool::initialize()
{
  // ATH_CHECK(m_muonSelectionTool.retrieve());
  ATH_CHECK(m_selection_tool.retrieve());
  return StatusCode::SUCCESS;
}


//**********************************************************************

ProbeContainer* DiMuonTPSelectionTool::selectProbes(const xAOD::MuonContainer* tags, const xAOD::IParticleContainer* probes) const
{
    // use m_muonSelectionTool to select muons
    ProbeContainer* probeCont = new ProbeContainer();
    FillCutFlows("Processed");
  
    // check GRL
    const xAOD::EventInfo* info = 0;
    ATH_MSG_DEBUG(""<<evtStore());
    if (evtStore()->retrieve(info, "EventInfo").isFailure()){
        ATH_MSG_FATAL( "Unable to retrieve Event Info" );
    }
    if (!passGRL(info)) return probeCont;
    FillCutFlows("GRL");
  
  if(!Event_Trigger()) return probeCont;
  FillCutFlows("Trigger");
  bool have_tag = false;
  bool have_probe = false;
  // truth probes are selected separately
  if (m_TruthProbe){
      have_tag = true;
      for(auto probe : *probes) {

          FillCutFlows("TagCandidates");
          FillCutFlows("TagQuality");
          if (!isFinalStateTruthMuon(probe)) {
              continue;
          }
          FillCutFlows("TagPt");
          FillCutFlows("TagEta");
          FillCutFlows("TagTrigger");
          FillCutFlows("TagIP");
          FillCutFlows("TagIsolation");


            // Here, we only need to apply the kinematic selection
          if (!PassProbeKinematics(probe)) continue;

          FillCutFlows("ProbeKinematics");
          FillCutFlows("ProbeQuality");
          FillCutFlows("ProbeQuality");
          FillCutFlows("ProbeIP");
          FillCutFlows("ProbeIsolation");
          FillCutFlows("ProbeInvMass");
          FillCutFlows("ProbeCharge");
          FillCutFlows("ProbeDeltaPhi");
          FillCutFlows("ProbeDeltaEta");
            // in this case, fill the 'tag' slot of the probe with the probe - there is no tag for the truth efficiencies.
          probeCont->push_back( new Probe(*probe, *probe) );
          have_probe = true;
      }
  }

  // truth matched muon probes are selected separately (used for truth-closure by isolation and trigger TP)
  else if (m_TruthMatchedProbe){
      have_tag = true;
      for(auto probe : *probes) {

          FillCutFlows("TagCandidates");
          FillCutFlows("TagQuality");
          if (!isTruthMatched(probe)) {
	    continue;
          }
          FillCutFlows("TagPt");
          FillCutFlows("TagEta");
          FillCutFlows("TagTrigger");
          FillCutFlows("TagIP");
          FillCutFlows("TagIsolation");

	  // Here, we only need to apply the kinematic selection
          if (!PassProbeKinematics(probe)) continue;

          FillCutFlows("ProbeKinematics");
          FillCutFlows("ProbeQuality");
          FillCutFlows("ProbeQuality");
          FillCutFlows("ProbeIP");
          FillCutFlows("ProbeIsolation");
          FillCutFlows("ProbeInvMass");
          FillCutFlows("ProbeCharge");
          FillCutFlows("ProbeDeltaPhi");
          FillCutFlows("ProbeDeltaEta");
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
        if (m_selection_tool->getQuality(*tag) > xAOD::Muon::Medium)  continue;

        if (!m_selection_tool->passedIDCuts(*tag)) continue;

        FillCutFlows("TagQuality");
        if(tag->pt() < m_tagPtCut) continue;
        FillCutFlows("TagPt");
        if(fabs(tag->eta()) > m_tagEtaCut) continue;
        FillCutFlows("TagEta");
        if(!this->TagTriggerMatch(tag)) continue;
        FillCutFlows("TagTrigger");

        if (!PassTagIPCuts(tag)) continue;
        FillCutFlows("TagIP");

        if (!PassTagIsoCuts(tag)) continue;
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

          if (!PassProbeIPCuts(probe)) continue;
          FillCutFlows("ProbeIP");
          
          if (!PassProbeIsoCuts(probe)) continue;
          FillCutFlows("ProbeIsolation");
          
          // Cut on tag-probe invariant mass
          double mtp = (tag->p4()+probe->p4()).M();
          if(mtp<m_lowMassWindow || mtp>m_highMassWindow) continue;
          FillCutFlows("ProbeInvMass");
          
	  if(m_doProbeChargeSys)
	  {
	      if(m_ProbeCharge=="positive")
	      {
                  // case of a muon probe
                  const xAOD::Muon* muprobe = dynamic_cast<const xAOD::Muon*>(probe);
                  if (muprobe && (muprobe->trackParticle(xAOD::Muon::Primary)->charge() < 0) ) continue;
                  
		  // case of an ID probe
                  const xAOD::TrackParticle* trkprobe = dynamic_cast<const xAOD::TrackParticle*>(probe);
                  if (trkprobe && (trkprobe->charge() < 0) )  continue;
             }
	      if(m_ProbeCharge=="negative")
	      {
                  // case of a muon probe
                  const xAOD::Muon* muprobe = dynamic_cast<const xAOD::Muon*>(probe);
                  if (muprobe && (muprobe->trackParticle(xAOD::Muon::Primary)->charge() > 0) ) continue;
                  
		  // case of an ID probe
                  const xAOD::TrackParticle* trkprobe = dynamic_cast<const xAOD::TrackParticle*>(probe);
                  if (trkprobe && (trkprobe->charge() > 0) )  continue;
              }
          }

          // Charge cut
          //if(m_oppositeCharge &&
          //	 tag->trackParticle(xAOD::Muon::Primary)->charge()
          int cp = ChargeProd(tag,probe);
          if (cp == 1 && !m_accept_sameCharge) continue;
          if (cp == -1 && !m_accept_oppCharge) continue;
          FillCutFlows("ProbeCharge");    // does this work?
//         if (m_MSProbe && m_accept_oppCharge && m_probe_antiEtConeIso < 0 && !probemu){
//             const xAOD::TrackParticle* mutrk = dynamic_cast <const xAOD::TrackParticle*> (probe);
//             uint8_t nPhiHits = 0;
//             uint8_t nLayers = 0;
//             uint8_t nHoleLayers = 0;
//             uint8_t nPhiHoleLayers = 0;
//             if (mutrk
//                 && mutrk->summaryValue(nPhiHits,xAOD::numberOfPhiLayers)
//                 && mutrk->summaryValue(nLayers,xAOD::numberOfPrecisionLayers)
//                 && mutrk->summaryValue(nHoleLayers,xAOD::numberOfPrecisionHoleLayers)
//                 && mutrk->summaryValue(nPhiHoleLayers,xAOD::numberOfPhiHoleLayers)
//             )
//             ATH_MSG_WARNING("MS probe has "<<(int)nPhiHits<<" phi hits, "<<(int)nLayers<<" precision layers, "<<(int)nHoleLayers<<" precision holes and "<< (int)nPhiHoleLayers<<" Phi holes");
//         }

          // Delta Phi cut
          if (DeltaPhiTP(tag,probe) < m_deltaPhiCut) continue;
          FillCutFlows("ProbeDeltaPhi");

          // Delta Eta cut
          if (fabs(tag->eta()-probe->eta()) < m_deltaEtaCut) continue;
          FillCutFlows("ProbeDeltaEta");

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



bool DiMuonTPSelectionTool::PassTagIsoCuts (const xAOD::Muon* tag) const{
     float tagiso = 0.0;
     bool haveIso = tag->isolation(tagiso,xAOD::Iso::ptcone40);
     if (m_tagPtConeIso > 0 && (!haveIso || tagiso > m_tagPtConeIso * tag->p4().Pt())) return false;
     if (m_tag_antiPtConeIso > 0 && (!haveIso || tagiso < m_tag_antiPtConeIso * tag->pt())) return false;
     bool haveEtIso = tag->isolation(tagiso,xAOD::Iso::topoetcone40);
     if (m_tagEtConeIso > 0 && (!haveEtIso || tagiso > m_tagEtConeIso * tag->p4().Pt())) return false;
     if (m_tag_antiEtConeIso > 0 && (!haveEtIso || tagiso < m_tag_antiEtConeIso * tag->pt())) return false;
     return true;
}
bool DiMuonTPSelectionTool::PassProbeIsoCuts (const xAOD::IParticle* probe) const{
      // probe iso cut
    float probeiso = 0.0;
    float probeEtiso = 0.0;
    bool haveProbeIso = true;
    bool haveProbeEtIso = true;
    const xAOD::Muon* probemu = dynamic_cast<const xAOD::Muon*>(probe);
    if (probemu){
        haveProbeIso = probemu->isolation(probeiso,xAOD::Iso::ptcone40);
        haveProbeEtIso = probemu->isolation(probeEtiso,xAOD::Iso::topoetcone40);
    }
    else if (m_IDProbe || m_MSProbe){
        probeiso = ptcone_trk(probe);
        probeEtiso = etcone_trk(probe);
    //             probeiso = r19_ptcone40(probe);
        haveProbeIso = (probeiso != -1);
        haveProbeEtIso = (probeEtiso != -1);
    }

    if (m_probePtConeIso > 0 && (!haveProbeIso || probeiso > m_probePtConeIso * probe->pt())) return false;
    if (m_probe_antiPtConeIso > 0 && (!haveProbeIso || probeiso < m_probe_antiPtConeIso * probe->pt())) return false;

    if (m_probeEtConeIso > 0 && (!haveProbeEtIso || probeEtiso > m_probeEtConeIso * probe->pt())) return false;
    if (m_probe_antiEtConeIso > 0 && (!haveProbeEtIso || probeEtiso < m_probe_antiEtConeIso * probe->pt())) return false;

    return true;
}

bool DiMuonTPSelectionTool::isTag(const xAOD::Muon* tag, const xAOD::IParticle* probe) const
{
  return (tag->p4().DeltaR(probe->p4()) < 0.01);
}

bool DiMuonTPSelectionTool::PassProbeIPCuts(const xAOD::IParticle* probe) const{

    const xAOD::TrackParticle* tp = 0;
    const xAOD::Muon* muprobe = dynamic_cast<const xAOD::Muon*>(probe);
    if (muprobe) {
        tp = muprobe->primaryTrackParticle();
    }
    else {
        tp = dynamic_cast<const xAOD::TrackParticle*>(probe);
    }
    if (!tp){
        ATH_MSG_WARNING("Unable to find a track particle to cut on!");
    }
    return PassIPCuts(tp,m_probe_d0, m_probe_d0Sign, m_probe_z0);
}
bool DiMuonTPSelectionTool::PassTagIPCuts(const xAOD::Muon* tag) const{

    const xAOD::TrackParticle* tp = tag->primaryTrackParticle();
    if (!tp){
        ATH_MSG_WARNING("Unable to find the tag muon's track particle to cut on!");
    }
    return PassIPCuts(tp,m_tag_d0, m_tag_d0Sign, m_tag_z0);
}

bool DiMuonTPSelectionTool::isRightType(const xAOD::IParticle* probe) const{

    const xAOD::Muon* muprobe = dynamic_cast <const xAOD::Muon*> (probe);

    // if the particle is not a muon, assume that the user knows what TrackParticleContainer he is providing!
    //  This covers the ID and MS probe use cases
    if (! muprobe) return true;

    // otherwise, we have to manually pick the right probe

    // for VeryLoose/Loose/Medium/Tight selection
    if (m_VeryLooseProbe)
    {
        return (muprobe && m_selection_tool->getQuality(*muprobe) <= xAOD::Muon::VeryLoose);
    }
    if (m_LooseProbe)
    {
        return (muprobe && m_selection_tool->getQuality(*muprobe) <= xAOD::Muon::Loose);
    }
    if (m_MediumProbe)
    {
        return (muprobe && m_selection_tool->getQuality(*muprobe) <= xAOD::Muon::Medium);
    }
    if (m_TightProbe)
    {
        return (muprobe && m_selection_tool->getQuality(*muprobe) <= xAOD::Muon::Tight);
    }
    if (m_HighPtProbe)
    {
      return (muprobe && m_selection_tool->passedHighPtCuts(*muprobe));
    }


    // MS probe if we are running on the muon collection
    if (m_MSProbe){
//         static int n_zerophi = 0;
//         uint8_t naux = 0;
//         if (muprobe->trackParticle(xAOD::Muon::MuonSpectrometerTrackParticle)){
//             if (!muprobe->summaryValue(naux, xAOD::numberOfPhiLayers) ||  naux < 1 || !muprobe->summaryValue(naux, xAOD::numberOfPrecisionHoleLayers) || naux > 0 ){
//                 n_zerophi++;
//                 ATH_MSG_WARNING("Found a low phi hit probe. This is number "<<n_zerophi);
//                 return false;
//             }
//         }
        return (muprobe &&  muprobe->trackParticle(xAOD::Muon::ExtrapolatedMuonSpectrometerTrackParticle)!=0);
    }
    // CT Probe
    if (m_CaloProbe){
        return (muprobe &&  muprobe->isAuthor(xAOD::Muon::CaloTag) && m_selection_tool->passedCaloTagQuality(*muprobe));
    }
    // CB probe
    if (m_CBProbe){
        return (muprobe->muonType() == xAOD::Muon::Combined && muprobe->trackParticle(xAOD::Muon::CombinedTrackParticle) != NULL);
    }
    return false;

}

bool DiMuonTPSelectionTool::PassProbeKinematics (const xAOD::IParticle* probe) const{

    if(probe->pt() < m_probePtCut) return false;
    if(fabs(probe->eta()) > m_probeEtaCut) return false;
    if (fabs(probe->eta()) < m_probe_abseta_min) return false;
    if (fabs(probe->eta()) > m_probe_abseta_max) return false;
    if (m_only_A_side && probe->eta() < 0) return false;
    if (m_only_C_side && probe->eta() > 0) return false;
    return true;
}

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/*
 * JPsiMuonTPSelectionTool.cxx
 *
 *  Created on: Sep 01, 2014
 *      Author: Maximiliano Sioli
 */

#include "MuonTPTools/JPsiMuonTPSelectionTool.h"

JPsiMuonTPSelectionTool::JPsiMuonTPSelectionTool(std::string myname)
: AsgTool(myname), MuonTPSelectionTool(myname) {

  declareProperty("UseIDProbes",  m_IDProbe = false);
  declareProperty("UseMSProbes",  m_MSProbe = false);
  declareProperty("UseCBProbes",  m_CBProbe = false);
  declareProperty("UseCaloProbes",  m_CaloProbe = false);

  declareProperty("TagIsoCut",  m_tagIsolation = 0.2);
  declareProperty("ProbeIsoCut",  m_probeIsolation = -1);

  declareProperty("AcceptSameCharge",  m_accept_sameCharge = false);
  declareProperty("AcceptOppCharge",  m_accept_oppCharge = true);

  declareProperty("DeltaPhiCut",      m_deltaPhiCut = -1.00);

  declareProperty("EfficiencyFlag", m_efficiencyFlag = "IDProbes");

  declareProperty("ProbeIDHitCut", m_probe_ID_hits = false);
}

StatusCode JPsiMuonTPSelectionTool::initialize()
{
  // ATH_CHECK(m_muonSelectionTool.retrieve());
  ATH_CHECK(m_selection_tool.retrieve());
  return StatusCode::SUCCESS;
}


//**********************************************************************

ProbeContainer* JPsiMuonTPSelectionTool::selectProbes(const xAOD::MuonContainer* tags, const xAOD::IParticleContainer* probes) const
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

    Root::TAccept tagResult = m_selection_tool->accept(*tag);		// we only use the ID cuts for now

    if (! tagResult.getCutResult("IDHits")) continue;
    // Cut on tag eta, pT
    if(tag->pt() < m_tagPtCut) continue;
    if(fabs(tag->eta()) > m_tagEtaCut) continue;

    // tag iso cut
    float tagiso = 0.;
    bool haveIso = tag->isolation(tagiso,xAOD::Iso::ptcone40);
    if (m_tagIsolation > 0 && (!haveIso || tagiso > m_tagIsolation * tag->p4().Pt())) continue;

    // for each selected tag, loop over probes
    for(auto probe : *probes) {

      // check the probe type
      if (!isRightType(probe)) continue;

      // remove the probe track matched to the tag
      if(isTag(tag, probe)) continue;

      // select good probe tracks,
      // again a selecton ASG duel-tool could be used
      // tba

      // Cut on probe eta, pT
      if(probe->pt() < m_probePtCut) continue;
      if(fabs(probe->eta()) > m_probeEtaCut) continue;

      // ID hits
      xAOD::Muon* probemu = dynamic_cast<xAOD::Muon*> (probe);
      if (m_probe_ID_hits && probemu) {
    	  Root::TAccept  probeResult = m_selection_tool->accept(*probemu);
    	  if (!probeResult.getCutResult("IDHits")) continue;
      }

      // probe iso cut
      // NYI if (m_probeIsolation > 0 && tag->isolation(xAOD::Iso::ptcone40) > m_probeIsolation * probe->p4().Pt());

      // Cut on tag-probe invariant mass
      double mtp = (tag->p4()+probe->p4()).M();
      if(mtp<m_lowMassWindow || mtp>m_highMassWindow) continue;

      // Charge cut
      //if(m_oppositeCharge &&
      //	 tag->trackParticle(xAOD::Muon::Primary)->charge()
      int cp = ChargeProd(tag,probe);
      if (cp == 1 && !m_accept_sameCharge) continue;
      if (cp == -1 && !m_accept_oppCharge) continue;    // does this work?

      // Delta Phi cut
      if (DeltaPhiTP(tag,probe) < m_deltaPhiCut) continue;
      // for each selected probe build a Probe object
      probeCont->push_back( new Probe(*tag, *probe) );
    }
  }

  ATH_MSG_DEBUG("Number of selected probes   : " << probeCont->size() );

  return probeCont;
}
//**********************************************************************

bool JPsiMuonTPSelectionTool::isTag(const xAOD::Muon* tag, const xAOD::IParticle* probe) const
{
  return (tag->p4().DeltaR(probe->p4()) < 0.01);
}

bool JPsiMuonTPSelectionTool::isRightType(const xAOD::IParticle* probe) const{

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
        return (muprobe && muprobe->isAuthor(xAOD::Muon::CaloTag) );
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

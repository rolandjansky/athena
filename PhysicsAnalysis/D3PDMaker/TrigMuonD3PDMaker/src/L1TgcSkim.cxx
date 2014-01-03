/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "GaudiKernel/ISvcLocator.h"
#include "GaudiKernel/ITHistSvc.h"
#include "AthenaKernel/errorcheck.h"
#include "muonEvent/MuonContainer.h"
#include "TrigDecisionTool/TrigDecisionTool.h"

#include "TH1I.h"

#include "L1TgcSkim.h"



L1TgcSkim::L1TgcSkim(const std::string& name,
                     ISvcLocator* pSvcLocator)
  : AthFilterAlgorithm(name, pSvcLocator),
    m_thistSvc("THistSvc", name),
    m_tdt("Trig::TrigDecisionTool/TrigDecisionTool"),
    m_jetFilter(0),
    m_hNEvents(0),
    nEventsProcessed(0),
    nEventsSkipped(0),
    nEventsPreselection(0),
    nEventsTandpMs(0),
    nEventsTandpId(0),
    nJpsiCandidates(0),
    nZCandidates(0)
{
  declareProperty("MuonContainer1", m_muonContainerName1="StacoMuonCollection", "SG key for muon");
  declareProperty("MuonContainer2", m_muonContainerName2="MuidMuonCollection", "SG key for muon");
  declareProperty("TrackParticleContainer", m_trackParticleContainerName="TrackParticleCandidate", "SG key for TrackParticle");
  declareProperty("PreselectionMuonPtCut", m_preselMuonPtCut=-1., "minimum pt required for muon in MeV");
  declareProperty("JpsiTagPtCut", m_jpsiTagPtCut=6000., "minimum pt required for Jpsi tag track in MeV");
  declareProperty("JpsiProbePtCut", m_jpsiProbePtCut=3500., "minimum pt required for Jpsi probe track in MeV");
  declareProperty("ZTagPtCut", m_zTagPtCut=15000., "minimum pt required for Z tag track in MeV");
  declareProperty("ZProbePtCut", m_zProbePtCut=10000., "minimum pt required for Z probe track in MeV");
  declareProperty("JpsiMassLowerLimit", m_jpsiMassLowerLimit=2600., "minimum mass required for J/psi in MeV");
  declareProperty("JpsiMassUpperLimit", m_jpsiMassUpperLimit=3600., "maximum mass required for J/psi in MeV");
  declareProperty("ZMassLowerLimit", m_zMassLowerLimit=50000., "minimum mass required for Z in MeV");
  declareProperty("ZMassUpperLimit", m_zMassUpperLimit=14000000., "maximum mass required for Z in MeV");
  declareProperty("HasOppositeCharge", m_hasOppositeCharge=true, "opposite charge requirement for tag-and-probe pair");
  declareProperty("SingleMuon", m_singleMuon=false, "set it to true for single muon analysis");
  declareProperty("DumpAll", m_dumpAll=false, "set it to true for dump all the events");
  declareProperty("PreScaleTandp", m_prescaleTandp=0, "pre scale factor for events with Jpsi/Z candidate");
  declareProperty("PreScaleNotTandp", m_prescaleNotTandp=0, "pre scale factor for events without Jpsi/Z candidate");
  declareProperty("JetFilterPattern", m_jetFilterPattern="", "regular expression for jet trigger");
  declareProperty("PassThroughFilterPattern", m_passThroughFilterPattern= "", "regular expression for pass through trigger pattern");
}



StatusCode
L1TgcSkim::initialize()
{
  CHECK(m_thistSvc.retrieve());
  m_hNEvents = new TH1I("hNEvents", "hNEvents", 10, -0.5, 9.5);
  CHECK(m_thistSvc->regHist("/D3PD/hNEvents", m_hNEvents));
  CHECK(m_tdt.retrieve());

  nEventsProcessed = 0;
  nEventsSkipped = 0;
  nEventsPreselection = 0;
  nEventsTandpMs = 0;
  nEventsTandpId = 0;
  nJpsiCandidates = 0;
  nZCandidates = 0;

  m_jetFilter = false;
  if (not m_jetFilterPattern.empty()) m_jetFilter = true;

  return StatusCode::SUCCESS;
}



StatusCode
L1TgcSkim::execute()
{
  nEventsProcessed++;

  if (m_dumpAll) {
    setFilterPassed(true);
    return StatusCode::SUCCESS;
  }

  if (m_tdt->isPassed(m_passThroughFilterPattern)) {
    setFilterPassed(true);
    return StatusCode::SUCCESS;
  }
  
  if (m_singleMuon and (not m_tdt->isPassed(m_jetFilterPattern))) {
    setFilterPassed(false);
    nEventsSkipped++;
    return StatusCode::SUCCESS;
  }

  bool passedPreSel1 = preSelection(m_muonContainerName1);
  bool passedPreSel2 = preSelection(m_muonContainerName2);
  if (m_singleMuon and ((not passedPreSel1) and (not passedPreSel2))) {
    setFilterPassed(false);
    nEventsSkipped++;
    return StatusCode::SUCCESS;
  }

  nEventsPreselection++;

  if (m_singleMuon) {
    if ((nEventsProcessed % m_prescaleNotTandp) == 1) {
      setFilterPassed(true);
    } else {
      setFilterPassed(false);
      nEventsSkipped++;
    }
    return StatusCode::SUCCESS;
  }


  bool tandpMs1 = tagAndProbeMs(m_muonContainerName1);
  bool tandpMs2 = tagAndProbeMs(m_muonContainerName2);

  if (tandpMs1 or tandpMs2) {
    nEventsTandpMs++;
    setFilterPassed(true);
    return StatusCode::SUCCESS;
  }

  bool tandpId1 = tagAndProbeId(m_muonContainerName1);
  bool tandpId2 = tagAndProbeId(m_muonContainerName2);

  if (tandpId1 or tandpId2) {
    nEventsTandpId++;
    setFilterPassed(true);
    return StatusCode::SUCCESS;
  }

  if (m_prescaleNotTandp > 0) {
    if ((nEventsProcessed % m_prescaleNotTandp) == 1) {
      setFilterPassed(true);
      return StatusCode::SUCCESS;
    }
  }

  setFilterPassed(false);
  nEventsSkipped++;
  return StatusCode::SUCCESS;
}



StatusCode
L1TgcSkim::finalize()
{

  ATH_MSG_INFO("nEventsProcessed    = " << nEventsProcessed << " events");
  ATH_MSG_INFO("nEventsPreselection = " << nEventsPreselection << " events");
  ATH_MSG_INFO("nEventsTandpMs      = " << nEventsTandpMs << " events");
  ATH_MSG_INFO("nEventsTandpId      = " << nEventsTandpId << " events");

  TAxis *axis = (TAxis*)m_hNEvents->GetXaxis();
  axis->SetBinLabel(1, "nEventsProcessed");
  axis->SetBinLabel(2, "nEventsSkipped");
  axis->SetBinLabel(3, "nEventsPreselection");
  axis->SetBinLabel(4, "nEventsTandpMs");
  axis->SetBinLabel(5, "nEventsTandpId");
  axis->SetBinLabel(6, "nJpsiCandidates");
  axis->SetBinLabel(7, "nZCandidates");

  m_hNEvents->SetBinContent(1, nEventsProcessed);
  m_hNEvents->SetBinContent(2, nEventsSkipped);
  m_hNEvents->SetBinContent(3, nEventsPreselection);
  m_hNEvents->SetBinContent(4, nEventsTandpMs);
  m_hNEvents->SetBinContent(5, nEventsTandpId);
  m_hNEvents->SetBinContent(6, nJpsiCandidates);
  m_hNEvents->SetBinContent(7, nZCandidates);

  return StatusCode::SUCCESS;
}



bool
L1TgcSkim::preSelection(const std::string& key)
{
  const Analysis::MuonContainer* muons = 0;
  CHECK(evtStore()->retrieve(muons, key));

  bool hasCombinedMuon = false;
  bool rc = false;
  int nMuons = 0;
  for (Analysis::MuonContainer::const_iterator cit = muons->begin();
       cit != muons->end(); cit++) {
    const Analysis::Muon *muon = *cit;
    if (not muon->hasInDetTrackParticle()) continue;
    if (muon->pt() > m_preselMuonPtCut) {
      nMuons++;
      if (muon->hasCombinedMuon()) hasCombinedMuon = true;
    }
  }

  if ((nMuons > 1) or hasCombinedMuon) rc = true;
  return rc;
}



bool
L1TgcSkim::tagAndProbeMs(const std::string& key)
{
  const Analysis::MuonContainer* muons = 0;
  CHECK(evtStore()->retrieve(muons, key));

  for (size_t ii = 0; ii < muons->size(); ii++) {
    const Analysis::Muon *tag = muons->at(ii);
    if (not tag->hasCombinedMuonTrackParticle()) continue;
    if (not isGoodIdTrack(tag->inDetTrackParticle())) continue;

    for (size_t jj = ii; jj < muons->size(); jj++) {
      const Analysis::Muon *probe = muons->at(jj);
      if (not probe->hasInDetTrackParticle()) continue;

      const Rec::TrackParticle *tp = 0;
      if (probe->hasCombinedMuonTrackParticle()) {
        tp = probe->combinedMuonTrackParticle();
      } else if (probe->hasInnerExtrapolatedTrackParticle()) {
        tp = probe->innerExtrapolatedTrackParticle();
      } else {
        tp = probe->inDetTrackParticle();
      }

      if (not isGoodIdTrack(probe->inDetTrackParticle())) continue;
      if (not isCandidate(tag->combinedMuonTrackParticle(), tp)) continue;
      return true;
    }
  }

  return false;
}



bool
L1TgcSkim::tagAndProbeId(const std::string& key)
{
  const Analysis::MuonContainer* muons = 0;
  CHECK(evtStore()->retrieve(muons, key));

  std::map<const Rec::TrackParticle*, const Rec::TrackParticle*> isCombined;
  for (Analysis::MuonContainer::const_iterator cit = muons->begin();
      cit != muons->end(); cit++) {
    const Analysis::Muon *muon = *cit;
    if (not muon->hasCombinedMuonTrackParticle()) continue;
    isCombined.insert(std::pair<const Rec::TrackParticle*, const Rec::TrackParticle*>(muon->inDetTrackParticle(),
                                                                                      muon->combinedMuonTrackParticle()));
  }

  const Rec::TrackParticleContainer* tracks = 0;
  CHECK(evtStore()->retrieve(tracks, m_trackParticleContainerName));

  for (Rec::TrackParticleContainer::const_iterator tag = tracks->begin();
       tag != tracks->end(); tag++) {
    std::map<const Rec::TrackParticle*, const Rec::TrackParticle*>::const_iterator cit = isCombined.find(*tag);
    if (cit == isCombined.end()) continue;
    if (not isGoodIdTrack(*tag)) continue;
    for (Rec::TrackParticleContainer::const_iterator probe = tracks->begin();
         probe != tracks->end(); probe++) {
      if (*tag == *probe) continue;
      if (not isGoodIdTrack(*probe)) continue;
      if (not isCandidate(cit->second, *probe, m_prescaleTandp)) continue; // (combined, id) pair
      return true;
    }
  }

  return false;
}



bool
L1TgcSkim::isGoodIdTrack(const Rec::TrackParticle* tp) const
{
  const Trk::TrackSummary* summary = tp->trackSummary();
  if ((summary->get(Trk::expectBLayerHit) == 0) and (summary->get(Trk::numberOfBLayerHits) < 1)) return false;
  if ((summary->get(Trk::numberOfPixelHits) + summary->get(Trk::numberOfPixelDeadSensors)) < 2) return false;
  if ((summary->get(Trk::numberOfSCTHits) + summary->get(Trk::numberOfSCTDeadSensors)) < 6) return false;
  if ((summary->get(Trk::numberOfPixelHoles) + summary->get(Trk::numberOfSCTHoles)) > 2) return false;
  const int nTrtOutliers = summary->get(Trk::numberOfTRTOutliers);
  const int nTrtHits = summary->get(Trk::numberOfTRTHits) + nTrtOutliers;

  double fraction = 1.;
  if (nTrtHits != 0) fraction = (double)nTrtOutliers/(double)nTrtHits;

  if (fabs(tp->eta()) < 1.9) {
    if (nTrtHits < 6) return false;
    if (fraction >= 0.9) return false;
  } else {
    if (nTrtHits > 5) {
      if (fraction >= 0.9) return false;
    }
  }

  return true;
}



bool
L1TgcSkim::isCandidate(const Rec::TrackParticle* tag,
                       const Rec::TrackParticle* probe,
                       const int prescale) const
{
  if (m_hasOppositeCharge) {
    if (tag->charge() * probe->charge() > 0.) return false;
  }

  const double mass = (tag->hlv() + probe->hlv()).m();
  bool inJpsiWindow = ((m_jpsiMassLowerLimit < mass) and (mass < m_jpsiMassUpperLimit));
  bool inZWindow = ((m_zMassLowerLimit < mass) and (mass < m_zMassUpperLimit));

  if (inJpsiWindow) {
    if (tag->pt() < m_jpsiTagPtCut) inJpsiWindow=false;
    if (probe->pt() < m_jpsiProbePtCut) inJpsiWindow=false;
  }

  if (inZWindow) {
    if (tag->pt() < m_zTagPtCut) inZWindow=false;
    if (probe->pt() < m_zProbePtCut) inZWindow=false;
  }

  if (prescale and inJpsiWindow) nJpsiCandidates++;
  if (prescale and inZWindow) nZCandidates++;

  bool rc = false;
  if (inJpsiWindow or inZWindow) {
    if (not prescale) {
      rc = true;
    } else {
      if (((nJpsiCandidates + nZCandidates) % prescale) == 1) {
        rc = true;
      }
    }
  }

  return rc;
}
/* eof */

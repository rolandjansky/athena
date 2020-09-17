/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// This filter can be used to select events at generator level which
// have a lepton AND/OR missing pT.
//
// For the purpose of this filter a "lepton" is defined as an electron
// or muon, with pT and eta within certain user-defined
// thresholds. (The thresholds are independently set for electrons and
// for muons.) In addition, the missing pT in the event is calculated
// from non-interacting particles as well as particles that are lost
// to the limited detector acceptance, and has to be above a
// user-defined pT threshold value.
//
// The filter will accept events which have at least one lepton above
// threshold AND/OR missing pT above threshold. The AND/OR choice is
// also made by the user, via the job options file.
//
// The events kept by the filter can be dumped onto an ntuple.
//
// IMPORTANT NOTE: at the moment the missing pT calculation treats the
// Higgs (pdg-id=25) as a stable non-interacting particle. THIS MAY
// LIMIT THE APPLICABILITY OF THIS FILTER TO SOME CHANNELS, e.g. which
// have a Higgs boson which decays in the detector volume.
#include "GeneratorFilters/PtmissAndOrLeptonFilter.h"

#include "GaudiKernel/SystemOfUnits.h"

#include "EventInfo/EventInfo.h"
#include "EventInfo/EventID.h"
#include "EventInfo/EventType.h"

PtmissAndOrLeptonFilter::PtmissAndOrLeptonFilter(const std::string& name, ISvcLocator* pSvcLocator)
  : GenFilter(name, pSvcLocator)
{
  declareProperty("PtmissANDLepton",m_PtmissANDLepton = true);
  declareProperty("PtminElectron",m_PtminElectron = 12.5*Gaudi::Units::GeV);
  declareProperty("MaxEtaElectron",m_MaxEtaElectron = 2.7);
  declareProperty("PtminMuon",m_PtminMuon = 12.5*Gaudi::Units::GeV);
  declareProperty("MaxEtaMuon",m_MaxEtaMuon = 3.2);
  declareProperty("PtminLostTrack",m_PtminLostTrack = 5000.);
  declareProperty("MinEtaLost",m_MinEtaLost = 5.0);
  declareProperty("PtminMissing",m_PtminMissing = 100000.);
}


StatusCode PtmissAndOrLeptonFilter::filterInitialize() {
  if (m_PtmissANDLepton) ATH_MSG_WARNING("Filtering events with BOTH Missing Pt AND a lepton.");
  else ATH_MSG_WARNING("Filtering events with EITHER Missing Pt OR a lepton.");

  ATH_MSG_INFO("Minimum Pt for electrons = " << m_PtminElectron << " CLHEP::MeV");
  ATH_MSG_INFO("Maximum eta for electrons to be counted = " << m_MaxEtaElectron);
  ATH_MSG_INFO("Minimum Pt for muons = " << m_PtminMuon << " CLHEP::MeV");
  ATH_MSG_INFO("Maximum eta for muons to be counted = " << m_MaxEtaMuon);
  ATH_MSG_INFO("Minimum Pt for significant lost tracks = " << m_PtminLostTrack << " CLHEP::MeV");
  ATH_MSG_INFO("Minimum eta for lost tracks = " << m_MinEtaLost);
  ATH_MSG_INFO("Minimum missing Pt = " << m_PtminMissing << " CLHEP::MeV");
  return StatusCode::SUCCESS;
}


StatusCode PtmissAndOrLeptonFilter::filterFinalize() {
  ATH_MSG_INFO(" # events failing filter selection: " << m_nFail);
  return StatusCode::SUCCESS;
}


StatusCode PtmissAndOrLeptonFilter::filterEvent() {
  McEventCollection::const_iterator itr;

  double px = 0.;
  double py = 0.;
  double sumPxLost = 0.;
  double sumPyLost = 0.;
  double sumPxInvis = 0.;
  double sumPyInvis = 0.;
  double pxsum = 0.;
  double pysum = 0.;
  double ptmiss = 0.;
  int ninvis = 0;
  int nelec = 0;
  int nmuon = 0;
  int nleptons = 0;
  int nlost = 0;

  // Loop over all events in this event: event 0 = hard interaction; events 1,2,3... = pile-up
  for (itr = events()->begin(); itr != events()->end(); ++itr) {
    const HepMC::GenEvent* genEvt = (*itr);
    for (auto part: *genEvt){
      if (part->status() != 1) continue;
      const int abspid = std::abs(part->pdg_id());
      const double abseta = std::abs(part->momentum().pseudoRapidity());
      if (abspid == 11 && part->momentum().perp() > m_PtminElectron && abseta <= m_MaxEtaElectron) nelec++;
      if (abspid == 13 && part->momentum().perp() > m_PtminMuon     && abseta <= m_MaxEtaMuon) nmuon++;

      nleptons = nelec + nmuon;
      // Check that the number of leptons does not exceed the maximum given in PtmissAndOrLeptonFilter.h for ntuple
      if (nleptons > 19) ATH_MSG_ERROR(" # leptons found = " << nleptons << " Exceeds maximum allowed.");

      // calculate ptmiss by summing over px and py for all
      // stable and invisible particles:
      // neutrinos, neutralinos and stable Higgs,

      if (abspid == 12 || abspid == 14 || abspid == 16 || abspid == 25 || abspid == 1000022) {
        ATH_MSG_DEBUG(" found an invisible particle: " << part->pdg_id());
        px = part->momentum().x();
        py = part->momentum().y();
        sumPxInvis += px;
        sumPyInvis += py;
        ninvis++;
        // Check that the number of invisible particles does not exceed the maximum given in header file *.h for ntuple
        if (ninvis > 19) ATH_MSG_ERROR(" # invisible particles found = " << ninvis << " Exceeds maximum allowed.");
      }

      // Treat particles with significant pt lost outside the acceptance region separately
      if (abseta > m_MinEtaLost && part->momentum().perp() >= m_PtminLostTrack) {
        nlost++;
        // Check that the number of lost particles does not exceed the maximum given in PtmissAndOrLeptonFilter.h for ntuple
        if (nlost > 19) ATH_MSG_ERROR(" # lost particles found = " << nlost << " Exceeds maximum allowed.");

        // Now sum over px and py for all lost particles
        px = part->momentum().x();
        py = part->momentum().y();
        sumPxLost += px;
        sumPyLost += py;
      }
    } // end loop on all particles in the event (pitr)

    // Retrieve event info
    const EventInfo* eventInfo;
    CHECK(evtStore()->retrieve(eventInfo));
  } // end loop on events

  pxsum = sumPxInvis + sumPxLost;
  pysum = sumPyInvis + sumPyLost;
  ptmiss = std::sqrt(pxsum*pxsum + pysum*pysum);

  // Finally, make filter decision for this event
  if (m_PtmissANDLepton) setFilterPassed(nleptons > 0 && ptmiss > m_PtminMissing);
  else setFilterPassed(nleptons > 0 || ptmiss > m_PtminMissing);
  return StatusCode::SUCCESS;
}

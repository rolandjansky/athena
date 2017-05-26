/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "xAODTau/TauJetContainer.h"
#include "TrigTauMatching_example.h"

const std::string tauTrigger = "HLT_tau125_medium1_tracktwo";
const std::string diTauTrigger = "HLT_tau35_medium1_tracktwo_tau25_medium1_tracktwo";

TrigTauMatching_example::TrigTauMatching_example(const std::string &name,
                                                 ISvcLocator *svcLoc)
  : AthAlgorithm(name, svcLoc),
    m_eventCount(0),
    m_tauEvents(0, 0),
    m_diTauEvents(0, 0),
    m_trigDecTool("TrigDecisionTool"),
    m_matchTool("TrigTauMatchingTool")
{
  declareProperty("TrigDecisionTool", m_trigDecTool);
  declareProperty("TrigTauMatchingTool", m_matchTool);
}

StatusCode TrigTauMatching_example::initialize()
{
  //ATH_CHECK(m_trigDecTool.retrieve());
  ATH_CHECK(m_matchTool.retrieve());
  return StatusCode::SUCCESS;
}

StatusCode TrigTauMatching_example::execute()
{

  bool tauTriggered = false;
  bool diTauTriggered = false;
  int tauMatches = 0;
  int diTauMatches = 0;

  const xAOD::TauJetContainer* taus = 0;
  ATH_CHECK(evtStore()->retrieve(taus, "TauJets"));

  m_eventCount++;

  tauTriggered = m_trigDecTool->isPassed(tauTrigger);
  diTauTriggered = m_trigDecTool->isPassed(diTauTrigger);

  for (auto it: *taus)
  {
    // Cut on tau pT >= 25 GeV, |eta| <= 2.5, and medium ID
    if (   it->pt() < 25000
        || std::abs(it->eta()) > 2.5
        || !it->isTau(xAOD::TauJetParameters::JetBDTSigMedium))
    {
      continue;
    }


    if (m_matchTool->match(it, tauTrigger))
    {
      tauMatches++;
    }

    if (m_matchTool->match(it, diTauTrigger))
    {
      diTauMatches++;
    }
  }

  if (tauTriggered)
  {
    m_tauEvents.first++;
    m_tauEvents.second += (tauMatches > 0);
    std::cout << "[" << m_eventCount << "] " << tauMatches
        << " taus matched with " << tauTrigger << std::endl;
  }

  if (diTauTriggered)
  {
    m_diTauEvents.first++;
    m_diTauEvents.second += (diTauMatches > 1);
    std::cout << "[" << m_eventCount << "] " << diTauMatches
        << " taus matched with " << diTauTrigger << std::endl;
  }

  return StatusCode::SUCCESS;
}

StatusCode TrigTauMatching_example::finalize()
{
  std::cout << m_tauEvents.second << "/" << m_tauEvents.first
      << " events for " << tauTrigger << std::endl;
  std::cout << m_diTauEvents.second << "/" << m_diTauEvents.first
      << " events for " << diTauTrigger << std::endl;
  return StatusCode::SUCCESS;
}


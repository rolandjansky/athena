/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "TrigTauMatching/TrigTauMatching.h"

#include "xAODTrigger/EmTauRoIContainer.h"

const std::string Trig::TrigTauMatchingTool::
    kDefaultHLTLabel = "TrigTauRecMerged";

const std::string Trig::TrigTauMatchingTool::
    kDefaultL1Label = "LVL1EmTauRoIs";

Trig::TrigTauMatchingTool::TrigTauMatchingTool(const std::string &name) :
    asg::AsgTool(name),
    m_trigDecTool("TrigDecisionTool"),
    m_hltLabel(kDefaultHLTLabel),
    m_l1Label(kDefaultL1Label),
    m_allowResurrectedDecision(false)
{
  declareProperty("TrigDecisionTool", m_trigDecTool,
                  "Trigger decision tool for the trigger navigagtion");
  declareProperty("HLTLabel", m_hltLabel,
                  "Label of the HLT tau jet container to use for matching");
  declareProperty("L1Label", m_l1Label,
                  "Label of the L1 EmTau container to use for matching");
  declareProperty("AllowResurrectedDecision", m_allowResurrectedDecision,
                  "Allow use of resurrected decision when retrieving "
                  "trigger decisions and features.");
}

StatusCode Trig::TrigTauMatchingTool::initialize()
{
  ATH_MSG_INFO("Initialising TrigTauMatchingTool");

  ATH_CHECK(m_trigDecTool.retrieve());

  return StatusCode::SUCCESS;
}

bool Trig::TrigTauMatchingTool::match(const xAOD::TauJet *p,
                                      const std::string &trigger,
                                      double deltaRCut)
{
  return getHLT(p, trigger, deltaRCut) != 0;
}

bool Trig::TrigTauMatchingTool::match(const xAOD::TauJet *p,
                                      const std::string &trigger,
                                      const xAOD::TauJetContainer *c,
                                      double deltaRCut)
{
  if (m_trigDecTool->isPassed(trigger, getCondition()))
  {
    for (auto it : *c)
    {
      double dR = it->p4().DeltaR(p->p4());
      if (dR < deltaRCut)
      {
        return true;
      }
    }
  }
  return false;
}

bool Trig::TrigTauMatchingTool::matchL1(const xAOD::TauJet *p,
                                        const std::string &trigger,
                                        const xAOD::EmTauRoIContainer *c,
                                        double deltaRCut)
{
  if (m_trigDecTool->isPassed(trigger, getCondition()))
  {
    if (c)
    {
      for (const xAOD::EmTauRoI* it : *c)
      {
        if (it->roiType() == xAOD::EmTauRoI::TauRoIWord)
        {
          TLorentzVector l1;
          double dR;
          l1.SetPtEtaPhiM(it->tauClus(), it->eta(), it->phi(), 1.0);
          dR = l1.DeltaR(p->p4());

          if (dR < deltaRCut)
          {
            return true;
          }
        }
      }
    }
  }

  return false;
}

const xAOD::TauJet *
Trig::TrigTauMatchingTool::getHLT(const xAOD::TauJet *p,
                                  const std::string &trigger,
                                  double deltaRCut)
{
  if (m_trigDecTool->isPassed(trigger, getCondition()))
  {
    auto features = m_trigDecTool->features(trigger, getCondition());
    decltype(features.containerFeature<xAOD::TauJetContainer>()) tauFeatures;

    if (!m_hltLabel.empty())
    {
      tauFeatures = features.containerFeature<xAOD::TauJetContainer>(
          m_hltLabel);
    }
    else
    {
      tauFeatures = features.containerFeature<xAOD::TauJetContainer>();
    }

    if (tauFeatures.size() == 0)
    {
      ATH_MSG_WARNING("Trigger " << trigger
                      << " has no features of type xAOD::TauJetContainer");
    }

    for (auto &tauContainer : tauFeatures)
    {
      // Access the tau jet container stored in the container feature
      if (tauContainer.cptr())
      {
        for (auto itTauHLT : *tauContainer.cptr())
        {
          double dR = itTauHLT->p4().DeltaR(p->p4());
          if (dR < deltaRCut)
          {
            return itTauHLT;
          }
        }
      }
    }
  }
  return 0;
}

const xAOD::EmTauRoI *
Trig::TrigTauMatchingTool::getL1FromHLT(const xAOD::TauJet *p,
                                        const std::string &trigger,
                                        const xAOD::EmTauRoIContainer *c,
                                        double deltaRCut)
{
  const xAOD::TauJet *hltTau = getHLT(p, trigger, deltaRCut);
  if (hltTau)
  {
    for (auto it : *c)
    {
      if (hltTau->ROIWord() == it->roiWord())
      {
        return it;
      }
    }
  }
  return 0;
}

unsigned int
Trig::TrigTauMatchingTool::getCondition()
{
  if (m_allowResurrectedDecision)
  {
    return TrigDefs::Physics | TrigDefs::allowResurrectedDecision;
  }
  else
  {
    return TrigDefs::Physics;
  }
}

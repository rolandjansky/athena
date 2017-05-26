/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRIGTAUMATCHING_TRIGTAUMATCHING_H
#define TRIGTAUMATCHING_TRIGTAUMATCHING_H


#include "TrigTauMatching/ITrigTauMatching.h"

#include <AsgTools/AsgTool.h>
#include <AsgTools/ToolHandle.h>

#include <TrigDecisionTool/TrigDecisionTool.h>

namespace Trig
{
  class TrigTauMatchingTool :
      virtual public ITrigTauMatchingTool,
      virtual public asg::AsgTool
  {
    /// Proper constructor for Athena
    ASG_TOOL_CLASS(TrigTauMatchingTool, ITrigTauMatchingTool)

  public:
    /// Standard constructor for standalone usage
    TrigTauMatchingTool(const std::string &name);

    virtual ~TrigTauMatchingTool() {};

    virtual StatusCode initialize(void);

    // Check if an HLT trigger is passed, and match a tau to a HLT tau
    // associated with the chain within a cone of dR < deltaRCut
    virtual bool match(const xAOD::TauJet *p,
                       const std::string &trigger,
                       double deltaRCut = 0.2);

    // Check if a trigger is passed and match a reco tau to an HLT tau in
    // a given container within a cone of dR < deltaRCut
    virtual bool match(const xAOD::TauJet *p,
                       const std::string &trigger,
                       const xAOD::TauJetContainer *c,
                       double deltaRCut = 0.2);

    // Check if an L1 trigger is passed, and match a tau to a L1 EmTau (with
    // TauRoIWord) within a cone of dR < deltaRCut
    virtual bool matchL1(const xAOD::TauJet *p,
                         const std::string &trigger,
                         const xAOD::EmTauRoIContainer *c,
                         double deltaRCut = 0.3);

    // Check if an HLT trigger is passed, and return the associated HLT
    // TauJet matching the given tau.
    virtual const xAOD::TauJet *
        getHLT(const xAOD::TauJet *p,
               const std::string &trigger,
               double deltaRCut = 0.2);

    // Check if an HLT trigger is passed, get the HLT associated HLT tau
    // matching the given tau, and return the EmTauRoI associated with
    // it.
    virtual const xAOD::EmTauRoI *
        getL1FromHLT(const xAOD::TauJet *p,
                     const std::string &trigger,
                     const xAOD::EmTauRoIContainer *c,
                     double deltaRCut = 0.3);

    // Get the trigger condition that the tool currently uses
    virtual unsigned int getCondition();
  private:
    ToolHandle<Trig::TrigDecisionTool> m_trigDecTool;
    std::string m_hltLabel;
    std::string m_l1Label;
    bool m_allowResurrectedDecision;

  private:
    static const std::string kDefaultHLTLabel;
    static const std::string kDefaultL1Label;
  };
}

#endif

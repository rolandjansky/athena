/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRIGTAUMATCHING_ITRIGTAUMATCHING_H
#define TRIGTAUMATCHING_ITRIGTAUMATCHING_H

#include "AsgTools/IAsgTool.h"

#include "xAODTau/TauJetContainer.h"
#include "xAODTrigger/EmTauRoIContainer.h"

namespace Trig
{
  class ITrigTauMatchingTool : virtual public asg::IAsgTool
  {
    // Declare the interface that the class provides 
    ASG_TOOL_INTERFACE(ITrigTauMatchingTool)

  public:
    virtual ~ITrigTauMatchingTool() {};

    // Check if an HLT trigger is passed, and match a tau to a HLT tau
    // associated with the chain within a cone of dR < deltaRCut
    virtual bool match(const xAOD::TauJet *p,
                       const std::string &trigger,
                       double deltaRCut = 0.2) = 0;

    // Check if a trigger is passed and match a reco tau to an HLT tau in
    // a given container within a cone of dR < deltaRCut
    virtual bool match(const xAOD::TauJet *p,
                       const std::string &trigger,
                       const xAOD::TauJetContainer *c,
                       double deltaRCut = 0.2) = 0;

    // Check if an L1 trigger is passed, and match a tau to a L1 EmTau (with
    // TauRoIWord) within a cone of dR < deltaRCut
    virtual bool matchL1(const xAOD::TauJet *p,
                         const std::string &trigger,
                         const xAOD::EmTauRoIContainer *c,
                         double deltaRCut = 0.3) = 0;

    // Check if an HLT trigger is passed, and return the associated HLT
    // TauJet matching the given tau.
    virtual const xAOD::TauJet *
        getHLT(const xAOD::TauJet *p,
               const std::string &trigger,
               double deltaRCut = 0.2) = 0;

    // Check if an HLT trigger is passed, get the HLT associated HLT tau
    // matching the given tau, and return the EmTauRoI associated with
    // it.
    virtual const xAOD::EmTauRoI *
        getL1FromHLT(const xAOD::TauJet *p,
                     const std::string &trigger,
                     const xAOD::EmTauRoIContainer *c,
                     double deltaRCut = 0.3) = 0;

    // Get the trigger condition that the tool currently uses
    virtual unsigned int getCondition() = 0;
  };
}

#endif

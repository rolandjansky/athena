// Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef PMGTOOLS_IPMGSHERPAVJETSSYSTOOL_H
#define PMGTOOLS_IPMGSHERPAVJETSSYSTOOL_H

// Infrastructure include(s):
#include "AsgTools/IAsgTool.h"

// Local include(s):
#include "PATInterfaces/CorrectionCode.h"

namespace PMGTools {

  // Define parametrization type (i.e. reference process)
  enum SysParType {
    None = 0,
    Znunu,
    Zll,
    Wlnu,
    GammaJets
  };

  class IPMGSherpaVjetsSysTool : public virtual asg::IAsgTool { 

    // Declare the interface that the class provides
    ASG_TOOL_INTERFACE( PMGTools::IPMGSherpaVjetsSysTool )

  public:

    /// Return weight for a given systematic variation
    virtual double getWeight( const std::string& sysVariation = "nom") = 0;

    /// Some setters to speed up the calculation for multiple variations/events
    virtual void setNjets(int njets) = 0;
    virtual void cacheDSID(bool do_it=true) = 0;

    /// Helpers to get the list of supported variations for Z/W+jets and gamma+jets
    virtual std::vector<std::string> getSupportedVariations(SysParType ptype) = 0;

    /// get number of truth jets as defined in the parametrization analysis
    virtual unsigned int getNtruthJets() = 0;  

  }; // class IPMGSherpaVjetsSysTool

} // namespace PMGTools

#endif //> !PMGTOOLS_IPMGSHERPAVJETSSYSTOOL_H

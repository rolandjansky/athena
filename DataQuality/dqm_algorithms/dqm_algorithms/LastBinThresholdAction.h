/*
Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/

#ifndef DQM_ALGORITHMS_LASTBINTHRESHOLDACTION_H
#define DQM_ALGORITHMS_LASTBINTHRESHOLDACTION_H

#include <dqm_core/Algorithm.h>
#include <string>
#include <iosfwd>

namespace dqm_algorithms {

  class LessThan {
  public:
    bool operator() (double a, double b, double /*error = 0.*/) const {
      return a < b;
    }
  };

  class GreaterThan {
  public:
    bool operator() (double a, double b, double /*error = 0.*/) const {
      return a > b;
    }
  };

  class DifferentThan {
  public:
    bool operator() (double a, double b, double error = 0.) const {
      return std::abs(a - b) > error;
    }
  };

  class NoAction {
  public:
    void operator() (const std::string&, std::string&, double, double) const {}
  };

  class TileDQAction {
  public:
    void operator() (const std::string& histogramName, std::string action,
                     double averageBinContent, double lastBinContent) const;
  };

  template<class Exceed, class Action>
  class LastBinThresholdAction : public dqm_core::Algorithm  {
  public:
    LastBinThresholdAction(const std::string & name);

    // Overwrites virtual functions
    virtual LastBinThresholdAction* clone( ) override;
    virtual dqm_core::Result* execute( const std::string& , const TObject& , const dqm_core::AlgorithmConfig& ) override;
    using dqm_core::Algorithm::printDescription;
    virtual void printDescription(std::ostream& out) const;

  private:
    std::string m_name;
    Exceed m_exceeds;
    Action m_doAction;
  };
}

#endif // DQM_ALGORITHMS_LASTBINTHRESHOLDACTION_H

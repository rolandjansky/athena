/*
Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#ifndef DQM_ALGORITHMS_LASTBINTHRESHOLDACTION_H
#define DQM_ALGORITHMS_LASTBINTHRESHOLDACTION_H

#include <dqm_core/Algorithm.h>

namespace dqm_algorithms {

  struct LessThan {
    bool operator() (double a, double b, double /*error = 0.*/) const {
      return a < b;
    }
  };

    struct GreaterThan {
    bool operator() (double a, double b, double /*error = 0.*/) const {
      return a > b;
    }
  };

  struct DifferentThan {
    bool operator() (double a, double b, double error = 0.) const {
      return std::abs(a - b) > error;
    }
  };

  struct NoAction {
    void operator() (const std::string&, std::string&, double, double) const {}
  };

  struct TileDQAction {
    void operator() (const std::string& histogramName, std::string action,
                     double averageBinContent, double lastBinContent) const;
  };

  template<class Exceed, class Action>
  struct LastBinThresholdAction : public dqm_core::Algorithm  {
    LastBinThresholdAction(const std::string & name);

    // Overwrites virtual functions
    virtual LastBinThresholdAction* clone( ) override;
    virtual dqm_core::Result* execute( const std::string& , const TObject& , const dqm_core::AlgorithmConfig& ) override;
    using dqm_core::Algorithm::printDescription;
    virtual void printDescription(std::ostream& out);

  private:
    std::string m_name;
    Exceed m_exceeds;
    Action m_doAction;
  };
}

#endif // DQM_ALGORITHMS_LASTBINTHRESHOLDACTION_H

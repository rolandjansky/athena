// Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/


#ifndef TRIGGERMATCHINGTOOL_MATCHINGIMPLEMENTATION_H
#define TRIGGERMATCHINGTOOL_MATCHINGIMPLEMENTATION_H

#include <map>

#include "TriggerMatchingTool/MatchingTool.h"
#include "TriggerMatchingTool/IAssociationStrategy.h"
#include "AsgMessaging/AsgMessaging.h"

namespace Trig {


class TrigDecisionTool;


class MatchingTool;

namespace MatchingStrategy {
    enum Strategy {
      MinimalSum = 0,
      MaximalMatched = 1
    };
}


class MatchingImplementation : public asg::AsgMessaging {
public:
  MatchingImplementation(MatchingTool& mt, double threshold=0.4);
  Trig::TrigDecisionTool* tdt();
  bool matchDistanceMatrix(const std::vector<std::vector<double> >& matrix, const Trig::MatchingStrategy::Strategy strategy = Trig::MatchingStrategy::MinimalSum, double threshold = 0.0) const ;
  inline void setThreshold(double in) { m_threshold=in; }
private:
  bool assocIsMatched(IAssociationStrategy::index_assignment_t association, const std::vector<std::vector<double> >& matrix, double threshold) const ;
  MatchingTool& m_tool;
  std::map<Trig::MatchingStrategy::Strategy,std::unique_ptr<IAssociationStrategy> > m_strategies;
  double m_threshold; //the distance threshold for a match
};

}

#endif

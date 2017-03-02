// Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MATCHRESULT_H
#define MATCHRESULT_H

#include "xAODBase/ObjectType.h"

#include "TriggerMatchingTool/IMatchResult.h"
#include "TriggerMatchingTool/IAssociationStrategy.h"

#include <map>
#include "xAODBase/IParticle.h"
#include "xAODBase/ObjectType.h"
#include "TrigDecisionTool/Feature.h"

namespace Trig {


  struct ComboMatch {
    struct SingleTypeMatch {
      typedef Feature<xAOD::IParticle> TrigFeature;
      IAssociationStrategy::index_assignment_t assignment;
      std::vector<TrigFeature> features;
      double total_cost;
      bool matched;
    };
    std::map<xAOD::Type::ObjectType,SingleTypeMatch> singlematch_by_type;
    bool matched;
  };
  
  class MatchingTool;
  class MatchResult : public IMatchResult {
  public:
    friend class MatchingTool;
    MatchResult();
    ~MatchResult();
    virtual bool isMatched() override;
    std::vector<const xAOD::IParticle*> bestMatchedObjects() override;
  private:
    bool m_matched;

    //the original list of reco particles
    std::vector<const xAOD::IParticle*> original_recolist;
    
    //we'll keep a type_sorted list to the reco particles that initiated this match
    std::map<xAOD::Type::ObjectType,std::vector<const xAOD::IParticle*> > m_recos_by_type;
    std::vector<ComboMatch> m_combresults;
  };
  
}

#endif

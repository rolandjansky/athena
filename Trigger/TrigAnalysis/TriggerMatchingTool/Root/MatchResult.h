// Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MATCHRESULT_H
#define MATCHRESULT_H

#include "xAODBase/ObjectType.h"

#include "TriggerMatchingTool/IMatchResult.h"
#include "TriggerMatchingTool/IAssociationStrategy.h"

#include "TrigDecisionTool/Feature.h"

namespace Trig {


  typedef Feature<xAOD::IParticle> > TrigFeature;
  struct ComboMatch {
    std::map<xAOD::Type::ObjectType,std::vector<TrigFeature> > trigfeats_by_type;
    std::map<xAOD::Type::ObjectType,IAssociationStrategy::index_assignment_t> assoc_by_type;
    std::string message;
  };
  

  class MatchingTool;
  class MatchResult : public IMatchResult {
  public:
    friend class MatchingTool;
    MatchResult();
    virtual bool isMatched() override;
    
  private:
    bool m_matched;

    //we'll keep a type_sorted list to the reco particles that initiated this match
    std::map<xAOD::Type::ObjectType,std::vector<const xAOD::IParticle*> > m_recos_by_type;
 
    std::vector<ComboMatch> m_combresults;
  };
  
}

#endif

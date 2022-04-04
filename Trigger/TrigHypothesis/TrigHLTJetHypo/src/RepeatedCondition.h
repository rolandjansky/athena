/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRIGHLTJETHYPO_REPEATEDCONDITION_H
#define TRIGHLTJETHYPO_REPEATEDCONDITION_H

/********************************************************************
 *
 * NAME:     RepeatedCondition.h
 * PACKAGE:  Trigger/TrigHypothesis/TrigHLTJetHypo
 *
 * AUTHOR:   P. Sherwood
 * 
 * an implementation of the  IRepeatedCondition PABC.
 *          
 *********************************************************************/

#include "TrigHLTJetHypo/TrigHLTJetHypoUtils/HypoJetDefs.h"
#include "IRepeatedCondition.h"

#include <memory>
#include <string>
#include <ostream>


class ITrigJetHypoInfoCollector;

class RepeatedCondition: public IRepeatedCondition {
 public:
 RepeatedCondition(std::unique_ptr<ICondition> cp,
		   std::size_t mult,
		   int clique,
		   int chainPartind = -1,
		   bool invert=false);
  virtual ~RepeatedCondition();
  
  virtual bool
  multiplicitySatisfied(std::size_t jgMultiplicity,
			const Collector&) const override;
  virtual bool
    isSatisfied(const HypoJetVector& v,
		const std::unique_ptr<ITrigJetHypoInfoCollector>& c) const override;


  // capacity: this is the size of a jet group which is requred to
  // satisfy a Condition.
  // eg the five  ComoundConditions for the chain 3JX_2JX have capcity 1
  // A Dijet Condition will have multiplicity 2.

  
  virtual unsigned int capacity() const override;
  
  // multiplicity: RepeatedConditions which contain CompoundConditions
  // with identical functionality will have the multiplicity
  // given by the chainPart of the condiguriung chainDict.
  //
  // eg the two  conditions for the chain 3JX_2JX will have multiplicty
  // of 3 and 2 respectively.
  virtual std::size_t multiplicity() const override;

  // clique: RepeatedConditions which contain CompoundConditions
  // with identical functionality form a clique, and receive
  // the same value for their clique index.
  //
  // eg the two RepeteadConditions for the chain 3JX_2JX form a clique.
  virtual int clique() const override;
  
  virtual std::string toString() const override;

  virtual int label() const override;

  virtual bool isFromChainPart() const override;

private:

  std::unique_ptr<ICondition> m_condition;
  std::size_t m_multiplicity{1};
  int  m_clique{-1};
  // record of which chain part in the chainDict this conditions comes from
  int m_chainPartInd{-1};
  bool m_invert{false};
};

std::ostream& operator<<(std::ostream&,
			 const RepeatedCondition&);

#endif

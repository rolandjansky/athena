/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRIGHLTJETHYPO_IJETGROUPER_H
#define TRIGHLTJETHYPO_IJETGROUPER_H

#include "TrigHLTJetHypo/TrigHLTJetHypoUtils/HypoJetDefs.h"
#include <string>
#include <optional>

class IJetGrouper{
  public:
  virtual ~IJetGrouper(){}

  /* Why such a complex return type?
   * Most groupers take a collection of jets, and divide it up into 
   * the smaller groups to be tested against a Condition.
   * However, it is possible (eg partition grouper) for there
   * to be many ways to breal the orinal jet collection into 
   * a groups of jets that can be matched to conditions. Partition
   * grouper returns all possibilities. Each such diviion is 
   * store in a HypoJetVector. The various HypoJetVectors are
   * collected into a vector of HypoJetGroupVectors.
   */
  virtual std::vector<HypoJetGroupVector> group(HypoJetIter&,
						HypoJetIter&) const = 0;
  virtual std::optional<HypoJetVector> next() = 0;
  virtual std::string toString() const = 0; 
  virtual std::string getName() const = 0; 
};
#endif

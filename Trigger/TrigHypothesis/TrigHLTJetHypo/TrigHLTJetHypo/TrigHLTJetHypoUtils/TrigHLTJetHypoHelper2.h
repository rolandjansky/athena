/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRIGHLTJETHYPO_TRIGHLTJETHYPOHELPER2_H
#define TRIGHLTJETHYPO_TRIGHLTJETHYPOHELPER2_H

/**
 * A configurable helper class to implement Jet Trigger algorithms.
 * Initial jet removal from incomming container is done using the ICleaner predicates.
 * The surviving jets are grouped into subsets by the IJetGroup object.
 *
 * The IMatcher objector owns a set of Conditions objects. The MAtcher determines 
 * wether the container of jet groups satisfies the Conditions. If so,
 * the event passes, otherwise it fails.
 *
 */

#include <vector>
#include <memory>
#include "TrigHLTJetHypo/TrigHLTJetHypoUtils/CleanerBridge.h"
#include "TrigHLTJetHypo/TrigHLTJetHypoUtils/IJetGrouper.h"
#include "TrigHLTJetHypo/TrigHLTJetHypoUtils/IGroupsMatcher.h"

class TrigHLTJetHypoHelper2{

 public:
  TrigHLTJetHypoHelper2(const std::vector<CleanerBridge>&,
                      std::unique_ptr<IJetGrouper>,
                      std::unique_ptr<IGroupsMatcher>
                      );
  
  bool pass(HypoJetVector&);  // not const as updates passed and failed jets
  
  HypoJetVector passedJets() const noexcept;
  HypoJetVector failedJets() const noexcept;
  Conditions getConditions() const noexcept;

  std::string toString() const;

 private:

  // Object to make jet groups. Jet groups
  // are vectors of jets selected from a jet vector
  // which is, in this case, the incoming jet vector.
  std::unique_ptr<IJetGrouper> m_grouper;

  // Object that matchs jet groups with Conditions
  std::unique_ptr<IGroupsMatcher> m_matcher;

  // Bridge objects to ICleaner predicate function objects to allow polymorphic
  // pointers to be used with the STL (pass by value).
  std::vector<CleanerBridge> m_cleaners;  

};

#endif

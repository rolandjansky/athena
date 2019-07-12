/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/
#ifndef TRIGHLTJETHYPO_ITRIGJETHYPOTOOLCONFIG_H
#define TRIGHLTJETHYPO_ITRIGJETHYPOTOOLCONFIG_H

#include "GaudiKernel/IAlgTool.h"
#include "./ConditionsDefsMT.h"
#include "TrigHLTJetHypo/TrigHLTJetHypoUtils/IJetGrouper.h"
#include "./IGroupsMatcherMT.h"

#include "TrigHLTJetHypo/TrigHLTJetHypoUtils/CleanerBridge.h"
#include <optional>

class ITrigJetHypoToolConfig : virtual public ::IAlgTool {
    
public:
  DeclareInterfaceID(ITrigJetHypoToolConfig, 1, 0);
  virtual ~ITrigJetHypoToolConfig(){};

  virtual StatusCode checkVals()  const = 0;


  // allow non-standard cleaners to be added to the cleaner factory.
  virtual std::vector<std::shared_ptr<ICleaner>> getCleaners() const = 0;
  
  virtual std::unique_ptr<IJetGrouper> getJetGrouper() const = 0;
  virtual std::unique_ptr<IGroupsMatcherMT> getMatcher() const = 0;
  virtual std::optional<ConditionsMT> getConditions() const = 0;
  virtual std::size_t requiresNJets() const =0;
};
#endif

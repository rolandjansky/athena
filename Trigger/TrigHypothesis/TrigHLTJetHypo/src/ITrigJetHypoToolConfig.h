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
  /** PABC (Pure Abstract Base Class) for JetHypoTool Condiguration classes.
   TrigJetHypoToolHelperMT is a general purpose class to run
   parts of a jet hypo. The Condiguration tools supply them
   with the components according to the hypo scenario. */
public:
  DeclareInterfaceID(ITrigJetHypoToolConfig, 1, 0);
  virtual ~ITrigJetHypoToolConfig(){};


  /** check the input values */
  virtual StatusCode checkVals()  const = 0;


  /** Provide cleaner objects to sdiscard "dirty" jets before preocessing. */
  virtual std::vector<std::shared_ptr<ICleaner>> getCleaners() const = 0;

  /** return a grouper used to package a set of jets into a subsets or "groups"
   */
  
  virtual std::unique_ptr<IJetGrouper> getJetGrouper() const = 0;

  /** return an object that tests jet group - Conditions matching */
  virtual std::unique_ptr<IGroupsMatcherMT> getMatcher() const = 0;

  /** return the Conditions (which check whether a jet group satisfies one
      or more requirements 
  */
   virtual std::optional<ConditionsMT> getConditions() const = 0;

   /*( Gives the minimum number of jets the helper using this Confiog
   class will require to pass the event. */
  virtual std::size_t requiresNJets() const =0;
};
#endif

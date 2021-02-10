/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/
#ifndef TRIGHLTJETHYPO_ITRIGJETHYPOTOOLNOGROUPERCONFIG_H
#define TRIGHLTJETHYPO_ITRIGJETHYPOTOOLNOGROUPERCONFIG_H


#include "GaudiKernel/IAlgTool.h"
#include "./ConditionsDefsMT.h"
#include "./IJetsMatcherMT.h"
#include "./ConditionFilter.h"

#include "TrigHLTJetHypo/TrigHLTJetHypoUtils/CleanerBridge.h"
#include <optional>

class ITrigJetHypoToolNoGrouperConfig : virtual public ::IAlgTool {
  /** PABC (Pure Abstract Base Class) for JetHypoTool Condiguration classes.
   TrigJetHypoToolHelperMT is a general purpose class to run
   parts of a jet hypo. The Condiguration tools supply them
   with the components according to the hypo scenario. */
public:
  DeclareInterfaceID(ITrigJetHypoToolNoGrouperConfig, 1, 0);
  virtual ~ITrigJetHypoToolNoGrouperConfig(){};


  /** check the input values */
  virtual StatusCode checkVals()  const = 0;


  /** return an object that tests jet group - Conditions matching */
  virtual std::unique_ptr<IJetsMatcherMT> getMatcher() const = 0;

  /** return the Conditions (which check whether a jet group satisfies one
      or more requirements 
  */
   virtual std::optional<ConditionsMT> getConditions() const = 0;

  /** return the ConditionFilters. These reduce the input jet collection
      before it is passed to a Condition. Example: HT conditions sometimes
      need and eta - Et filtered vector on which to do the Et sum.
  */
  
  virtual std::vector<std::unique_ptr<ConditionFilter>>
  getConditionFilters() const = 0;

  /*( Gives the minimum number of jets the helper using this Confiog
   class will require to pass the event. */
  virtual std::size_t requiresNJets() const =0;
};
#endif

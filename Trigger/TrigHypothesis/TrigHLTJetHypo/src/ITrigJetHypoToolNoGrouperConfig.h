/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/
#ifndef TRIGHLTJETHYPO_ITRIGJETHYPOTOOLNOGROUPERCONFIG_H
#define TRIGHLTJETHYPO_ITRIGJETHYPOTOOLNOGROUPERCONFIG_H


#include "GaudiKernel/IAlgTool.h"
#include "./ConditionsDefs.h"
#include "./IJetsMatcher.h"
#include "./ConditionFilter.h"

class ITrigJetHypoToolNoGrouperConfig : virtual public ::IAlgTool {
  /** PABC (Pure Abstract Base Class) for JetHypoTool Condiguration classes.
   TrigJetHypoToolHelper is a general purpose class to run
   parts of a jet hypo. The Condiguration tools supply them
   with the components according to the hypo scenario. */
public:
  DeclareInterfaceID(ITrigJetHypoToolNoGrouperConfig, 1, 0);
  virtual ~ITrigJetHypoToolNoGrouperConfig(){};


  /** check the input values */
  virtual StatusCode checkVals()  const = 0;


  /** return an object that tests jet group - Conditions matching */
  virtual std::unique_ptr<IJetsMatcher> getMatcher() const = 0;


  /*( Gives the minimum number of jets the helper using this Confiog
   class will require to pass the event. */
  virtual std::size_t requiresNJets() const =0;
};
#endif

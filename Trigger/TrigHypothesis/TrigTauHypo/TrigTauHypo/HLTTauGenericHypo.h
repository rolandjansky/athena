/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef HLTTAUGENERICHYPO_H
#define HLTTAUGENERICHYPO_H

/********************************************************************
 *
 * NAME:     HLTTauGenericHypo.h
 * PACKAGE:  Trigger/TrigHypothesis/TrigTauHypo
 *
 * AUTHORS:   P.O. DeViveiros
 * CREATED:   Sometime in early 2015
 *
 * DESCRIPTION:
 *
 * Implementation of generic cuts on TauID variables
 *
 *********************************************************************/

#include <vector>
#include "TFormula.h"
#include "TrigInterfaces/HypoAlgo.h"

class StoreGateSvc;

namespace HLT {
  class TriggerElement;
}

class HLTTauGenericHypo : public HLT::HypoAlgo
{

 public:

  /** constructor of EF tauRec hypo algo */
  HLTTauGenericHypo(const std::string& name, ISvcLocator* pSvcLocator);
  /** destructor */
  ~HLTTauGenericHypo();

  /** HLT method for initialize */
  HLT::ErrorCode hltInitialize();

  /** HLT method for finalize */
  HLT::ErrorCode hltFinalize();

  /** HLT method for execute Hypo on  a TE.
      input is TE, output is pass=True or False */
  HLT::ErrorCode hltExecute(const HLT::TriggerElement* outputTE, bool& pass);

 private:

  /** Configurables */

  std::vector<int> m_member;
  std::vector<std::string> m_formula;
  std::vector<TFormula> m_store;

};
#endif


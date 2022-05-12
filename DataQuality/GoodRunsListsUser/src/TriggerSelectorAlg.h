// -*- C++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef GoodRunsListsUser_TriggerSelectorAlg_H
#define GoodRunsListsUser_TriggerSelectorAlg_H

#include "AthenaBaseComps/AthAlgorithm.h"
#include "GoodRunsLists/RegularFormula.h"
#include "GaudiKernel/ToolHandle.h"

#include <list>
#include <string>
#include "TString.h"

/////////////////////////////////////////////////////////////////////////////

class ITriggerRegistryTool;

namespace Trig {
  class TrigDecisionTool;
}

class TriggerSelectorAlg : public AthAlgorithm 
{
 public:
  TriggerSelectorAlg( const std::string& name, ISvcLocator* pSvcLocator );
  virtual ~TriggerSelectorAlg();

  StatusCode initialize();
  StatusCode execute();
  StatusCode finalize();

 private:

  ToolHandle< Trig::TrigDecisionTool > m_tdTool;
  ToolHandle< ITriggerRegistryTool > m_TriggerRegistryTool;

  double* m_trigpar;

  unsigned int m_counter;
  unsigned int m_accept;
  unsigned int m_prescale;

  std::string  m_regexpr, m_regexprRegistrationOnly;
  std::list<TString> m_par;
  Root::RegularFormula m_formula;
  bool m_applyTriggerSelection;

  std::string m_myname;
};

#endif // GoodRunsListsUser_TriggerSelectorAlg_H


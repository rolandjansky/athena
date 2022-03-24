// -*- C++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef GoodRunsListsUser_GRLTriggerSelectorAlg_H
#define GoodRunsListsUser_GRLTriggerSelectorAlg_H

#include "GaudiKernel/ToolHandle.h"
#include "AthenaBaseComps/AthAlgorithm.h"
#include "GoodRunsLists/RegularFormula.h"

#include <list>
#include <string>
#include <vector>
#include "TString.h"

/////////////////////////////////////////////////////////////////////////////

class IGoodRunsListSelectorTool;
class ITriggerRegistryTool;

namespace Trig {
  class TrigDecisionTool;
}


class GRLTriggerSelectorAlg : public AthAlgorithm 
{
 public:
  GRLTriggerSelectorAlg( const std::string& name, ISvcLocator* pSvcLocator );
  virtual ~GRLTriggerSelectorAlg(); 

  StatusCode initialize();
  StatusCode execute();
  StatusCode finalize();

 private:
  ToolHandle< IGoodRunsListSelectorTool > m_GoodRunsListSelectorTool;
  ToolHandle< ITriggerRegistryTool > m_TriggerRegistryTool;
  ToolHandle< Trig::TrigDecisionTool > m_tdTool;

  unsigned int m_prescale;
  unsigned int m_gcounter;
  unsigned int m_gaccept;
  unsigned int m_tcounter;
  unsigned int m_taccept;

  std::vector<std::string> m_grlnameVec;
  std::vector<std::string> m_brlnameVec;
  std::string m_myname;

  double* m_trigpar;

  std::string  m_regexpr, m_regexprRegistrationOnly;
  std::list<TString> m_par;
  Root::RegularFormula m_formula;
  bool m_applyTriggerSelection;

};

#endif // GoodRunsListsUser_GRLTriggerSelectorAlg_H


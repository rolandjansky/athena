/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef PYANALYSISEXAMPLES_MYSELECTIONALG_H
#define PYANALYSISEXAMPLES_MYSELECTIONALG_H

class MyCutClass;

#include "AthenaBaseComps/AthAlgorithm.h"

namespace AthPyEx {
class MyTool;

class MySelectionAlg : public AthAlgorithm
{
public:
  MySelectionAlg(const std::string& name, ISvcLocator* pSvcLocator);
  ~MySelectionAlg() {}  

  StatusCode initialize();
  StatusCode execute();
  StatusCode finalize();

private:
  double      m_ThrPT;
  std::string m_NewKey;
  std::string m_ContainerKey;

  MyCutClass      * m_cut;
  AthPyEx::MyTool * m_myTool;
};

} //> end namespace AthPyEx

#endif

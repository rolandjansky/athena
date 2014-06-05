/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef JetTagD3PDMaker_JetTagJetFilterAlg_H
#define JetTagD3PDMaker_JetTagJetFilterAlg_H

#include "AthenaBaseComps/AthAlgorithm.h"

class Jet;

namespace D3PD {


class JetTagJetFilterAlg : public AthAlgorithm {
 public:
  JetTagJetFilterAlg(const std::string& name, ISvcLocator* pSvcLocator);
  
  StatusCode initialize();
  StatusCode finalize();
  StatusCode execute();

 private:

  bool pass(const Jet* jet);

  std::string m_jetContainerName;
  std::string m_filteredJetContainerName;

  double m_ptMin;
  double m_etaCut;
  bool m_useEMScale;

};


} // namespace D3PD


#endif // JetTagD3PDMaker_JetTagJetFilterAlg_H

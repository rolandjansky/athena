/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef ViewAlgs_PrescaleDecision_h
#define ViewAlgs_PrescaleDecision_h

#include <string>
#include <map>
#include "AthenaBaseComps/AthAlgorithm.h"

#include "xAODTrigger/TrigCompositeContainer.h"
#include "xAODTrigger/TrigCompositeAuxContainer.h"

//#include "AthViews/View.h"

#include "TrigConfHLTData/HLTUtils.h"


class PrescaleDecision : public AthAlgorithm {
public:
  PrescaleDecision(const std::string& name, ISvcLocator* pSvcLocator);
  StatusCode initialize();
  StatusCode execute();
  
  typedef double PSValue;

  
private:
  std::vector<std::string> m_psValuesConf;
  std::map<TrigConf::HLTHash, PSValue> m_prescales;


  SG::ReadHandle< xAOD::TrigCompositeContainer > m_inputChainDecisions;
  SG::WriteHandle< xAOD::TrigCompositeContainer > m_outputChainDecisions;
  SG::WriteHandle< xAOD::TrigCompositeAuxContainer > m_outputChainDecisionsAux;
  

};

#endif

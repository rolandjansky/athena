/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef ViewAlgs_MenuAlg_h
#define ViewAlgs_MenuAlg_h
#include <string>
#include "AthenaBaseComps/AthAlgorithm.h"

#include "xAODTrigger/TrigCompositeContainer.h"
#include "xAODTrigger/TrigCompositeAuxContainer.h"

//#include "AthViews/View.h"

#include "TrigConfHLTData/HLTUtils.h"

class MenuAlg : public AthAlgorithm {
 public:
  MenuAlg(const std::string& name, ISvcLocator* pSvcLocator);
  StatusCode initialize();
  StatusCode execute();

  struct Requirement {
    TrigConf::HLTHash chain; 
    TrigConf::HLTHash hypo; 
    size_t multiplicity;
  };


 private:
  std::vector<std::string> m_requiredConf;
  std::vector<Requirement> m_required;
  SG::ReadHandle< xAOD::TrigCompositeContainer > m_inputChainDecisions;
  SG::WriteHandle< xAOD::TrigCompositeContainer > m_outputChainDecisions;
  SG::WriteHandle< xAOD::TrigCompositeAuxContainer > m_outputChainDecisionsAux;


  SG::ReadHandle< xAOD::TrigCompositeContainer > m_hypoDecisions;
  SG::WriteHandle< xAOD::TrigCompositeContainer > m_outputDecisions;
  SG::WriteHandle< xAOD::TrigCompositeAuxContainer > m_outputDecisionsAux;


  bool hasTE(const xAOD::TrigComposite*, const std::set<TrigConf::HLTHash>&) const;



};


#endif 

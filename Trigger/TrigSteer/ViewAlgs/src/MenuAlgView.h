/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef ViewAlgs_MenuAlgView_h
#define ViewAlgs_MenuAlgView_h
#include <string>
//#include "AthenaBaseComps/AthAlgorithm.h"
#include "AthViews/AthViewAlgorithm.h"

#include "xAODTrigger/TrigCompositeContainer.h"
#include "xAODTrigger/TrigCompositeAuxContainer.h"

#include "AthViews/View.h"
#include "TrigConfHLTData/HLTUtils.h"

namespace AthViews {

class MenuAlgView : public ::AthViewAlgorithm  {

 public:

  MenuAlgView(const std::string& name, ISvcLocator* pSvcLocator);
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


  SG::ReadHandle< xAOD::TrigCompositeContainer > m_inputProxyDecisions;
  SG::WriteHandle< xAOD::TrigCompositeContainer > m_outputProxyDecisions;
  SG::WriteHandle< xAOD::TrigCompositeAuxContainer > m_outputProxyDecisionsAux;


  bool hasTE(const xAOD::TrigComposite*, const std::set<TrigConf::HLTHash>&) const;



};

}

#endif 

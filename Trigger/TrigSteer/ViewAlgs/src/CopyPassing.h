/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef ViewAlgs_CopyPassing_h
#define ViewAlgs_CopyPassing_h
#include <string>
#include <set>
#include "AthenaBaseComps/AthAlgorithm.h"

#include "AthContainers/ConstDataVector.h"
#include "xAODTrigger/TrigCompositeContainer.h"
#include "xAODTrigger/TrigCompositeAuxContainer.h"





class CopyPassing : public AthAlgorithm {
 public:
  CopyPassing(const std::string& name, ISvcLocator* pSvcLocator);
  StatusCode initialize();
  StatusCode execute();

 private:
  SG::ReadHandle<xAOD::TrigCompositeContainer> m_all;
  SG::ReadHandle<xAOD::TrigCompositeContainer> m_passing;

  SG::WriteHandleKey< ConstDataVector<xAOD::TrigCompositeContainer> > m_output;
  /* SG::WriteHandle< xAOD::TrigCompositeContainer > m_output; */
  /* SG::WriteHandle< xAOD::TrigCompositeAuxContainer > m_outputAux; */
};


#endif 

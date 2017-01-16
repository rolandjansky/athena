/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef ViewAlgs_DumpDecisions_h
#define ViewAlgs_DumpDecisions_h

#include "AthenaBaseComps/AthAlgorithm.h"


#include "xAODTrigger/TrigCompositeContainer.h"
#include "xAODTrigger/TrigCompositeAuxContainer.h"

//#include "AthViews/View.h"

class DumpDecisions : public AthAlgorithm {
public:
  DumpDecisions(const std::string& name, ISvcLocator* pSvcLocator);
  StatusCode initialize();
  StatusCode execute();

private:
  SG::ReadHandle< xAOD::TrigCompositeContainer > m_decisions;
};


#endif // re-include protection

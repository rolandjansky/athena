/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "AthenaBaseComps/AthAlgorithm.h"
#include "GaudiKernel/ServiceHandle.h"

// Data members classes
#include <list>
#include "PileUpTools/PileUpMergeSvc.h"
 

/////////////////////////////////////////////////////////////////////////////

class MyMultiInputAlg
  : public AthAlgorithm
{
public:
  MyMultiInputAlg (const std::string& name, ISvcLocator* pSvcLocator);
  StatusCode initialize();
  StatusCode execute();
  StatusCode finalize();

private:
  ServiceHandle<PileUpMergeSvc> m_mergeSvc; // Pile up service
};
 

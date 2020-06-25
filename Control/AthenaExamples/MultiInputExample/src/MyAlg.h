/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "AthenaBaseComps/AthAlgorithm.h"
#include "GaudiKernel/ServiceHandle.h"

// Data members classes
#include <list>
#include "PileUpTools/PileUpMergeSvc.h"
 

/////////////////////////////////////////////////////////////////////////////

class MyAlg
  : public AthAlgorithm 
{
public:
  MyAlg (const std::string& name, ISvcLocator* pSvcLocator);
  StatusCode initialize();
  StatusCode execute();
  StatusCode finalize();

private:
  ServiceHandle<PileUpMergeSvc> m_mergeSvc; // Pile up service
};
 

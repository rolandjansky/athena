/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef ANALYSISTEST_READTRIGGER_H
#define ANALYSISTEST_READTRIGGER_H

/*
  Read Trigger

  @author Tadashi Maeno
*/

#include "AthenaBaseComps/AthAlgorithm.h"
#include "GaudiKernel/NTuple.h"

class StoreGateSvc;

/////////////////////////////////////////////////////////////////////////////

class ReadTrigger : public AthAlgorithm
{
public:
  ReadTrigger (const std::string& name, ISvcLocator* pSvcLocator);
  ~ReadTrigger () {}
  StatusCode initialize();
  StatusCode execute();
  StatusCode finalize();
};

#endif








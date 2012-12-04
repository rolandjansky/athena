/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef ANALYSISTEST_READTRIGGER_H
#define ANALYSISTEST_READTRIGGER_H

/*
  Read Trigger

  @author Tadashi Maeno
*/

#include "GaudiKernel/Algorithm.h"
#include "GaudiKernel/NTuple.h"

class StoreGateSvc;

/////////////////////////////////////////////////////////////////////////////

class ReadTrigger : public Algorithm
{
public:
  ReadTrigger (const std::string& name, ISvcLocator* pSvcLocator);
  ~ReadTrigger () {}
  StatusCode initialize();
  StatusCode execute();
  StatusCode finalize();

private:
  StoreGateSvc * m_storeGate;
};

#endif








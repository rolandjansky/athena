/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef __MTNtup_H__
#define __MTNtup_H__

#include "GaudiKernel/Algorithm.h"
#include "GaudiKernel/NTuple.h"
#include <string>

class StoreGateSvc;
/////////////////////////////////////////////////////////////////////////////

class MTNtup:public Algorithm {
public:
  MTNtup (const std::string& name, ISvcLocator* pSvcLocator);
  StatusCode initialize();
  StatusCode execute();
  StatusCode finalize();
  
private:
  // Ntuple data members
  NTuple::Item<long>  m_size;
  NTuple::Item<long>  m_run,m_event;
  NTuple::Array<long> m_rundata;

  // Pointer to an ntuple
  NTuple::Tuple*      m_ntuple;

  // Pointer to event store
  StoreGateSvc*       m_StoreGateSvc;
};

#endif

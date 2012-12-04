/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef ANALYSISTEST_NAVTEST_H
#define ANALYSISTEST_NAVTEST_H

/*
  test for navigation

  @author Tadashi Maeno
*/

#include "GaudiKernel/Algorithm.h"
#include "GaudiKernel/NTuple.h"

class StoreGateSvc;

/////////////////////////////////////////////////////////////////////////////

class NavTest : public Algorithm
{
public:
  NavTest (const std::string& name, ISvcLocator* pSvcLocator);
  ~NavTest () {}
  StatusCode initialize();
  StatusCode execute();
  StatusCode finalize();

private:
  StoreGateSvc * m_storeGate;
};

#endif








/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef __MTHist_H__
#define __MTHist_H__

#include "GaudiKernel/Algorithm.h"
#include <string>

class IHistogram1D;
class StoreGateSvc;

/////////////////////////////////////////////////////////////////////////////

class MTHist:public Algorithm {
public:
  MTHist (const std::string& name, ISvcLocator* pSvcLocator);
  StatusCode initialize();
  StatusCode execute();
  StatusCode finalize();
  
private:
  IHistogram1D*         m_hist;
  StoreGateSvc*         m_StoreGateSvc;
  int m_previousEvent;
};

#endif

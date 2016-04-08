/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef __MTHist_H__
#define __MTHist_H__

#include "AthenaBaseComps/AthAlgorithm.h"
#include <string>

namespace AIDA {
  class IHistogram1D;
}

/////////////////////////////////////////////////////////////////////////////

class MTHist:public AthAlgorithm {
public:
  MTHist (const std::string& name, ISvcLocator* pSvcLocator);
  StatusCode initialize();
  StatusCode execute();
  StatusCode finalize();
  
private:
  AIDA::IHistogram1D*         m_hist;
  int m_previousEvent;
};

#endif

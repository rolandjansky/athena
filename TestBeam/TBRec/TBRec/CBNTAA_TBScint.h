/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef CBNTAA_TBSCINT_H
#define CBNTAA_TBSCINT_H

#include "GaudiKernel/NTuple.h"
#include "GaudiKernel/SmartDataPtr.h"
#include "CBNT_TBRecBase.h"
#include <fstream>
#include <string>
#include <vector>

class StoreGateSvc;

class CBNTAA_TBScint : public CBNT_TBRecBase
{
 public:
  CBNTAA_TBScint(const std::string & name, ISvcLocator * pSvcLocator);

  ~CBNTAA_TBScint() {};

  virtual StatusCode CBNT_initialize();
  virtual StatusCode CBNT_execute();
  virtual StatusCode CBNT_finalize();

 private:

  // Scint. parameters from MC
  std::vector<int> *m_scint_num; 
  std::vector<float> *m_scint_en; 
  std::vector<int> *m_scint_trackid; 
  std::vector<int> *m_scint_pdg; 

  StatusCode CBNT_clear();
};

#endif

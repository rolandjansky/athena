/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TBREC_CBNTAA_TBSCINT_H
#define TBREC_CBNTAA_TBSCINT_H

#include "GaudiKernel/NTuple.h"
#include "GaudiKernel/SmartDataPtr.h"
#include "CBNT_TBRecBase.h"
#include <fstream>
#include <string>
#include <vector>


class CBNTAA_TBScint : public CBNT_TBRecBase
{
 public:
  CBNTAA_TBScint(const std::string & name, ISvcLocator * pSvcLocator);

  virtual ~CBNTAA_TBScint() {};

  virtual StatusCode CBNT_initialize() override;
  virtual StatusCode CBNT_execute() override;
  virtual StatusCode CBNT_finalize() override;

 private:

  // Scint. parameters from MC
  std::vector<int> *m_scint_num; 
  std::vector<float> *m_scint_en; 
  std::vector<int> *m_scint_trackid; 
  std::vector<int> *m_scint_pdg; 

  virtual StatusCode CBNT_clear() override;
};

#endif

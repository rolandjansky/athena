/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TBREC_TBBEAMQUALITYMC_H
#define TBREC_TBBEAMQUALITYMC_H

#include "AthenaBaseComps/AthAlgorithm.h"
#include <vector>

class TBBeamQualityMC : public AthAlgorithm
{
 public:
  TBBeamQualityMC(const std::string & name, ISvcLocator * pSvcLocator);

  virtual ~TBBeamQualityMC() {};

  virtual StatusCode initialize() override;
  virtual StatusCode execute() override;
  virtual StatusCode finalize() override;

 private:
  /** Get Xcryo and Ytable from a text file */
  bool m_readFileforXcryo;
  StatusCode getXcryoYtable(float &x, float &y, float &eBeam);
  int m_nRun;

  float m_bm_cut_x;
  float m_bm_cut_y;
  bool m_check_trackpar;

  bool m_check_primary;
  std::vector<int> m_scint_prim;

  bool m_check_veto;

  bool m_check_clus;
  bool m_check_trackreco;

  std::string m_clusterCollName;
};

#endif

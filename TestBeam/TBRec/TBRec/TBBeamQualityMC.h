/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TBBEAMQUALITYMC_H
#define TBBEAMQUALITYMC_H

#include "AthenaBaseComps/AthAlgorithm.h"
#include <vector>

class StoreGateSvc;

class TBBeamQualityMC : public AthAlgorithm
{
 public:
  TBBeamQualityMC(const std::string & name, ISvcLocator * pSvcLocator);

  ~TBBeamQualityMC() {};

  virtual StatusCode initialize();
  virtual StatusCode execute();
  virtual StatusCode finalize();

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

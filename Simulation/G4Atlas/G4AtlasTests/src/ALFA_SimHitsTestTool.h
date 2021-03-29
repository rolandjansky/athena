/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#ifndef G4AT_ALFA_SIMHITSTESTTOOL
#define G4AT_ALFA_SIMHITSTESTTOOL

#include "SimTestToolBase.h"


class ALFA_SimHitsTestTool : public SimTestToolBase {

  public:

  ALFA_SimHitsTestTool(const std::string& type, const std::string& name, const IInterface* parent);

  StatusCode initialize();
  StatusCode processEvent();

  private:

  // ALFA test histograms

  TH2 *m_hit_map_h[8];
  TH2 *m_E_layer_sum_h[8];
  TH1 *m_E_full_sum_h[8];
  TH1 *m_E_gvs{};
};

#endif

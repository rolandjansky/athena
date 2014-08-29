/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
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

  TH2 *hit_map_h[8], *E_layer_sum_h[8];
  TH1 *E_full_sum_h[8];

};

#endif
 

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef G4AT_LUCIDHITSTESTTOOL
#define G4AT_LUCIDHITSTESTTOOL

#include "SimTestToolBase.h"

class LucidHitsTestTool : public SimTestToolBase {

public:

  LucidHitsTestTool(const std::string& type, const std::string& name, const IInterface* parent);
  virtual StatusCode initialize(); 
  StatusCode processEvent();

 protected:
  TH2 *m_x_v_y_edep, *m_x_v_y_hits, *m_R_v_z_edep, *m_R_v_z_hits;
  TH2 *m_x_v_y_post, *m_R_v_z_post, *m_time_edep;
  TH1 *m_pdgid, *m_tubeid, *m_gen_volume, *m_wavelength, *m_hit_edep, *m_total_e;
};

#endif

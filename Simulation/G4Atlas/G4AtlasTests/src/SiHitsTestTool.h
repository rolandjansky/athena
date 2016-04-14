/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef G4AT_SIHITSTESTTOOL
#define G4AT_SIHITSTESTTOOL

#include "SimTestToolBase.h"


class SiHitsTestTool : public SimTestToolBase {


public:

  SiHitsTestTool(const std::string& type, const std::string& name, const IInterface* parent);

  StatusCode initialize();

  StatusCode processEvent();

 private:
  std::string m_collection;
  // general
  TH2 *m_indetBarrel, *m_indetLongView;
  // specialised (Pixel/SCT)
  TH1 *m_hits_time, *m_hits_edep, *m_hits_log_edep;
  TH2 *m_hits_xy, *m_hits_zr;
  TH2 *m_hits_edep_zr;
  TH1 *m_hits_edep_z, *m_hits_edep_r;
  TH1 *m_hits_log_barcode;
  TH1 *m_hits_step_length, *m_hits_log_step_length;
};

#endif

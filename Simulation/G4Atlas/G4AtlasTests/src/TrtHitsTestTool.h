/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef G4AT_TRTHITSTESTTOOL
#define G4AT_TRTHITSTESTTOOL

#include "SimTestToolBase.h"


class TrtHitsTestTool : public SimTestToolBase {


public:

  TrtHitsTestTool(const std::string& type, const std::string& name, const IInterface* parent);

  StatusCode initialize();
  StatusCode processEvent();

 private:
  std::string m_collection;
  // general inner detector
  TH2 *m_indetBarrel, *m_indetLongView;
  // specialised (TRT)
  TH1 *m_hits_time_photons, *m_hits_edep_photons;
  TH1 *m_hits_time_nonphotons, *m_hits_edep_nonphotons;
  TH2 *m_hits_xy, *m_hits_zr;
  TH2 *m_hits_edep_zr_photons, *m_hits_edep_zr_nonphotons;
  TH1 *m_hits_log_barcode;
};

#endif

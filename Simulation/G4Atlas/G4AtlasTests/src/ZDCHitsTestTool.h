/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef G4AT_ZDCHITSTESTTOOL
#define G4AT_ZDCHITSTESTTOOL

#include "SimTestToolBase.h"


class ZDCHitsTestTool : public SimTestToolBase {


public:

  ZDCHitsTestTool(const std::string& type, const std::string& name, const IInterface* parent);

  StatusCode initialize(); 

  StatusCode processEvent();
 private:

  // globals
  TH1 *m_zdc_sidea_0,*m_zdc_sidea_1,*m_zdc_sidea_2,*m_zdc_sidea_3;
  TH1 *m_zdc_sidec_0,*m_zdc_sidec_1,*m_zdc_sidec_2,*m_zdc_sidec_3;

};

#endif

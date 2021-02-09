//  -*- c++ -*- 

/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#ifndef JETMONITORING_JETSUBSTRUCTUREHISTOS_H
#define JETMONITORING_JETSUBSTRUCTUREHISTOS_H
///////////////////////////////////////////
///
/// \class JetSubStructureHistos
/// A tool producing the jet substructre histos
/// 
////////////////////////////////////////////

#include "JetMonitoring/JetHistoBase.h"
#include "TH1.h"
#include "TH2.h"
#include "TProfile2D.h"

class JetSubStructureHistos : public JetHistoBase {
  ASG_TOOL_CLASS0(JetSubStructureHistos);
  
public:
  
  JetSubStructureHistos(const std::string &t);
  ~JetSubStructureHistos(){}

  virtual int buildHistos();

  virtual int fillHistosFromJet(const xAOD::Jet &j);

protected:

  TH1F* m_tau21;
  TH1F* m_tau32;
  TH1F* m_tau21_wta;
  TH1F* m_tau32_wta;

  TH1F* m_C1;
  TH1F* m_C2;
  TH1F* m_D2;

  std::string  m_jetScale;

};
#endif

//  -*- c++ -*- 

/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
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

//  virtual int fillHistosFromContainer(const xAOD::JetContainer & cont);
  virtual int fillHistosFromJet(const xAOD::Jet &j);

protected:

  TH1F* m_tau21;
//  TH1F* m_tau32;
//  TH1F* m_tau21_wta;
//  TH1F* m_tau32_wta;

//  TH1F* m_C1;
//  TH1F* m_C2;
//  TH1F* m_D2;


//  TH1F* m_njet;
//  TH1F* m_pt;
//  TH1F* m_eta;
//  TH1F* m_phi;
//  TH1F* m_m;
//  TH1F* m_e;
//  TH1F* m_nConstit;
//
//  // high pT
//  TH1F* m_pt_high;
//  TH1F* m_eta_high;
//  TH1F* m_m_high;
//  TH1F* m_e_high;
//  TH1F* m_nConstit_high;
//
//  TH2F* m_occupancyEtaPhi;
//  TProfile2D* m_averagePtEtaPhi;
//  TProfile2D* m_averageE_EtaPhi;
//  
  std::string  m_jetScale;
//
//  bool m_doN;
//  bool m_doM;
//  bool m_doE;
//  bool m_doOccupancy;
//  bool m_doAveragePt;
//  bool m_doAverageE;
//  bool m_doNConstit;
};
#endif

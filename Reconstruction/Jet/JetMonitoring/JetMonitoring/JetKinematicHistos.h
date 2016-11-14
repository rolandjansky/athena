//  -*- c++ -*- 

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef JETMONITORING_JETKINEMATICHISTOS_H
#define JETMONITORING_JETKINEMATICHISTOS_H
///////////////////////////////////////////
///
/// \class JetKinematicHistos
/// A tool producing basic jet kinematics histogramms
/// 
////////////////////////////////////////////

#include "JetMonitoring/JetHistoBase.h"
#include "TH1.h"
#include "TH2.h"
#include "TProfile2D.h"

class JetKinematicHistos : public JetHistoBase {
  ASG_TOOL_CLASS0(JetKinematicHistos);
  
public:
  
  JetKinematicHistos(const std::string &t);
  ~JetKinematicHistos(){}

  virtual int buildHistos();

  virtual int fillHistosFromContainer(const xAOD::JetContainer & cont);
  virtual int fillHistosFromJet(const xAOD::Jet &j);

protected:

  TH1F* m_njet;
  TH1F* m_pt;
  TH1F* m_eta;
  TH1F* m_phi;
  TH1F* m_m;
  TH1F* m_e;
  TH1F* m_nConstit;

  // high pT
  TH1F* m_pt_high;
  TH1F* m_eta_high;
  TH1F* m_m_high;
  TH1F* m_e_high;
  TH1F* m_nConstit_high;

  TH2F* m_occupancyEtaPhi;
  TProfile2D* m_averagePtEtaPhi;
  TProfile2D* m_averageE_EtaPhi;
  
  int m_jetScale;

  bool m_doN;
  bool m_doM;
  bool m_doE;
  bool m_doOccupancy;
  bool m_doAveragePt;
  bool m_doAverageE;
  bool m_doNConstit;
};
#endif

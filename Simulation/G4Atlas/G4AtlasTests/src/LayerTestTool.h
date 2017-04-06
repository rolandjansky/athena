/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef G4AT_LAYERTESTTOOL
#define G4AT_LAYERTESTTOOL

#include "SimTestToolBase.h"

class LayerTestTool : public SimTestToolBase{ 


 public:

  LayerTestTool(const std::string& type, const std::string& name, const IInterface* parent);
  StatusCode initialize();
  StatusCode processEvent();

 private:
  std::string m_collection;
  // histograms
  TH1 *m_n_energy;
  TH1 *m_n_energy_e;
  TH1 *m_n_energy_ep;
  TH1 *m_n_energy_gamma;
  TH1 *m_n_energy_neutron;
  TH1 *m_n_energy_muon;
  TH1 *m_n_energy_muonp;

  TH2 *m_n_pos;

  TH1 *m_n_px;
  TH1 *m_n_py;
  TH1 *m_n_pz;

  TH1 *m_n_rho;
  TH1 *m_n_eta;
  TH1 *m_n_phi;

  double m_energy;
  double m_pos_x;
  double m_pos_y;
  double m_pos_z;
  double m_pos_r;
  double m_p_x;
  double m_p_y;
  double m_p_z;
  double m_p_rho;
  double m_p_phi;
  double m_p_eta;

};

#endif

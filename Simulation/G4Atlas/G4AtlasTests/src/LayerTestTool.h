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

  double energy;
  double pos_x;
  double pos_y;
  double pos_z;
  double pos_r;
  double p_x;
  double p_y;
  double p_z;
  double p_rho;
  double p_phi;
  double p_eta;

};

#endif

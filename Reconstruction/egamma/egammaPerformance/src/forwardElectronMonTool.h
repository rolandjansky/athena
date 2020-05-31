/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/////////////////////////////////////////////////////////////
//
//      2014-10-22 Author: Remi Lafaye (Annecy) 
//
/////////////////////////////////////////////////////////////

#ifndef forwardElectronMonTool_H
#define forwardElectronMonTool_H

#include "egammaMonToolBase.h"

class forwardElectronMonTool : public egammaMonToolBase
{
 public:
  
  forwardElectronMonTool(const std::string& type, const std::string& name, const IInterface* parent); 
  
  virtual ~forwardElectronMonTool();
  
  virtual StatusCode initialize() override;
  virtual StatusCode bookHistograms() override;
  virtual StatusCode fillHistograms() override;
     
 private:

 protected:
  // Properties
  SG::ReadHandleKey<xAOD::ElectronContainer> m_ForwardElectronContainer{this, "ForwardElectronContainer", "egammaForwardCollection", "Name of the forward electron collection"}; // Container name for forwardElectrons

  // Loose electrons histograms
  TH1 * m_hN;       // Histogram for number of electrons
  TH1 * m_hEt;      // Histogram for electron transverse energies
  TH1 * m_hEta;     // Histogram for electron eta
  TH1 * m_hPhi;     // Histogram for electron phi
  TH2 * m_hEtaPhi;  // Histogram for electron eta,phi for candidates with more than 10 GeV energy
  //TH1 * m_hTopoEtCone40; // Histogram for calo based isolation energy
  TH1 * m_hTime;         // Histogram for forward cluster time

  // Loose electrons per region histograms
  std::vector<TH1*> m_hvN;   // Histograms for number of electrons
  std::vector<TH1*> m_hvEt;  // Histograms for electron transverse energies
  std::vector<TH1*> m_hvEta; // Histograms for electron transverse eta
  std::vector<TH1*> m_hvPhi; // Histograms for electron transverse phi
  //std::vector<TH1*> m_hvTopoEtCone40;  // Histograms for electron calo-based isolation transverse energies
  std::vector<TH1*> m_hvTime;  // Histograms for electron times

  // Loose electron ID per region histograms
  std::vector<TH1*> m_hvDensity; // energy density
  std::vector<TH1*> m_hvFrac;    // max energy fraction
  std::vector<TH1*> m_hvLongitu; // longitudinal moment
  std::vector<TH1*> m_hvLambda;  // second lambda
  std::vector<TH1*> m_hvLateral; // lateral moment
  std::vector<TH1*> m_hvSecondR; // second R
  std::vector<TH1*> m_hvCenterL; // center lambda

  // Tight electrons histograms
  TH1 * m_hTightN;            // Histogram for number of electrons
  TH1 * m_hTightEt;           // Histogram for electron transverse energies
  TH1 * m_hTightEta;          // Histogram for electron eta
  TH1 * m_hTightPhi;          // Histogram for electron phi
  TH2 * m_hTightEtaPhi;       // Histogram for electron eta,phi
  //TH1 * m_hTightTopoEtCone40; // Histogram for calo based isolation energy
  TH1 * m_hTightTime;         // Histogram for forward cluster time

  // Tight electrons per region histograms
  std::vector<TH1*> m_hvTightN;   // Histograms for number of electrons
  std::vector<TH1*> m_hvTightEt;  // Histograms for electron transverse energies
  std::vector<TH1*> m_hvTightEta; // Histograms for electron transverse eta
  std::vector<TH1*> m_hvTightPhi; // Histograms for electron transverse phi
  //std::vector<TH1*> m_hvTightTopoEtCone40;  // Histograms for electron calo-based isolation transverse energies
  std::vector<TH1*> m_hvTightTime;  // Histograms for electron times

  // Loose electron ID per region histograms
  std::vector<TH1*> m_hvTightDensity; // energy density
  std::vector<TH1*> m_hvTightFrac;    // max energy fraction
  std::vector<TH1*> m_hvTightLongitu; // longitudinal moment
  std::vector<TH1*> m_hvTightLambda;  // second lambda
  std::vector<TH1*> m_hvTightLateral; // lateral moment
  std::vector<TH1*> m_hvTightSecondR; // second R
  std::vector<TH1*> m_hvTightCenterL; // center lambda

  TH1 *m_hLB_N; // Histogram for number of forward electrons vs LB
  unsigned int m_lumiBlockNumber;
  unsigned int m_nForwardElectronsInCurrentLB;
  std::vector<int> m_nForwardElectronsPerLumiBlock;
  unsigned int m_nForwardElectrons;

};

#endif

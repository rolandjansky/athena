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

#include "egammaPerformance/egammaMonToolBase.h"

class forwardElectronMonTool : public egammaMonToolBase
{
 public:
  
  forwardElectronMonTool(const std::string& type, const std::string& name, const IInterface* parent); 
  
  virtual ~forwardElectronMonTool();
  
  virtual StatusCode bookHistograms();
  virtual StatusCode fillHistograms();
     
 private:

 protected:
  // Properties
  std::string m_ForwardElectronContainer; // Container name for forwardElectrons

  // Loose electrons histograms
  TH1 * m_hN;       // Histogram for number of electrons
  TH1 * m_hEt;      // Histogram for electron transverse energies
  TH1 * m_hEta;     // Histogram for electron eta
  TH1 * m_hPhi;     // Histogram for electron phi
  TH2 * m_hEtaPhi;  // Histogram for electron eta,phi

  // Loose electrons per region histograms
  std::vector<TH1*> m_hvN;   // Histograms for number of electrons
  std::vector<TH1*> m_hvEt;  // Histograms for electron transverse energies
  std::vector<TH1*> m_hvEta; // Histograms for electron transverse eta
  std::vector<TH1*> m_hvPhi; // Histograms for electron transverse phi

  // Tight electrons histograms
  TH1 * m_hTightN;       // Histogram for number of electrons
  TH1 * m_hTightEt;      // Histogram for electron transverse energies
  TH1 * m_hTightEta;     // Histogram for electron eta
  TH1 * m_hTightPhi;     // Histogram for electron phi
  TH2 * m_hTightEtaPhi;  // Histogram for electron eta,phi

  // Tight electrons per region histograms
  std::vector<TH1*> m_hvTightN;   // Histograms for number of electrons
  std::vector<TH1*> m_hvTightEt;  // Histograms for electron transverse energies
  std::vector<TH1*> m_hvTightEta; // Histograms for electron transverse eta
  std::vector<TH1*> m_hvTightPhi; // Histograms for electron transverse phi

  // electron ID histograms
  std::vector<TH1*> m_hvDensity; // energy density
  std::vector<TH1*> m_hvFrac;    // max energy fraction
  std::vector<TH1*> m_hvLongitu; // longitudinal moment
  std::vector<TH1*> m_hvLambda;  // second lambda
  std::vector<TH1*> m_hvLateral; // lateral moment
  std::vector<TH1*> m_hvSecondR; // second R
  std::vector<TH1*> m_hvCenterL; // center lambda
};

#endif

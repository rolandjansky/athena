/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/////////////////////////////////////////////////////////////
//
//      2014-05-21 Author: Remi Lafaye (Annecy) 
//
/////////////////////////////////////////////////////////////

#ifndef photonMonTool_H
#define photonMonTool_H

#include "egammaPerformance/egammaMonToolBase.h"

class photonMonTool : public egammaMonToolBase
{
 public:
  
  photonMonTool(const std::string& type, const std::string& name, const IInterface* parent); 
  
  virtual ~photonMonTool();
  
  virtual StatusCode bookHistograms();
  virtual StatusCode fillHistograms();
     
 protected:

  // Properties
  std::string m_PhotonContainer;   // Container name for photons

  // Global panel histograms
  TH1 * m_hN;       // Histogram for number of photons
  TH1 * m_hEt;      // Histogram for photon transverse energies
  TH1 * m_hPhi;     // Histogram for photon phi
  TH2 * m_hEtaPhi;  // Histogram for photon eta,phi

  // Track panel histograms
  TH1 * m_hTightN;       // Histogram for number of electrons
  TH1 * m_hTightEt;      // Histogram for electron transverse energies
  TH2 * m_hTightEtaPhi;  // Histogram for photon eta,phi
  std::vector<TH1*> m_hvTightNOfTRTHits;    
  std::vector<TH1*> m_hvTightConvType;

  // 1D Histograms Loose electrons
  std::vector<TH1*> m_hvN;   // Histograms for number of photons
  std::vector<TH1*> m_hvEt;  // Histograms for photon transverse energies
  std::vector<TH1*> m_hvEta; // Histograms for photon transverse eta
  std::vector<TH1*> m_hvPhi; // Histograms for photon transverse phi

  // photon ID histograms
  std::vector<TH1*> m_hvEhad1;         
  std::vector<TH1*> m_hvCoreEM;         
  std::vector<TH1*> m_hvF1;
  std::vector<TH1*> m_hvF2;
  std::vector<TH1*> m_hvF3;
  std::vector<TH1*> m_hvRe233e237;
  std::vector<TH1*> m_hvRe237e277;
  std::vector<TH1*> m_hvNOfTRTHits;    
  std::vector<TH1*> m_hvConvType;
  std::vector<TH1*> m_hvConvTrkMatch1;
  std::vector<TH1*> m_hvConvTrkMatch2;

  // 1D Histograms Tight photons
  std::vector<TH1*> m_hvTightN;   // Histograms for number of photons
  std::vector<TH1*> m_hvTightEt;  // Histograms for photon transverse energies
  std::vector<TH1*> m_hvTightEta; // Histograms for photon transverse eta
  std::vector<TH1*> m_hvTightPhi; // Histograms for photon transverse phi
  std::vector<TH1*> m_hvTightConvTrkMatch1;
  std::vector<TH1*> m_hvTightConvTrkMatch2;
};

#endif

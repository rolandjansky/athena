/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/////////////////////////////////////////////////////////////
//
//      2014-05-21 Author: Remi Lafaye (Annecy) 
//
/////////////////////////////////////////////////////////////

#ifndef ZeeTaPMonTool_H
#define ZeeTaPMonTool_H

#include "egammaPerformance/egammaMonToolBase.h"
#include "xAODEgamma/Electron.h"

class ZeeTaPMonTool : public egammaMonToolBase
{
 public:
  
  ZeeTaPMonTool(const std::string& type, const std::string& name, const IInterface* parent); 
  
  virtual ~ZeeTaPMonTool();
  
  virtual StatusCode bookHistograms();
  virtual StatusCode fillHistograms();
  virtual StatusCode procHistograms();
     
 private:
  void fillElectronProbe(const xAOD::Electron *el, bool isTight, float mass);

 protected:

  // Properties
  std::string m_ElectronContainer; // Container name for electrons
  float m_MassPeak;
  float m_ElectronEtCut;
  float m_MassLowerCut;
  float m_MassUpperCut;

  // Zee mass distributions
  std::vector<TH1*> m_hvMass;         

  // Efficiencies vs pT, eta
  TH1 * m_hIDEt;
  TH1 * m_hIDEta;
  TH1 * m_hISOEt;
  TH1 * m_hISOEta;

  // Loose electrons histograms
  TH1 * m_hN;       // Histogram for number of electrons
  TH1 * m_hEt;      // Histogram for electron transverse energies
  TH1 * m_hEta;     // Histogram for electron eta
  TH1 * m_hPhi;     // Histogram for electron phi
  TH2 * m_hEtaPhi;  // Histogram for electron eta,phi

  // electron tracks
  std::vector<TH1*> m_hvDeltaEta1;
  std::vector<TH1*> m_hvDeltaPhi2;     
  std::vector<TH1*> m_hvNOfBLayerHits; 
  std::vector<TH1*> m_hvNOfSiHits;
  std::vector<TH1*> m_hvNOfTRTHits;    

  // electron ID
  std::vector<TH1*> m_hvEhad1;         
  std::vector<TH1*> m_hvEoverP;        
  std::vector<TH1*> m_hvCoreEM;        
  std::vector<TH1*> m_hvF1;            
  std::vector<TH1*> m_hvF2;            
  std::vector<TH1*> m_hvF3;            
  std::vector<TH1*> m_hvRe233e237;     
  std::vector<TH1*> m_hvRe237e277;     

  // electron ISO
  std::vector<TH1*> m_hvCaloIso;
  std::vector<TH1*> m_hvTrackIso;

};

#endif

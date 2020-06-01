/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

/////////////////////////////////////////////////////////////
//
//      2014-05-21 Author: Remi Lafaye (Annecy) 
//      2015-05-15 Author: Bertrand Laforge (LPNHE, Paris)
//
/////////////////////////////////////////////////////////////

#ifndef ZeeTaPMonTool_H
#define ZeeTaPMonTool_H

#include "egammaMonToolBase.h"
#include "xAODEgamma/Electron.h"

class ZeeTaPMonTool : public egammaMonToolBase
{
 public:
  
  ZeeTaPMonTool(const std::string& type, const std::string& name, const IInterface* parent); 
  
  virtual ~ZeeTaPMonTool();
  
  virtual StatusCode initialize() override;
  virtual StatusCode bookHistograms() override;
  virtual StatusCode fillHistograms() override;
  virtual StatusCode procHistograms() override;
     
 private:
  void fillElectronProbe(const xAOD::Electron *el, bool isTight, bool isIso, double mass);

 protected:

  // Properties
  SG::ReadHandleKey<xAOD::ElectronContainer> m_ElectronContainer{this, "ElectronContainer", "Electrons", "Name of the electron collection"}; // Container name for electrons
  float m_MassPeak;
  float m_ElectronEtCut;
  float m_MassLowerCut;
  float m_MassUpperCut;
  int m_PhiBinning;

  // Number of Z candidates vs leading electron eta

  TH1 * m_hNZcandidates;

  MonGroup* m_electronGroup;
  MonGroup* m_electronTrkGroup;
  MonGroup* m_electronIdGroup;
  MonGroup* m_electronIsoGroup;
  MonGroup* m_electronLBGroup;
  MonGroup *m_electronEffGroup;

  // Zee mass distributions
  std::vector<TH1*> m_hvMass;         
  TH1 * m_hMass;       // Histogram for Mass of Z candidates

  // Efficiencies of LHTight vs pT, eta and phi
  TH1 * m_hIDEt;
  TH1 * m_hIDEta;
  TH1 * m_hIDPhi;
  TProfile * m_effhIDEt;
  TProfile * m_effhIDEta;
  TProfile * m_effhIDPhi;

  // Efficiencies of topoEtCone40 isolation criteria vs pT, eta and phi
  TH1 * m_hISOEt;
  TH1 * m_hISOEta;
  TH1 * m_hISOPhi;
  TProfile * m_effhISOEt;
  TProfile * m_effhISOEta;
  TProfile * m_effhISOPhi;

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

  // Monitoring per lumiblock

  unsigned int m_lumiBlockNumber;
  unsigned int m_nZCandidatesInCurrentLB;
  std::vector<int> m_nZCandidatesPerLumiBlock;
  std::vector<int> m_nZCandidatessPerRegion;
  unsigned int m_nZCandidates;

  TH1 *m_hLB_N; // Histogram for number of electrons vs LB

};

#endif

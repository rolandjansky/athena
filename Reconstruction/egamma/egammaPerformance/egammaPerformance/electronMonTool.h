/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/////////////////////////////////////////////////////////////
//
//      2014-05-21 Author: Remi Lafaye (Annecy) 
//      2015-01-29 Author: Bertrand LAFORGE (LPNHE Paris)
//
/////////////////////////////////////////////////////////////

#ifndef electronMonTool_H
#define electronMonTool_H

#include "egammaPerformance/egammaMonToolBase.h"
#include <vector>

class electronMonTool : public egammaMonToolBase
{
 public:
  
  electronMonTool(const std::string& type, const std::string& name, const IInterface* parent); 

  virtual ~electronMonTool();
  
  virtual StatusCode bookHistograms();
  virtual StatusCode fillHistograms();
     
 private:

 protected:
  // Properties

  std::string m_ElectronContainer; // Container name for electrons

  // Loose electrons histograms
  TH1 *m_hN;                // Histogram for number of electrons
  TH1 *m_hEt;               // Histogram for electron transverse energies
  TH1 *m_hEta;              // Histogram for electron eta
  TH1 *m_hPhi;              // Histogram for electron phi
  TH2 *m_hEtaPhi;           // Histogram for electron eta,phi
  TH1 *m_hTopoEtCone40;     // Histogram for electron isolation energy TopoEtcone40 
  TH1 *m_hPtCone20;         // Histogram for electron isolation energy PtCone20 
  TH1 *m_hTime;         // Histogram for electron cluster time

  // Loose electrons per region histograms
  std::vector<TH1*> m_hvN;   // Histograms for number of electrons
  std::vector<TH1*> m_hvEt;  // Histograms for electron transverse energies
  std::vector<TH1*> m_hvEta; // Histograms for electron transverse eta
  std::vector<TH1*> m_hvPhi; // Histograms for electron transverse phi
  std::vector<TH1*> m_hvDeltaEta1;   // electron track histograms
  std::vector<TH1*> m_hvDeltaPhi2;     
  std::vector<TH1*> m_hvNOfBLayerHits; 
  std::vector<TH1*> m_hvNOfSiHits;
  std::vector<TH1*> m_hvNOfTRTHits;    
  std::vector<TH1*> m_hvNOfTRTHighThresholdHits;    
  std::vector<TH1*> m_hvTopoEtCone40;  // Histograms for electron calo-based isolation transverse energies
  std::vector<TH1*> m_hvPtCone20;  // Histograms for electron track-based isolation transverse energies
  std::vector<TH1*> m_hvTime;  // Histograms for electron times

  // Tight electrons histograms
  TH1 *m_hTightN;            // Histogram for number of electrons
  TH1 *m_hTightEt;           // Histogram for electron transverse energies
  TH1 *m_hTightEta;          // Histogram for electron eta
  TH1 *m_hTightPhi;          // Histogram for electron phi
  TH2 *m_hTightEtaPhi;       // Histogram for electron eta,phi
  TH1 *m_hTightTopoEtCone40; // Histogram for electron isolation energy TopoEtcone40 
  TH1 *m_hTightPtCone20;     // Histogram for electron isolation energy PtCone20 

  // Tight electrons per region histograms
  std::vector<TH1*> m_hvTightN;   // Histograms for number of electrons
  std::vector<TH1*> m_hvTightEt;  // Histograms for electron transverse energies
  std::vector<TH1*> m_hvTightEta; // Histograms for electron transverse eta
  std::vector<TH1*> m_hvTightPhi; // Histograms for electron transverse phi
  std::vector<TH1*> m_hvTightDeltaEta1;   // electron track histograms
  std::vector<TH1*> m_hvTightDeltaPhi2;     
  std::vector<TH1*> m_hvTightNOfBLayerHits; 
  std::vector<TH1*> m_hvTightNOfSiHits;
  std::vector<TH1*> m_hvTightNOfTRTHits;    
  std::vector<TH1*> m_hvTightNOfTRTHighThresholdHits;    

  // electron ID per region histograms
  std::vector<TH1*> m_hvEhad1;         
  std::vector<TH1*> m_hvEoverP;        
  std::vector<TH1*> m_hvCoreEM;        
  std::vector<TH1*> m_hvF1;            
  std::vector<TH1*> m_hvF2;            
  std::vector<TH1*> m_hvF3;            
  std::vector<TH1*> m_hvRe233e237;     
  std::vector<TH1*> m_hvRe237e277;     
};

#endif

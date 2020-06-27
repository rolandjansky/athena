/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

/////////////////////////////////////////////////////////////
//
//      2014-05-21 Author: Remi Lafaye (Annecy) 
//      2015-01-29 Author: Bertrand LAFORGE (LPNHE Paris)
//
/////////////////////////////////////////////////////////////

#ifndef electronMonTool_H
#define electronMonTool_H

#include "AthenaMonitoring/AthenaMonManager.h"
#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/StatusCode.h"
#include "StoreGate/StoreGateSvc.h"
#include "TH1F.h"
#include "TH2F.h"
#include "egammaMonToolBase.h"
#include "xAODEgamma/Electron.h"
#include "xAODEgamma/ElectronContainer.h"
#include "xAODEgamma/ElectronxAODHelpers.h"
#include "xAODEventInfo/EventInfo.h"
#include <iostream>
#include <string>
#include <utility>
#include <vector>

class electronHist
{
 public :

  std::string m_nameOfElectronType;
  bool m_fullHistoList;

  enum electronType
   {
      LhMedium=0,
      CbLoose,
      LhLoose,
      LhTight,
      CbTight,
      NumberOfTypesToMonitor
   };

  // electrons histograms
  TH1 *m_hN;                // Histogram for number of electrons
  TH1 *m_hEt;               // Histogram for electron transverse energies
  TH1 *m_hEta;              // Histogram for electron eta
  TH1 *m_hPhi;              // Histogram for electron phi
  TH2 *m_hEtaPhi;           // Histogram for electron eta,phi
  TH2 *m_hEtaPhi4GeV;       // Histogram for electron eta,phi (only candidates with a pt greater than 4 GeV)
  TH2 *m_hEtaPhi20GeV;      // Histogram for electron eta,phi (only candidates with a pt greater than 20 GeV)
  TH1 *m_hTopoEtCone40;     // Histogram for electron isolation energy TopoEtcone40 
  TH1 *m_hPtCone20;         // Histogram for electron isolation energy PtCone20 
  TH1 *m_hTime;             // Histogram for electron cluster time
  
  // electron ID per region histograms
  std::vector<TH1*> m_hvEhad1;         
  std::vector<TH1*> m_hvEoverP;        
  std::vector<TH1*> m_hvCoreEM;        
  std::vector<TH1*> m_hvF0;            
  std::vector<TH1*> m_hvF1;            
  std::vector<TH1*> m_hvF2;            
  std::vector<TH1*> m_hvF3;            
  std::vector<TH1*> m_hvRe233e237;     
  std::vector<TH1*> m_hvRe237e277;     
  
  // electrons per region histograms
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
  std::vector<TH1*> m_hvd0;
  std::vector<TH1*> m_hvTopoEtCone40;  // Histograms for electron calo-based isolation transverse energies
  std::vector<TH1*> m_hvPtCone20;  // Histograms for electron track-based isolation transverse energies
  std::vector<TH1*> m_hvTime;  // Histograms for electron times

  // Monitoring per lumiblock

  unsigned int m_lumiBlockNumber;
  unsigned int m_nElectronsInCurrentLB;
  std::vector<int> m_nElectronsPerLumiBlock;
  std::vector<int> m_nElectronsPerRegion;
  unsigned int m_nElectrons;

  TH1 *m_hLB_N; // Histogram for number of electrons vs LB

 electronHist(std::string name, bool FullHistoList):
    m_hN (nullptr),
    m_hEt (nullptr),
    m_hEta (nullptr),
    m_hPhi (nullptr),
    m_hEtaPhi (nullptr),
    m_hEtaPhi4GeV (nullptr),
    m_hEtaPhi20GeV (nullptr),
    m_hTopoEtCone40 (nullptr),
    m_hPtCone20 (nullptr),
    m_hTime (nullptr),
    m_lumiBlockNumber(0),
    m_nElectronsInCurrentLB(0),
    m_nElectrons(0),
    m_hLB_N(nullptr)
    {
      m_nameOfElectronType = std::move(name);
      m_fullHistoList = FullHistoList;
    }

    ~electronHist(){
    }
};

class electronMonTool : public egammaMonToolBase
{
 public:
  
  electronMonTool(const std::string& type, const std::string& name, const IInterface* parent); 

  virtual ~electronMonTool();
  
  virtual StatusCode initialize() override;
  virtual StatusCode bookHistograms() override;
  virtual StatusCode bookHistogramsForOneElectronType(electronHist& myHist);

  virtual StatusCode fillHistograms() override;
  virtual StatusCode fillHistogramsForOneElectron(xAOD::ElectronContainer::const_iterator e_iter,
						  electronHist& myHist);

 private:

 protected:
  // Properties

  SG::ReadHandleKey<xAOD::ElectronContainer> m_ElectronContainer{this, "ElectronContainer", "Electrons", "Name of the electron collection"}; // Container name for electrons

  // LH Loose electrons histograms
  electronHist *m_LhLooseElectrons;
  // LH Medium electrons histograms
  electronHist *m_LhMediumElectrons;
  // Medium cut based electrons histograms
  electronHist *m_CbLooseElectrons;
  // LH Tight electrons histograms
  electronHist *m_LhTightElectrons;
  // Cut based Tight electrons histograms
  electronHist *m_CbTightElectrons;

  MonGroup* m_electronGroup; 
  MonGroup* m_electronTrkGroup;
  MonGroup* m_electronIdGroup;
  MonGroup* m_electronLBGroup;

};

#endif

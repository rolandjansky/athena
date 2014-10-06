/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef _EGAMMAANALYSISUTILS_EISOTOOL2012_H_
#define _EGAMMAANALYSISUTILS_EISOTOOL2012_H_

//  ***************************************************************************
//  *                                                                         *
//  *   EisoTool2012 - Providing a uniform electron isolation efficiency      *
//  *                  across Et and Eta for Athena 17 electrons              *
//  *                  2012 data12_8TeV version                               *
//  *                                                                         *
//  *   Author: John Morris (john.morris@cern.ch)                             *
//  *           Queen Mary University of London                               *
//  *                                                                         *
//  *   Use:                                                                  *
//  *       EisoTool2012 m_eiso("_PATH_TO_ROOT_FILE_");                       *
//  *  or   EisoTool2012 m_eiso("_PATH_TO_ROOT_FILE_",isMC);                  *
//  *                                                                         *
//  *  Please use the file share/EisoTool2012_data12_Zee.root                 *
//  *                                                                         *
//  *       Select Etcone, Ptcone or Topocone isolation for your electron     *
//  *       m_eiso.Etcone(cone_size,Efficiency_required,.....)                *
//  *                                                                         *
//  *   If you want 90% isolation effieincy for Etcone20, do:                 *
//  *       if( m_eiso.Etcone(0.2,90,.....) == 1){// Isolation passed}        *
//  *                                                                         *
//  *   If you want 93% isolation effieincy for Ptcone30, do:                 *
//  *       if( m_eiso.Ptcone(0.3,93,.....) == 1){// Isolation passed}        *
//  *                                                                         *
//  *                                                                         *
//  *   Valid Cone sizes are 0.2, 0.3                                         *
//  *   Valid Isolation efficiencies from 80% to 99%                          *
//  *                                                                         *
//  *                                                                         *
//  *   Setup isMC in constructor or by calling:                              *
//  *      m_eiso.SetMC() or m_eiso.SetData()                                 *
//  *   Or choise isMC on a per electron basis with the overlaoded function   *
//  *                                                                         *
//  *                                                                         *
//  ***************************************************************************

#include <TROOT.h>
#include <TFile.h>
#include <TString.h>
#include <TH1.h>
#include <TH3.h>

#include <cmath>
#include <iostream>
using std::cout;
using std::endl;
#include <vector>
using std::vector;

#include "ElectronIsolationSelection/CaloIsoCorrection.h"

class EisoTool2012{
public:
  // Please use either of the top 2 construtors
  explicit EisoTool2012(const TString& inputFileName);
  EisoTool2012(const TString& inputFileName,const bool isMC);

  // These constructors, operators make EisoTool2012 vector safe
  EisoTool2012();
  EisoTool2012(const EisoTool2012& rhs);
  EisoTool2012& operator=(const EisoTool2012& rhs);
  virtual ~EisoTool2012();
  
  // Switch between egamma PIDs
  void SetPID_MediumPP();
  void SetPID_TightPP(); // default - no need to do anything post constructor
  void SetPID_LH_Medium();
  void SetPID_LH_Tight();
  void SetPID_LH_VeryTight();

  inline void SetMC()    {m_isMC = true;}
  inline void SetData()  {m_isMC = false;}


  virtual unsigned int Etcone(const double& cone,
                              const unsigned int& Efficiency_required,
                              const unsigned int& NPV,
                              const double& Electron_Cluster_E,
                              const double& Electron_Track_Eta,
                              const double& Electron_Cluster_Eta,
                              const double& Electron_Etas2,
                              const double& Electron_Etap,
                              const double& Electron_Etcone_Uncorrected,
                              const bool& isMC);

  virtual unsigned int Etcone(const double& cone,
                              const unsigned int& Efficiency_required,
                              const unsigned int& NPV,                              
                              const double& Electron_Cluster_E,
                              const double& Electron_Track_Eta,
                              const double& Electron_Cluster_Eta,
                              const double& Electron_Etas2,
                              const double& Electron_Etap,
                              const double& Electron_Etcone_Uncorrected) const;
                              
  virtual unsigned int Etcone_DivEt(const double& cone,
                                    const unsigned int& Efficiency_required,
                                    const unsigned int& NPV,
                                    const double& Electron_Cluster_E,
                                    const double& Electron_Track_Eta,
                                    const double& Electron_Cluster_Eta,
                                    const double& Electron_Etas2,
                                    const double& Electron_Etap,
                                    const double& Electron_Etcone_Uncorrected,
                                    const bool& isMC);

  virtual unsigned int Etcone_DivEt(const double& cone,
                                    const unsigned int& Efficiency_required,
                                    const unsigned int& NPV,                              
                                    const double& Electron_Cluster_E,
                                    const double& Electron_Track_Eta,
                                    const double& Electron_Cluster_Eta,
                                    const double& Electron_Etas2,
                                    const double& Electron_Etap,
                                    const double& Electron_Etcone_Uncorrected) const;                             

  unsigned int Ptcone(const double& cone,
                      const unsigned int& Efficiency_required,
                      const double& Electron_Cluster_E,
                      const double& Electron_Track_Eta,
                      const double& Electron_Cluster_Eta,
                      const double& Electron_Ptcone) const;
                      
  unsigned int Ptcone_DivEt(const double& cone,
                            const unsigned int& Efficiency_required,
                            const double& Electron_Cluster_E,
                            const double& Electron_Track_Eta,
                            const double& Electron_Cluster_Eta,
                            const double& Electron_Ptcone) const;                      


  virtual unsigned int Topocone(const double& cone,
                                const unsigned int& Efficiency_required,
                                const double& Electron_Cluster_E,
                                const double& Electron_Track_Eta,
                                const double& Electron_Cluster_Eta,
                                const double& Electron_Etas2,
                                const double& Electron_Etap,
                                const double& Electron_ED_Median,
                                const double& Electron_topocone,
                                const bool& isMC);

  virtual unsigned int Topocone(const double& cone,
                                const unsigned int& Efficiency_required,
                                const double& Electron_Cluster_E,
                                const double& Electron_Track_Eta,
                                const double& Electron_Cluster_Eta,
                                const double& Electron_Etas2,
                                const double& Electron_Etap,
                                const double& Electron_ED_Median,
                                const double& Electron_topocone) const;
                                
                                
  virtual unsigned int Topocone_DivEt(const double& cone,
                                      const unsigned int& Efficiency_required,
                                      const double& Electron_Cluster_E,
                                      const double& Electron_Track_Eta,
                                      const double& Electron_Cluster_Eta,
                                      const double& Electron_Etas2,
                                      const double& Electron_Etap,
                                      const double& Electron_ED_Median,
                                      const double& Electron_topocone,
                                      const bool& isMC);

  virtual unsigned int Topocone_DivEt(const double& cone,
                                      const unsigned int& Efficiency_required,
                                      const double& Electron_Cluster_E,
                                      const double& Electron_Track_Eta,
                                      const double& Electron_Cluster_Eta,
                                      const double& Electron_Etas2,
                                      const double& Electron_Etap,
                                      const double& Electron_ED_Median,
                                      const double& Electron_topocone) const;                                
                                
  // MiniIsolation:
  // For MI10_max30_ptsum, use MI=10, max=30
  // For MI15_max40_ptsum, use MI=15, max=40                              
  unsigned int MiniIsolation(const unsigned int& MI,
                             const unsigned int& max,
                             const unsigned int& Efficiency_required,
                             const double& Electron_Cluster_E,
                             const double& Electron_Track_Eta,
                             const double& Electron_Cluster_Eta,
                             const double& MiniIsolationVariable) const;
                             
  unsigned int MiniIsolation_DivEt(const unsigned int& MI,
                                   const unsigned int& max,
                                   const unsigned int& Efficiency_required,
                                   const double& Electron_Cluster_E,
                                   const double& Electron_Track_Eta,
                                   const double& Electron_Cluster_Eta,
                                   const double& MiniIsolationVariable) const;                             
                               

  double getCutValueEtcone(const double& cone,
                           const unsigned int& Efficiency_required,
                           const double& Electron_Cluster_E,
                           const double& Electron_Track_Eta,
                           const double& Electron_Cluster_Eta) const;
                           
  double getCutValueEtcone_DivEt(const double& cone,
                                 const unsigned int& Efficiency_required,
                                 const double& Electron_Cluster_E,
                                 const double& Electron_Track_Eta,
                                 const double& Electron_Cluster_Eta) const;                           

  double getCutValuePtcone(const double& cone,
                           const unsigned int& Efficiency_required,
                           const double& Electron_Cluster_E,
                           const double& Electron_Track_Eta,
                           const double& Electron_Cluster_Eta) const;
                           
  double getCutValuePtcone_DivEt(const double& cone,
                                 const unsigned int& Efficiency_required,
                                 const double& Electron_Cluster_E,
                                 const double& Electron_Track_Eta,
                                 const double& Electron_Cluster_Eta) const;                           

  double getCutValueTopocone(const double& cone,
                             const unsigned int& Efficiency_required,
                             const double& Electron_Cluster_E,
                             const double& Electron_Track_Eta,
                             const double& Electron_Cluster_Eta) const;
                             
  double getCutValueTopocone_DivEt(const double& cone,
                                   const unsigned int& Efficiency_required,
                                   const double& Electron_Cluster_E,
                                   const double& Electron_Track_Eta,
                                   const double& Electron_Cluster_Eta) const;                             
                             
  double getCutValueMiniIsolation(const unsigned int& MI,
                                  const unsigned int& max,
                                  const unsigned int& Efficiency_required,
                                  const double& Electron_Cluster_E,
                                  const double& Electron_Track_Eta,
                                  const double& Electron_Cluster_Eta) const;
                                  
  double getCutValueMiniIsolation_DivEt(const unsigned int& MI,
                                        const unsigned int& max,
                                        const unsigned int& Efficiency_required,
                                        const double& Electron_Cluster_E,
                                        const double& Electron_Track_Eta,
                                        const double& Electron_Cluster_Eta) const;                                  

  double EtconeCorrected(const unsigned int& NPV,
                         const double& Electron_Cluster_E,
                         const double& Electron_Cluster_Eta,
                         const double& Electron_Etas2,
                         const double& Electron_Etap,
                         const double& Electron_Etcone_Uncorrected,
                         const double& cone) const;

  double TopoconeCorrected(const double& Electron_Cluster_E,
                           const double& Electron_Cluster_Eta,
                           const double& Electron_Etas2,
                           const double& Electron_Etap,
                           const double& Electron_ED_Median,
                           const double& Electron_topocone,
                           const double& cone) const;
                           
                           
                           
protected:
  void LoadHistograms(const TString& pid);
  void SetupBinningHistos();
  int FindEtBin(const double& Et) const;
  int FindEtaBin(const double& Eta) const;

  double ElectronEt(const double& Electron_Cluster_E,const double& Electron_Track_Eta) const;
  
  unsigned int PassOrFail(const double& Et,const double& Eta,const unsigned int& Efficiency_required, const double& value,TH3D* histo) const;
  unsigned int PassOrFail_DivEt(const double& Et,const double& Eta,const unsigned int& Efficiency_required, const double& value,TH3D* histo) const;  
  double getCutValue(const double& Et,const double& Eta,const unsigned int& Efficiency_required,TH3D* histo) const;
  
private:
  double m_GeV;
  bool m_isMC;
  
  TString m_inputFileName;
  
  TString s_PID_MediumPP;
  TString s_PID_TightPP;
  TString s_PID_LH_Medium;
  TString s_PID_LH_Tight;
  TString s_PID_LH_VeryTight;
  
  TString s_etconeCor20;
  TString s_etconeCor30;
  TString s_ptcone20;
  TString s_ptcone30;
  TString s_topoconeCor20;
  TString s_topoconeCor30;
  TString s_MI10_max30_ptsum;
  TString s_MI10_max40_ptsum;
  TString s_MI15_max30_ptsum;
  TString s_MI15_max40_ptsum;
  
  TString s_etconeCor20_DivEt;
  TString s_etconeCor30_DivEt;
  TString s_ptcone20_DivEt;
  TString s_ptcone30_DivEt;
  TString s_topoconeCor20_DivEt;
  TString s_topoconeCor30_DivEt;
  TString s_MI10_max30_ptsum_DivEt;
  TString s_MI10_max40_ptsum_DivEt;
  TString s_MI15_max30_ptsum_DivEt;
  TString s_MI15_max40_ptsum_DivEt; 


  TH3D* h_etconeCor20;
  TH3D* h_etconeCor30;
  TH3D* h_ptcone20;
  TH3D* h_ptcone30;
  TH3D* h_topoconeCor20;
  TH3D* h_topoconeCor30;
  TH3D* h_MI10_max30_ptsum;
  TH3D* h_MI10_max40_ptsum;
  TH3D* h_MI15_max30_ptsum;
  TH3D* h_MI15_max40_ptsum;
  
  TH3D* h_etconeCor20_DivEt;
  TH3D* h_etconeCor30_DivEt;
  TH3D* h_ptcone20_DivEt;
  TH3D* h_ptcone30_DivEt;
  TH3D* h_topoconeCor20_DivEt;
  TH3D* h_topoconeCor30_DivEt;
  TH3D* h_MI10_max30_ptsum_DivEt;
  TH3D* h_MI10_max40_ptsum_DivEt;
  TH3D* h_MI15_max30_ptsum_DivEt;
  TH3D* h_MI15_max40_ptsum_DivEt;  
      
  TH1D* h_etBins;
  TH1D* h_etaBins;
};
#endif

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRIGHLTPSVSFSREC_TRIGHLTJETDIAGNOSTICS_H
#define TRIGHLTPSVSFSREC_TRIGHLTJETDIAGNOSTICS_H

#include "TrigInterfaces/HypoAlgo.h"
#include <map>
#include "xAODCaloEvent/CaloClusterContainer.h"
#include "TrigSteeringEvent/TrigRoiDescriptor.h"
#include "xAODJet/JetContainer.h"

#include "CxxUtils/checker_macros.h"
ATLAS_NO_CHECK_FILE_THREAD_SAFETY;  // legacy trigger code

class TH1D;
class TH2D;
class TFile;
class TProfile;
	
class TrigHLTPSvsFSDiagnostics: public HLT::HypoAlgo {
	
public:
  TrigHLTPSvsFSDiagnostics(const std::string& name, ISvcLocator* pSvcLocator);
  ~TrigHLTPSvsFSDiagnostics();

  HLT::ErrorCode hltInitialize();
  HLT::ErrorCode hltFinalize();
  HLT::ErrorCode hltExecute(const HLT::TriggerElement* inputTE, bool& pass);
  
  void addHist(std::map<TString,TH1D*> &hMap, TString tag, TString title, const int &bins, double min, double max);
  void addHist(std::map<TString,TH2D*> &hMap, TString tag, TString title, const int &binsX, double minX, double maxX, const int &binsY, double minY, double maxY);
  void addHist(std::map<TString,TProfile*> &hMap, TString tag, TString title, const int &binsX, double minX, double maxX);
  double fixPhi(double phi);
  
  std::string histStr(std::string name);
  std::string histStr(std::string preName, std::string postname);
  std::string histStr(std::string preName, int number, std::string postname);
  bool loadContainers(const HLT::TriggerElement* inputTE);
  void clustersCheck();
  void jetsCheck();
  
private:
  TFile* m_fOut;
  std::string m_chainName;
  std::map<TString,TH1D*>     m_hMap1D;
  std::map<TString,TH2D*>     m_hMap2D;
  std::map<TString,TProfile*> m_hMapProf;
  
  int          m_event;
  float        m_hypoEt;
  bool         m_haveSuperRoi;
  bool         m_havePSClustContainer;
  bool         m_haveFSClustContainer;
  bool         m_havePSJetContainer;
  bool         m_haveFSJetContainer;
  
  //!-- prepare automated histograms (see source constructor)  -- 
  std::vector<std::string> m_scanType;
  std::vector<std::string> m_objType;
  std::vector<std::string> m_objTitle;
  int m_nEvToPlot;
  
  const xAOD::CaloClusterContainer* m_PSclusterCont; 
  const xAOD::CaloClusterContainer* m_FSclusterCont;
  const xAOD::JetContainer* m_PS_j_container;
  const xAOD::JetContainer* m_FS_j_container;
  const TrigRoiDescriptor* m_superRoi;
  xAOD::JetContainer* m_PS_jetsPassingCont;
  xAOD::JetContainer* m_FS_jetsPassingCont;
  
  void findLeadingJet(const xAOD::JetContainer* jetContainer, xAOD::JetContainer* leadJets);
  void findFocalJets(const xAOD::JetContainer* jetContainer, xAOD::JetContainer* focalJets);
  void findPassJets(const xAOD::JetContainer* jetContainer, xAOD::JetContainer* passJets);
  void fillHists(std::string scanType, std::string objType, xAOD::Jet_v1* jet);
  bool strContains(std::vector<std::string> strList, std::string strToFind);
  double deltaR(double delta_eta, double delta_phi);
  double minDeltaRoI(double eta, double phi);
  std::string compareJets(xAOD::Jet_v1* jet1, xAOD::Jet_v1* jet2);
}; 

#endif

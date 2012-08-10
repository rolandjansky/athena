/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include <string>

#include "AthenaBaseComps/AthFilterAlgorithm.h"

#include "GaudiKernel/MsgStream.h"              // Athena stuff
#include "GaudiKernel/ISvcLocator.h"
#include "GaudiKernel/ToolHandle.h"
#include "GaudiKernel/ObjectVector.h"

#include "StoreGate/StoreGateSvc.h"             // Storegate stuff
#include "StoreGate/DataHandle.h"
#include "AthenaKernel/DefaultKey.h"

#include "GaudiKernel/ToolHandle.h"
#include "GaudiKernel/Algorithm.h"
#include "GaudiKernel/ObjectVector.h"

#include "muonEvent/MuonContainer.h"            // Physics objects containers
#include "egammaEvent/ElectronContainer.h"
#include "MissingETEvent/MissingET.h"
#include "Particle/TrackParticle.h"

#include "JetEvent/Jet.h"
#include "JetEvent/JetCollection.h"

#include "TrigDecisionTool/TrigDecisionTool.h"  // Trigger tool
class TriggerAnalysisTool;

using namespace Analysis;

/////////////////////////////////////////////////////////////////////////////
class WZSkimsMain:public AthFilterAlgorithm {

 public:
   WZSkimsMain (const std::string& name, ISvcLocator* pSvcLocator);
   StatusCode initialize();
   StatusCode execute();
   StatusCode finalize();

   bool CheckCuts();
   bool SetCuts(std::string thisSkim);
   bool performSkim(std::string thisSkim);
   
   std::vector<const Jet*> OpenJets();
   std::vector<const Electron*> OpenElectrons();
   std::vector<const Analysis::Muon*> OpenMuons(std::string l_muonContainerName);

   std::vector<bool> CheckElectronVector(std::vector<const Electron*> AllElectronsVector, double l_electronPtCut, double l_electronEtaCut, std::string l_electronType);
   std::vector<bool> CheckMuonVector(std::vector<const Analysis::Muon*> AllMuonsVector, double l_muonPtCut, double l_muonEtaCut, std::string l_muonType);
   bool CheckLeptonCuts(unsigned int l_electronNumber, unsigned int l_electronNumber2, std::vector<bool> l_GoodElectronsFlag1, std::vector<bool> l_GoodElectronsFlag2);

   struct lepton
   {
      bool medium;
      bool tight;
      double eta;
      double phi;
      double pt;
   };
    
   void ReadMuons(const std::vector<const Analysis::Muon*> & muons, std::vector<struct lepton> & leptons);
   void ReadElectrons(const std::vector<const Electron*> & electrons, std::vector<struct lepton> & leptons);
   bool CheckJetLeptonOverlap(const std::vector<const Jet*> & jets, const std::vector<struct lepton> & leptons);

   bool CheckInvMass(std::vector<const Electron*> ElectronV, std::vector<bool> l_GoodElectronsFlag1, std::vector<bool> l_GoodElectronsFlag2);
   bool CheckInvMass(std::vector<const Analysis::Muon*> MuonV, std::vector<bool> l_GoodMuonsFlag1, std::vector<bool> l_GoodMuonsFlag2);   

   bool Prescale();
   
   bool SelectTrack();
   bool SelectEtmiss(std::string l_missingEtObjectName);
   bool SelectEtmiss();
   bool SelectTrigger();
// bool SelectEvent();

   enum SkimTypes { Jpsiee, Jpsimumu, Zee, Zmumu, Wenu, Wmunu, TagAndProbeElec, TagAndProbeMuon, 
   		    BackgroundElec, BackgroundMuon, Ztautau, TwoElectrons, TwoMuons, 
                    eee, eemu, emumu, mumumu, Wenujj, Wmunujj, Custom };


 private:
   /** a handle on Store Gate for access to the Event Store */
   StoreGateSvc* m_storeGate;
   
   long n_events;
   long n_good_events;
   
   bool m_doSkim;

   bool m_doJpsieeSkim;
   bool m_doJpsimumuSkim;
   bool m_doZeeSkim;
   bool m_doZmumuSkim;
   bool m_doWenuSkim;
   bool m_doWmunuSkim;
   bool m_doZtautauSkim;
   bool m_doTagAndProbeElecSkim;
   bool m_doTagAndProbeMuonSkim;
   bool m_doBackgroundElecSkim;
   bool m_doBackgroundMuonSkim;
   bool m_doZeeWithTagAndProbeSkim;
   bool m_doZmumuWithTagAndProbeSkim;
   bool m_doTwoLeptonsSkim;
   bool m_doThreeLeptonsSkim;
   bool m_doWenujjSkim;
   bool m_doWmunujjSkim;
   bool m_doCustomSkim;

   bool m_skimVec[20]; // 20 = Number of existing skims ; additive skims like doZeeWithTagAndProbeSkim dont count


   std::string m_electronContainerName; 
   std::string m_muonContainerName; 
   std::string m_trackContainerName; 
   std::string m_missingEtObjectName; 
   std::string m_jetContainerName; 
 

   unsigned int m_electronNumber;
   unsigned int m_electronNumber2;
   double m_electronPtCut;   
   double m_electronPtCut2;   
   double m_electronEtaCut;   
   double m_electronEtaCut2;   
   std::string m_electronType;
   std::string m_electronType2;

 
   unsigned int m_muonNumber;
   unsigned int m_muonNumber2;
   double m_muonPtCut;   
   double m_muonPtCut2;   
   double m_muonEtaCut;   
   double m_muonEtaCut2;   
   std::string m_muonType;
   std::string m_muonType2;
 

   unsigned int m_jetNumber;
   double m_jetPtCut;
   double m_jetEtaCut;
   bool m_jetOverlapRemovalWithTight;
   bool m_jetOverlapRemovalWithMedium;
   bool m_jetOverlapRemovalWithMediumIfNoTight;
   std::vector<double> m_jetOverlapEtaRange;
   double m_jetOverlapDrCut;


   bool m_doTrackCut;
   double m_TrackPtCut;
   double m_TrackEtaCut;
 
 
   bool m_doEtmissCut;
   double m_EtmissCut;

   bool m_doInvMassCut;
   double m_Mmin;
   double m_Mmax;
   
   bool m_doTriggerCut;
   std::string m_triggerName;
   std::string m_triggerName2;
   Trig::TrigDecisionTool* m_trigDec;
   
   bool m_doPrescale;
   unsigned int m_prescaleRate;
   unsigned int m_prescaleCounter;
};

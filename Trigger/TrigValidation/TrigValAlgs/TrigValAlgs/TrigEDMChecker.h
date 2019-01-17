/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRIG_EDM_CHECKER_H
#define TRIG_EDM_CHECKER_H

#include "GaudiKernel/ToolHandle.h"
#include "GaudiKernel/ObjectVector.h"
#include "CLHEP/Units/SystemOfUnits.h"

#include "AthAnalysisBaseComps/AthAnalysisAlgorithm.h"

#include "xAODTrigger/TrigCompositeContainer.h"

#include "AthenaKernel/IClassIDSvc.h"

#include "StoreGate/ReadHandleKey.h"
#include "StoreGate/WriteHandleKey.h"
#include "xAODTrigger/TrigNavigation.h"
#include "DecisionHandling/TrigCompositeUtils.h"
#include "TrigNavigation/Navigation.h"
#include "GaudiKernel/ToolHandle.h"

#include "TrigDecisionTool/TrigDecisionTool.h"

#include <string>

// forward declarations of muon track classes used in TrigMuonEFInfo
class TrigMuonEFTrack;
class TrigMuonEFCbTrack;

class IAnalysisTools;

// fwd declare muon printing tool
namespace Rec {
  class IMuonPrintingTool;
}

class TrigEDMChecker : public AthAnalysisAlgorithm  {

 public:

   TrigEDMChecker(const std::string& name, ISvcLocator* pSvcLocator);
   ~TrigEDMChecker();

   StatusCode initialize();
   StatusCode execute();

 private:

   /** a handle on Store Gate for access to the Event Store */
   bool m_doDumpAll;

   bool m_doDumpTrigPassBits;
   StatusCode dumpTrigPassBits();
   bool m_doDumpLVL1_ROI;
  StatusCode dumpLVL1_ROI();

   bool m_doDumpTrackParticleContainer;
   StatusCode dumpTrackParticleContainer();

   bool m_doDumpTrigMissingET;
   StatusCode dumpTrigMissingET();

   bool m_doDumpxAODTrigMissingET;
   StatusCode dumpxAODTrigMissingET();

   bool m_doDumpxAODJetContainer;
   StatusCode dumpxAODJetContainer();

   bool m_doDumpTrigL2BphysContainer;
   StatusCode dumpTrigL2BphysContainer();

   bool m_doDumpTrigEFBphysContainer;
   StatusCode dumpTrigEFBphysContainer();

   bool m_doDumpTrigEFBjetContainer;
   StatusCode dumpTrigEFBjetContainer();

   bool m_doDumpTrigL2BjetContainer;
   StatusCode dumpTrigL2BjetContainer();

   bool m_doDumpTrigTauClusterContainer;
   StatusCode dumpTrigTauClusterContainer();

   bool m_doDumpTrigEMCluster;
   StatusCode dumpTrigEMCluster();

   bool m_doDumpTrigEMClusterContainer;
   StatusCode dumpTrigEMClusterContainer();


   bool m_doDumpxAODTrigEMCluster;
   StatusCode dumpxAODTrigEMCluster();

   bool m_doDumpxAODTrigEMClusterContainer;
   StatusCode dumpxAODTrigEMClusterContainer();

   bool m_doDumpCombinedMuonFeature;
   StatusCode dumpCombinedMuonFeature();
   StatusCode dumpCombinedMuonFeatureContainer();

   bool m_doDumpMuonFeature;
   StatusCode dumpMuonFeature();

   bool m_doDumpTileMuFeature;
   StatusCode dumpTileMuFeatureContainer();

   bool m_doDumpTileTrackMuFeature;
   StatusCode dumpTileTrackMuFeatureContainer();

   bool m_doDumpTrigPhotonContainer;
   StatusCode dumpTrigPhotonContainer();

   bool m_doDumpTrigMuonEFContainer;
   StatusCode dumpTrigMuonEFContainer();

   bool m_doDumpTrigMuonEFInfoContainer;
   StatusCode dumpTrigMuonEFInfoContainer();
   void printMuonTrk(const TrigMuonEFTrack* muonTrack);
   void printMuonTrk(const TrigMuonEFCbTrack* muonTrack);

   bool m_doDumpxAODMuonContainer;
   StatusCode dumpxAODMuonContainer();

   bool m_doDumpTrigMuonEFIsolationContainer;
   StatusCode dumpTrigMuonEFIsolationContainer();

   bool m_doDumpTrigElectronContainer;
   StatusCode dumpTrigElectronContainer();

   bool m_doDumpxAODTrigElectronContainer;
   StatusCode dumpxAODTrigElectronContainer();
   
   bool m_doDumpxAODTrigPhotonContainer;
   StatusCode dumpxAODTrigPhotonContainer();
   
   bool m_doDumpxAODElectronContainer;
   StatusCode dumpxAODElectronContainer();
   
   bool m_doDumpxAODPhotonContainer;
   StatusCode dumpxAODPhotonContainer();
   
   bool m_doDumpTrigTauContainer;
   StatusCode dumpTrigTauContainer();

   bool m_doDumpTrigTauTracksInfo;
   StatusCode dumpTrigTauTracksInfo();

   bool m_doDumpHLTResult;
   StatusCode dumpHLTResult();

   bool m_doDumpTrigInDetTrackCollection;
   StatusCode dumpTrigInDetTrackCollection();

   bool m_doDumpTrigVertexCollection;
   StatusCode dumpTrigVertexCollection();

   bool m_doDumpxAODTauJetContainer;
   StatusCode dumpxAODTauJetContainer();
 
   bool m_doDumpTauJetContainer;
   StatusCode dumpTauJetContainer();

   bool m_doDumpxAODTrackParticle;
   StatusCode dumpxAODTrackParticle();

   bool m_doDumpxAODVertex;
   StatusCode dumpxAODVertex();

   bool m_doDumpStoreGate;
   StatusCode dumpStoreGate();

   bool m_doTDTCheck;
   StatusCode dumpTDT();

   bool m_doDumpxAODTrigMinBias;
   StatusCode dumpxAODTrigMinBias();
   void dumpTrigSpacePointCounts();
   void dumpTrigT2MBTSBits();
   void dumpTrigVertexCounts();
   void dumpTrigTrackCounts();

   bool m_doDumpAllTrigComposite;
   std::vector<std::string> m_dumpTrigCompositeContainers;

   bool m_doDumpNavigation;
   StatusCode dumpNavigation();

   /**
    * @brief Dump information on TrigComposite collections
    *
    * Only dumpTrigCompositeContainers are dumped unless doDumpAllTrigComposite is set
    */
   StatusCode dumpTrigComposite();

   /**
    * @brief Dump details on element links within TrigComposites
    *
    * With specific checking of the Run-3 relationships
    */
   StatusCode checkTrigCompositeElementLink(const xAOD::TrigComposite* tc, size_t element); 

   bool m_doDumpTrigCompsiteNavigation;

   /**
    * @brief Construct graph of HLT navigation in Run-3
    * @param returnValue String to populate with dot graph.
    *
    * Navigates all TrigComposite objects in store gate and forms a relational graph in the dot format
    */
   StatusCode TrigCompositeNavigationToDot(std::string& returnValue);

   ToolHandle<Rec::IMuonPrintingTool>            m_muonPrinter;

   ServiceHandle< ::IClassIDSvc > m_clidSvc;

   SG::ReadHandleKey< xAOD::TrigNavigation > m_navigationHandleKey{ this, "TrigNavigation", "TrigNavigation", "" };
   SG::WriteHandleKey<TrigCompositeUtils::DecisionContainer> m_decisionsKey{ this, "Decisions", "RoIDecisions", "Decisions created from TEs" };
   ToolHandle< HLT::Navigation > m_navigationTool{ this, "NavigationTool", "HLT::Navigation/Navigation", "" };
   ToolHandle< Trig::TrigDecisionTool > m_trigDec{ this, "TriggerDecisionTool", "Trig::TrigDecisionTool/TrigDecisionTool", ""};
};

#endif // TRIG_EDM_CHECKER_H


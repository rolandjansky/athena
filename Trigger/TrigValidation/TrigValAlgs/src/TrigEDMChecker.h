/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRIG_EDM_CHECKER_H
#define TRIG_EDM_CHECKER_H

#include "GaudiKernel/ToolHandle.h"
#include "GaudiKernel/ObjectVector.h"

#include "AthAnalysisBaseComps/AthAnalysisAlgorithm.h"
#include "CxxUtils/checker_macros.h"

#include "xAODTrigger/TrigCompositeContainer.h"

#include "xAODTracking/TrackParticleContainer.h"

#include "GaudiKernel/IClassIDSvc.h"

#include "StoreGate/ReadHandleKey.h"
#include "StoreGate/WriteHandleKey.h"
#include "xAODTrigger/TrigNavigation.h"
#include "TrigCompositeUtils/TrigCompositeUtils.h"
#include "TrigNavigation/Navigation.h"

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
   virtual ~TrigEDMChecker();

   virtual StatusCode initialize() override;
   virtual StatusCode execute() override;

 private:
   StatusCode do_execute ATLAS_NOT_THREAD_SAFE();

   /** a handle on Store Gate for access to the Event Store */
   bool m_doDumpAll;

   bool m_doDumpTrigPassBits;
   StatusCode dumpTrigPassBits();
   bool m_doDumpLVL1_ROI;
  StatusCode dumpLVL1_ROI();

   bool m_doDumpTrackParticleContainer;
   StatusCode dumpTrackParticleContainer();

   bool m_doDumpTrigMissingET;
   StatusCode dumpTrigMissingET ATLAS_NOT_THREAD_SAFE();

   bool m_doDumpxAODTrigMissingET;
   StatusCode dumpxAODTrigMissingET();

   bool m_doDumpxAODJetContainer;
   StatusCode dumpxAODJetContainer();

   bool m_doDumpTrigL2BphysContainer;
   StatusCode dumpTrigL2BphysContainer();

   bool m_doDumpTrigEFBphysContainer;
   StatusCode dumpTrigEFBphysContainer();

   bool m_doDumpTrigEFBjetContainer;
   StatusCode dumpTrigEFBjetContainer ATLAS_NOT_THREAD_SAFE();

   bool m_doDumpTrigL2BjetContainer;
   StatusCode dumpTrigL2BjetContainer ATLAS_NOT_THREAD_SAFE();

   bool m_doDumpTrigTauClusterContainer;
   StatusCode dumpTrigTauClusterContainer ATLAS_NOT_THREAD_SAFE();

   bool m_doDumpTrigEMCluster;
   StatusCode dumpTrigEMCluster ATLAS_NOT_THREAD_SAFE();

   bool m_doDumpTrigEMClusterContainer;
   StatusCode dumpTrigEMClusterContainer ATLAS_NOT_THREAD_SAFE();


   bool m_doDumpxAODTrigEMCluster;
   StatusCode dumpxAODTrigEMCluster ATLAS_NOT_THREAD_SAFE();

   bool m_doDumpxAODTrigEMClusterContainer;
   StatusCode dumpxAODTrigEMClusterContainer ATLAS_NOT_THREAD_SAFE();

   bool m_doDumpCombinedMuonFeature;
   StatusCode dumpCombinedMuonFeature ATLAS_NOT_THREAD_SAFE();
   StatusCode dumpCombinedMuonFeatureContainer ATLAS_NOT_THREAD_SAFE();

   bool m_doDumpMuonFeature;
   StatusCode dumpMuonFeature ATLAS_NOT_THREAD_SAFE();

   bool m_doDumpTileMuFeature;
   StatusCode dumpTileMuFeatureContainer ATLAS_NOT_THREAD_SAFE();

   bool m_doDumpTileTrackMuFeature;
   StatusCode dumpTileTrackMuFeatureContainer ATLAS_NOT_THREAD_SAFE();

   bool m_doDumpTrigPhotonContainer;
   StatusCode dumpTrigPhotonContainer ATLAS_NOT_THREAD_SAFE();

   bool m_doDumpTrigMuonEFContainer;
   StatusCode dumpTrigMuonEFContainer ATLAS_NOT_THREAD_SAFE();

   bool m_doDumpTrigMuonEFInfoContainer;
   StatusCode dumpTrigMuonEFInfoContainer ATLAS_NOT_THREAD_SAFE();
   void printMuonTrk(const TrigMuonEFTrack* muonTrack);
   void printMuonTrk(const TrigMuonEFCbTrack* muonTrack);

   bool m_doDumpxAODMuonContainer;
   StatusCode dumpxAODMuonContainer();

   bool m_doDumpTrigMuonEFIsolationContainer;
   StatusCode dumpTrigMuonEFIsolationContainer ATLAS_NOT_THREAD_SAFE();

   bool m_doDumpTrigElectronContainer;
   StatusCode dumpTrigElectronContainer ATLAS_NOT_THREAD_SAFE();

   bool m_doDumpxAODTrigElectronContainer;
   StatusCode dumpxAODTrigElectronContainer();
   
   bool m_doDumpxAODTrigPhotonContainer;
   StatusCode dumpxAODTrigPhotonContainer();
   
   bool m_doDumpxAODElectronContainer;
   StatusCode dumpxAODElectronContainer();
   
   bool m_doDumpxAODPhotonContainer;
   StatusCode dumpxAODPhotonContainer();
   
   bool m_doDumpTrigTauContainer;
   StatusCode dumpTrigTauContainer ATLAS_NOT_THREAD_SAFE();

   bool m_doDumpTrigTauTracksInfo;
   StatusCode dumpTrigTauTracksInfo ATLAS_NOT_THREAD_SAFE();

   bool m_doDumpHLTResult;
   StatusCode dumpHLTResult();

   bool m_doDumpTrigInDetTrackCollection;
   StatusCode dumpTrigInDetTrackCollection();

   bool m_doDumpTrigVertexCollection;
   StatusCode dumpTrigVertexCollection ATLAS_NOT_THREAD_SAFE();

   bool m_doDumpxAODTauJetContainer;
   StatusCode dumpxAODTauJetContainer ATLAS_NOT_THREAD_SAFE();
 
   bool m_doDumpTauJetContainer;
   StatusCode dumpTauJetContainer ATLAS_NOT_THREAD_SAFE ();

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
   Gaudi::Property<std::string> m_dumpNavForChain {this, "DumpNavigationForChain", "", "Optional chain to restrict navigation dump info."};
   Gaudi::Property<bool> m_excludeFailedHypoNodes {this, "excludeFailedHypoNodes", false,
    "Optional flag to exclude nodes which fail the hypothesis tool for a chain when dumping navigation graphs."};

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
    * @param pass When using a chain filter, if the chain group passed raw.
    *
    * Navigates all TrigComposite objects in store gate and forms a relational graph in the dot format
    */
   StatusCode TrigCompositeNavigationToDot(std::string& returnValue, bool& pass);

   ToolHandle<Rec::IMuonPrintingTool>            m_muonPrinter;

   ServiceHandle< ::IClassIDSvc > m_clidSvc;

   SG::ReadHandleKey< xAOD::TrackParticleContainer > m_muonTracksKey{ this, "MuonTracksKey", "HLT_IDTrack_Muon_FTF"};
   SG::ReadHandleKey< xAOD::TrigNavigation > m_navigationHandleKey{ this, "TrigNavigation", "TrigNavigation", "" };
   SG::WriteHandleKey<TrigCompositeUtils::DecisionContainer> m_decisionsKey{ this, "Decisions", "RoIDecisions", "Decisions created from TEs" };
   ToolHandle< HLT::Navigation > m_navigationTool{ this, "NavigationTool", "HLT::Navigation/Navigation", "" };
   ToolHandle< Trig::TrigDecisionTool > m_trigDec{ this, "TriggerDecisionTool", "Trig::TrigDecisionTool/TrigDecisionTool", ""};

   int m_trackWarningNum{0};
   int m_vertexWarningNum{0};
};

#endif // TRIG_EDM_CHECKER_H


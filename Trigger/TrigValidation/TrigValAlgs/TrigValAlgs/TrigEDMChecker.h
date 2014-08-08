/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRIG_EDM_CHECKER_H
#define TRIG_EDM_CHECKER_H

#include "GaudiKernel/ToolHandle.h"
#include "GaudiKernel/ObjectVector.h"
#include "CLHEP/Units/SystemOfUnits.h"
#include "StoreGate/StoreGateSvc.h"

#include "AthenaBaseComps/AthAlgorithm.h"

//#include "CBNT_Utils/CBNT_AthenaAwareBase.h"

#include <string>

// forward declarations of muon track classes used in TrigMuonEFInfo
class TrigMuonEFTrack;
class TrigMuonEFCbTrack;

class IAnalysisTools;

// fwd declare muon printing tool
namespace Rec {
  class IMuonPrintingTool;
}

class TrigEDMChecker : public AthAlgorithm  {

 public:

   TrigEDMChecker(const std::string& name, ISvcLocator* pSvcLocator);
   ~TrigEDMChecker();

   StatusCode initialize();
   StatusCode finalize();
   StatusCode execute();
   //   StatusCode CBNT_clear();



 private:


   /** get a handle to the tool helper */
   //   IAnalysisTools * m_analysisTools;


   /** a handle on Store Gate for access to the Event Store */
   StoreGateSvc* m_storeGate;

   bool doDumpAll;

   bool doDumpLVL1_ROI;
  StatusCode dumpLVL1_ROI();

   bool doDumpTrackParticleContainer;
   StatusCode dumpTrackParticleContainer();

   bool doDumpTrigMissingET;
   StatusCode dumpTrigMissingET();

   bool doDumpxAODTrigMissingET;
   StatusCode dumpxAODTrigMissingET();

   bool doDumpxAODJetContainer;
   StatusCode dumpxAODJetContainer();

   bool doDumpTrigL2BphysContainer;
   StatusCode dumpTrigL2BphysContainer();

   bool doDumpTrigEFBphysContainer;
   StatusCode dumpTrigEFBphysContainer();

   bool doDumpTrigEFBjetContainer;
   StatusCode dumpTrigEFBjetContainer();

   bool doDumpTrigL2BjetContainer;
   StatusCode dumpTrigL2BjetContainer();

   bool doDumpTrigTauClusterContainer;
   StatusCode dumpTrigTauClusterContainer();

   bool doDumpTrigEMCluster;
   StatusCode dumpTrigEMCluster();

   bool doDumpTrigEMClusterContainer;
   StatusCode dumpTrigEMClusterContainer();


   bool doDumpxAODTrigEMCluster;
   StatusCode dumpxAODTrigEMCluster();

   bool doDumpxAODTrigEMClusterContainer;
   StatusCode dumpxAODTrigEMClusterContainer();

   bool doDumpCombinedMuonFeature;
   StatusCode dumpCombinedMuonFeature();
   StatusCode dumpCombinedMuonFeatureContainer();

   bool doDumpMuonFeature;
   StatusCode dumpMuonFeature();

   bool doDumpTileMuFeature;
   StatusCode dumpTileMuFeatureContainer();

   bool doDumpTileTrackMuFeature;
   StatusCode dumpTileTrackMuFeatureContainer();

   bool doDumpTrigPhotonContainer;
   StatusCode dumpTrigPhotonContainer();

   bool doDumpTrigMuonEFContainer;
   StatusCode dumpTrigMuonEFContainer();

   bool doDumpTrigMuonEFInfoContainer;
   StatusCode dumpTrigMuonEFInfoContainer();
   void printMuonTrk(MsgStream &mLog, TrigMuonEFTrack* muonTrack);
   void printMuonTrk(MsgStream &mLog, TrigMuonEFCbTrack* muonTrack);

   bool doDumpxAODMuonContainer;
   StatusCode dumpxAODMuonContainer();

   bool doDumpTrigMuonEFIsolationContainer;
   StatusCode dumpTrigMuonEFIsolationContainer();

   bool doDumpTrigElectronContainer;
   StatusCode dumpTrigElectronContainer();

   bool doDumpxAODTrigElectronContainer;
   StatusCode dumpxAODTrigElectronContainer();
   
   bool doDumpxAODTrigPhotonContainer;
   StatusCode dumpxAODTrigPhotonContainer();
   
   bool doDumpxAODElectronContainer;
   StatusCode dumpxAODElectronContainer();
   
   bool doDumpxAODPhotonContainer;
   StatusCode dumpxAODPhotonContainer();
   
   bool doDumpTrigTauContainer;
   StatusCode dumpTrigTauContainer();

   bool doDumpTrigTauTracksInfo;
   StatusCode dumpTrigTauTracksInfo();

   bool doDumpHLTResult;
   StatusCode dumpHLTResult();

   bool doDumpTrigInDetTrackCollection;
   StatusCode dumpTrigInDetTrackCollection();

   bool doDumpTrigVertexCollection;
   StatusCode dumpTrigVertexCollection();

   bool doDumpxAODTauJetContainer;
   StatusCode dumpxAODTauJetContainer();
 
   bool doDumpTauJetContainer;
   StatusCode dumpTauJetContainer();

   bool doDumpxAODTrackParticle;
   StatusCode dumpxAODTrackParticle();

   bool doDumpxAODVertex;
   StatusCode dumpxAODVertex();

   bool doDumpxAODTrigMinBias;
   StatusCode dumpxAODTrigMinBias();
   void dumpTrigSpacePointCounts(MsgStream &mLog);
   void dumpTrigT2MBTSBits(MsgStream &mLog);
   void dumpTrigVertexCounts(MsgStream &mLog);
   void dumpTrigTrackCounts(MsgStream &mLog);

   ToolHandle<Rec::IMuonPrintingTool>            m_muonPrinter;

};

#endif // TRIG_EDM_CHECKER_H


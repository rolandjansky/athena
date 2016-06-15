/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRIG_EDM_CHECKER_H
#define TRIG_EDM_CHECKER_H

#include "GaudiKernel/ToolHandle.h"
#include "GaudiKernel/ObjectVector.h"
#include "CLHEP/Units/SystemOfUnits.h"

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
   void printMuonTrk(MsgStream &mLog, const TrigMuonEFTrack* muonTrack);
   void printMuonTrk(MsgStream &mLog, const TrigMuonEFCbTrack* muonTrack);

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

   bool m_doDumpxAODTrigMinBias;
   StatusCode dumpxAODTrigMinBias();
   void dumpTrigSpacePointCounts(MsgStream &mLog);
   void dumpTrigT2MBTSBits(MsgStream &mLog);
   void dumpTrigVertexCounts(MsgStream &mLog);
   void dumpTrigTrackCounts(MsgStream &mLog);

   ToolHandle<Rec::IMuonPrintingTool>            m_muonPrinter;

};

#endif // TRIG_EDM_CHECKER_H


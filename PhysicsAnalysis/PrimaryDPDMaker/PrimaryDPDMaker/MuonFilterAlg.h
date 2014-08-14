/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MUONFILTERALG_H
#define MUONFILTERALG_H

/** 
 *  @brief This file contains the class definition for the MuonFilterAlg class.
 *  @author David Lopez Mateos <David.Lopez@cern.ch>
 **/
/** mantained by claudio.gatti@lnf.infn.it since 2009 **/

#include <string>
#include <vector>
#include "AthenaBaseComps/AthFilterAlgorithm.h"
#include "GaudiKernel/ToolHandle.h"
#include "NavFourMom/INavigable4MomentumCollection.h"


// forward declarations
namespace Rec
{
  class TrackParticle;
  class TrackParticleContainer;
}
class JetCollection;
//class IIsolationTool;
class INavigable4Momentum;
//class INavigable4MomentumCollection;
class IExtrapolateToCaloTool;
//class IIsolationTool;
namespace Reco
{
  class ITrackToVertex;
}
namespace Analysis
{
  class Muon;
  class MuonContainer;
}



class MuonFilterAlg : public AthFilterAlgorithm {
public: // Constructor and Destructor
   /// Standard Service Constructor
   MuonFilterAlg(const std::string& name, ISvcLocator* pSvcLocator);
   /// Destructor
   virtual ~MuonFilterAlg();

public:
   /// Gaudi Service Interface method implementations:
   StatusCode initialize();
   StatusCode execute();
   StatusCode finalize();

private:

   bool hasAnyMuon(const INavigable4MomentumCollection* muons);
   bool hasIDActivity();

   bool passCaloIsol(const     Analysis::Muon* muon);
   bool passNoTracksIsol(const Analysis::Muon* muon);
   bool passPtTracksIsol(const Analysis::Muon* muon);
   
   bool passCaloIsol(const Rec::TrackParticle* muon);
   bool passNoTracksIsol(const Rec::TrackParticle* muon);
   bool passPtTracksIsol(const Rec::TrackParticle* muon);
   bool passJetIsol(const Rec::TrackParticle* muon);
   bool checkMuonAuthor(const Analysis::Muon* muon, std::string authorName);
   bool passd0z0Cut(const Rec::TrackParticle* muon);
   double deltaR(const INavigable4Momentum* part1, const Rec::TrackParticle* part2);
   const Rec::TrackParticle* getTrackParticle(const Analysis::Muon* muon, std::string authorName, std::string trackType);


   int m_nProcessed;
   int m_nMuons;
   int m_nEventMuPassEta;
   int m_nEventMuPassPt;
   int m_nEventMuPassAuthor;
   int m_nEventMuPassChi2;
   int m_nEventMuPassIsolation;
   int m_nEventMuPassTriggerHits;
   int m_nEventMuPassPrecisionHits;
   int m_nEventMuPassIDHits;
   int m_nEventMuPassd0z0Cut;
   int m_nEventMuPassIDActivity;
   int m_nEventMuPassHasMuon;

   const Rec::TrackParticleContainer* m_IDtrackCollection;
   const JetCollection* m_jetCollection;
   //ToolHandle<IIsolationTool> m_muonIsolationTool;

   double              m_cutPtMinMu;
   double              m_cutChi2;
   std::string         m_muonAuthors;
   std::string         m_muonContainer;
   std::string         m_trackType;
   double              m_cutEtaMax;
   bool                m_passAll;
   bool                m_hasIDActivity;
   std::string         m_IDtrackContainerName;
   bool                m_doNoTrackIsol;
   int                 m_noTrackIsol;
   double              m_innerNoTrackRadius;
   double              m_outerNoTrackRadius;
   bool                m_doPtTrackIsol;
   double              m_ptTrackIsol;
   double              m_innerPtTrackRadius;
   double              m_outerPtTrackRadius;
   bool                m_doCaloIsol;
   double              m_caloIsol;
   double              m_innerCaloRadius;
   double              m_outerCaloRadius;
   bool                m_doJetIsol;
   double              m_jetIsolEt;
   double              m_jetDistance;
   std::string         m_jetContainerName;
   int                 m_minRPCHits;
   int                 m_minMDTHits;
   int                 m_minPrecisionHits;
   int                 m_minTGCHits;
   int                 m_minSCTHits;
   int                 m_minTRTHits;
   int                 m_minPIXELHits;   
   int                 m_minTOTSIHits;   
   int                 m_minTotTrigHits;   
   bool                m_doz0Cut;
   float               m_z0MaxValue;
   bool                m_dod0Cut;
   float               m_d0MaxValue;
   bool                m_useTrackContainer;
   bool                m_useIsolationFromContainer;

};


#endif

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRACKFILTERALG_H
#define TRACKFILTERALG_H

/** 
 *  @brief This file contains the class definition for the TrackFilterAlg class.
 *  @author Eric Feng <Eric.Feng@cern.ch>
 *  Based on:  MuonFilterAlg by David Lopez Mateos <David.Lopez@cern.ch>
 *  $Id: TrackFilterAlg.h,v 1.2 2008-10-22 12:23:01 efeng Exp $
 **/

#include <string>
#include <vector>
//#include "GaudiKernel/Algorithm.h"
//#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/ToolHandle.h"

#include "AthenaBaseComps/AthFilterAlgorithm.h"

//#include "Particle/TrackParticle.h"
//#include "JetEvent/JetCollection.h"
//#include "Particle/TrackParticleContainer.h"
//#include "IsolationTool/IIsolationTool.h"
//#include "EventKernel/INavigable4Momentum.h"

//#include "StoreGate/StoreGateSvc.h"

// SingleTracks
//#include "RecoToolInterfaces/IExtrapolateToCaloTool.h"
//#include "ITrackToVertex/ITrackToVertex.h"
// SingleTracks end

// forward declarations
namespace Rec
{
  class TrackParticle;
  class TrackParticleContainer;
}
class JetCollection;
//class IIsolationTool;
class INavigable4Momentum;
class IExtrapolateToCaloTool;
namespace Reco
{
  class ITrackToVertex;
}


class TrackFilterAlg : public AthFilterAlgorithm {
public: // Constructor and Destructor
   /// Standard Service Constructor
   TrackFilterAlg(const std::string& name, ISvcLocator* pSvcLocator);
   /// Destructor
   virtual ~TrackFilterAlg();

public:
   /// Gaudi Service Interface method implementations:
   StatusCode initialize();
   StatusCode execute();
   StatusCode finalize();

private:

   bool hasAnyTrack(const Rec::TrackParticleContainer* tracks);
   bool passCaloIsol(const Rec::TrackParticle* track);
   bool passNoTracksIsol(const Rec::TrackParticle* track);
   bool passPtTracksIsol(const Rec::TrackParticle* track);
   bool passJetIsol(const Rec::TrackParticle* track);
   double deltaR(const INavigable4Momentum* part1, const Rec::TrackParticle* part2);
   const Rec::TrackParticle* getTrackParticle(Rec::TrackParticle* track, std::string trackType);

   // StoreGate service pointer
   StoreGateSvc* m_storeGate;

   // Message stream
   MsgStream m_log;

   int m_nProcessed;
   int m_nTracks;
   int m_nEventTrPassEta;
   int m_nEventTrPassPt;
   int m_nEventTrPassSumPt;
   int m_nEventTrPassIsolation;
   int m_nEventTrPassChi2DOF;
   int m_nEventTrPassBLayerHits;
   int m_nEventTrPassHoles;
   int m_nEventTrPassSiHits;

   const JetCollection* m_jetCollection;
   //ToolHandle<IIsolationTool> m_trackIsolationTool;

// SingleTracks
   // track to calo extrapolation tool
   ToolHandle<IExtrapolateToCaloTool> m_toCalo;
   ToolHandle<Reco::ITrackToVertex> m_toVertex;
   
   int m_nEventPassNumberOfPrimaryVertexes;
 
   int m_nEventTrPassCaloEta;
   int m_nEventTrPassTrackD0;
   int m_nEventTrPassTrackZ0;
   int m_nEventTrPassSCTHits;
   int m_nEventTrPassPixelHits;
// SingleTracks end   

   double              m_cutEtaMax;
   double              m_cutPtMinTr;
   double              m_cutSumPtRatio;     
   double              m_cutChi2DOF;
   double              m_cutBLayerHits;
   double              m_cutSiHits;
   double              m_cutHoles;
   std::string         m_trackContainer;
   bool                m_passAll;
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

// SingleTracks
   std::string         m_vertexContainerName;
   bool                m_useEPFilter;
   double              m_TrackAtCaloEta;
   double              m_TrackD0;
   double              m_TrackZ0;
   int                 m_TrackNumberOfSCTHits;
   int                 m_TrackNumberOfPixelHits;
   unsigned int        m_NumberOfPrimaryVertexes;
   double              m_trackIsoDR;
// SingleTracks end   

};


#endif

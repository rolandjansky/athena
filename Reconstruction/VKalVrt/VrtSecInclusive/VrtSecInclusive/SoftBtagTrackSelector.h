/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/

// SoftBtagTrackSelector.h
//
#ifndef _VrtSecInclusive_SoftBtagTrackSelector
#define _VrtSecInclusive_SoftBtagTrackSelector

#include "VrtSecInclusive/Constants.h"

#include "AthenaBaseComps/AthAlgorithm.h"

// Gaudi includes
#include "GaudiKernel/ToolHandle.h"
#include "GaudiKernel/ServiceHandle.h"
#include "GaudiKernel/ITHistSvc.h"
#include "StoreGate/StoreGateSvc.h"
//
/* #include "TrkVKalVrtFitter/TrkVKalVrtFitter.h" */

// for truth
/* #include "GeneratorObjects/McEventCollection.h" */

// xAOD Classes
#include "xAODEventInfo/EventInfo.h"
#include "xAODTracking/TrackParticleContainer.h"
#include "xAODTracking/TrackParticleAuxContainer.h"
#include "xAODTracking/TrackParticle.h"
#include "xAODTruth/TruthParticleContainer.h"
#include "xAODTruth/TruthEventContainer.h"
#include "xAODTruth/TruthVertexContainer.h"
#include "xAODJet/JetContainer.h"

// Normal STL and physical vectors
/* #include <vector> */
/* #include <deque> */
/* #include <functional> */

#include "TH1.h"
/* #include "TH2.h" */

/** Forward declarations **/

namespace VKalVrtAthena {

  class SoftBtagTrackSelector : public AthAlgorithm {
  public:
    /** Standard Athena-Algorithm Constructor */
    SoftBtagTrackSelector(const std::string& name, ISvcLocator* pSvcLocator);

    /** Default Destructor */
    ~SoftBtagTrackSelector();

    virtual StatusCode beginRun()   override;
    virtual StatusCode initialize() override;
    virtual StatusCode finalize()   override;
    virtual StatusCode execute()    override;

    virtual StatusCode initEvent();

  private:

    /////////////////////////////////////////////////////////
    //
    //  Member Variables
    //

    struct JobProperties {

      std::string          inputTrackContainer;
      std::string          outputTrackContainer;
      std::string          jetContainer;
      std::string          trackJetContainer;

      bool                 FillHist;
      bool                 StoreSelectedTracks;

      double               pTMinCut;
      double               pTMaxCut;

      double               etaMinCut;
      double               etaMaxCut;

      double               chi2MaxCut;

      double               d0MinCut;
      double               d0MaxCut;
      double               z0MinCut;
      double               z0MaxCut;

      double               d0errMaxCut;
      double               z0errMaxCut;

      double               z0wrtPVMinCut;
      double               z0wrtPVMaxCut;
      double               z0wrtClosestVtxMinCut;

      double               z0sinThetaMinCut;
      double               z0sinThetaMaxCut;

      double               d0signifMinCut;
      double               z0signifMinCut;
      double               z0signifwrtPVMinCut;

      double jetDeltaRCut;

      double jetPtCut;

      int  CutPixelHits;
      int  CutSctHits;
      int  CutTRTHits;
      int  CutSiHits;
      int  CutBLayHits;
      int  CutSharedHits;

      double truthSVMinDist;
      bool useTruthMatchedTrack;
      bool skipTrackSelection;

    };

    bool m_isMC;

    std::map<std::string, double> m_trackVariables;

    struct JobProperties m_jp;

    // xAOD Accessors
    const xAOD::VertexContainer*  m_primaryVertices;
    const xAOD::JetContainer*     m_jets;
    const xAOD::JetContainer*     m_trackJets;
    xAOD::Vertex*                 m_thePV;
    const xAOD::TruthVertex*      m_truthPV;

    // Histograms for stats
    std::map<std::string, TH1*> m_hists;

    ////////////////////////////////////////////////////////////////////////////////////////
    //
    // Private member functions
    //
    void clearTrackVariables();

    // for event info to new ntuple (used to go by default in CollectionTree)
    void declareProperties();

    StatusCode processPrimaryVertices();
    StatusCode processJets();

    using TrackSelectionAlg = bool (SoftBtagTrackSelector::*)(const xAOD::TrackParticle*);
    std::map<std::string,TrackSelectionAlg> m_trackSelectionAlgs;
    std::map<std::string,bool> m_flags;

    /** track-by-track selection strategies */
    bool selectTrack_pTCut              ( const xAOD::TrackParticle* );
    bool selectTrack_etaCut             ( const xAOD::TrackParticle* );
    bool selectTrack_d0Cut              ( const xAOD::TrackParticle* );
    bool selectTrack_d0errCut           ( const xAOD::TrackParticle* );
    bool selectTrack_d0signifCut        ( const xAOD::TrackParticle* );
    bool selectTrack_z0Cut              ( const xAOD::TrackParticle* );
    bool selectTrack_z0errCut           ( const xAOD::TrackParticle* );
    bool selectTrack_z0wrtPVCut         ( const xAOD::TrackParticle* );
    bool selectTrack_z0signifwrtPVCut   ( const xAOD::TrackParticle* );
    bool selectTrack_z0sinThetaCut      ( const xAOD::TrackParticle* );
    bool selectTrack_chi2Cut            ( const xAOD::TrackParticle* );
    bool selectTrack_JetDeltaRCut       ( const xAOD::TrackParticle* );
    bool selectTrack_pixelHits          ( const xAOD::TrackParticle* );
    bool selectTrack_sctHits            ( const xAOD::TrackParticle* );
    bool selectTrack_bLayHits           ( const xAOD::TrackParticle* );
    bool selectTrack_sharedHits         ( const xAOD::TrackParticle* );
    bool selectTrack_siHits             ( const xAOD::TrackParticle* );

    int TrackTruthMatchParent(const xAOD::TrackParticle* track);
    bool TrackTruthMatchFromSV(const xAOD::TrackParticle* track);
    void FillHists(const bool isFromSV);

  };

} // end of namespace bracket


#endif /* _SoftBtagTrackSelector_SoftBtagTrackSelector */

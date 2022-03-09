/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/
#ifndef TRIGTOOLS_TRIGVRTSECINCLUSIVE_H
#define TRIGTOOLS_TRIGVRTSECINCLUSIVE_H

#include "TrigVrtSecInclusive/IWrkVrt.h"
#include "TrigVrtSecInclusive/VtxMap.h"

#include "xAODTracking/TrackParticleContainer.h"
#include "xAODTracking/VertexContainer.h"
#include "xAODTracking/VertexAuxContainer.h"
#include "TrkVKalVrtFitter/TrkVKalVrtFitter.h"
#include "InDetConversionFinderTools/VertexPointEstimator.h"

#include "Gaudi/Property.h"
#include "AthenaBaseComps/AthReentrantAlgorithm.h"
#include "AthenaMonitoringKernel/GenericMonitoringTool.h"

#include <TH3D.h>
#include <TMatrixT.h>

#include <vector>
#include <deque>
#include <iostream>

namespace TrigVSI {

/**
 * @class TrigVrtSecInclusive
 * @brief
 * @author Kunihiro Nagano <kunihiro.nagano@cern.ch> - KEK
 **/


class TrigVrtSecInclusive : public AthReentrantAlgorithm
{
public:
   TrigVrtSecInclusive(const std::string &name, ISvcLocator *pSvcLocator);
   virtual ~TrigVrtSecInclusive();

   virtual StatusCode initialize() override;
   virtual StatusCode execute(const EventContext& ctx) const override;
   virtual StatusCode finalize() override;

private:

   using xAODContainers = std::pair< std::unique_ptr<xAOD::VertexContainer>, std::unique_ptr<xAOD::VertexAuxContainer> >;

   // Properties
   Gaudi::Property<int> m_vtxAlgorithm{this,  "vtxAlgorithm", 0,  "Vertexing algorithm. 0 : TrigVSI algorithm, 1 : Offline VSI like algorithm"};

   Gaudi::Property<int> m_cutPixelHits  {this, "CutPixelHits",  0,  "Track selection : Hit requirements"};
   Gaudi::Property<int> m_cutSctHits    {this, "CutSctHits",    2,  "Track selection : Hit requirements"};
   Gaudi::Property<int> m_cutSharedHits {this, "CutSharedHits", 99, "Track selection : Hit requirements"};
   Gaudi::Property<int> m_cutSiHits     {this, "CutSiHits",     0,  "Track selection : Hit requirements"};
   Gaudi::Property<int> m_cutBLayHits   {this, "CutBLayHits",   0,  "Track selection : Hit requirements"};

   Gaudi::Property<double> m_trkChi2Cut {this, "TrkChi2Cut",  50.0, "Track selection : Chi2 requirements"};
   Gaudi::Property<double> m_trkPtCut   {this, "TrkPtCut",    (m_vtxAlgorithm == 0)? 2000. : 1000.,  "Track selection : pT requirements. 2 GeV in TrigVSI, 1 GeV in VSI"};

   Gaudi::Property<double> m_d0TrkPVDstMinCut       {this, "d0TrkPVDstMinCut",      2.0,    "Track selection : Impact parameter requirements [mm]"};
   Gaudi::Property<double> m_d0TrkPVDstMaxCut       {this, "d0TrkPVDstMaxCut",      300.0,  "Track selection : Impact parameter requirements [mm]"};
   Gaudi::Property<double> m_z0TrkPVDstMinCut       {this, "z0TrkPVDstMinCut",      0.0,    "Track selection : Impact parameter requirements [mm]"};
   Gaudi::Property<double> m_z0TrkPVDstMaxCut       {this, "z0TrkPVDstMaxCut",      1500.0, "Track selection : Impact parameter requirements [mm]"};
   Gaudi::Property<double> m_twoTrkVtxFormingD0Cut  {this, "twoTrkVtxFormingD0Cut", 1.,     "Track selection : Impact parameter requirements [mm]"};


   Gaudi::Property<double>  m_maxR {this,  "maxR", 563., "Track-pair selection : Max value for R of vertex position"};

   Gaudi::Property<bool>    m_doTwoCircRCut {this, "doTwoCircRCut", false,  "Track-pair selection : Flag for R cut derived from two-circles-intersection-point"};
   Gaudi::Property<bool>    m_doFastRCut    {this, "doFastRCut",    false,  "Track-pair selection : Flag for R cut derived from VKalVrtFitFast"};

   Gaudi::Property<double>  m_fastD0minCut  {this, "fastD0minCut",    5.,   "Track-pair selection : Threshold for rough d0 cut"};
   Gaudi::Property<double>  m_fastD0deltaCut{this, "fastD0deltaCut",  4.2,  "Track-pair selection : Threshold for rough d0 cut"};
   Gaudi::Property<double>  m_fastZ0minCut  {this, "fastZ0minCut",    120., "Track-pair selection : Threshold for rough z0 cut"};
   Gaudi::Property<double>  m_fastZ0deltaCut{this, "fastZ0deltaCut",  8.,   "Track-pair selection : Threshold for rough z0 cut"};

   Gaudi::Property<double>  m_selVrtChi2Cut {this, "SelVrtChi2Cut", 4.5,  "Track-pair selection : Threshold for the chi2 value of track pair fitting with VKalVrtFit"};

   Gaudi::Property<bool>    m_doPVCompatibilityCut  {this, "doPVCompatibilityCut",  false,  "Track-pair selection : When set to true, require track pairs to be compatible with PV"};
   Gaudi::Property<double>  m_dphiPVCut             {this, "dphiPVCut",             -0.8,   "Track-pair selection : Threshold for the cos of angle of the tracks and the momentum of the track pair"};
   Gaudi::Property<double>  m_pvCompatibilityCut    {this, "PVcompatibilityCut",    -20.,   "Track-pair selection : Threshold for the track pair position along the momentum vector"};

   Gaudi::Property<bool>    m_doMaterialMapVeto     {this, "doMaterialMapVeto", false,  "Track-pair selection : When set to true, perform material map veto on track pairs"};

   Gaudi::Property<bool>    m_skipLargeCluster  {this, "skipLargeCluster",  true, "Flag to skip vertexing on clusters with too many tracks"};
   Gaudi::Property<size_t>  m_maxTrks           {this, "maxTrks",           100,  "Max track number to run vertexing"};

   Gaudi::Property<size_t>  m_minTrkPairsMerge  {this, "minTrkPairsMerge",  6,  "Clusters with track pairs less than minTrkPairsMerge and tracks less than minTrksMerge will be merged into single vertex"};
   Gaudi::Property<size_t>  m_minTrksMerge      {this, "minTrksMerge",      4,  "Clusters with track pairs less than minTrkPairsMerge and tracks less than minTrksMerge will be merged into single vertex"};

   Gaudi::Property<bool>    m_truncateWrkVertices     {this,  "truncateWrkVertices",      true,   "Offline VSI option"};
   Gaudi::Property<size_t>  m_maxWrkVertices          {this,  "maxWrkVertices",           10000,  "Offline VSI option"};
   Gaudi::Property<double>  m_improveChi2ProbThreshold{this,  "improveChi2ProbThreshold", 4.5,    "Offline VSI option"};

   //
   struct WrkVrt : public TrigVSI::IWrkVrt{
      bool isGood = false;                            //! flaged true for good vertex candidates
      bool isPair = false;                            //! flaged true for track pair vertex
      Amg::Vector3D        vertex;                    //! VKalVrt fit vertex position
      TLorentzVector       vertexMom;                 //! VKalVrt fit vertex 4-momentum
      std::vector<double>  vertexCov;                 //! VKalVrt fit covariance
      double               chi2;                      //! VKalVrt fit chi2 result
      double               chi2Core;                  //! VKalVrt fit chi2 result
      std::vector<double>  chi2PerTrk;                //! list of VKalVrt fit chi2 for each track
      long int             charge;                    //! total charge of the vertex
      std::vector< std::vector<double> > trkAtVrt;    //! list of track parameters wrt the reconstructed vertex
      unsigned long        closestWrkVrtIndex;        //! stores the index of the closest WrkVrt in std::vector<WrkVrt>
      double               closestWrkVrtValue;        //! stores the value of some observable to the closest WrkVrt ( observable = e.g. significance )

      inline double ndof() const { return 2.0*( m_selectedTrackIndices.size() + m_associatedTrackIndices.size() ) - 3.0; }
      inline double ndofCore() const { return 2.0*( m_selectedTrackIndices.size() ) - 3.0; }
      inline unsigned nTracksTotal() const { return m_selectedTrackIndices.size() + m_associatedTrackIndices.size(); }
      inline double fitQuality() const { return chi2 / ndof(); }

      struct AlgParam {
         double               twoCirc_dphi;
         double               twoCirc_dr;
         double               twoCirc_int_r;

         double               vPos;
         double               vPosMomAngT;
         double               dphi1;
         double               dphi2;

         double               vrtFast_r;
         double               vrtFast_eta;
         double               vrtFast_phi;
         std::vector<float>   vrtFast_trkd0;
         std::vector<float>   vrtFast_trkz0;

         bool                 isPassMMV;
      };

      struct AlgCuts {
         bool  trkd0cut      = false;
         bool  twoCircErrcut = false;
         bool  twoCircRcut   = false;
         bool  fastErrcut    = false;
         bool  fastRcut      = false;
         bool  fitErrcut     = false;
         bool  chi2cut       = false;

         bool  isFullPass() const { return !(trkd0cut||twoCircErrcut||twoCircRcut||fastErrcut||fastRcut||fitErrcut||chi2cut); };
      };

      AlgParam  param;
      AlgCuts   cuts;

      virtual std::deque<size_t>&        selectedTrackIndices() override { return this->m_selectedTrackIndices; }        //!<  Return indices of tracks associated with the vertex.
      virtual const std::deque<size_t>&  selectedTrackIndices() const override { return this->m_selectedTrackIndices; }  //!<  Return indices of tracks associated with the vertex.

      /// @name Position in cartesian coordinate
      /// @{
      virtual double x() const { return vertex.x(); }
      virtual double y() const { return vertex.y(); }
      virtual double z() const { return vertex.z(); }
      /// @}

      private :
         std::deque<size_t> m_selectedTrackIndices;        //! list if indices in TrackParticleContainer for selectedBaseTracks
         std::deque<size_t> m_associatedTrackIndices;      //! list if indices in TrackParticleContainer for associatedTracks

   };

   enum TrkParameter    { k_d0=0, k_z0=1, k_theta=2, k_phi=3, k_qOverP=4 ,k_nTP=5 };
   enum TrkParameterUnc { k_d0d0=0, k_z0z0=1, k_nTPU=2 };


   // Type Definition
   using WrkVrtContainer = std::vector<WrkVrt>;

   // material map
   TH3S* m_materialMapInner=0;
   TH3S* m_materialMapOuter=0;
   TMatrixT<double>* m_materialMapMatrix=0;
   std::string m_materialMapInnerFileName;
   std::string m_materialMapInnerHistName;
   std::string m_materialMapInnerMatrixName;
   std::string m_materialMapOuterFileName;
   std::string m_materialMapOuterHistName;

   // selection methods
   bool selectTrack( const xAOD::TrackParticle* trk ) const;
   bool selectTrack_hitPattern( const xAOD::TrackParticle* trk ) const;
   bool selectTrack_d0Cut( const xAOD::TrackParticle* trk ) const;
   bool selectTrack_z0Cut( const xAOD::TrackParticle* trk ) const;
   bool selectTrack_pTCut( const xAOD::TrackParticle* trk ) const;
   bool selectTrack_chi2Cut( const xAOD::TrackParticle* trk ) const;

   // keys
   SG::ReadHandleKey<xAOD::TrackParticleContainer> m_firstPassTracksName  { this,"FirstPassTracksName","InDetTrackParticles","xAOD::TrackParticle Collection used in Vertexing" };
   SG::ReadHandleKey<xAOD::TrackParticleContainer> m_secondPassTracksName { this,"SecondPassTracksName","InDetTrackParticles","xAOD::TrackParticle Collection used in Vertexing" };
   SG::WriteHandleKey<xAOD::VertexContainer> m_vxCandidatesOutputName { this,"VxCandidatesOutputName","InclusiveSecVtx","Output Vertex Collection" };
   SG::WriteHandleKey<xAOD::VertexContainer> m_trkPairOutputName      { this,"TrkPairOutputName","InclusiveSecVtx","Track pair Collection" };
   SG::ReadHandleKey<xAOD::VertexContainer> m_PrimaryVxInputName{ this,"PrimaryVertexInputName","PrimaryVertices","Input Vertex Collection" };
   //
   ToolHandle<Trk::TrkVKalVrtFitter> m_fitSvc { this, "VertexFitter", "", "VKalVrtFitter tool to fit tracks into the common vertex" };
   ToolHandle<InDet::VertexPointEstimator> m_vertexPointEstimator { this, "VertexPointEstimator", "", "tool to find starting point for the vertex fitter"};
   //
   ToolHandle<GenericMonitoringTool> m_monTool{ this, "MonTool", "", "Monitoring tool" };

   //================================================================
   //
   // Util member functions
   //
   StatusCode fillVtxContainer( xAODContainers&, const WrkVrtContainer&, std::vector<const xAOD::TrackParticle*>&) const;


   //================================================================
   //
   // Vertex finding algorithm member functions
   //
   size_t     nTrkCommon( WrkVrtContainer&, const std::pair<unsigned, unsigned>& ) const;
   StatusCode cleanUp   ( WrkVrtContainer& ) const;
   StatusCode trackSelection    ( const xAOD::TrackParticleContainer*, const xAOD::TrackParticleContainer*, std::vector<const xAOD::TrackParticle*>& ) const;
   StatusCode findDiTrackVertex ( WrkVrtContainer&, std::vector<std::pair<size_t,size_t>>&, std::vector<const xAOD::TrackParticle*>&, const EventContext&, const xAOD::Vertex*) const;
   StatusCode findDiTrackVertexVSI ( WrkVrtContainer&, std::vector<std::pair<size_t,size_t>>&, std::vector<const xAOD::TrackParticle*>&, const EventContext&, const xAOD::Vertex*) const;
   StatusCode findNtrackVerticesVSI( WrkVrtContainer&, std::vector<std::pair<size_t,size_t>>&, std::vector<const xAOD::TrackParticle*>&, const EventContext& ) const;

   template<typename VrtType, typename Coord>
   StatusCode findNTrackVertex  ( WrkVrtContainer&, TrigVSI::VtxMap<VrtType,Coord>&, const std::vector<const xAOD::TrackParticle*>&, const EventContext& ) const;

   StatusCode fitVertexFromTracks     ( WrkVrt&, const std::vector<const xAOD::TrackParticle*>&, const EventContext& ) const;
   StatusCode mergeVertexFromDiTrkVrt ( WrkVrtContainer&, const std::vector<std::pair<size_t,size_t>>&, const std::vector<size_t>&, const std::vector<const xAOD::TrackParticle*>&, const EventContext& ) const;
};

} // end of namespace TrigVSI

#endif //> TRIGTOOLS_TRIGVRTSECINCLUSIVE_H

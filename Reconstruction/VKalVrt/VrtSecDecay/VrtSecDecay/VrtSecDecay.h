/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

///
/////////////////////////////////////////////////////////////////////////////////
//  Header file for class VrtSecDecay 
/////////////////////////////////////////////////////////////////////////////////
// (c) ATLAS Detector software
/////////////////////////////////////////////////////////////////////////////////
//

#ifndef VrtSecDecay_H
#define VrtSecDecay_H

#include "GaudiKernel/ServiceHandle.h"
#include "GaudiKernel/ToolHandle.h"
#include "AthenaBaseComps/AthAlgorithm.h"

#include "ITrackToVertex/ITrackToVertex.h"
#include "TrkVKalVrtFitter/TrkVKalVrtFitter.h"

#include "xAODTracking/TrackParticleContainer.h"
#include "xAODTracking/TrackParticle.h"
#include "xAODTracking/VertexContainer.h"
#include "xAODTracking/Vertex.h"
#include "xAODTracking/VertexAuxContainer.h"
#include "TrkTrack/TrackCollection.h"

// Forward declarations
class MsgStream;

namespace Trk
{
  class TrkVKalVrtFitter;
}


/// Class to fit displaced vertices from a collection of two input track containers
/// The topology that is targeted is one where a charged particle (parent) decays into n-outgoing track(s) (child).
/// The algoirthm can be configured with various selections on the parent and child tracks,
/// and into modes with a single parent and child track, or a single parent into n-child tracks.
/// Written by Matthew Gignac (SCIPP)

namespace VKalVrtAthena {

  class VrtSecDecay : public AthAlgorithm 
    {

      ///////////////////////////////////////////////////////////////////
      // Public methods:
      ///////////////////////////////////////////////////////////////////
      
    public:
      
      ///////////////////////////////////////////////////////////////////
      // Standard tool methods
      ///////////////////////////////////////////////////////////////////

      VrtSecDecay(const std::string &name, ISvcLocator *pSvcLocator);
      ~VrtSecDecay() {};
      StatusCode initialize() override; 
      StatusCode execute () override;

     private:

       StatusCode doTrackParticleMethod();
 
       std::string        m_outputVtxContainer;
       std::string        m_parentxAODTrkContainer;
       std::string        m_childxAODTrkContainer;
       std::string        m_decorationPreFix;

       bool               m_doHitPatternMatching;
       bool               m_expandToMultiTrackVtx;

       int                m_maxVtxPerEvent;

       float              m_vtxQuality;
       float              m_minVtxRadius;
       float              m_maxVtxRadius;
       float              m_parentChild_maxdr;

       float              m_parentPt;
       float              m_parentMinEta;
       float              m_parentMaxEta;
       float              m_parentZ0PVSinTheta;
       int                m_parentIBLHits;
       int                m_parentPixLayers;
       int                m_parentSCTHits;
       

       float              m_childMinPt;
       float              m_childMaxPt;
       int                m_childBLayerHits;
       int                m_childNextToInPixLayerHits;
       int                m_childPixHits;
       int                m_childSCTHits;

       struct VtxObj {
         std::vector<const xAOD::TrackParticle*> inputTrks;
         std::unique_ptr<xAOD::Vertex> vtx;
         VtxObj(std::unique_ptr<xAOD::Vertex>&& _vtx,std::vector<const xAOD::TrackParticle*>&& _inputTrks) :
           inputTrks(std::move(_inputTrks)), vtx(std::move(_vtx)) {}
       };


       // Preselection of soft tracks
       std::vector<const xAOD::TrackParticle*> preselectTracks(const xAOD::TrackParticleContainer* tracks,const xAOD::Vertex* pV);

       // Selection of parent particles
       bool passParentSelection(const xAOD::TrackParticle* tracklet,const xAOD::Vertex* pV);

       // Apply vertex selection
       bool passVtxSelection(const xAOD::Vertex* vtx);

       // Consistency check between hit pattern of the tracklet and soft track
       bool consistentHitPattern(const xAOD::TrackParticle* tracklet, const xAOD::TrackParticle* track);

       //
       void decorateTrkWithVertexParameters(const xAOD::TrackParticle* trk,xAOD::Vertex* myVertex);

     protected:

      /// Vertex fitter interface.
      ToolHandle < Trk::TrkVKalVrtFitter >       m_iVertexFitter;
      ToolHandle< Reco::ITrackToVertex >         m_trackToVertexTool;

      static bool rank_radius(const VtxObj& a, const VtxObj& b){return a.vtx->position().perp() > b.vtx->position().perp();}
      static bool rank_quality(const VtxObj& a, const VtxObj& b){return (a.vtx->chiSquared()/a.vtx->numberDoF())<(b.vtx->chiSquared()/b.vtx->numberDoF());}

    }; // Class declaration
} // end of name space

#endif //


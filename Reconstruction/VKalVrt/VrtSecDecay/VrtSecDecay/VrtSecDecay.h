/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/

//
/////////////////////////////////////////////////////////////////////////////////
//  Header file for class VrtSecDecay 
/////////////////////////////////////////////////////////////////////////////////
// (c) ATLAS Detector software
/////////////////////////////////////////////////////////////////////////////////
//
// Class to fit displaced vertices from a pixel tracklet and a soft-displaced track
// Written by Matthew Gignac (SCIPP)
//

#ifndef VrtSecDecay_H
#define VrtSecDecay_H

#include "GaudiKernel/ServiceHandle.h"
#include "GaudiKernel/ToolHandle.h"
#include "AthenaBaseComps/AthAlgorithm.h"

#include "ITrackToVertex/ITrackToVertex.h"

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

//
namespace VKalVrtAthena {

  // This class
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
      ~VrtSecDecay();
      StatusCode initialize();
      StatusCode execute ();
      StatusCode finalize  ();

     private:

       StatusCode doTrackParticleMethod();
 
       std::string        m_outputVtxContainer;
       std::string        m_parentxAODTrkContainer;
       std::string        m_childxAODTrkContainer;

       bool               m_doHitPatternMatching;
       bool               m_doAngularMatching;
       bool               m_doRadiusSorting;

       int                m_maxVtxPerEvent;

       float              m_vtxQuality;
       float              m_minVtxRadius;
       float              m_maxVtxRadius;

       float              m_parentPt;
       float              m_parentMinEta;
       float              m_parentMaxEta;

       float              m_childMinPt;
       float              m_childMaxPt;
       int                m_childBLayerHits;
       int                m_childNextToInPixLayerHits;
       int                m_childPixHits;
       int                m_childSCTHits;

       struct VtxObj {
         std::vector<const xAOD::TrackParticle*> inputTrks;
         xAOD::Vertex* vtx;
         VtxObj(xAOD::Vertex* _vtx,std::vector<const xAOD::TrackParticle*> _inputTrks) :
           inputTrks(_inputTrks), vtx(_vtx) {}
       };


       // Preselection of soft tracks
       std::vector<const xAOD::TrackParticle*> preselectTracks(const xAOD::TrackParticleContainer* tracks);

       // Note: cleans up the vtx if it doesn't pass the selection
       bool passVtxSelection(xAOD::Vertex*& vtx);

       // Consistency check between hit pattern of the tracklet and soft track
       bool consistentHitPattern(const xAOD::TrackParticle* tracklet, const xAOD::TrackParticle* track);

       //
       void decorateTrkWithVertexParameters(const xAOD::TrackParticle* trk,xAOD::Vertex* myVertex);

     protected:

      /// Vertex fitter interface.
      ToolHandle < Trk::TrkVKalVrtFitter >       m_iVertexFitter;
      ToolHandle< Reco::ITrackToVertex >         m_trackToVertexTool;



      static bool rank_radius(const VtxObj& a, const VtxObj& b){return a.vtx->position().perp() > b.vtx->position().perp();}

    }; // Class declaration
} // end of name space

/*
struct radiusSort
{
  bool operator()( const xAOD::Vertex_v1*& lhs,const xAOD::Vertex_v1*& rhs) const
  { return lhs->position.perp()<rhs->position.perp(); }
  
};

*/

#endif //


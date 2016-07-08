/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// ZVTOP_Tool.h, (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////

#ifndef INDETZVTOP_SECVTXTOOL_H
#define INDETZVTOP_SECVTXTOOL_H

#include "AthenaBaseComps/AthAlgTool.h"
#include "GaudiKernel/ToolHandle.h"
#include "InDetRecToolInterfaces/ISecVertexInJetFinder.h"

/* Forward declarations */
namespace Trk
{
  class Track;
  class VxSecVertexInfo;
  class RecVertex;
  class TrackParticleBase;
  class IVertexFitter;
}
namespace InDet 
{
    class ISecVertexInJetFinder;
    class IZVTOP_SpatialPointFinder;
    class IZVTOP_TrkProbTubeCalc;
    class IZVTOP_SimpleVtxProbCalc;
    class IZVTOP_AmbiguitySolver;
    class ZVTOP_TrkPartBaseVertexState;
    class ZVTOP_VertexState;

  /** @class ZVTOP_Tool 

      ---Topological Vertex Finder Tool ---
      This tool reconstructs a set of topological vertices each 
      associated with an independent subset of the charged tracks.
      Vertices are reconstructed by associating tracks with 3D spatial regions
      according to the vertex probability function which is based on the trajectories
      and position resolution of the tracks.
      Docu: "A Topological Vertex Reconstruction Algorithm for Hadronic Jets" 
            by David J. Jackson, SLAC-PUB-7215, December 1996 

      Following properties can be set/changed via jobOptions:
 
      ---Input: TrackCollection
      - \c TrackParticleName: The name of the StoreGate input container from
      which the TrackParticle are read. The default is "???", the container
      from the legacy converters/ambiguity processor.
      
      ---Output: VxContainer
      - \c VxCollectionOutputName: The name of the StoreGate container where the fit
      results are put. default is "VxCollection".

      ---Vertex Fitter: Billoir FullVertexFitter
      - \c FitRoutine: The routine which should be used for the fitting. The
      default is "FullVertexFitter".
      

      
      @author  Tatjana Lenz <tatjana.lenz@NOSPAMcern.ch>
  */  
  class ZVTOP_SecVtxTool : virtual public ISecVertexInJetFinder, public AthAlgTool
    {
    public:
      ZVTOP_SecVtxTool(const std::string&,const std::string&,const IInterface*);

       /** default destructor */
      virtual ~ZVTOP_SecVtxTool ();
      
       /** standard Athena-Algorithm method */
      virtual StatusCode initialize();
       /** standard Athena-Algorithm method */
      virtual StatusCode finalize  ();
 
     virtual const Trk::VxSecVertexInfo* findSecVertex(const Trk::RecVertex & primaryVertex,const TLorentzVector & jetMomentum,const std::vector<const Trk::TrackParticleBase*> & inputTracks) const;

     //Added purely to satisfy new inheritance in ISecVertexInJetFinder, not yet implemented --David S.
     virtual const Trk::VxSecVertexInfo* findSecVertex(const xAOD::Vertex & primaryVertex, const TLorentzVector & jetMomentum,const std::vector<const xAOD::IParticle*> & inputTracks) const;

     virtual const Trk::VxSecVertexInfo* findSecVertex(const Trk::RecVertex & primaryVertex,const TLorentzVector & jetMomentum,const std::vector<const Trk::Track*> & inputTracks) const;

    private:
      
      ToolHandle <Trk::IVertexFitter>					m_iVertexFitter;
      ToolHandle <InDet::IZVTOP_SpatialPointFinder>	m_iSpatialPointFinder;
      ToolHandle <InDet::IZVTOP_TrkProbTubeCalc>	m_iTrkProbTubeCalc;
      ToolHandle <InDet::IZVTOP_SimpleVtxProbCalc>	m_iVtxProbCalc;
      ToolHandle <InDet::IZVTOP_AmbiguitySolver>	m_iAmbiguitySolver;
      double										m_resolvingCut;
      double										m_resolvingStep;
      double										m_vtxProb_cut;
      double										m_trk_chi2_cut;
    }; 
} // end of namespace

#endif 
 

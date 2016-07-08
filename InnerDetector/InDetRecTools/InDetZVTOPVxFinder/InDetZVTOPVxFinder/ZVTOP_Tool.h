/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// ZVTOP_Tool.h, (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////

#ifndef INDETZVTOP_TOOL_H
#define INDETZVTOP_TOOL_H

#include "AthenaBaseComps/AthAlgTool.h"
#include "GaudiKernel/ToolHandle.h"
#include "GaudiKernel/ServiceHandle.h"
#include "InDetRecToolInterfaces/IVertexFinder.h"
#include "xAODTracking/Vertex.h"
#include "xAODTracking/TrackParticle.h"
#include "xAODTracking/VertexContainer.h"
#include "xAODTracking/TrackParticleContainer.h"

class IBeamCondSvc;

namespace Trk
{
  class IVertexFitter;
}

namespace InDet 
{

  class IZVTOP_SpatialPointFinder;
  class IZVTOP_TrkProbTubeCalc;
  class IZVTOP_SimpleVtxProbCalc;

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
      - \c TracksName: The name of the StoreGate input container from
      which the tracks are read. The default is "Tracks", the container
      from the legacy converters/ambiguity processor.
      
      ---Output: VxContainer
      - \c VxCollectionOutputName: The name of the StoreGate container where the fit
      results are put. default is "VxCollection".

      ---Vertex Fitter: Billoir FullVertexFitter
      - \c FitRoutine: The routine which should be used for the fitting. The
      default is "FullVertexFitter".
      

      
      @author  Tatjana Lenz <tatjana.lenz@cern.ch>
  */  
  class ZVTOP_Tool : virtual public IVertexFinder, public AthAlgTool
    {
    public:
      ZVTOP_Tool(const std::string&,const std::string&,const IInterface*);

       /** default destructor */
      virtual ~ZVTOP_Tool ();
      
       /** standard Athena-Algorithm method */
      virtual StatusCode initialize();
       /** standard Athena-Algorithm method */
      virtual StatusCode finalize  ();
      std::pair<xAOD::VertexContainer*, xAOD::VertexAuxContainer*> findVertex(const TrackCollection* trackTES);
      std::pair<xAOD::VertexContainer*, xAOD::VertexAuxContainer*> findVertex(const Trk::TrackParticleBaseCollection* trackTES);
      std::pair<xAOD::VertexContainer*, xAOD::VertexAuxContainer*> findVertex(const xAOD::TrackParticleContainer* trackParticles);
    private:
      
      ToolHandle <Trk::IVertexFitter>			m_iVertexFitter;
      ToolHandle <InDet::IZVTOP_SpatialPointFinder>	m_iSpatialPointFinder;
      ToolHandle <InDet::IZVTOP_TrkProbTubeCalc>	m_iTrkProbTubeCalc;
      ToolHandle <InDet::IZVTOP_SimpleVtxProbCalc>	m_iVtxProbCalc;
      
      double						m_resolvingCut;
      double						m_resolvingStep;
      double						m_vtxProb_cut;
      double						m_trk_chi2_cut;
      ServiceHandle<IBeamCondSvc>                       m_iBeamCondSvc; //!< pointer to the beam condition service
    };
} // end of namespace

#endif 

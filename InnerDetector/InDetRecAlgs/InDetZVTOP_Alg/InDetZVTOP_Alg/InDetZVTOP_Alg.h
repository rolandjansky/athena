/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// InDetZVTOP_Alg.h, (c) ATLAS Detector software
// begin   : 30-10-2006
// authors : Tatjana Lenz
// email   : tatjana.lenz@cern.ch
// changes :
///////////////////////////////////////////////////////////////////

#ifndef INDETZVTOP_ALG_H
#define INDETZVTOP_ALG_H

// Gaudi includes
#include "AthenaBaseComps/AthAlgorithm.h"
#include "GaudiKernel/ToolHandle.h"
#include "TrkParticleBase/TrackParticleBase.h"
namespace InDet 
{

  /** @class InDetZVTOP_Alg
     ---Topological Vertex Finder ---
      This Algorithm reconstructs a set of topological vertices each 
      associated with an independent subset of the charged tracks.
      Vertices are reconstructed by associating tracks with 3D spatial regions
      according to the vertex probability function which is based on the trajectories
      and position resolution of the tracks.
      
      Docu: "A Topological Vertex Reconstruction Algorithm for Hadronic Jets" 
            by David J. Jackson, SLAC-PUB-7215, December 1996 
      
      @author  Tatjana Lenz <tatjana.lenz@cern.ch>
  */  

  class IVertexFinder;

  class InDetZVTOP_Alg : public AthAlgorithm
    {
    public:

       /** Standard Athena-Algorithm Constructor */
       InDetZVTOP_Alg(const std::string& name, ISvcLocator* pSvcLocator);
       /** Default Destructor */
       ~InDetZVTOP_Alg();

       /** standard Athena-Algorithm method */
       StatusCode          initialize();
       /** standard Athena-Algorithm method */
       StatusCode          execute();
       /** standard Athena-Algorithm method */
       StatusCode          finalize();

    private:

      /** member variables for algorithm properties: */

      std::string				m_tracksName; //!< Name of track container in StoreGate
      std::string				m_vxCollectionOutputName; //!< Name of output container to store results
    
      ToolHandle <IVertexFinder>		m_VertexFinderTool;

      
    }; 
} // end of namespace

#endif 

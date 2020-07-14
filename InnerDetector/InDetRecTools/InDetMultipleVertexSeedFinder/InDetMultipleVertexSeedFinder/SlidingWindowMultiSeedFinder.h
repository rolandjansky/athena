/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#ifndef InDetMultipleVertexSeedFinder_SlidingWindowMultiSeedFinder_H
#define InDetMultipleVertexSeedFinder_SlidingWindowMultiSeedFinder_H

#include "AthenaBaseComps/AthAlgTool.h"
#include "GaudiKernel/ToolHandle.h"
//#include "TrkParameters/TrackParameters.h"
#include "InDetRecToolInterfaces/IMultiPVSeedFinder.h"

class IBeamCondSvc;

namespace Trk
{
 class Track;
 class ITrackSelectorTool; 
 class IExtrapolator;
 class IVertexSeedFinder;
}


namespace InDet
{
 class InDetTrackZ0SortingTool;
  
 class SlidingWindowMultiSeedFinder : public AthAlgTool, virtual public IMultiPVSeedFinder
 {
  public:
  
   StatusCode initialize();
 
   StatusCode finalize();

/**
 * Constructor and destructor
 */
   SlidingWindowMultiSeedFinder(const std::string& t, const std::string& n, const IInterface*  p);
 
   ~SlidingWindowMultiSeedFinder();

/**
 * Clustering method itself
 */   
   std::vector< std::vector<const Trk::Track *> > seeds(const std::vector<const Trk::Track*>& tracks )const; 
  
   std::vector< std::vector<const Trk::TrackParticleBase *> > seeds(const std::vector<const Trk::TrackParticleBase*>& tracks )const;

   std::vector< std::vector<const Trk::TrackParameters *> > seeds(const std::vector<const xAOD::TrackParticle*>& tracks )const;

   //std::vector<int> m_z0sort(std::vector<const xAOD::TrackParticle*>& perigeeList,xAOD::Vertex * beamVtx) const;

  private:
    
   float m_clusterLength;
   float m_breakingDistance;
   float m_addingDistance;
   bool   m_useMaxInCluster;
   int m_ignoreLevel;
   bool m_ignoreBeamSpot;
    
   ToolHandle<Trk::ITrackSelectorTool> m_trkFilter;   
   ToolHandle<InDetTrackZ0SortingTool>  m_sortingTool;
   ToolHandle<Trk::IVertexSeedFinder> m_vtxSeedFinder;
   
//beam spot finder
  ServiceHandle< IBeamCondSvc > m_beamService;    
  ToolHandle<Trk::IExtrapolator> m_extrapolator; //<! Extrapolator tool 

 };//end of class definitions
}//eend of namespace definitions

#endif

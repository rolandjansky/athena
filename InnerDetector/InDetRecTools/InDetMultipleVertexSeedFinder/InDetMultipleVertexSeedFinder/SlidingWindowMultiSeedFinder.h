/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#ifndef InDetMultipleVertexSeedFinder_SlidingWindowMultiSeedFinder_H
#define InDetMultipleVertexSeedFinder_SlidingWindowMultiSeedFinder_H

#include "AthenaBaseComps/AthAlgTool.h"
#include "GaudiKernel/ToolHandle.h"
//#include "TrkParameters/TrackParameters.h"
#include "InDetRecToolInterfaces/IMultiPVSeedFinder.h"
#include "BeamSpotConditionsData/BeamSpotData.h"


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
  
   virtual StatusCode initialize() override;

/**
 * Constructor and destructor
 */
   SlidingWindowMultiSeedFinder(const std::string& t, const std::string& n, const IInterface*  p);
 
   ~SlidingWindowMultiSeedFinder();

/**
 * Clustering method itself
 */   
   virtual std::vector< std::vector<const Trk::Track *> > seeds(const std::vector<const Trk::Track*>& tracks )const override;
  
   virtual std::vector< std::vector<const Trk::TrackParticleBase *> > seeds(const std::vector<const Trk::TrackParticleBase*>& tracks )const override;

   virtual std::vector< std::vector<const Trk::TrackParameters *> > seeds(const std::vector<const xAOD::TrackParticle*>& tracks )const override;

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
  SG::ReadCondHandleKey<InDet::BeamSpotData> m_beamSpotKey { this, "BeamSpotKey", "BeamSpotData", "SG key for beam spot" };
  ToolHandle<Trk::IExtrapolator> m_extrapolator; //<! Extrapolator tool 

 };//end of class definitions
}//eend of namespace definitions

#endif

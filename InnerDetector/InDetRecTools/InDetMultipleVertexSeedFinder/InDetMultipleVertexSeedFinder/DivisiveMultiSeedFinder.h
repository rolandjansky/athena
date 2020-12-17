/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#ifndef InDetMultipleVertexSeedFinder_DivisiveMultiSeedFinder_H
#define InDetMultipleVertexSeedFinder_DivisiveMultiSeedFinder_H

#include "AthenaBaseComps/AthAlgTool.h"
#include "GaudiKernel/ToolHandle.h"
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
 class InDetTrackClusterCleaningTool;

 class DivisiveMultiSeedFinder : public AthAlgTool, virtual public IMultiPVSeedFinder
 {
 
  public:
   
   virtual StatusCode initialize() override;

/**
 * Constructor and destructor
 */
   DivisiveMultiSeedFinder(const std::string& t, const std::string& n, const IInterface*  p);
 
   ~DivisiveMultiSeedFinder();

/**
 * Clustering method itself
 */   
   virtual std::vector< std::vector<const Trk::Track *> > seeds(const std::vector<const Trk::Track*>& tracks )const override;

   virtual std::vector< std::vector<const Trk::TrackParticleBase *> > seeds(const std::vector<const Trk::TrackParticleBase*>& tracks )const override;

   virtual std::vector< std::vector<const Trk::TrackParameters *> > seeds(const std::vector<const xAOD::TrackParticle*>& tracks )const override;
    
  private:

// tuning parameters
   double m_sepDistance;
      
   unsigned int m_nRemaining; 

   bool m_ignoreBeamSpot;

   // std::vector<int> m_z0sort(std::vector<const xAOD::TrackParticle*>& perigeeList,xAOD::Vertex  * beamVtx) const;

   //  std::pair<std::vector<const Trk::TrackParameters *>, 
   //std::vector<const xAOD::TrackParticle *> > m_clusterAndOutliers(std::vector<const xAOD::TrackParticle *> tracks_to_clean, xAOD::Vertex * beamposition) const;

//track filter 
  ToolHandle<Trk::ITrackSelectorTool> m_trkFilter; 
  
//sorting tool 
  ToolHandle<InDetTrackZ0SortingTool> m_sortingTool;
 
//cluster cleaning tool 
  ToolHandle<InDetTrackClusterCleaningTool> m_cleaningTool;
  
//beam spot finder
  ServiceHandle< IBeamCondSvc > m_beamService; 
  ToolHandle<Trk::IExtrapolator> m_extrapolator; //<! Extrapolator tool
   
   //vertex seed finder
   ToolHandle<Trk::IVertexSeedFinder> m_vtxSeedFinder;

 };

}//end of namespace definitions

#endif



/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#ifndef InDetMultipleVertexSeedFinder_HistogrammingMultiSeedFinder_H
#define InDetMultipleVertexSeedFinder_HistogrammingMultiSeedFinder_H

#include "AthenaBaseComps/AthAlgTool.h"
#include "GaudiKernel/ToolHandle.h"
#include "InDetRecToolInterfaces/IMultiPVSeedFinder.h"

/**
 * A multi seed finder for primary vertexing. Uses a 
 * histogramming method. Operates in a fixed range 
 * +/-15cm along z-axis. 
 *
 * Configurables: number of histogram bins and
 * maximal separation distance between bins to be 
 * merged. in a cluster.
 *
 * Kirill Prokodiev <Kirill.Prokofiev@cern.ch>
 *
 * Mai 2007
 */

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
 
 class InDetTrackClusterCleaningTool;

 class HistogrammingMultiSeedFinder : public AthAlgTool, virtual public IMultiPVSeedFinder
 {
  public:
   
   virtual StatusCode initialize() override;

/**
 * Constructor and destructor
 */
   HistogrammingMultiSeedFinder(const std::string& t, const std::string& n, const IInterface*  p);
 
   ~HistogrammingMultiSeedFinder();

/**
 * Clustering method itself
 */   
   virtual std::vector< std::vector<const Trk::Track *> > seeds(const std::vector<const Trk::Track*>& tracks )const override;

   virtual std::vector< std::vector<const Trk::TrackParticleBase *> > seeds(const std::vector<const Trk::TrackParticleBase*>& tracks )const override;
    
   virtual std::vector< std::vector<const Trk::TrackParameters *> > seeds(const std::vector<const xAOD::TrackParticle*>& tracks )const override;

   //  std::pair<std::vector<const Trk::TrackParameters *>, 
   //  std::vector<const xAOD::TrackParticle *> > m_clusterAndOutliers(std::vector<const xAOD::TrackParticle *> tracks_to_clean, xAOD::Vertex * beamposition) const;
  

  private:

// tuning parameters
   unsigned int m_sepNBins;

   unsigned int m_nBins;
   
   unsigned int m_nRemaining;
   
   float  m_histoRange;
   
   bool m_ignoreBeamSpot;

//track filter and cleaning tool
   ToolHandle<Trk::ITrackSelectorTool>  m_trkFilter;
   ToolHandle<InDetTrackClusterCleaningTool> m_cleaningTool;
   ToolHandle<Trk::IVertexSeedFinder> m_vtxSeedFinder;

//beam spot finder
  ServiceHandle< IBeamCondSvc > m_beamService;  
  ToolHandle<Trk::IExtrapolator> m_extrapolator; //<! Extrapolator tool
 };
}//end of namespace definitions

#endif

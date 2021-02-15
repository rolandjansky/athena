/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#ifndef InDetMultipleVertexSeedFinderUtils_InDetTrackClusterCleaningTool_H
#define InDetMultipleVertexSeedFinderUtils_InDetTrackClusterCleaningTool_H

#include "AthenaBaseComps/AthAlgTool.h"
#include "GaudiKernel/ToolHandle.h"
#include "xAODTracking/Vertex.h"
#include "xAODTracking/TrackParticle.h"

/**
 * A tool for cleaning of the PV candidate clusters.
 * Rejects tracks which are further away than several
 * standard deviations from the center of the cluster.
 * The cleaning is done on the basis of the extrapolated 
 * perigee. If the empty extrapolation poit is passed,
 * the cleaning is done on the basis of  the usual 
 * perigee.
 *
 *
 * Kirill Prokofiev <Kirill.Prokofiev@cern.ch>
 * Mai 2007
 */

namespace Trk
{
 class Track;
 class Vertex;
 class TrackParticleBase;
 class IExtrapolator;
}

namespace InDet
{
 static const InterfaceID IID_InDetTrackClusterCleaningTool("InDetTrackClusterCleaningTool", 1, 1);
 
 class InDetTrackClusterCleaningTool : public AthAlgTool
 {
  public:
   
   virtual StatusCode initialize() override;
    
   InDetTrackClusterCleaningTool(const std::string& t, const std::string& n, const IInterface*  p);
    
   static const InterfaceID& interfaceID() {return IID_InDetTrackClusterCleaningTool;}
 
   ~InDetTrackClusterCleaningTool();
   
/**
 * Cleaning method. Returns a cluster core (first element of the pair)
 * and  outliers (second element)
 */   
   std::pair<std::vector<const Trk::Track*>,std::vector<const Trk::Track*> > clusterAndOutliers(const std::vector<const Trk::Track*>& cluster, 
                                                                                                const Trk::Vertex * reference = 0)const;
   
   std::pair<std::vector<const Trk::TrackParticleBase*>,std::vector<const Trk::TrackParticleBase*> > clusterAndOutliers(const std::vector<const Trk::TrackParticleBase*>& cluster,const Trk::Vertex * reference = 0)const;
   
   std::pair<std::vector<const Trk::TrackParameters *>, 
     std::vector<const xAOD::TrackParticle *> > clusterAndOutliers(std::vector<const xAOD::TrackParticle *> tracks_to_clean, const xAOD::Vertex * beamposition = 0 ) const;
 
  private:
  
   ToolHandle<Trk::IExtrapolator> m_extrapolator; //<! Extrapolator tool 
  
   double m_zOffset;
 
 };//end of class definitions

}//end of namespace definitions

#endif

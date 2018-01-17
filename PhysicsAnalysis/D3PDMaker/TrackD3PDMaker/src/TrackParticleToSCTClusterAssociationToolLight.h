/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRACKD3PD_TRACKPARTICLETOSCTCLUSTERASSOCIATIONTOOLLIGHT_H
#define TRACKD3PD_TRACKPARTICLETOSCTCLUSTERASSOCIATIONTOOLLIGHT_H

#include "D3PDMakerUtils/MultiAssociationTool.h"
#include "AthContainers/DataVector.h"
#include "GaudiKernel/ToolHandle.h"
#include "TrkToolInterfaces/ITrackHoleSearchTool.h"
#include "InDetRIO_OnTrack/SiClusterOnTrack.h"
#include "InDetPrepRawData/SiCluster.h"
#include "InDetPrepRawData/SCT_Cluster.h"



#include <vector>
#include <map>

class AtlasDetectorID;

namespace Rec{
class TrackParticle;
}

namespace Trk{
  class TrackStateOnSurface;
}

namespace D3PD {

class TrackParticleToSCTClusterAssociationToolLight
  : public MultiAssociationTool<Rec::TrackParticle, Trk::TrackStateOnSurface>
{
public:

  typedef MultiAssociationTool<Rec::TrackParticle, Trk::TrackStateOnSurface> Base;

  TrackParticleToSCTClusterAssociationToolLight (const std::string& type,
						 const std::string& name,
						 const IInterface* parent);

  virtual StatusCode initialize();

  /**
   * @brief Start the iteration for a new association.
   * @param p The object from which to associate.
   */
  virtual StatusCode reset (const Rec::TrackParticle& p);

  /**
   * @brief Return a pointer to the next element in the association.
   *
   * Return 0 when the association has been exhausted.
   */
  virtual const Trk::TrackStateOnSurface* next();


private:


  const AtlasDetectorID* m_idHelper;

  std::vector<const Trk::TrackStateOnSurface*> m_sctClusterForAssociation;

  std::vector<const Trk::TrackStateOnSurface*>::const_iterator m_clusItr;
  std::vector<const Trk::TrackStateOnSurface*>::const_iterator m_clusEnd;
};


} // namespace D3PD


#endif // not TRACKD3PD_TRACKPARTICLETOSCTCLUSTERASSOCIATIONTOOLLIGHT_H

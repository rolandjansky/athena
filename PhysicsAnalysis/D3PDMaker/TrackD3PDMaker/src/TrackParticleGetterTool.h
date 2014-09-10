/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRACKD3PDMAKER_TRACKPARTICLEGETTERTOOL_H
#define TRACKD3PDMAKER_TRACKPARTICLEGETTERTOOL_H

#include "D3PDMakerUtils/SGCollectionGetterTool.h"
#include "Particle/TrackParticle.h"
#include "Particle/TrackParticleContainer.h"
#include "TrkToolInterfaces/ITrackSelectorTool.h"
#include "GaudiKernel/ToolHandle.h"

namespace Trk {
  class VxCandidate;
}

namespace D3PD {

class TrackParticleGetterTool
  : public D3PD::SGCollectionGetterTool<Rec::TrackParticleContainer>
{
public:
  typedef D3PD::SGCollectionGetterTool<Rec::TrackParticleContainer> Base;

  TrackParticleGetterTool (const std::string& type,
			   const std::string& name,
			   const IInterface* parent);

  StatusCode initialize();
  StatusCode reset(bool=false);
  const void* nextUntyped();

private:

  /* apply track selection */
  bool selectTrack(const Rec::TrackParticle *track);

  /* iterators **/
  Rec::TrackParticleContainer::const_iterator m_trkItr;
  Rec::TrackParticleContainer::const_iterator m_trkEnd;

  /* for primary vertex **/
  std::string m_vxCandidateSGKey;
  const Trk::VxCandidate *m_vertex;
  bool m_hasVertex;

  /* track selector **/
  ToolHandle< Trk::ITrackSelectorTool > m_trackSelector;

}; // class TrackParticleGetterTool

} // namespace D3PD

#endif // not TRACKD3PDMAKER_TRACKPARTICLEGETTERTOOL_H

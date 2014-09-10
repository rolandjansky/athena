/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRACKD3PDMAKER_TRKTRACKGETTERTOOL_H
#define TRACKD3PDMAKER_TRKTRACKGETTERTOOL_H

#include "D3PDMakerUtils/SGCollectionGetterTool.h"
#include "TrkTrack/Track.h"
#include "TrkTrack/TrackCollection.h"
#include "TrkToolInterfaces/ITrackSelectorTool.h"
#include "GaudiKernel/ToolHandle.h"

namespace Trk {
  class VxCandidate;
}

namespace D3PD {

class TrkTrackGetterTool
  : public D3PD::SGCollectionGetterTool<TrackCollection>
{
public:
  typedef D3PD::SGCollectionGetterTool<TrackCollection> Base;

  TrkTrackGetterTool (const std::string& type,
		      const std::string& name,
		      const IInterface* parent);

  StatusCode initialize();
  StatusCode reset(bool=false);
  const void* nextUntyped();

private:

  /* apply track selection */
  bool selectTrack(const Trk::Track *track);

  /* iterators **/
  TrackCollection::const_iterator m_trkItr;
  TrackCollection::const_iterator m_trkEnd;

  /* for primary vertex **/
  std::string m_vxCandidateSGKey;
  const Trk::VxCandidate *m_vertex;
  bool m_hasVertex;

  /* track selector **/
  ToolHandle< Trk::ITrackSelectorTool > m_trackSelector;

}; // class TrkTrackGetterTool

} // namespace D3PD

#endif // not TRACKD3PDMAKER_TRKTRACKGETTERTOOL_H

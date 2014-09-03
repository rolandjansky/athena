/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// JetDetailedTrackSelectorTool.h

#ifndef JetDetailedTrackSelectorTool_H
#define JetDetailedTrackSelectorTool_H

#include "AthenaBaseComps/AthAlgTool.h"
#include "GaudiKernel/ToolHandle.h"
#include "GaudiKernel/ServiceHandle.h"
#include "TrkToolInterfaces/ITrackSelectorTool.h"
#include "xAODTracking/TrackParticle.h"
#include "xAODTracking/TrackParticle.h"
#include "xAODTracking/VertexFwd.h"

/**
 * @brief TrackSelector that pases all tracks.
 *
 * Replacement for InDetDetailedTrackSelectorTool that does
 * not use track propagation and so does not require magnetic
 * field or any other Athena-specific service.
 *
 * @author David Adams
 */

namespace Trk {
  class Track;
  class TrackParticleBase;
  class Vertex;
}

class JetDetailedTrackSelectorTool : virtual public Trk::ITrackSelectorTool, public AthAlgTool {

public:

  StatusCode initialize();

  StatusCode finalize();

  JetDetailedTrackSelectorTool(const std::string& t, const std::string& n, const IInterface*  p);

  ~JetDetailedTrackSelectorTool();

  bool decision(const Trk::Track& track,const Trk::Vertex* vertex) const;

  bool decision(const Trk::TrackParticleBase& track,const Trk::Vertex* vertex) const;

  bool decision(const xAOD::TrackParticle& track,const xAOD::Vertex* vertex) const;

};

#endif

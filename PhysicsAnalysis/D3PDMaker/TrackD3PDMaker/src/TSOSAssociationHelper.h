/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TSOS_ASSOCIATION_HELPER_H
#define TSOS_ASSOCIATION_HELPER_H

//#include "GaudiKernel/ToolHandle.h"
//#include "TrkToolInterfaces/ITrackHoleSearchTool.h"
#include "AthContainers/DataVector.h"

#include <vector>
#include <map>
#include <string>

class AtlasDetectorID;
class PixelID;

namespace Rec{
class TrackParticle;
}

namespace Trk{
  class Track;
  class TrackStateOnSurface;
}

namespace D3PD {


class TSOSAssociationHelper {
 public:
  TSOSAssociationHelper();
  virtual ~TSOSAssociationHelper();

  StatusCode doReset (const Rec::TrackParticle& p);
  StatusCode doReset (const Trk::Track& p);
  const Trk::TrackStateOnSurface* getNext();

protected:

  int getDetectorType(const Trk::TrackStateOnSurface *TSO);
  bool isBLayer(const Trk::TrackStateOnSurface *TSO);

  /* Hole search tool **/
  /*   ToolHandle<Trk::ITrackHoleSearchTool> m_idHoleSearch; */
  /*   bool m_doHolesInDet; */

  const AtlasDetectorID* m_idHelper;
  const PixelID* m_pixelId;

  std::map<std::string, bool> m_getTypes;

  DataVector<const Trk::TrackStateOnSurface>::const_iterator m_TSOItr;
  DataVector<const Trk::TrackStateOnSurface>::const_iterator m_TSOEnd;
};

} // namespace D3PD

#endif

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRACKD3PD_TRKTRACKTSOASSOCIATIONTOOL_H
#define TRACKD3PD_TRKTRACKTSOASSOCIATIONTOOL_H

#include "D3PDMakerUtils/MultiAssociationTool.h"
#include "TSOSAssociationHelper.h"

namespace Trk{
  class Track;
  class TrackStateOnSurface;
}

namespace D3PD {

class TrkTrackTSOAssociationTool
  : public MultiAssociationTool<Trk::Track, Trk::TrackStateOnSurface>,
    TSOSAssociationHelper
{
public:

  typedef MultiAssociationTool<Trk::Track, Trk::TrackStateOnSurface> Base;

  TrkTrackTSOAssociationTool (const std::string& type,
			      const std::string& name,
			      const IInterface* parent);

  virtual StatusCode initialize();

  /**
   * @brief Start the iteration for a new association.
   * @param p The object from which to associate.
   */
  virtual StatusCode reset (const Trk::Track& p);

  /**
   * @brief Return a pointer to the next element in the association.
   *
   * Return 0 when the association has been exhausted.
   */
  virtual const Trk::TrackStateOnSurface* next();

};


} // namespace D3PD


#endif // not TRACKD3PD_TRKTRACKTSOASSOCIATIONTOOL_H

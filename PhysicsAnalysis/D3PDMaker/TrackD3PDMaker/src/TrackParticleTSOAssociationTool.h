/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRACKD3PD_TRACKPARTICLETSOASSOCIATIONTOOL_H
#define TRACKD3PD_TRACKPARTICLETSOASSOCIATIONTOOL_H

#include "D3PDMakerUtils/MultiAssociationTool.h"
#include "TSOSAssociationHelper.h"

namespace Rec{
class TrackParticle;
}

namespace Trk{
  class TrackStateOnSurface;
}

namespace D3PD {

class TrackParticleTSOAssociationTool
  : public MultiAssociationTool<Rec::TrackParticle, Trk::TrackStateOnSurface>,
    TSOSAssociationHelper
{
public:

  typedef MultiAssociationTool<Rec::TrackParticle, Trk::TrackStateOnSurface> Base;

  TrackParticleTSOAssociationTool (const std::string& type,
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

};


} // namespace D3PD


#endif // not TRACKD3PD_TRACKPARTICLETSOASSOCIATIONTOOL_H

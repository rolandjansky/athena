// This file's extension implies that it's C, but it's really -*- C++ -*-.

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: PerigeeCovarianceAssociationTool.h 281348 2010-02-24 23:15:11Z zaidan $
/**
 * @file TrackD3PDMaker/src/PerigeeCovarianceAssociationTool.h
 * @author remi zaidan <remi.zaidan@cern.ch>
 * @date Feb, 2010
 * @brief Associate from a Perigee to its ErrorMatrix.
 */
#ifndef TRACKD3PDMAKER_PERIGEECOVARIANCEASSOCIATIONTOOL_H
#define TRACKD3PDMAKER_PERIGEECOVARIANCEASSOCIATIONTOOL_H

#include "D3PDMakerUtils/SingleAssociationTool.h"
#include "EventPrimitives/EventPrimitives.h"
#include "TrkParameters/TrackParameters.h"

namespace D3PD {

/**
 * @brief Associate from a Perigee to its ErrorMatrix.
 */
class PerigeeCovarianceAssociationTool
  : public SingleAssociationTool<Trk::TrackParameters, AmgSymMatrix(5)>
{
public:
  typedef SingleAssociationTool<Trk::TrackParameters, AmgSymMatrix(5)> Base;

  /**
   * @brief Standard Gaudi tool constructor.
   * @param type The name of the tool type.
   * @param name The tool name.
   * @param parent The tool's Gaudi parent.
   */
  PerigeeCovarianceAssociationTool (const std::string& type,
				    const std::string& name,
				    const IInterface* parent);


  /**
   * @brief Return the target object.
   * @param p The source object for the association.
   *
   * Return the target of the association, or 0.
   */
  virtual const AmgSymMatrix(5)* get(const Trk::TrackParameters& p);
};


} // namespace D3PD



#endif // not TRACKD3PDMAKER_PERIGEECOVARIANCEASSOCIATIONTOOL_H

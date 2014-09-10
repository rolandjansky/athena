// This file's extension implies that it's C, but it's really -*- C++ -*-.

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: TSOFitQualityAssociationTool.h 281348 2010-02-24 23:15:11Z zaidan $
/**
 * @file TrackD3PDMaker/src/TSOFitQualityAssociationTool.h
 * @author remi zaidan <remi.zaidan@cern.ch>
 * @date Feb, 2010
 * @brief Associate from a TSOS to its FitQualityOnSurface.
 */
#ifndef TRACKD3PDMAKER_TSOFITQUALITYASSOCIATIONTOOL_H
#define TRACKD3PDMAKER_TSOFITQUALITYASSOCIATIONTOOL_H

#include "D3PDMakerUtils/SingleAssociationTool.h"
#include "TrkEventPrimitives/FitQualityOnSurface.h"

namespace Trk {
  class TrackStateOnSurface;
}

namespace D3PD {

/**
 * @brief Associate from a TSOS to its FitQualityOnSurface.
 */
class TSOFitQualityAssociationTool
  : public SingleAssociationTool<Trk::TrackStateOnSurface, Trk::FitQualityOnSurface>
{
public:
  typedef SingleAssociationTool<Trk::TrackStateOnSurface, Trk::FitQualityOnSurface> Base;

  /**
   * @brief Standard Gaudi tool constructor.
   * @param type The name of the tool type.
   * @param name The tool name.
   * @param parent The tool's Gaudi parent.
   */
  TSOFitQualityAssociationTool (const std::string& type,
				 const std::string& name,
				 const IInterface* parent);


  /**
   * @brief Return the target object.
   * @param p The source object for the association.
   *
   * Return the target of the association, or 0.
   */
  virtual const Trk::FitQualityOnSurface* get (const Trk::TrackStateOnSurface& p);
};


} // namespace D3PD



#endif // not TRACKD3PDMAKER_TSOFITQUALITYASSOCIATIONTOOL_H

// This file's extension implies that it's C, but it's really -*- C++ -*-.

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: TrkTrackFitQualityAssociationTool.h 281348 2010-02-24 23:15:11Z zaidan $
/**
 * @file TrackD3PDMaker/src/TrkTrackFitQualityAssociationTool.h
 * @author remi zaidan <remi.zaidan@cern.ch>
 * @date Feb, 2010
 * @brief Associate from a TrkTrack to its FitQuality.
 */
#ifndef TRACKD3PDMAKER_TRKTRACKFITQUALITYASSOCIATIONTOOL_H
#define TRACKD3PDMAKER_TRKTRACKFITQUALITYASSOCIATIONTOOL_H

#include "D3PDMakerUtils/SingleAssociationTool.h"

namespace Trk {
  class Track;
  class FitQuality;
}

namespace D3PD {

/**
 * @brief Associate from a Trk::Track to its FitQuality.
 */
class TrkTrackFitQualityAssociationTool
  : public SingleAssociationTool<Trk::Track, Trk::FitQuality>
{
public:
  typedef SingleAssociationTool<Trk::Track, Trk::FitQuality> Base;

  /**
   * @brief Standard Gaudi tool constructor.
   * @param type The name of the tool type.
   * @param name The tool name.
   * @param parent The tool's Gaudi parent.
   */
  TrkTrackFitQualityAssociationTool (const std::string& type,
				       const std::string& name,
				       const IInterface* parent);


  /**
   * @brief Return the target object.
   * @param p The source object for the association.
   *
   * Return the target of the association, or 0.
   */
  virtual const Trk::FitQuality* get (const Trk::Track& p);
};


} // namespace D3PD



#endif // not TRACKD3PDMAKER_TRKTRACKFITQUALITYASSOCIATIONTOOL_H

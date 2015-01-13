/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/



#ifndef TRKITRACKSUMMARYTOOL_H
#define TRKITRACKSUMMARYTOOL_H

#include "GaudiKernel/IAlgTool.h"

namespace Trk {

class Track;
class TrackSummary;

static const InterfaceID IID_ITrackSummaryTool("Trk::ITrackSummaryTool", 1, 0);

/** @class ITrackSummaryTool
    @brief Interface for condensing Trk::Track properties and associated
           hits to a (non-fittable) foot print, the Trk::TrackSummary object.

    @author Edward Moyse, Martin Siebel <http://consult.cern.ch/xwho>
*/
class ITrackSummaryTool : virtual public IAlgTool {
  public:
  static const InterfaceID& interfaceID( ) ;

  /** create a summary object from passed Track. The summary object belongs to
      you, the user, and so you must take care of deletion of it.*/
  virtual const TrackSummary* createSummary( const Track& track, bool onlyUpdateTrack=false ) const = 0;

  /** create a summary object of passed track without doing the tedious hole search.
      Same comments as for createSummary apply here, of course, too. */	   
  virtual const Trk::TrackSummary* createSummaryNoHoleSearch( const Track& track ) = 0;
  
  /** method which can be used to update the track and add a summary to it.
   *  this can be used to add a summary to a track and then retrieve it from it without the need to clone. */
  virtual void updateTrack(Track& track) const = 0;
  
  /** method to update the shared hit content only, this is optimised for track collection merging. */
  virtual void updateSharedHitCount(Track& track) const = 0;


  virtual void updateAdditionalInfo(Track& track) const = 0;
  
};

inline const InterfaceID& Trk::ITrackSummaryTool::interfaceID()
{ 
	return IID_ITrackSummaryTool; 
}


}
#endif 

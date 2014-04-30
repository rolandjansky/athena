/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// ITrackSelectionTool.h, (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////
#ifndef ITRKAMBITRACKSELECTIONTOOL_H
#define ITRKAMBITRACKSELECTIONTOOL_H

#include "GaudiKernel/IAlgTool.h"
#include "TrkEventPrimitives/TrackScore.h"
#include <vector>

namespace Trk 
{
  class Track;
  class PrepRawData;
  
  static const InterfaceID IID_IAmbiTrackSelectionTool("InDet::IAmbiTrackSelectionTool", 1, 0);

  /** @class Trk::IAmbiTrackSelectionTool
      @brief Interface for building new tracks using information about shared and already associated hits.

      @author Martin Siebel <http://consult.cern.ch/xwho>
   */

  class IAmbiTrackSelectionTool : virtual public IAlgTool {
  public:
    static const InterfaceID& interfaceID( ) ;

    /** performs cleaning of a track from already used hits.
        returns input pointer, if track is ok as is,
        returns different pointer, if a new, fragmentary Trk::Track has been created,
        returns 0, if the track should be discarded altogether.
	No delete operation is done, the client has to take care of this!
    */
    virtual const Trk::Track* getCleanedOutTrack(const Trk::Track*, const Trk::TrackScore score) =0;

    /** These methods are meant to access some functionality of the association tool
        used by the selection tool. For their functionality please, see IPRD_AssociationTool.
    */
    virtual StatusCode registerPRDs(const Trk::Track* ptrTrack) = 0;
    virtual void reset() = 0;
    virtual std::vector<const Trk::PrepRawData*> getPrdsOnTrack(const Trk::Track* ptrTrack)=0;
  };

  inline const InterfaceID& Trk::IAmbiTrackSelectionTool::interfaceID()
    { 
      return IID_IAmbiTrackSelectionTool; 
    }

} // end of namespace

#endif 

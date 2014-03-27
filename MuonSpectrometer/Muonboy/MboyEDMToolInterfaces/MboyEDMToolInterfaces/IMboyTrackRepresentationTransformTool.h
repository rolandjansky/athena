/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef IMboyTrackRepresentationTransformTool_H
#define IMboyTrackRepresentationTransformTool_H

#include "GaudiKernel/IAlgTool.h" 

//Can not be forwarded due to typedef
#include "TrkTrack/Track.h"
#include "TrkTrack/TrackCollection.h"

/////////////////////////////////////////////////////////////////////////////
#include "MboyEDMToolInterfaces/IMboyEDMToolMisc.h"

namespace Muon {

static const InterfaceID IID_IMboyTrackRepresentationTransformTool("IMboyTrackRepresentationTransformTool", 1, 0);

  /**
   @class IMboyTrackRepresentationTransformTool

  @author samusog@cern.ch
  
  */

class IMboyTrackRepresentationTransformTool:virtual public IAlgTool {

public:
///////////////////////////////////

   static const InterfaceID& interfaceID();

   virtual const Trk::TrackParameters* AtACOrAtAD2MeasuredPerigee(const Trk::TrackParameters* pTrackParameters) = 0 ;
   virtual const Trk::TrackParameters* MeasuredPerigee2AtACOrAtAD(const Trk::TrackParameters* pTrackParameters) = 0 ;
  
};

inline const InterfaceID& Muon::IMboyTrackRepresentationTransformTool::interfaceID() { return IID_IMboyTrackRepresentationTransformTool;}

}

#endif


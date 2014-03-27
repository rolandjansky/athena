/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef IMboyTrackUndressingTool_H
#define IMboyTrackUndressingTool_H

#include "GaudiKernel/IAlgTool.h" 

//Can not be forwarded due to typedef
#include "TrkTrack/Track.h"
#include "TrkTrack/TrackCollection.h"

#include "TrkTrack/TrackStateOnSurface.h"

/////////////////////////////////////////////////////////////////////////////
#include "MboyEDMToolInterfaces/IMboyEDMToolMisc.h"


namespace Muon {

static const InterfaceID IID_IMboyTrackUndressingTool("IMboyTrackUndressingTool", 1, 0);

  /**
   @class IMboyTrackUndressingTool

  @author samusog@cern.ch
  
  */

class IMboyTrackUndressingTool:virtual public IAlgTool {

public:
///////////////////////////////////

   static const InterfaceID& interfaceID();

   virtual Trk::Track* UndressTrack (const Trk::Track* pTrack_atIP) = 0 ;
   virtual DataVector<const Trk::TrackStateOnSurface>* UndressTrackTSOSs(const Trk::Track* pTrack_atIP) = 0 ;

};

inline const InterfaceID& Muon::IMboyTrackUndressingTool::interfaceID() { return IID_IMboyTrackUndressingTool;}

}

#endif


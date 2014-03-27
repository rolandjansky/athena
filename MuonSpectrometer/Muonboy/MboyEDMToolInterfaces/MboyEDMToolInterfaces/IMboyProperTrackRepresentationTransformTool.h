/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef IMboyProperTrackRepresentationTransformTool_H
#define IMboyProperTrackRepresentationTransformTool_H

#include "GaudiKernel/IAlgTool.h" 

//Can not be forwarded due to typedef
#include "TrkTrack/Track.h"
#include "TrkTrack/TrackCollection.h"

/////////////////////////////////////////////////////////////////////////////
#include "MboyEDMToolInterfaces/IMboyEDMToolMisc.h"

class MB_Track_MboyCyl ;
class MB_Track_Perigee ;
class MB_Track_SpecEntr ;

namespace Muon {

static const InterfaceID IID_IMboyProperTrackRepresentationTransformTool("IMboyProperTrackRepresentationTransformTool", 1, 0);

  /**
   @class IMboyProperTrackRepresentationTransformTool

  @author samusog@cern.ch
  
  */

class IMboyProperTrackRepresentationTransformTool:virtual public IAlgTool {

public:
///////////////////////////////////

   static const InterfaceID& interfaceID();

   /** return the appropriate track measurements */
   virtual const Trk::TrackParameters* TrkParMboyToAtaCylinderOrAtaDisc(const MB_Track_MboyCyl* pMB_Track_MboyCyl) = 0 ; 

   virtual const Trk::TrackParameters* TrkParPerigee(const MB_Track_Perigee* pMB_Track_Perigee) = 0 ;

   virtual MB_Track_SpecEntr* AtaCylinderOrAtaDiscToTrkParMboy(const Trk::TrackParameters* pTrackParameters) = 0 ; 

};

inline const InterfaceID& Muon::IMboyProperTrackRepresentationTransformTool::interfaceID() { return IID_IMboyProperTrackRepresentationTransformTool;}

}

#endif


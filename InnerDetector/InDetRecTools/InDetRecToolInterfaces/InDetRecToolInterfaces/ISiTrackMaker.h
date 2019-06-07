/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

/////////////////////////////////////////////////////////////////////////////////
//  Header file for class ISiTrackMaker
/////////////////////////////////////////////////////////////////////////////////
// (c) ATLAS Detector software
/////////////////////////////////////////////////////////////////////////////////
//  Base class for local track-finding in Pixels and SCT
/////////////////////////////////////////////////////////////////////////////////
// Version 1.0 26/02/2005 I.Gavrilenko
/////////////////////////////////////////////////////////////////////////////////

#ifndef ISiTrackMaker_H
#define ISiTrackMaker_H

#include "GeoPrimitives/GeoPrimitives.h"

#include "TrkParameters/TrackParameters.h"
#include "TrkSpacePoint/SpacePoint.h"
#include "TrkTrack/Track.h"

#include "GaudiKernel/AlgTool.h"

#include <list>

class MsgStream;

namespace InDetDD {
  class SiDetectorElement;
}

namespace InDet {

  class SiTrackMakerEventData_xk;

  static const InterfaceID IID_ISiTrackMaker
    ("InDet::ISiTrackMaker",1,0);

  class ISiTrackMaker : virtual public IAlgTool 
    {
      ///////////////////////////////////////////////////////////////////
      // Public methods:
      ///////////////////////////////////////////////////////////////////
      
    public:

      // InterfaceID
      DeclareInterfaceID(ISiTrackMaker, 1, 0);

      ///////////////////////////////////////////////////////////////////
      // Main methods for track-finding
      ///////////////////////////////////////////////////////////////////

      virtual std::list<Trk::Track*>
	getTracks(SiTrackMakerEventData_xk& data, const std::list<const Trk::SpacePoint*>&) const =0;
      virtual std::list<Trk::Track*>
	getTracks(SiTrackMakerEventData_xk& data, const Trk::TrackParameters&, const std::list<Amg::Vector3D>&) const =0;
      
      virtual void newEvent(SiTrackMakerEventData_xk& data, bool, bool) const =0;
      virtual void newTrigEvent(SiTrackMakerEventData_xk& data, bool, bool) const =0;
      
      virtual void endEvent(SiTrackMakerEventData_xk& data) const =0;
     

      ///////////////////////////////////////////////////////////////////
      // Print internal tool parameters and status
      ///////////////////////////////////////////////////////////////////
     
      virtual MsgStream& dump(SiTrackMakerEventData_xk& data, MsgStream& out) const=0;
     
    };

} // end of name space


#endif // ISiTrackMaker_H


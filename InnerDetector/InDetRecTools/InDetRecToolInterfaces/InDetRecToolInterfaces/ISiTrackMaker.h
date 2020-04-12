/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

/////////////////////////////////////////////////////////////////////////////////
//  Header file for class ISiTrackMaker
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
#include "GaudiKernel/EventContext.h"

#include <list>

class MsgStream;

namespace InDetDD {
  class SiDetectorElement;
}

namespace InDet {

  class SiTrackMakerEventData_xk;

  static const InterfaceID IID_ISiTrackMaker
    ("InDet::ISiTrackMaker",1,0);

  /**
   * @class ISiTrackMaker
   * Base class for local track-finding in Pixels and SCT
   *
   * In AthenaMT, event dependent cache inside ISiTrackMaker is not
   * preferred. SiTrackMakerEventData_xk class holds event dependent
   * data for ISiTrackMaker and its object is instantiated in
   * SiSPSeededTrackFinder::execute as an inherited class,
   * ExtendedSiTrackMakerEventData_xk.
   */

  class ISiTrackMaker : virtual public IAlgTool 
    {
      ///////////////////////////////////////////////////////////////////
      // Public methods:
      ///////////////////////////////////////////////////////////////////
      
    public:

      /// @name InterfaceID
      //@{
      DeclareInterfaceID(ISiTrackMaker, 1, 0);
      //@}

      ///////////////////////////////////////////////////////////////////
      /// @name Main methods for track-finding
      ///////////////////////////////////////////////////////////////////
      //@{
      virtual std::list<Trk::Track*>
        getTracks(const EventContext& ctx, SiTrackMakerEventData_xk& data, const std::vector<const Trk::SpacePoint*>&) const =0;
      virtual std::list<Trk::Track*>
	getTracks(const EventContext& ctx, SiTrackMakerEventData_xk& data, const Trk::TrackParameters&, const std::list<Amg::Vector3D>&) const =0;

      virtual void newEvent(const EventContext& ctx, SiTrackMakerEventData_xk& data, bool, bool) const =0;

      virtual void newTrigEvent(const EventContext& ctx, SiTrackMakerEventData_xk& data, bool, bool) const =0;

      virtual void endEvent(SiTrackMakerEventData_xk& data) const =0;
      //@}

      ///////////////////////////////////////////////////////////////////
      /// @name Print internal tool parameters and status
      ///////////////////////////////////////////////////////////////////
      //@{
      virtual MsgStream& dump(SiTrackMakerEventData_xk& data, MsgStream& out) const=0;
      //@}

    };

} // end of name space


#endif // ISiTrackMaker_H


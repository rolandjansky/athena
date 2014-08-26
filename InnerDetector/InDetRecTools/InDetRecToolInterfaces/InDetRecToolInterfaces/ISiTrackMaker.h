/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
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

#include <list>
#include "GaudiKernel/AlgTool.h"
#include "TrkParameters/TrackParameters.h"
#include "TrkSpacePoint/SpacePoint.h"
#include "GeoPrimitives/GeoPrimitives.h"
#include "TrkTrack/Track.h"

class MsgStream;

namespace InDetDD {
  class SiDetectorElement;
}

namespace InDet {

  static const InterfaceID IID_ISiTrackMaker
    ("InDet::ISiTrackMaker",1,0);

  class ISiTrackMaker : virtual public IAlgTool 
    {
      ///////////////////////////////////////////////////////////////////
      // Public methods:
      ///////////////////////////////////////////////////////////////////
      
    public:

      ///////////////////////////////////////////////////////////////////
      // Standard tool methods
      ///////////////////////////////////////////////////////////////////

      static const InterfaceID& interfaceID();
      virtual StatusCode initialize ()=0;
      virtual StatusCode finalize   ()=0;

      ///////////////////////////////////////////////////////////////////
      // Main methods for track-finding
      ///////////////////////////////////////////////////////////////////

      virtual const std::list<Trk::Track*>& 
	getTracks(const std::list<const Trk::SpacePoint*>&)=0;
      virtual const std::list<Trk::Track*>& 
	getTracks(const Trk::TrackParameters&,const std::list<Amg::Vector3D>&)=0;
      
      virtual void newEvent    (bool,bool)=0;
      virtual void newTrigEvent(bool,bool)=0;
      
      virtual void endEvent()=0;
     

      ///////////////////////////////////////////////////////////////////
      // Print internal tool parameters and status
      ///////////////////////////////////////////////////////////////////
     
      virtual MsgStream&    dump(MsgStream&    out) const=0;
      virtual std::ostream& dump(std::ostream& out) const=0;
     
    };
  
  ///////////////////////////////////////////////////////////////////
  // Overload of << operator for MsgStream and  std::ostream
  ///////////////////////////////////////////////////////////////////
  
  MsgStream&    operator << (MsgStream&   ,const ISiTrackMaker&);
  std::ostream& operator << (std::ostream&,const ISiTrackMaker&);
  
  ///////////////////////////////////////////////////////////////////
  // Inline methods
  ///////////////////////////////////////////////////////////////////

  inline const InterfaceID& ISiTrackMaker::interfaceID()
    {
      return IID_ISiTrackMaker;
    }

  ///////////////////////////////////////////////////////////////////
  // Overload of << operator MsgStream
  ///////////////////////////////////////////////////////////////////
   
  inline MsgStream& operator    << 
    (MsgStream& sl,const ISiTrackMaker& se)
    { 
      return se.dump(sl); 
    }
  ///////////////////////////////////////////////////////////////////
  // Overload of << operator std::ostream
  ///////////////////////////////////////////////////////////////////
  
  inline std::ostream& operator << 
    (std::ostream& sl,const ISiTrackMaker& se)
    { 
      return se.dump(sl); 
    }   

} // end of name space


#endif // ISiTrackMaker_H


/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/////////////////////////////////////////////////////////////////////////////////
//  Header file for class ITRT_DetElementsRoadMaker
/////////////////////////////////////////////////////////////////////////////////
// (c) ATLAS Detector software
/////////////////////////////////////////////////////////////////////////////////
//  Base class for detector elements road builder in TRT
//  All detector elements should be destributed in propagation order.
/////////////////////////////////////////////////////////////////////////////////
// Version 1.0 11/08/2005 I.Gavrilenko
/////////////////////////////////////////////////////////////////////////////////

#ifndef ITRT_DetElementsRoadMaker_H
#define ITRT_DetElementsRoadMaker_H

#include <list>
#include "GaudiKernel/AlgTool.h"
#include "TrkEventPrimitives/PropDirection.h"
#include "TrkParameters/TrackParameters.h"

class MsgStream;

namespace InDetDD {
  class TRT_BaseElement;
}

namespace InDet {

 
  static const InterfaceID IID_ITRT_DetElementsRoadMaker
    ("InDet::ITRT_DetElementsRoadMaker",1,0);

  class ITRT_DetElementsRoadMaker : virtual public IAlgTool 
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
      // Main methods for road builder
      ///////////////////////////////////////////////////////////////////
      
      virtual void detElementsRoad
	(const Trk::TrackParameters&,Trk::PropDirection, 
	 std::list<const InDetDD::TRT_BaseElement*>&)=0;

      virtual void detElementsRoad
	(const Trk::TrackParameters&,Trk::PropDirection, 
	 std::list<std::pair<const InDetDD::TRT_BaseElement*,const Trk::TrackParameters*> >&)=0;
      
      ///////////////////////////////////////////////////////////////////
      // Print internal tool parameters and status
      ///////////////////////////////////////////////////////////////////
     
      virtual MsgStream&    dump(MsgStream&    out) const=0;
      virtual std::ostream& dump(std::ostream& out) const=0;
     
    };
  
  ///////////////////////////////////////////////////////////////////
  // Overload of << operator for MsgStream and  std::ostream
  ///////////////////////////////////////////////////////////////////
  
  MsgStream&    operator << (MsgStream&   ,const ITRT_DetElementsRoadMaker&);
  std::ostream& operator << (std::ostream&,const ITRT_DetElementsRoadMaker&);
  
  ///////////////////////////////////////////////////////////////////
  // Inline methods
  ///////////////////////////////////////////////////////////////////

  inline const InterfaceID& ITRT_DetElementsRoadMaker::interfaceID()
    {
      return IID_ITRT_DetElementsRoadMaker;
    }

  ///////////////////////////////////////////////////////////////////
  // Overload of << operator MsgStream
  ///////////////////////////////////////////////////////////////////
   
  inline MsgStream& operator    << 
    (MsgStream& sl,const ITRT_DetElementsRoadMaker& se)
    { 
      return se.dump(sl); 
    }
  ///////////////////////////////////////////////////////////////////
  // Overload of << operator std::ostream
  ///////////////////////////////////////////////////////////////////
  
  inline std::ostream& operator << 
    (std::ostream& sl,const ITRT_DetElementsRoadMaker& se)
    { 
      return se.dump(sl); 
    }   

} // end of name space


#endif // ITRT_DetElementsRoadMaker_H


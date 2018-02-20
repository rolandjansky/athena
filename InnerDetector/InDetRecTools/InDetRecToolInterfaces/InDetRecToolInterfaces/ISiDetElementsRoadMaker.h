/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/////////////////////////////////////////////////////////////////////////////////
//  Header file for class ISiDetElementsRoadMaker
/////////////////////////////////////////////////////////////////////////////////
// (c) ATLAS Detector software
/////////////////////////////////////////////////////////////////////////////////
//  Base class for detector elements road builder in Pixels and SCT
//  All detector elements should be destributed in propagation order.
/////////////////////////////////////////////////////////////////////////////////
// Version 1.0 26/02/2005 I.Gavrilenko
/////////////////////////////////////////////////////////////////////////////////

#ifndef ISiDetElementsRoadMaker_H
#define ISiDetElementsRoadMaker_H

#include <list>
#include "GaudiKernel/AlgTool.h"
#include "TrkEventPrimitives/PropDirection.h"
#include "TrkParameters/TrackParameters.h"
#include "TrkSpacePoint/SpacePoint.h"
#include "GeoPrimitives/GeoPrimitives.h"

class MsgStream;

namespace InDetDD {
  class SiDetectorElement;
}

namespace InDet {

 
  static const InterfaceID IID_ISiDetElementsRoadMaker
    ("InDet::ISiDetElementsRoadMaker",1,0);

  class ISiDetElementsRoadMaker : virtual public IAlgTool 
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
	(const std::list<const Trk::SpacePoint*>&,
	std::vector<const InDetDD::SiDetectorElement*>&)=0;

      virtual void detElementsRoad
	(std::list<Amg::Vector3D>&,
	 std::vector<const InDetDD::SiDetectorElement*>&)=0;
    
      virtual void detElementsRoad
	(const Trk::TrackParameters&,Trk::PropDirection, 
	 std::vector<const InDetDD::SiDetectorElement*>&)=0;
      
      ///////////////////////////////////////////////////////////////////
      // Print internal tool parameters and status
      ///////////////////////////////////////////////////////////////////
     
      virtual MsgStream&    dump(MsgStream&    out) const=0;
      virtual std::ostream& dump(std::ostream& out) const=0;
     
    };
  
  ///////////////////////////////////////////////////////////////////
  // Overload of << operator for MsgStream and  std::ostream
  ///////////////////////////////////////////////////////////////////
  
  MsgStream&    operator << (MsgStream&   ,const ISiDetElementsRoadMaker&);
  std::ostream& operator << (std::ostream&,const ISiDetElementsRoadMaker&);
  
  ///////////////////////////////////////////////////////////////////
  // Inline methods
  ///////////////////////////////////////////////////////////////////

  inline const InterfaceID& ISiDetElementsRoadMaker::interfaceID()
    {
      return IID_ISiDetElementsRoadMaker;
    }

  ///////////////////////////////////////////////////////////////////
  // Overload of << operator MsgStream
  ///////////////////////////////////////////////////////////////////
   
  inline MsgStream& operator    << 
    (MsgStream& sl,const ISiDetElementsRoadMaker& se)
    { 
      return se.dump(sl); 
    }
  ///////////////////////////////////////////////////////////////////
  // Overload of << operator std::ostream
  ///////////////////////////////////////////////////////////////////
  
  inline std::ostream& operator << 
    (std::ostream& sl,const ISiDetElementsRoadMaker& se)
    { 
      return se.dump(sl); 
    }   

} // end of name space


#endif // ISiDetElementsRoadMaker_H


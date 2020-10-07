/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

/////////////////////////////////////////////////////////////////////////////////
//  Header file for class ISiDetElementsRoadMaker
/////////////////////////////////////////////////////////////////////////////////
// Version 1.0 26/02/2005 I.Gavrilenko
/////////////////////////////////////////////////////////////////////////////////

#ifndef ISiDetElementsRoadMaker_H
#define ISiDetElementsRoadMaker_H

#include "GeoPrimitives/GeoPrimitives.h"
#include "TrkEventPrimitives/PropDirection.h"
#include "TrkParameters/TrackParameters.h"
#include "TrkSpacePoint/SpacePoint.h"

#include "GaudiKernel/AlgTool.h"
#include "GaudiKernel/EventContext.h"
#include "MagFieldElements/AtlasFieldCache.h"

#include <list>

class MsgStream;

namespace InDetDD {
  class SiDetectorElement;
}

namespace InDet {
  class SiDetElementRoadMakerData_xk; 

  /**
   * @class ISiDetElementsRoadMaker
   * Base class for detector elements road builder in Pixels and SCT.
   * All detector elements should be destributed in propagation order.
   */

  class ISiDetElementsRoadMaker : virtual public IAlgTool 
    {
      ///////////////////////////////////////////////////////////////////
      // Public methods:
      ///////////////////////////////////////////////////////////////////
      
    public:

      /// @name InterfaceID
      //@{
      DeclareInterfaceID(ISiDetElementsRoadMaker, 1, 0);
      //@}

      ///////////////////////////////////////////////////////////////////
      /// @name Main methods for road builder
      ///////////////////////////////////////////////////////////////////
      //@{
      virtual void detElementsRoad
	(std::list<Amg::Vector3D>&,
	 std::list<const InDetDD::SiDetectorElement*>&,
         bool test, InDet::SiDetElementRoadMakerData_xk &) const=0;

      virtual void detElementsRoad
      (const EventContext& ctx,
       MagField::AtlasFieldCache& fieldCache,
       const Trk::TrackParameters&,Trk::PropDirection,
       std::list<const InDetDD::SiDetectorElement*>&, InDet::SiDetElementRoadMakerData_xk &) const=0;
      //@} 

      ///////////////////////////////////////////////////////////////////
      /// @name Print internal tool parameters and status
      ///////////////////////////////////////////////////////////////////
      //@{
      virtual MsgStream&    dump(MsgStream&    out) const=0;
      virtual std::ostream& dump(std::ostream& out) const=0;
      //@}

    };
  
  ///////////////////////////////////////////////////////////////////
  // Overload of << operator for MsgStream and  std::ostream
  ///////////////////////////////////////////////////////////////////
  
  MsgStream&    operator << (MsgStream&   ,const ISiDetElementsRoadMaker&);
  std::ostream& operator << (std::ostream&,const ISiDetElementsRoadMaker&);
  
  ///////////////////////////////////////////////////////////////////
  // Inline methods
  ///////////////////////////////////////////////////////////////////

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


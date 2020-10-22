/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
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
      /// This signature assumes you already have a list of positions
      /// along the trajectory. It will look for detector elements 
      /// compatible with being crossed by the linearised 
      /// trajectory provided and fill those into the 'Road' argument. 
      /// If 'testDirection' is used, we only fill detector elements 
      /// encountered while traversing in the positive direction. 
      /// 
      /// @param[in] globalPositions: set of points along the trajectory. Will linearise between them. 
      /// @param[out] Road: List to be populated with the elements of the search road. Will be sorted along the trajectory. 
      /// @param[in] testDirection: If set, avoid adding detector elements encountered only when travelling in the negative direction. Set true for inside-out tracking, false for cosmic tracking. 
      /// @param[in,out] roadMakerData: event data object used to cache information during an event in a thread-safe way 
      //@{
      virtual void detElementsRoad
      ( std::list<Amg::Vector3D>& globalPositions,
	      std::list<const InDetDD::SiDetectorElement*>& Road,
        bool testDirection, 
        InDet::SiDetElementRoadMakerData_xk & roadMakerData,
        const EventContext& ctx) const=0;

      /// This is the signature used in most ATLAS clients. 
      /// @param[in] ctx: Event context
      /// @param[in] fieldCache: Magnetic field cache
      /// @param[in] Tp: Track parameter hypothesis used for road building. For example obtained from a seed. Will be used to populate a set of space points along the expected trajectory, and to search 
      /// for detector elements along this linearised trajectory using the signature above
      /// @param[in] direction: Direction of propagation - either along (inside out) or against (cosmic) momentum.
      /// @param[out] Road: List to be populated with the elements of the search road. Will be sorted along the trajectory. 
      /// @param[in,out] roadMakerData: event data object used to cache information during an event in a thread-safe way   
      virtual void detElementsRoad
      (const EventContext& ctx,
       MagField::AtlasFieldCache& fieldCache,
       const Trk::TrackParameters& Tp,
       Trk::PropDirection direction,
       std::list<const InDetDD::SiDetectorElement*>& Road, 
       InDet::SiDetElementRoadMakerData_xk & roadMakerData) const=0;
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


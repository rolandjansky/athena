/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/////////////////////////////////////////////////////////////////////////////////
//  Header file for class ITRT_SeededSpacePointFinder
/////////////////////////////////////////////////////////////////////////////////
// (c) ATLAS Detector software
/////////////////////////////////////////////////////////////////////////////////
//
//  Base class for track candidates generation using space points information
//
//  Example implementation
//
//  seedsmaker->newEvent();      
//
//  const InDet::SiSpacePointsSeed* seed = 0;
//  seedsmaker->find2Sp();
//  while((seed=seedsmaker->next())) {
//       do some method with two space points seed;
//  }
//
/////////////////////////////////////////////////////////////////////////////////
// Version 1.0 04/18/2006 T.Koffas
/////////////////////////////////////////////////////////////////////////////////

#ifndef ITRT_SeededSpacePointFinder_H
#define ITRT_SeededSpacePointFinder_H

#include "GaudiKernel/AlgTool.h"
#include "Identifier/IdentifierHash.h"
#include "SiSpacePointsSeed/SiSpacePointsSeed.h"
#include "TrkParameters/TrackParameters.h"

class MsgStream;

namespace InDet {

  class SiSpacePointsSeed;
 
  static const InterfaceID IID_ITRT_SeededSpacePointFinder
    ("InDet::ITRT_SeededSpacePointFinder",1,0);

  class ITRT_SeededSpacePointFinder : virtual public IAlgTool 
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
      // Methods to initialize tool for new event or region
      ///////////////////////////////////////////////////////////////////

      virtual void newEvent ()=0;
      virtual void newRegion
	(const std::vector<IdentifierHash>&,const std::vector<IdentifierHash>&)=0;
      
      ///////////////////////////////////////////////////////////////////
      // Methods to initilize different strategies of seeds production
      // with two space points with or without vertex constraint
      ///////////////////////////////////////////////////////////////////

      virtual std::list<std::pair<const Trk::SpacePoint*,const Trk::SpacePoint*> >* 
	find2Sp (const Trk::TrackParameters& tP)=0;

      ///////////////////////////////////////////////////////////////////
      // Iterator through seeds pseudo collection produced accordingly
      // methods find    
      ///////////////////////////////////////////////////////////////////
      
      virtual const SiSpacePointsSeed* next()=0;
      
      ///////////////////////////////////////////////////////////////////
      // Print internal tool parameters and status
      ///////////////////////////////////////////////////////////////////
     
      virtual MsgStream&    dump(MsgStream&    out) const=0;
      virtual std::ostream& dump(std::ostream& out) const=0;
     
    };
  
  ///////////////////////////////////////////////////////////////////
  // Overload of << operator for MsgStream and  std::ostream
  ///////////////////////////////////////////////////////////////////
  
  MsgStream&    operator << (MsgStream&   ,const ITRT_SeededSpacePointFinder&);
  std::ostream& operator << (std::ostream&,const ITRT_SeededSpacePointFinder&);
  
  ///////////////////////////////////////////////////////////////////
  // Inline methods
  ///////////////////////////////////////////////////////////////////

  inline const InterfaceID& ITRT_SeededSpacePointFinder::interfaceID()
    {
      return IID_ITRT_SeededSpacePointFinder;
    }

  ///////////////////////////////////////////////////////////////////
  // Overload of << operator MsgStream
  ///////////////////////////////////////////////////////////////////
   
  inline MsgStream& operator    << 
    (MsgStream& sl,const ITRT_SeededSpacePointFinder& se)
    { 
      return se.dump(sl); 
    }
  ///////////////////////////////////////////////////////////////////
  // Overload of << operator std::ostream
  ///////////////////////////////////////////////////////////////////
  
  inline std::ostream& operator << 
    (std::ostream& sl,const ITRT_SeededSpacePointFinder& se)
    { 
      return se.dump(sl); 
    }   
} // end of name space


#endif // ITRT_SeededSpacePointFinder_H


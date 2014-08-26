/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/////////////////////////////////////////////////////////////////////////////////
//  Header file for class ITRT_TrackSegmentsMaker
/////////////////////////////////////////////////////////////////////////////////
// (c) ATLAS Detector software
/////////////////////////////////////////////////////////////////////////////////
//
//  Base class for track candidates generation using TRT_DriftCircles information
//
//  Example implementation
//
//  segmentsmaker->newEvent();      
//
//  segmentsmaker->find();
//
//  const Trk::TrackSegment* tracksegment = 0;
//
//  while((tracksegment=segmentsmaker->next())) {
//       do some method with TRT track segment;
//  }
//
/////////////////////////////////////////////////////////////////////////////////
// Version 1.0 11/10/2005 I.Gavrilenko
/////////////////////////////////////////////////////////////////////////////////

#ifndef ITRT_TrackSegmentsMaker_H
#define ITRT_TrackSegmentsMaker_H

#include "GaudiKernel/AlgTool.h"
#include "Identifier/IdentifierHash.h"
#include "TrkSegment/TrackSegment.h"
#include "VxVertex/Vertex.h"

class MsgStream;

namespace InDet {

  static const InterfaceID IID_ITRT_TrackSegmentsMaker
    ("InDet::ITRT_TrackSegmentsMaker",1,0);

  class ITRT_TrackSegmentsMaker : virtual public IAlgTool 
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
      virtual void newRegion(const std::vector<IdentifierHash>&)=0;
      virtual void endEvent ()=0;
     
      ///////////////////////////////////////////////////////////////////
      // Methods of seeds production without vertex constraint
      ///////////////////////////////////////////////////////////////////

      virtual void find()=0;

      ///////////////////////////////////////////////////////////////////
      // Iterator through track segments pseudo collection produced 
      // accordingly methods find    
      ///////////////////////////////////////////////////////////////////
      
      virtual Trk::TrackSegment* next()=0;
      
      ///////////////////////////////////////////////////////////////////
      // Print internal tool parameters and status
      ///////////////////////////////////////////////////////////////////
     
      virtual MsgStream&    dump(MsgStream&    out) const=0;
      virtual std::ostream& dump(std::ostream& out) const=0;
     
    };
  
  ///////////////////////////////////////////////////////////////////
  // Overload of << operator for MsgStream and  std::ostream
  ///////////////////////////////////////////////////////////////////
  
  MsgStream&    operator << (MsgStream&   ,const ITRT_TrackSegmentsMaker&);
  std::ostream& operator << (std::ostream&,const ITRT_TrackSegmentsMaker&);
  
  ///////////////////////////////////////////////////////////////////
  // Inline methods
  ///////////////////////////////////////////////////////////////////

  inline const InterfaceID& ITRT_TrackSegmentsMaker::interfaceID()
    {
      return IID_ITRT_TrackSegmentsMaker;
    }

  ///////////////////////////////////////////////////////////////////
  // Overload of << operator MsgStream
  ///////////////////////////////////////////////////////////////////
   
  inline MsgStream& operator    << 
    (MsgStream& sl,const ITRT_TrackSegmentsMaker& se)
    { 
      return se.dump(sl); 
    }

  ///////////////////////////////////////////////////////////////////
  // Overload of << operator std::ostream
  ///////////////////////////////////////////////////////////////////
  
  inline std::ostream& operator << 
    (std::ostream& sl,const ITRT_TrackSegmentsMaker& se)
    { 
      return se.dump(sl); 
    }   
} // end of name space


#endif // ITRT_TrackSegmentsMaker_H


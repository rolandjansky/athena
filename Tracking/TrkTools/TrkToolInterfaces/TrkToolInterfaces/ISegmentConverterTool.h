/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/


#ifndef ITRKSEGMENTCONVERTERTOOL_H
#define ITRKSEGMENTCONVERTERTOOL_H

#include "GaudiKernel/IAlgTool.h"


static const InterfaceID IID_ISegmentConverterTool("Trk::ISegmentConverterTool", 1, 0);

namespace Trk {

  class Track; 
  class Segment;

/** @brief Interface for tool to convert a Trk::Segment object into a Trk::Track.

  @author Christian.Schmitt@cern.ch */
  class ISegmentConverterTool : virtual public IAlgTool
  {
    public:

      static const InterfaceID& interfaceID( ) ;

  /** create a track from a given segment
      @param segment must be a valid reference (i.e. the segment must exist)
      @return the returned Track is owned by the code calling this tool
   */
      virtual Track* convert(const Segment& segment)=0; 
  };

}//end of ns

inline const InterfaceID& Trk::ISegmentConverterTool::interfaceID()
{
  return IID_ISegmentConverterTool;
}

#endif 

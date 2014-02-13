/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MUON_IMUONSEGMENTCLEANER_H
#define MUON_IMUONSEGMENTCLEANER_H

#include <vector>
#include "GaudiKernel/IAlgTool.h"

static const InterfaceID IID_IMuonSegmentCleaner
    ("Muon::IMuonSegmentCleaner",1,0);

namespace Muon {

  class MuonSegment;

  /** Interface for tools resolving ambigueties and remove outliers in Muon::MuonSegment objects */
  class IMuonSegmentCleaner : virtual public IAlgTool 
  {      
    public:
    
    static const InterfaceID& interfaceID();

    /** @brief Resolve hit ambiguities in a given segment 
	@param segment a pointer to a segment
	@return a pointer to the cleaned segment, zero if cleaning failed
	        The ownership of the new segment is passed to the client calling the tool 
    */
    virtual const MuonSegment* resolve( const MuonSegment* segment ) const = 0;

  };
  
  inline const InterfaceID& IMuonSegmentCleaner::interfaceID()
  {
    return IID_IMuonSegmentCleaner;
  }
} // end of name space

#endif // IMuonSegmentCleaner_H


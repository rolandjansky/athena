/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MUON_IMUONSEGMENTSELECTOR_H
#define MUON_IMUONSEGMENTSELECTOR_H

#include "GaudiKernel/IAlgTool.h"

static const InterfaceID IID_IMuonSegmentSelector
    ("Muon::IMuonSegmentSelector",1,0);

namespace Muon {

  class MuonSegment;

  /** Interface for tools resolving ambigueties and remove outliers in Muon::MuonSegment objects */
  class IMuonSegmentSelector : virtual public IAlgTool 
  {      
    public:
    
    static const InterfaceID& interfaceID();

    /** @brief Tool to select segments
	@param segment a reference to a segment
	@return boolean whether segment is selected or not
    */
    virtual const bool select( const MuonSegment& segment ) const = 0;

  };
  
  inline const InterfaceID& IMuonSegmentSelector::interfaceID()
  {
    return IID_IMuonSegmentSelector;
  }
} // end of name space

#endif // IMuonSegmentSelector_H


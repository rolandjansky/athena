/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MUON_IMUON_CDOERRORSEGMENTTOOL_H
#define MUON_IMUON_CDOERRORSEGMENTTOOL_H
 
#include "GaudiKernel/IAlgTool.h"
#include "MuonSegment/MuonSegmentCombinationCollection.h"
#include "MuonPattern/MuonPatternCombinationCollection.h"

#include <vector>

static const InterfaceID IID_IMuon_CDoErrorSegmentTool("Muon::IMuon_CDoErrorSegmentTool", 1, 0);

namespace Trk {
  class Track;
}

namespace Muon {

  /** @brief The IMuonSegmentMaker is a pure virtual interface for tools to find tracks starting from MuonSegmentCombinations  */  
  class IMuon_CDoErrorSegmentTool : virtual public IAlgTool 
  {      
    public:
    /** access to tool interface */
    static const InterfaceID& interfaceID();

    /** @brief Dummy execute() method
	@param combi a reference to a MuonSegmentCombination
	@return a pointer to a vector of tracks, the ownership of the tracks is passed to the client calling the tool.
    */
    virtual void execute( ) const = 0;

  };
  
  inline const InterfaceID& IMuon_CDoErrorSegmentTool::interfaceID()
  {
    return IID_IMuon_CDoErrorSegmentTool;
  }

} // end of name space

#endif 


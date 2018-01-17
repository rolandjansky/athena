/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MUONSEGMENTMAKETOOLINTERFACES_MUON_IMUONPATTERNSEGMENTMAKER_H
#define MUONSEGMENTMAKETOOLINTERFACES_MUON_IMUONPATTERNSEGMENTMAKER_H
 
#include "GaudiKernel/IAlgTool.h"
#include "MuonSegment/MuonSegmentCombinationCollection.h"
#include "MuonPattern/MuonPatternCombinationCollection.h"
#include "MuonEDM_AssociationObjects/MuonSegmentCombPatternCombAssociationMap.h"

#include <vector>

static const InterfaceID IID_IMuonPatternSegmentMaker("Muon::IMuonPatternSegmentMaker",1,0);

namespace Trk {
  class Track;
}

namespace Muon {

  /** @brief The IMuonSegmentMaker is a pure virtual interface for tools to find tracks starting from MuonSegmentCombinations  */  
  class IMuonPatternSegmentMaker : virtual public IAlgTool 
  {      
    public:
    /** access to tool interface */
    static const InterfaceID& interfaceID();


    /** @brief find tracks starting from a MuonSegmentCombination
	@param combi a reference to a MuonSegmentCombination
	@return a pointer to a vector of tracks, the ownership of the tracks is passed to the client calling the tool.
    */
    virtual MuonSegmentCombination* find( const MuonPatternCombination& pattern ) const = 0;

    /** @brief find tracks starting from a MuonSegmentCombinationCollection
	@param combiCol a reference to a MuonSegmentCombinationCollection
	@return a pointer to a vector of tracks, the ownership of the tracks is passed to the client calling the tool.
    */
    virtual MuonSegmentCombinationCollection* find( const MuonPatternCombinationCollection& patterns, MuonSegmentCombPatternCombAssociationMap* segPattMap ) const = 0;

  };
  
  inline const InterfaceID& IMuonPatternSegmentMaker::interfaceID()
  {
    return IID_IMuonPatternSegmentMaker;
  }

} // end of name space

#endif 


/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MUON_IMUONSEGMENTCOMBINATIONCLEANERTOOL_H
#define MUON_IMUONSEGMENTCOMBINATIONCLEANERTOOL_H
 
#include "GaudiKernel/IAlgTool.h"

#include "MuonSegment/MuonSegmentCombinationCollection.h"
#include "MuonEDM_AssociationObjects/MuonSegmentCombPatternCombAssociationMap.h"

static const InterfaceID IID_IMuonSegmentCombinationCleanerTool("Muon::IMuonSegmentCombinationCleanerTool",1,0);

namespace Trk {
  class Track;
}

namespace Muon {

  /** @brief The IMuonSegmentMaker is a pure virtual interface for tools to find tracks starting from MuonSegmentCombinations  */  
  class IMuonSegmentCombinationCleanerTool : virtual public IAlgTool 
  {      
    public:
    /** access to tool interface */
    static const InterfaceID& interfaceID();

    /** clean phi hits on a segment combination */
    virtual MuonSegmentCombinationCollection* clean( const MuonSegmentCombinationCollection& combiCol, MuonSegmentCombPatternCombAssociationMap* segPattMap ) = 0;

  };
  
  inline const InterfaceID& IMuonSegmentCombinationCleanerTool::interfaceID()
  {
    return IID_IMuonSegmentCombinationCleanerTool;
  }

} // end of name space

#endif 


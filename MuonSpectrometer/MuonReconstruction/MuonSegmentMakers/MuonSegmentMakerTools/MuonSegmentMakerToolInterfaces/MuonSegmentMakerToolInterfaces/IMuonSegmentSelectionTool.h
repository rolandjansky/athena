/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MUON_IMUONSEGMENTSELECTIONTOOL_H
#define MUON_IMUONSEGMENTSELECTIONTOOL_H

#include "GaudiKernel/IAlgTool.h"


static const InterfaceID IID_IMuonSegmentSelectionTool("Muon::IMuonSegmentSelectionTool",1,0);


namespace Muon {
  
  class MuonSegment;

  /**
     @brief tool to select segments

  */
  class IMuonSegmentSelectionTool : virtual public IAlgTool {
  public:
    /** @brief access to tool interface */
    static const InterfaceID& interfaceID() { return IID_IMuonSegmentSelectionTool; }

    /** @brief check whether segment passes selection criteria
        @param seg input segment
        @param ignoreHoles flag to indicate that holes should be ignore
	@param qualityLevel indicate the quality of the segments, see implementation for details
	@return returns whether segment satisfies criteria
    */
    virtual bool select( const MuonSegment& seg, bool ignoreHoles = false, int qualityLevel = 0, bool useEta=true, bool usePhi=true ) const = 0;

    /** @brief quality */
    virtual int quality( const MuonSegment& seg, bool ignoreHoles = false, bool useEta=true, bool usePhi=true ) const = 0;

  };

}

#endif

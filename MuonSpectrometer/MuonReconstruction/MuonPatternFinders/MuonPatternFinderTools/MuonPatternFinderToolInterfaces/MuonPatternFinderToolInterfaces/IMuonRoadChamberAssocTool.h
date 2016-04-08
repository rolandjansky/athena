/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MUON_IMUONROADCHAMBERASSOCTOOL_H
#define MUON_IMUONROADCHAMBERASSOCTOOL_H
 
#include "GaudiKernel/IAlgTool.h"
#include "MuonPattern/MuonPatternCombinationCollection.h"
#include "TrkRoad/TrackRoad.h"
#include <vector>

static const InterfaceID IID_IMuonRoadChamberAssocTool("Muon::IMuonRoadChamberAssocTool", 1, 0);

namespace Trk {
  class Track;
}

namespace Muon {

  class IMuonRoadChamberAssocTool : virtual public IAlgTool 
  {      
    public:
    /** access to tool interface */
    static const InterfaceID& interfaceID();

    virtual Muon::MuonPatternCombination* execute( Trk::TrackRoad road ) = 0;

  };
  
  inline const InterfaceID& IMuonRoadChamberAssocTool::interfaceID()
  {
    return IID_IMuonRoadChamberAssocTool;
  }

} // end of name space

#endif 


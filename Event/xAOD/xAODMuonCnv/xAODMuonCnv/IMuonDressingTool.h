/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef XAOD_IMUONDRESSINGTOOL_H
#define XAOD_IMUONDRESSINGTOOL_H

#include "GaudiKernel/IAlgTool.h"
#include "xAODMuon/Muon.h"


static const InterfaceID IID_IMuonDressingTool("Muon::IMuonDressingTool",1,0);

namespace Trk {
  class TrackSummary;
}

namespace xAOD {
  /** Interface for tools calculating hit count summaries for track */
  class IMuonDressingTool : virtual public IAlgTool {      
  public:
    /** IAlgTool interface */
    static const InterfaceID& interfaceID();

    /** @brief add muon hit summary information to a Muon, uses the MuonTrackSummary if provided */
    virtual void addMuonHitSummary( xAOD::Muon& muon, const Trk::TrackSummary* summary = 0 ) const = 0;
  };
  
  inline const InterfaceID& IMuonDressingTool::interfaceID()
  {
    return IID_IMuonDressingTool;
  }

} // end of name space

#endif // IMuonDressingTool_H


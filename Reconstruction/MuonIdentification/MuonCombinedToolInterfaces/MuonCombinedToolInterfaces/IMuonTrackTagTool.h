/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MUONCOMBINEDTOOLS_IMUONTRACKTAGTOOL_H
#define MUONCOMBINEDTOOLS_IMUONTRACKTAGTOOL_H


#include "GaudiKernel/IAlgTool.h"
#include "TrkParameters/TrackParameters.h"


static const InterfaceID IID_IMuonTrackTagTool("MuonCombined::IMuonTrackTagTool",1,0);

namespace Trk {
  class Track;
}

namespace MuonCombined {

  class IMuonTrackTagTool : virtual public IAlgTool {
    
  public:
    
    static const InterfaceID& interfaceID();
    
    virtual double chi2(const Trk::TrackParameters& idParsAtEntry, const Trk::TrackParameters& msParsAtEntry) const = 0;

    virtual double chi2(const Trk::Track& id, const Trk::Track& ms) const = 0;
  };
  
  inline const InterfaceID& IMuonTrackTagTool::interfaceID() { return IID_IMuonTrackTagTool; }
  
}


#endif

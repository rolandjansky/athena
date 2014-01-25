/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MUONCOMBINEDTOOLS_MUONTRACKTAGTESTTOOL_H
#define MUONCOMBINEDTOOLS_MUONTRACKTAGTESTTOOL_H

//#define MUONCOMBDEBUG

#include "AthenaBaseComps/AthAlgTool.h"
#include "GaudiKernel/ToolHandle.h"
#include <string>
#include "TrkExInterfaces/IExtrapolator.h"
#include "TrkDetDescrInterfaces/ITrackingGeometrySvc.h"
#include "TrkParameters/TrackParameters.h"
#include "MuonCombinedToolInterfaces/IMuonTrackTagTool.h"

namespace Trk{
  class TrackingGeometry;
  class TrackingVolume;
}

namespace MuonCombined {
  
  class MuonTrackTagTestTool : public AthAlgTool, virtual public IMuonTrackTagTool {
    
  public:
    
    MuonTrackTagTestTool(const std::string& type,const std::string& name, const IInterface* parent);
    ~MuonTrackTagTestTool() {}
    
    StatusCode initialize();
    StatusCode finalize();
    
    double chi2(const Trk::TrackParameters& idParsAtEntry,const Trk::TrackParameters& msParsAtEntry) const;
    double chi2(const Trk::Track& id, const Trk::Track& ms) const;

  private:

    ToolHandle<Trk::IExtrapolator> m_extrapolator;
    mutable ServiceHandle<Trk::ITrackingGeometrySvc> m_trackingGeometrySvc;
    mutable const Trk::TrackingGeometry*  m_trackingGeometry;
    mutable const Trk::TrackingVolume*    m_msEntrance;
    
    double m_chi2cut;
#ifdef MUONCOMBDEBUG
    bool m_truth;
#endif
    
  };

}


#endif

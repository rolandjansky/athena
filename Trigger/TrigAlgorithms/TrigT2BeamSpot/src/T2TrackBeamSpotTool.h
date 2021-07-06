/*
   Copyright (C) 2020 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRIGT2BEAMSPOT_T2TRACKBEAMSPOTTOOL_H
#define TRIGT2BEAMSPOT_T2TRACKBEAMSPOTTOOL_H

#include <vector>

// base class
#include "AthenaBaseComps/AthAlgTool.h"

//Athena tools
#include "GaudiKernel/EventIDBase.h"
#include "GaudiKernel/ToolHandle.h"
#include "TrkTrack/Track.h"
#include "T2BSTrackFilterTool.h"

//Monitoring tool
#include "AthenaMonitoringKernel/Monitored.h"


namespace PESA {

class T2Track;

class T2TrackBeamSpotTool :  public AthAlgTool {
public:

    using TrackData = T2BSTrackFilterTool::TrackData;

    T2TrackBeamSpotTool(const std::string& type, const std::string& name, const IInterface* parent);
    ~T2TrackBeamSpotTool() = default;

    StatusCode initialize() final;

    /**
     * Update beam spot data with new track information.
     *
     * Parameter provides a lists of tracks.
     */
    void updateBS(const TrackCollection& tracks, EventIDBase const& eventID) const;

private:

    /**
     * Update beam spot data with new track information.
     *
     * Parameter provides a lists of tracks.
     */
    void updateBS(std::vector<const Trk::Track*>&& tracks, unsigned bcid) const;

    /**
     * Update beam spot data with new track information.
     *
     * This method does exactly the same as above but uses different track
     * representation. Two methods defined mostly for performance reasons
     * to avoid conversion.
     */
    void updateBS(std::vector<TrackData>&& tracks) const;


    bool m_doLeastSquares;
    bool m_doLogLikelihood;
    double m_beamSizeLS;

    ToolHandle<T2BSTrackFilterTool> m_trackFilterTool{this, "TrackFilter", "PESA::T2BSTrackFilterTool/T2BSTrackFilterTool" };
    ToolHandle<GenericMonitoringTool> m_monTool{this, "MonTool", "", "Monitoring tool"};
};

} // namespace PESA

#endif //  TRIGT2BEAMSPOT_T2TRACKBEAMSPOTTOOL_H

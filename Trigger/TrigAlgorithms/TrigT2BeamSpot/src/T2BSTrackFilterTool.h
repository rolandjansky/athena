/*
   Copyright (C) 2020 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRIGT2BEAMSPOT_T2BSTRACKFILTERTOOL_H
#define TRIGT2BEAMSPOT_T2BSTRACKFILTERTOOL_H

#include <bitset>
#include <memory>
#include <mutex>
#include <string>
#include <vector>

// base class
#include "AthenaBaseComps/AthAlgTool.h"

//Athena tools
#include "GaudiKernel/ToolHandle.h"
#include "TrkTrack/Track.h"
#include "TrkTrack/TrackCollection.h"

//Monitoring tool
#include "AthenaMonitoringKernel/Monitored.h"


namespace PESA {

class T2Track;

class T2BSTrackFilterTool :  public AthAlgTool {
public:

    using TrackVector = std::vector<const Trk::Track*>;

    /**
     * Class which holds track parameters.
     */
    struct TrackData {
        TrackData(Trk::Perigee const& trackPars, unsigned lbn_, unsigned bcid_)
            : lbn(lbn_), bcid(bcid_)
        {
            auto const& par = trackPars.parameters();
            d0 = par[Trk::d0];
            phi0 = par[Trk::phi0];
            z0 = par[Trk::z0];
            auto const& cov = trackPars.covariance();
            d0_var = (*cov)(Trk::d0, Trk::d0);
        }

        unsigned lbn = 0;
        unsigned bcid = 0;
        double d0 = 0;
        double phi0 = 0;
        double z0 = 0;
        double d0_var = 0;  // d0 error squared
    };

    // Reasons for tracks not passing selection, one track can fail for multiple reasons,
    // these are used for monitoring purposes only
    enum TrackRejectReason {
        fail_PT = 0,
        fail_SiHits = 1,
        fail_PIXHits = 2,
        fail_SCTHits = 3,
        fail_TRTHits = 4,
        fail_NDF = 5,
        fail_D0 = 6,
        fail_Z0 = 7,
        fail_D0err = 8,
        fail_Z0err = 9,
        fail_Eta = 10,
        fail_MinQual = 11,
        fail_MaxQual = 12,
        fail_Chi2Prob = 13,
        fail_D0Chi2 = 14,
        //
        numRejectReasons
    };
    using TrackRejectReasonSet = std::bitset<numRejectReasons>;


    T2BSTrackFilterTool(const std::string& type, const std::string& name, const IInterface* parent);
    ~T2BSTrackFilterTool() = default;

    StatusCode initialize() final;

    /**
    * Select tracks that can be used for further processing.
    *
    * This method only looks at parameters of individual tracks and does
    * not check track distance to an estimated beam spot.
    */
    TrackVector filter(const TrackCollection& tracks) const;

    /**
    * Update beam spot estimate with new tracks.
    *
    * Normally the tracks would be the same as returned from previous
    * method as some filtering is needed for good estimation.
    *
    * This method returns true when it has a reasonable estimate of the
    * beamspot, if true is returned then further filtering can be done with
    * `filterBS()` method. If `bsTracks` pointer is non-zero then the vector
    * will be filled with tracks that were used for beam spot estimate, this
    * only happens when new estimate is calculated. Returned track list can
    * include tracks from multiple event in the same lumi block or even
    * multiple lumi blocks, and it will include all tracks from a current
    * call (after filtering).
    */
    bool updateBS(const TrackVector& tracks,
                  unsigned lbn,  unsigned bcid,
                  std::vector<TrackData>* bsTracks = nullptr) const;

    /**
    * Select tracks based on their distance to estimated beamspot.
    *
    * This method should only be called when `updateBS()` returns true.
    */
    TrackVector filterBS(const TrackVector& tracks) const;

private:

    /*
     * This struct holds accumulated track data for specific LBN, used
     * to produce solution for beam spot estimate.
     */
    struct TrackStatAccumulator {

        TrackStatAccumulator();

        // add single track statistics to accumulator
        void update(TrackData const& seed, double beamSize);

        // add statistics from other accumulator to this one
        void update(TrackStatAccumulator const& other);

        // Produce a solution based on current data, can only be called if trk_count > 0
        AmgVector(4) solution() const;

        unsigned lbn = 0;          // LBN for accumulated data
        unsigned trk_count = 0;    // Number of tracks accumualted
        double trk_d0_sq = 0;      // Sum(d0**2 * weight), used for chi2 calculation, not for fitting
        AmgSymMatrix(4) trk_param; // Matrix of Sum(param[i]*param[j] * weight)
        AmgVector(4) trk_d0;       // Matrix of Sum(d0*param[i] * weight)
    };

    /* Monitor track parameters */
    void monitor_tracks(std::string const& prefix, std::string const& suffix, std::vector<T2Track> const& tracks) const;

    // returns a set of TrackRejectReason for a track, empty means good track
    TrackRejectReasonSet isGoodTrack(const T2Track & track) const;

    // Calculate beam spot from accumulated seeds, returns true
    // if bootstrapping is complete and seeds will not be used any more.
    // Fills bsTracks with seeds that were used for BS estimate.
    bool bootstrapFromSeeds(std::vector<TrackData>* bsTracks = nullptr) const;

    /**
     * Calculate track chi2 w.r.t. current solution for beam spot.
     */
    double trackChi2(TrackData const& seed, AmgVector(4) const& solution) const;

    // dump track parameters
    void dumpTrackParams(Trk::Perigee const* trackPars, double chi2, std::string const& message) const;

    /* Track selection criteria */
    double       m_minTrackPt;
    double       m_maxTrackEta;
    double       m_maxTrackZ0;
    double       m_maxTrackD0;
    double       m_maxTrackZ0err;
    double       m_maxTrackD0err;
    double       m_minTrackNDF;
    double       m_minTrackQual;
    double       m_maxTrackQual;
    double       m_minTrackChi2Prob;
    int          m_minSiHits;
    int          m_minPIXHits;
    int          m_minSCTHits;
    int          m_minTRTHits;

    unsigned     m_goalSeedTracks;
    double       m_chi2cutoff;
    double       m_beamSizeLS;
    int          m_lbnInterval;  // number of LBNs the estimate is good for

    mutable std::mutex m_mutex;  // protects mutable members

    mutable std::vector<TrackData> m_bsSeeds ATLAS_THREAD_SAFE;  // tracks used for BS estimate

    using AccumKey = unsigned;  // LBN
    using AccumMap = std::map<AccumKey, TrackStatAccumulator>;
    mutable AccumMap m_accumulators ATLAS_THREAD_SAFE;  // maps LBN to its accumulator

    ToolHandle<GenericMonitoringTool> m_monTool{this,"MonTool","","Monitoring tool"};
};

} // namespace PESA

#endif //  TRIGT2BEAMSPOT_T2BSTRACKFILTERTOOL_H

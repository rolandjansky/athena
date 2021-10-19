/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef DCMATH_MATCHDCWITHLINE_H
#define DCMATH_MATCHDCWITHLINE_H

#include "TrkDriftCircleMath/DCOnTrack.h"
#include "TrkDriftCircleMath/DriftCircle.h"
#include "TrkDriftCircleMath/HitSelection.h"
#include "TrkDriftCircleMath/Line.h"
#include "TrkDriftCircleMath/ResidualWithLine.h"

namespace TrkDriftCircleMath {

    class MatchDCWithLine {
    public:
        enum MatchStrategy { Road, Pull };

    public:
        MatchDCWithLine() = default;
        MatchDCWithLine(const Line& l, double deltaCut, MatchStrategy strategy, double tubeRadius) :
            m_resWithLine{l}, m_deltaCut{deltaCut}, m_strategy{strategy} {
            m_dcOnTrackVec.reserve(50);
            setTubeRadius(tubeRadius);
        }

        void set(const Line& l, double deltaCut, MatchStrategy strategy, double tubeRadius) {
            m_resWithLine.set(l);
            m_deltaCut = deltaCut;
            m_strategy = strategy;
            setTubeRadius(tubeRadius);
        }

        void setTubeRadius(double radius) {
            m_tubeRadius = radius;
            m_closeByCut = 2 * radius;
            m_showerCut = 4 * radius;
        }

        const DCOnTrackVec& match(const DCVec& dcs);
        const DCOnTrackVec& match(const DCOnTrackVec& dcs, const HitSelection* selection = nullptr, bool recoverMdtHits = true,
                                  bool usePreciseErrors = false);

        unsigned int deltas() const { return m_deltas; }
        unsigned int hitsOutOfTime() const { return m_outOfTimes; }
        unsigned int hitsOnTrack() const { return m_onTracks; }
        unsigned int closeHits() const { return m_closeHits; }
        unsigned int showerHits() const { return m_showerHits; }
        unsigned int passedTubes() const { return m_passedTubes; }
        unsigned int hitsMl1() const { return m_ml1; }
        unsigned int hitsMl2() const { return m_ml2; }

        /** returns the number of DCOnTrack that have a different status after the match, returns 0 if used with DCVec */
        unsigned int matchDifference() const { return m_matchDifference; }

    private:
        void reset();
        void matchDC(DCOnTrack& dc, double& res, double& dline, bool forceOnTrack = false, bool forceOffTrack = false,
                     bool usePreciseErrors = false);
        bool select(double residual, double error) const;

        ResidualWithLine m_resWithLine{};
        double m_tubeRadius{0.};
        double m_closeByCut{0.};
        double m_showerCut{0.};
        double m_deltaCut{0.};
        MatchStrategy m_strategy{};
        unsigned int m_deltas{0};
        unsigned int m_outOfTimes{0};
        unsigned int m_onTracks{0};
        unsigned int m_passedTubes{0};
        unsigned int m_ml1{0};
        unsigned int m_ml2{0};
        unsigned int m_closeHits{0};
        unsigned int m_showerHits{0};
        unsigned int m_matchDifference{0};

        // cache results to reduce CPU usage
        DCOnTrackVec m_dcOnTrackVec;
    };

}  // namespace TrkDriftCircleMath

#endif

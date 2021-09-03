/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#include "TrkDriftCircleMath/MatchDCWithLine.h"

namespace TrkDriftCircleMath {

    const DCOnTrackVec& MatchDCWithLine::match(const DCVec& dcs) {
        // clear cached information
        reset();
        DCCit it = dcs.begin();
        DCCit it_end = dcs.end();
        for (; it != it_end; ++it) {
            double dline = m_resWithLine.residual(it->position());
            double rhit = it->r() < 0. ? -it->r() : it->r();
            if (dline < 0.) dline *= -1.;

            double res = dline - rhit;

            DCOnTrack dcOnTrack(*it, res);

            matchDC(dcOnTrack, res, dline);

            if (dcOnTrack.state() != DCOnTrack::OutsideRoad) {
                /** add check to remove hits in the same tube
                    check whether
                    - this is not the first tube
                    - the tubes have the same id */
                if (!m_dcOnTrackVec.empty() && m_dcOnTrackVec.back().id() == it->id()) {
                    // take the hit with the smallest residual
                    if (fabs(m_dcOnTrackVec.back().residual()) > fabs(res)) {
                        // exchange the two hits
                        m_dcOnTrackVec.pop_back();
                        m_dcOnTrackVec.push_back(dcOnTrack);
                    }
                } else {
                    m_dcOnTrackVec.push_back(dcOnTrack);
                }
            }
        }
        return m_dcOnTrackVec;
    }

    const DCOnTrackVec& MatchDCWithLine::match(const DCOnTrackVec& dcs, const HitSelection* selection, bool recoverMdtHits,
                                               bool usePrecise) {
        // clear cached information
        reset();

        DCOnTrackCit it = dcs.begin();
        DCOnTrackCit it_end = dcs.end();
        unsigned int index = 0;
        for (; it != it_end; ++it, ++index) {
            double dline = m_resWithLine.residual(it->position());
            double rhit = it->r() < 0. ? -it->r() : it->r();
            if (dline < 0.) dline *= -1.;

            double res = dline - rhit;

            DCOnTrack dcOnTrack(*it, res);
            dcOnTrack.errorTrack(it->errorTrack());

            // check whether any DCOnTracks change state from or to OnTrack with respect to the old selection
            if ((dcOnTrack.state() == DCOnTrack::OnTrack || it->state() == DCOnTrack::OnTrack) && dcOnTrack.state() != it->state())
                ++m_matchDifference;

            if (selection) {
                if ((*selection)[index] == 0) {
                    matchDC(dcOnTrack, res, dline, true, false, usePrecise);
                } else if (recoverMdtHits) {
                    matchDC(dcOnTrack, res, dline, false, false, usePrecise);
                } else {
                    matchDC(dcOnTrack, res, dline, false, true, usePrecise);
                }
            } else {
                matchDC(dcOnTrack, res, dline, false, false, usePrecise);
            }

            if (dcOnTrack.state() != DCOnTrack::OutsideRoad) m_dcOnTrackVec.push_back(dcOnTrack);
        }
        return m_dcOnTrackVec;
    }

    void MatchDCWithLine::matchDC(DCOnTrack& dcOnTrack, double& res, double& dline, bool forceOnTrack, bool forceOffTrack,
                                  bool usePrecise) {
        // check whether hit within range
        double error = usePrecise ? dcOnTrack.drPrecise() : dcOnTrack.dr();
        if (forceOnTrack || (!forceOffTrack && (dcOnTrack.DriftCircle::state() == DriftCircle::InTime && select(res, error)))) {
            ++m_onTracks;
            ++m_passedTubes;
            if (dcOnTrack.id().ml() == 0)
                ++m_ml1;
            else
                ++m_ml2;
            dcOnTrack.state(DCOnTrack::OnTrack);
        } else {
            if (dline < m_tubeRadius) {
                if (res < 0. || dcOnTrack.DriftCircle::state() == DriftCircle::LateHit) {
                    dcOnTrack.state(DCOnTrack::OutOfTime);
                    ++m_outOfTimes;
                } else {
                    dcOnTrack.state(DCOnTrack::Delta);
                    ++m_deltas;
                }
                ++m_passedTubes;
            } else {
                if (dline < m_closeByCut) {
                    dcOnTrack.state(DCOnTrack::CloseDC);
                    ++m_closeHits;
                } else {
                    dcOnTrack.state(DCOnTrack::OutsideRoad);
                }
                if (dline < m_showerCut) { ++m_showerHits; }
            }
        }
    }

    bool MatchDCWithLine::select(double residual, double error) const {
        if (residual < 0.) residual *= -1.;
        switch (m_strategy) {
            case Road: return residual < m_deltaCut;
            case Pull: return residual < m_deltaCut * error;
            default: return false;
        }
    }

    void MatchDCWithLine::reset() {
        // clear cache
        m_dcOnTrackVec.clear();

        m_outOfTimes = 0;
        m_deltas = 0;
        m_onTracks = 0;
        m_passedTubes = 0;
        m_ml1 = 0;
        m_ml2 = 0;
        m_closeHits = 0;
        m_showerHits = 0;
        m_matchDifference = 0;
    }

}  // namespace TrkDriftCircleMath

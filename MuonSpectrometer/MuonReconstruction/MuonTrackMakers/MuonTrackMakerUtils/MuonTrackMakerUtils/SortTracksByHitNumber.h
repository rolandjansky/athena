/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef SORTTRACKSBYHITNUMBER_H
#define SORTTRACKSBYHITNUMBER_H

#include <functional>

#include "TrkMeasurementBase/MeasurementBase.h"
#include "TrkTrack/Track.h"

namespace Muon {

    class SortTracksByHitNumber {
    public:
        bool operator()(const Trk::Track* tr1, const Trk::Track* tr2) { return this->operator()(*tr1, *tr2); }

        bool operator()(const Trk::Track& tr1, const Trk::Track& tr2) {
            // prefer tracks with fit quality (always expected)
            const Trk::FitQuality* fq1 = tr1.fitQuality();
            const Trk::FitQuality* fq2 = tr2.fitQuality();
            if (!fq1 && fq2) return false;
            if (fq1 && !fq2) return true;
            if (!fq1 && !fq2) return false;

            if (fq1->numberDoF() > fq2->numberDoF()) return true;
            if (fq1->numberDoF() < fq2->numberDoF()) return false;

            // select candidate with smallest chi2
            double chi2Ndof1 = fq1->chiSquared() / fq1->numberDoF();
            double chi2Ndof2 = fq2->chiSquared() / fq2->numberDoF();
            return chi2Ndof1 < chi2Ndof2;
        }
    };

    class SortTracksByChi2Ndof {
    public:
        bool operator()(const Trk::Track* tr1, const Trk::Track* tr2) { return this->operator()(*tr1, *tr2); }

        bool operator()(const Trk::Track& tr1, const Trk::Track& tr2) {
            // prefer tracks with fit quality (always expected)
            const Trk::FitQuality* fq1 = tr1.fitQuality();
            const Trk::FitQuality* fq2 = tr2.fitQuality();
            if (!fq1 && fq2) return false;
            if (fq1 && !fq2) return true;
            if (!fq1 && !fq2) return false;

            // select candidate with smallest chi2
            double chi2Ndof1 = fq1->chiSquared() / fq1->numberDoF();
            double chi2Ndof2 = fq2->chiSquared() / fq2->numberDoF();
            return chi2Ndof1 < chi2Ndof2;
        }
    };
}  // namespace Muon

#endif

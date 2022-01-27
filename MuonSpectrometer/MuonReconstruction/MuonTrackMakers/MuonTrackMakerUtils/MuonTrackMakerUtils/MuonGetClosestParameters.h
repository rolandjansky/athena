/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MUONGETCLOSESTPARAMETERS_H
#define MUONGETCLOSESTPARAMETERS_H

#include "TrkParameters/TrackParameters.h"
#include "TrkSurfaces/Surface.h"
#include "TrkTrack/Track.h"

namespace Muon {

    class MuonGetClosestParameters {
    public:
        static Trk::TrackParameters* closestParameters(const Trk::Track& track, const Amg::Vector3D& pos, bool onlyUseMeasured = false) {
            const DataVector<const Trk::TrackParameters>* pars = track.trackParameters();
            if (!pars || pars->empty()) { return 0; }

            bool firstOk = onlyUseMeasured ? pars->front()->covariance() != 0 : true;
	    bool lastOk = onlyUseMeasured ? pars->back()->covariance() !=0 : true;

            double distFront = (pars->front()->position() - pos).dot(pars->front()->momentum().unit());
            if (distFront > 0. && firstOk) { return pars->front()->clone(); }

            double distBack = (pars->back()->position() - pos).dot(pars->back()->momentum().unit());
            if (distBack < 0. && lastOk) { return pars->back()->clone(); }

            bool startFront = fabs(distFront) < distBack;

            const Trk::TrackParameters* result = 0;
            const Trk::TrackParameters* prevresult = 0;

            if (startFront) {
                double prevDist = distFront - 1.;
                // loop over parameters, calculate distance
                DataVector<const Trk::TrackParameters>::const_iterator it = pars->begin();
                DataVector<const Trk::TrackParameters>::const_iterator it_end = pars->end();
                for (; it != it_end; ++it) {
                    if (onlyUseMeasured && !(*it)->covariance()) continue;

                    double dist = ((*it)->position() - pos).dot((*it)->momentum().unit());

                    // check whether dist flips sign, if this happens select either the current hit or the previous
                    if (dist > 0.) {
                        if (fabs(dist) < fabs(prevDist))
                            result = *it;
                        else if (it == pars->begin())
                            result = *it;
                        else
                            result = prevresult;
                        break;
                    }
                    prevDist = dist;
                    prevresult = *it;
                }
            } else {
                double prevDist = distBack + 1.;
                // loop over parameters, calculate distance
                DataVector<const Trk::TrackParameters>::const_reverse_iterator it = pars->rbegin();
                DataVector<const Trk::TrackParameters>::const_reverse_iterator it_end = pars->rend();
                for (; it != it_end; ++it) {
                    if (onlyUseMeasured && !(*it)->covariance()) continue;

                    double dist = ((*it)->position() - pos).dot((*it)->momentum().unit());

                    // check whether dist flips sign, if this happens select either the current hit or the previous
                    if (dist < 0.) {
                        if (fabs(dist) < fabs(prevDist))
                            result = *it;
                        else if (it == pars->rbegin())
                            result = *it;
                        else
                            result = prevresult;
                        break;
                    }
                    prevDist = dist;
                    prevresult = *it;
                }
            }

            return result ? result->clone() : 0;
        }

        static Trk::TrackParameters* closestParameters(const Trk::Track& track, const Trk::Surface& surf, bool onlyUseMeasured = false) {
            return closestParameters(track, surf.center(), onlyUseMeasured);
        }

        static Trk::TrackParameters* closestParameters(const Trk::Track& track, const Trk::TrackParameters& pars,
                                                       bool onlyUseMeasured = false) {
            return closestParameters(track, pars.position(), onlyUseMeasured);
        }
    };

}  // namespace Muon

#endif

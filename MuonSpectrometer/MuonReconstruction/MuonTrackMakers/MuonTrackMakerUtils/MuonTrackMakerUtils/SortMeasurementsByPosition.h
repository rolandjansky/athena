/*
Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#ifndef SORTMEASUREMENTSBYPOSITION_H
#define SORTMEASUREMENTSBYPOSITION_H

#include <functional>
#include <iostream>

#include "CxxUtils/fpcompare.h"
#include "GeoPrimitives/GeoPrimitives.h"
#include "MuonCompetingRIOsOnTrack/CompetingMuonClustersOnTrack.h"
#include "MuonIdHelpers/IMuonIdHelperSvc.h"
#include "MuonRecHelperTools/IMuonEDMHelperSvc.h"
#include "TrkMeasurementBase/MeasurementBase.h"

namespace Muon {

    class DistanceToPars {
    public:
        DistanceToPars(const Trk::TrackParameters* pars) : m_pars(pars) {}
        double operator()(const Amg::Vector3D& pos) const {
            Amg::Vector3D difPos = pos - m_pars->position();
            double sign = difPos.dot(m_pars->momentum()) < 0 ? -1. : 1.;
            return difPos.mag() * sign;
        }

    private:
        const Trk::TrackParameters* m_pars;
    };

    class SortTSOSsByPos {
    public:
        SortTSOSsByPos(bool isEndcap) : m_isEndcap(isEndcap), m_dummyPosition(0., 0., 0.) {}
        double operator()(const Trk::TrackStateOnSurface* tsos1, const Trk::TrackStateOnSurface* tsos2) {
            if (m_isEndcap)
                return std::abs(position(*tsos1).z()) < std::abs(position(*tsos2).z());
            else
                return position(*tsos1).perp() < position(*tsos2).perp();
        }

    private:
        Amg::Vector3D position(const Trk::TrackStateOnSurface& tsos) {
            if (tsos.trackParameters())
                return tsos.trackParameters()->position();
            else if (tsos.measurementOnTrack())
                return tsos.measurementOnTrack()->globalPosition();
            return m_dummyPosition;
        }
        bool m_isEndcap;
        Amg::Vector3D m_dummyPosition;
    };

    class SortTSOSByDistanceToPars {
    public:
        SortTSOSByDistanceToPars(const Trk::TrackParameters* pars) : m_distToPars(pars), m_dummyPosition(0., 0., 0.) {}
        // SortTSOSByDistanceToPars(std::unique_ptr<const Trk::TrackParameters> pars) : SortTSOSByDistanceToPars(pars.get()) {}
        double operator()(const std::pair<bool, const Trk::TrackStateOnSurface*>& tsos1,
                          const std::pair<bool, const Trk::TrackStateOnSurface*>& tsos2) {
            return CxxUtils::fpcompare::less(m_distToPars(position(*tsos1.second)), m_distToPars(position(*tsos2.second)));
        }
        double operator()(const std::unique_ptr<const Trk::TrackStateOnSurface>& tsos1,
                          const std::unique_ptr<const Trk::TrackStateOnSurface>& tsos2) {
            return CxxUtils::fpcompare::less(m_distToPars(position(*tsos1)), m_distToPars(position(*tsos2)));
        }

    private:
        Amg::Vector3D position(const Trk::TrackStateOnSurface& tsos) const {
            if (tsos.trackParameters())
                return tsos.trackParameters()->position();
            else if (tsos.measurementOnTrack())
                return tsos.measurementOnTrack()->globalPosition();
            return m_dummyPosition;
        }
        DistanceToPars m_distToPars;
        Amg::Vector3D m_dummyPosition;
    };

    class SortTSOSs {
    public:
        bool operator()(const Trk::TrackStateOnSurface* tsos1, const Trk::TrackStateOnSurface* tsos2) const {
            if (!tsos1->trackParameters()) {
                std::cout << "Muon::SortTSOSs: state 1 without parameters " << std::endl;
                return false;
            }
            if (!tsos2->trackParameters()) {
                std::cout << "Muon::SortTSOSs: state 2 without parameters " << std::endl;
                return true;
            }
            // first, address rare problems with funky track directions leading to strange orderings by checking for comparisons of calo
            // deposits and MS hits
            const Trk::MeasurementBase* meas1 = tsos1->measurementOnTrack();
            Identifier id1 = meas1 ? m_helperSvc->getIdentifier(*meas1) : Identifier();

            const Trk::MeasurementBase* meas2 = tsos2->measurementOnTrack();
            Identifier id2 = meas2 ? m_helperSvc->getIdentifier(*meas2) : Identifier();

            bool okId1 = id1.is_valid() && m_idHelperSvc->isMuon(id1) ? true : false;
            bool okId2 = id2.is_valid() && m_idHelperSvc->isMuon(id2) ? true : false;

            if (tsos1->type(Trk::TrackStateOnSurface::TrackStateOnSurfaceType::CaloDeposit) ||
                tsos2->type(Trk::TrackStateOnSurface::TrackStateOnSurfaceType::CaloDeposit)) {
                if (okId1 && tsos2->type(Trk::TrackStateOnSurface::TrackStateOnSurfaceType::CaloDeposit)) return false;
                if (tsos1->type(Trk::TrackStateOnSurface::TrackStateOnSurfaceType::CaloDeposit) && okId2) return true;
            }

            // get average direction of the 2 TSOSs
            Amg::Vector3D trackDir = tsos1->trackParameters()->momentum().unit();
            trackDir += tsos2->trackParameters()->momentum().unit();
            const Amg::Vector3D& pos1 = tsos1->trackParameters()->position();
            const Amg::Vector3D& pos2 = tsos2->trackParameters()->position();
            double dist = (pos2 - pos1).dot(trackDir);

            if (std::abs(dist) < 1e-5) {
                // one is a good muon hit and one is not: good muon hit comes after
                if (okId1 && !okId2) return true;
                if (!okId1 && okId2) return false;
                // both invalid or non-muon: consider them equal
                if (!okId1 && !okId2) return false;
                // now we have 2 valid muon Ids
                bool measPhi1 = m_idHelperSvc->measuresPhi(id1);
                bool measPhi2 = m_idHelperSvc->measuresPhi(id2);
                // put phi measurement in front of eta measurements
                if (measPhi1 && !measPhi2) return true;
                if (!measPhi1 && measPhi2) return false;
                // now both are phi or both are eta
                // decide on identifier (increasing away from IP within same technology)
                double inOrOut = (pos1 + pos2).dot(trackDir);
                if (inOrOut >= 0.0) {
                    return id1 < id2;
                } else {
                    return id1 > id2;
                }

                // if we get here, there was no decision
                return false;  // consider them equal
            }
            return dist > 0.;
        }
        bool operator()(const std::unique_ptr<const Trk::TrackStateOnSurface>& tsos1,
                        const std::unique_ptr<const Trk::TrackStateOnSurface>& tsos2) const {
            return this->operator()(tsos1.get(), tsos2.get());
        }

        SortTSOSs(const IMuonEDMHelperSvc* h, const IMuonIdHelperSvc* idh) : m_helperSvc(h), m_idHelperSvc(idh) {}

        const IMuonEDMHelperSvc* m_helperSvc;
        const IMuonIdHelperSvc* m_idHelperSvc;
    };

    class SortMeasurementsByPosition {
    public:
        SortMeasurementsByPosition(bool hasEndcap = true) : m_isEndcap(hasEndcap) {}

        bool operator()(const Trk::MeasurementBase* meas1, const Trk::MeasurementBase* meas2) {
            const Trk::MeasurementBase* m1 = getMeas(meas1);
            const Trk::MeasurementBase* m2 = getMeas(meas2);

            double d1 = m_isEndcap ? std::abs(m1->globalPosition().z()) : std::abs(m1->globalPosition().perp());
            double d2 = m_isEndcap ? std::abs(m2->globalPosition().z()) : std::abs(m2->globalPosition().perp());
            bool result = d1 < d2;
            return result;
        }

    private:
        // hack to correctly sort competing rots
        const Trk::MeasurementBase* getMeas(const Trk::MeasurementBase* meas) const {
            const CompetingMuonClustersOnTrack* cm = dynamic_cast<const CompetingMuonClustersOnTrack*>(meas);
            if (cm) {
                if (cm->numberOfContainedROTs() != 0) return &cm->rioOnTrack(cm->indexOfMaxAssignProb());
            }
            return meas;
        }

        bool m_isEndcap;
    };

}  // namespace Muon

#endif

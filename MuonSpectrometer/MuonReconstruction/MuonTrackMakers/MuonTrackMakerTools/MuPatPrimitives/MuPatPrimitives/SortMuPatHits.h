/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/

#ifndef SORTMUPATHITS_H
#define SORTMUPATHITS_H

#include "MuPatPrimitives/MuPatHit.h"
#include "TrkMeasurementBase/MeasurementBase.h"
#include "TrkParameters/TrackParameters.h"
#include "TrkSurfaces/Surface.h"
#include "MuonIdHelpers/IMuonIdHelperSvc.h"

namespace Muon {

    class DistanceAlongParameters {
    public:
        double operator()(const Trk::TrackParameters& pars, const Amg::Vector3D& pos) const {
            Amg::Vector3D difPos = pos - pars.position();
            return difPos.dot(pars.momentum().unit());
        }

        double operator()(const Trk::TrackParameters& pars1, const Trk::TrackParameters& pars2) const {
            return this->operator()(pars1, pars2.position());
        }

        double operator()(const Trk::TrackParameters& pars, const Trk::MeasurementBase& meas) const {
            return this->operator()(pars, meas.globalPosition());
        }
    };

    class MuPatHitDistanceAlongParameters {
    public:
        double operator()(const MuPatHit* hit1, const MuPatHit* hit2) const {
            return distanceCalculator(hit1->parameters(), hit2->parameters());
        }

        DistanceAlongParameters distanceCalculator;
    };

    class SortMuPatHits {
    public:
      bool operator()(const MuPatHit* hit1, const MuPatHit* hit2, const IMuonIdHelperSvc* idh) const {
	    //first, check if both hits are in the same chamber, and at least one is an RPC
   
        const MuPatHit::Info& info1 = hit1->info();
        const MuPatHit::Info& info2 = hit2->info();
        const Identifier& id1 = info1.id;
        const Identifier& id2 = info2.id;

        if (idh->chamberIndex(id1) == idh->chamberIndex(id2) && (info1.type == MuPatHit::RPC || info2.type == MuPatHit::RPC)) {
            const RpcIdHelper& rpcHelper = idh->rpcIdHelper();
            if (info1.type == MuPatHit::MDT) {
                if (rpcHelper.doubletR(id2) == 1) 
                    return (idh->stationIndex(id1) == MuonStationIndex::StIndex::BM || idh->isSmallChamber(id1));
                return false;
            } else if (info2.type == MuPatHit::MDT) {
                if (rpcHelper.doubletR(id1) == 1)
                     return !(idh->stationIndex(id2) == MuonStationIndex::StIndex::BM || idh->isSmallChamber(id2));
                return true;
            }
            else { // both hits are RPC
                const int doubletR1 = rpcHelper.doubletR(id1);
                const int doubletR2 = rpcHelper.doubletR(id2);                
                if (doubletR1 !=doubletR2) return doubletR1 > doubletR2;
               
                const int doubletZ1 = rpcHelper.doubletZ(id1);
                const int doubletZ2 = rpcHelper.doubletZ(id2);                
                if (doubletZ1!= doubletZ2) return doubletZ1 > doubletZ2;
                
                const int doubletPhi1 = rpcHelper.doubletPhi(id1);
                const int doubletPHi2 = rpcHelper.doubletPhi(id2); 
                if (doubletPhi1 != doubletPHi2) return doubletPhi1 > doubletPHi2;
                 // last case can be handled below
            }
        }
        const double dist = distanceCalculator(hit1->parameters(), hit2->parameters());
        if (std::abs(dist)> 0.01) return dist < 0.;

        // for MDT's compare IDs
        if (info1.type == MuPatHit::MDT)
            return id1 < id2;
        // for cluster type measurements compare types and eta/phi
        if (info1.type != info2.type || info1.measuresPhi == info2.measuresPhi)
            return id1 < id2;
        // phi measurements larger than eta measurements
        return info1.measuresPhi;
      }

        DistanceAlongParameters distanceCalculator;
    };

}  // namespace Muon

#endif

/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
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
	    //if so, do some ID-based sorting
	    if(idh->chamberIndex(hit1->info().id) == idh->chamberIndex(hit2->info().id) && (hit1->info().type==1 || hit2->info().type==1)){
	        if(hit1->info().type==0){
		    if (idh->rpcIdHelper().doubletR(hit2->info().id) == 1) {
		        if (idh->stationIndex(hit1->info().id) == MuonStationIndex::StIndex::BM || idh->isSmallChamber(hit1->info().id))
		            return true;
		        else return false;
		    } 
		    else return false;
                }
		else if(hit2->info().type==0){
		    if (idh->rpcIdHelper().doubletR(hit1->info().id) == 1) {
		        if (idh->stationIndex(hit2->info().id) == MuonStationIndex::StIndex::BM || idh->isSmallChamber(hit2->info().id))
			    return false;
			else return true;
		    }
		    else return true;
		}
		else{ //both hits are RPC
		    Identifier id1=hit1->info().id;
		    Identifier id2=hit2->info().id;
		    if (idh->rpcIdHelper().doubletR(id1) != idh->rpcIdHelper().doubletR(id2)) {
		        return idh->rpcIdHelper().doubletR(id1) > idh->rpcIdHelper().doubletR(id2);
		    } else if (idh->rpcIdHelper().doubletZ(id1) != idh->rpcIdHelper().doubletZ(id2)) {
		        return idh->rpcIdHelper().doubletZ(id1) > idh->rpcIdHelper().doubletZ(id2);
		    } else if (idh->rpcIdHelper().doubletPhi(id1) != idh->rpcIdHelper().doubletPhi(id2)) {
		        return idh->rpcIdHelper().doubletPhi(id1) > idh->rpcIdHelper().doubletPhi(id2);
		    } //last case can be handled below
		}
	    }
            double dist = distanceCalculator(hit1->parameters(), hit2->parameters());
            if (dist < -0.01)
                return true;
            else if (dist > 0.01)
                return false;

            // most likely on same surface
            const MuPatHit::Info& info1 = hit1->info();
            const MuPatHit::Info& info2 = hit2->info();

            // for MDT's compare IDs
            if (info1.type == MuPatHit::MDT) return info1.id < info2.id;
            // for cluster type measurements compare types and eta/phi
            if (info1.type != info2.type || info1.measuresPhi == info2.measuresPhi) return info1.id < info2.id;
            // phi measurements larger than eta measurements
            if (info1.measuresPhi) return true;
            return false;
        }

        DistanceAlongParameters distanceCalculator;
    };

}  // namespace Muon

#endif

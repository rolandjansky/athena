/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MUONFIXEDIDMANIPULATOR_H
#define MUONFIXEDIDMANIPULATOR_H

#include <iostream>

#include "MuonCalibIdentifier/MuonFixedId.h"

namespace MuonCalib {

    class MuonFixedIdManipulator {
    public:
        /** enum defining station layers */
        enum StationIndex { BI = 0, BM, BO, BE, EIA, EMA, EOA, EEA, EIC, EMC, EOC, EEC, CSA, CSC, StationUnknown = 999 };
        /** enum defining trigger phi layers */
        enum PhiStationIndex { RPCBMR1 = 0, RPCBMR2, RPCBO, T4, T1, T2, T3, CS, PhiStationUnknown = 999 };

    public:
        /** return phi station layer index for a give identifier */
        PhiStationIndex phiStationLayerIndex(const MuonFixedId& id) const;

        /** returns station layer index for a give identifier */
        StationIndex stationLayerIndex(const MuonFixedId& id) const;

        /** returns a station layer identifier for the give identifier  */
        MuonFixedId stationLayerIdentifier(const MuonFixedId& id) const;

        /** returns a station layer identifier for the give station index */
        MuonFixedId stationLayerIdentifier(StationIndex stIndex) const;

        /** returns station Identifier for the give id
            MDT: station name (BMS)/eta/phi
            RPC: name of associated MDT station
            TGC: station name (T1)/eta/phi
        */
        MuonFixedId stationIdentifier(const MuonFixedId& id, bool mapOnMdts = false) const;

        /** returns chamber Identifier for the give id, same as stationIdentifier
            RPC: includes doubletR
        */
        MuonFixedId chamberIdentifier(const MuonFixedId& id) const;

        /** returns layer Identifier for the give id
            MDT: station name/eta/phi/ml/lay
            RPC: station name/eta/phi/double [R,Z,phi]/measuresPhi (if selected)
            TGC:
        */
        MuonFixedId moduleIdentifier(const MuonFixedId& id, bool includeMeasuresPhi = false) const;

        /** returns gas gap Identifier for the give id
            MDT: same as tube identifier
            RPC: station name/eta/phi/double [R,Z,phi]/lay/measuresPhi (if selected)
            TGC:
        */
        MuonFixedId gasGapIdentifier(const MuonFixedId& id, bool includeMeasuresPhi = false) const;

        /** returns whether this is a phi measurement */
        bool measuresPhi(const MuonFixedId& id) const;

        /** returns whether this is a phi measurement */
        bool isEndcap(const MuonFixedId& id) const;
    };

    inline bool MuonFixedIdManipulator::measuresPhi(const MuonFixedId& id) const {
        if (id.is_rpc())
            return id.rpcMeasuresPhi();
        else if (id.is_tgc())
            return id.tgcIsStrip();
        else if (id.is_csc())
            return id.cscMeasuresPhi();
        return false;
    }

    inline bool MuonFixedIdManipulator::isEndcap(const MuonFixedId& id) const {
        if (!id.isValid()) return false;
        if (id.is_rpc()) return false;
        if (id.is_csc()) return true;
        if (id.is_tgc()) return true;
        if (id.stationNameString()[0] == 'E') return true;
        return false;
    }

}  // namespace MuonCalib

#endif

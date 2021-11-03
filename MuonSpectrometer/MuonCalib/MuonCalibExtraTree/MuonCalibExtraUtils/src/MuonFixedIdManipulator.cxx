/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "MuonCalibExtraUtils/MuonFixedIdManipulator.h"

namespace MuonCalib {

    MuonFixedIdManipulator::PhiStationIndex MuonFixedIdManipulator::phiStationLayerIndex(const MuonFixedId& id) const {
        if (id.is_mdt()) return PhiStationUnknown;

        std::string stName = id.stationNumberToFixedStationString(id.stationName());
        if (id.is_rpc()) {
            if (stName[1] == 'M') {
                if (id.rpcDoubletR() == 1)
                    return RPCBMR1;
                else
                    return RPCBMR2;
            }
            return RPCBO;
        }

        if (id.is_tgc()) {
            if (stName[1] == '4') return T4;
            if (stName[1] == '1') return T1;
            if (stName[1] == '2') return T2;
            return T3;
        }
        if (id.is_csc()) return CS;

        return PhiStationUnknown;
    }

    MuonFixedId MuonFixedIdManipulator::stationLayerIdentifier(const MuonFixedId& id) const {
        return stationLayerIdentifier(stationLayerIndex(id));
    }

    MuonFixedId MuonFixedIdManipulator::stationLayerIdentifier(StationIndex stIndex) const {
        MuonFixedId newId;

        if (stIndex == CSA) {
            newId.set_csc();
            newId.setStationEta(-1);
        }
        if (stIndex == CSC) {
            newId.set_csc();
            newId.setStationEta(1);
        }

        newId.set_mdt();
        int stName = 0;
        int eta = 1;
        if (stIndex == BI)
            stName = 1;
        else if (stIndex == BM)
            stName = 3;
        else if (stIndex == BO)
            stName = 5;
        else if (stIndex == BE)
            stName = 7;
        else if (stIndex == EIA)
            stName = 14;
        else if (stIndex == EMA)
            stName = 19;
        else if (stIndex == EOA)
            stName = 22;
        else if (stIndex == EEA)
            stName = 16;
        else if (stIndex == EIC) {
            stName = 14;
            eta = -1;
        } else if (stIndex == EMC) {
            stName = 19;
            eta = -1;
        } else if (stIndex == EOC) {
            stName = 22;
            eta = -1;
        } else if (stIndex == EEC) {
            stName = 16;
            eta = -1;
        }
        newId.setStationName(stName);
        newId.setStationEta(eta);
        return newId;
    }

    MuonFixedIdManipulator::StationIndex MuonFixedIdManipulator::stationLayerIndex(const MuonFixedId& id) const {
        std::string stName = id.stationNumberToFixedStationString(id.stationName());
        if (stName[0] == 'B') {
            if (stName[1] == 'I') return BI;
            if (stName[1] == 'M') return BM;
            if (stName[1] == 'O') return BO;
            if (stName[1] == 'E') return BE;
        } else {
            int eta = id.eta();
            if (stName[1] == 'I') {
                if (eta < 0)
                    return EIA;
                else
                    return EIC;
            }
            if (stName[1] == 'M') {
                if (eta < 0)
                    return EMA;
                else
                    return EMC;
            }
            if (stName[1] == 'O') {
                if (eta < 0)
                    return EOA;
                else
                    return EOC;
            }
            if (stName[1] == 'E') {
                if (eta < 0)
                    return EEA;
                else
                    return EEC;
            }
            if (stName[2] == 'C') {
                if (eta < 0)
                    return CSA;
                else
                    return CSC;
            }
        }
        return StationUnknown;
    }

    MuonFixedId MuonFixedIdManipulator::stationIdentifier(const MuonFixedId& id, bool mapOnMdts) const {
        MuonFixedId newId;
        if (id.is_mdt()) {
            newId.set_mdt();
        } else if (id.is_rpc()) {
            if (mapOnMdts) {
                // map RPC onto MDT chamber
                newId.set_mdt();
            } else {
                newId.set_rpc();
                newId.setRpcDoubletR(id.rpcDoubletR());
            }
        } else if (id.is_tgc()) {
            newId.set_tgc();
        } else if (id.is_csc()) {
            newId.set_csc();
            newId.setCscChamberLayer(id.cscChamberLayer());
        }
        newId.setStationName(id.stationName());
        newId.setStationEta(id.eta());
        newId.setStationPhi(id.phi());
        return newId;
    }

    MuonFixedId MuonFixedIdManipulator::chamberIdentifier(const MuonFixedId& id) const {
        MuonFixedId newId = stationIdentifier(id);
        if (id.is_rpc()) newId.setRpcDoubletR(id.rpcDoubletR());
        return newId;
    }

    MuonFixedId MuonFixedIdManipulator::moduleIdentifier(const MuonFixedId& id, bool includeMeasuresPhi) const {
        MuonFixedId newId = stationIdentifier(id, false);
        if (id.is_rpc()) {
            newId.setRpcDoubletZ(id.rpcDoubletZ());
            newId.setRpcDoubletPhi(id.rpcDoubletPhi());
            if (includeMeasuresPhi) newId.setRpcMeasuresPhi(newId.rpcMeasuresPhi());

        } else if (id.is_tgc()) {
            if (includeMeasuresPhi) newId.setTgcIsStrip(newId.tgcIsStrip());

        } else if (id.is_csc()) {
            if (includeMeasuresPhi) newId.setCscMeasuresPhi(newId.cscMeasuresPhi());
        }
        return newId;
    }

    MuonFixedId MuonFixedIdManipulator::gasGapIdentifier(const MuonFixedId& id, bool includeMeasuresPhi) const {
        MuonFixedId newId = moduleIdentifier(id, includeMeasuresPhi);
        if (id.is_mdt()) {
            newId.setMdtMultilayer(id.mdtMultilayer());
            newId.setMdtTubeLayer(id.mdtTubeLayer());
        } else if (id.is_rpc()) {
            newId.setRpcGasGap(id.rpcGasGap());

        } else if (id.is_tgc()) {
            newId.setTgcGasGap(id.tgcGasGap());

        } else if (id.is_csc()) {
            newId.setCscWireLayer(id.cscWireLayer());
        }
        return newId;
    }

}  // namespace MuonCalib

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "MuonCalibExtraUtils/MuonFixedIdPrinter.h"

#include <sstream>

namespace MuonCalib {

    std::string MuonFixedIdPrinter::stationLayerIdentifier(const MuonFixedId& id) const {
        MuonFixedIdManipulator::StationIndex stIndex = m_manipulator.stationLayerIndex(id);
        return stationLayerIdentifier(stIndex);
    }

    std::string MuonFixedIdPrinter::stationLayerIdentifier(MuonFixedIdManipulator::StationIndex stIndex) const {
        std::string stName = "UnKnown";
        if (stIndex == MuonFixedIdManipulator::BI)
            stName = "BI";
        else if (stIndex == MuonFixedIdManipulator::BM)
            stName = "BM";
        else if (stIndex == MuonFixedIdManipulator::BO)
            stName = "BO";
        else if (stIndex == MuonFixedIdManipulator::BE)
            stName = "BE";
        else if (stIndex == MuonFixedIdManipulator::EIA)
            stName = "EIA";
        else if (stIndex == MuonFixedIdManipulator::EMA)
            stName = "EMA";
        else if (stIndex == MuonFixedIdManipulator::EOA)
            stName = "EOA";
        else if (stIndex == MuonFixedIdManipulator::EEA)
            stName = "EEA";
        else if (stIndex == MuonFixedIdManipulator::EIC)
            stName = "EIC";
        else if (stIndex == MuonFixedIdManipulator::EMC)
            stName = "EMC";
        else if (stIndex == MuonFixedIdManipulator::EOC)
            stName = "EOC";
        else if (stIndex == MuonFixedIdManipulator::EEC)
            stName = "EEC";
        return stName;
    }

    std::string MuonFixedIdPrinter::stationIdentifier(const MuonFixedId& id) const {
        std::ostringstream sout;
        if (!id.isValid()) return " Invalid Identifier";
        sout << id.technologyString() << " " << id.stationNameString() << " eta " << std::setw(2) << id.eta() << " phi " << std::setw(2)
             << id.phi();
        return sout.str();
    }

    std::string MuonFixedIdPrinter::chamberIdentifier(const MuonFixedId& id) const {
        std::ostringstream sout;
        if (!id.isValid()) return " Invalid Identifier";
        sout << stationIdentifier(id);
        if (id.is_rpc()) sout << " doubR " << id.rpcDoubletR();

        return sout.str();
    }

    std::string MuonFixedIdPrinter::moduleIdentifier(const MuonFixedId& id) const {
        std::ostringstream sout;
        if (!id.isValid()) return " Invalid Identifier";
        sout << chamberIdentifier(id);
        if (id.is_rpc())
            sout << " doubZ " << id.rpcDoubletZ() << " doubPhi " << id.rpcDoubletPhi();
        else if (id.is_mdt())
            sout << " ml " << id.mdtMultilayer();
        return sout.str();
    }

    std::string MuonFixedIdPrinter::gasGapIdentifier(const MuonFixedId& id, bool includeMeasuresPhi) const {
        if (!id.isValid()) return " Invalid Identifier";

        std::ostringstream sout;
        sout << moduleIdentifier(id);
        bool measuresPhi = false;
        if (id.is_rpc()) {
            sout << " gap " << id.rpcGasGap();
            measuresPhi = id.rpcMeasuresPhi();
        } else if (id.is_tgc()) {
            sout << " gap " << id.tgcGasGap();
            measuresPhi = id.tgcIsStrip();
        } else if (id.is_csc()) {
            sout << " lay " << id.cscWireLayer();
            measuresPhi = id.cscMeasuresPhi();
        } else {
            sout << " lay " << id.mdtTubeLayer() << " tube " << std::setw(2) << id.mdtTube();
        }

        if (measuresPhi && includeMeasuresPhi) sout << "  measuresPhi";

        return sout.str();
    }

    std::string MuonFixedIdPrinter::gasGapIdentifierForMap(const MuonFixedId& id, bool includeMeasuresPhi) const {
        std::string st = id.stationNumberToFixedStationString(id.stationName());
        std::ostringstream secondPart;
        bool measuresPhi = false;
        bool addSecondPart = false;
        std::ostringstream station;
        station << st << "_" << id.phi() << "_" << id.eta();
        if (id.is_rpc()) {
            int doublet = 2 * (id.rpcDoubletZ() - 1) + (id.rpcDoubletPhi() - 1);

            if (id.rpcMeasuresPhi()) measuresPhi = true;
            addSecondPart = true;
            secondPart << "_d_" << doublet;

        } else if (id.is_tgc()) {
            if (id.tgcIsStrip()) measuresPhi = true;
            addSecondPart = true;
            secondPart << "_gg_" << id.tgcGasGap();

        } else if (id.is_csc()) {
            if (id.cscMeasuresPhi()) measuresPhi = true;
            addSecondPart = true;
            secondPart << "_wl_" << id.cscWireLayer();
        }
        std::string rst = station.str();
        if (addSecondPart) { rst += secondPart.str(); }
        if (includeMeasuresPhi) {
            if (measuresPhi)
                rst += "_phi";
            else
                rst += "_eta";
        }
        return rst;
    }

    std::string MuonFixedIdPrinter::fullIdentifier(const MuonFixedId& id) const {
        std::ostringstream sout;
        if (!id.isValid()) return " Invalid Identifier";
        sout << gasGapIdentifier(id, false);
        bool measuresPhi = m_manipulator.measuresPhi(id);
        if (id.is_rpc())
            sout << " strip " << std::setw(2) << id.rpcStrip();
        else if (id.is_tgc())
            sout << " strip " << std::setw(2) << id.tgcChannel();
        else if (id.is_csc())
            sout << " strip " << std::setw(2) << id.cscStrip();

        if (measuresPhi) sout << " measuresPhi";

        return sout.str();
    }

}  // namespace MuonCalib

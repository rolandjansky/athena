/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MUONFIXEDIDPRINTER_H
#define MUONFIXEDIDPRINTER_H

#include "MuonCalibExtraUtils/MuonFixedIdManipulator.h"
#include "MuonCalibIdentifier/MuonFixedId.h"

namespace MuonCalib {

    class MuonFixedIdPrinter {
    public:
        /** prints a station layer identifier for the give identifier  */
        std::string stationLayerIdentifier(const MuonFixedId& id) const;

        /** prints a station layer identifier for the give station index */
        std::string stationLayerIdentifier(MuonFixedIdManipulator::StationIndex stIndex) const;

        /** prints station Identifier for the give id
            MDT: station name (BMS)/eta/phi
            RPC: name of associated MDT station
            TGC: station name (T1)/eta/phi
        */
        std::string stationIdentifier(const MuonFixedId& id) const;

        /** prints chamber Identifier for the give id. Consists of stationIdentifier.
            RPC: includes DoubletR
        */
        std::string chamberIdentifier(const MuonFixedId& id) const;

        /** prints layer Identifier for the give id
            MDT: station name/eta/phi/ml/lay
            RPC: station name/eta/phi/double [R,Z,phi]/measuresPhi (if selected)
            TGC:
        */
        std::string moduleIdentifier(const MuonFixedId& id) const;

        /** prints gas gap Identifier for the give id
            MDT: same as tube identifier
            RPC: station name/eta/phi/double [R,Z,phi]/lay/measuresPhi (if selected)
            TGC:
        */
        std::string gasGapIdentifier(const MuonFixedId& id, bool includeMeasuresPhi = false) const;

        /** prints full Identifier for the give id */
        std::string fullIdentifier(const MuonFixedId& id) const;

        /** print gapGap Id to be used in map */
        std::string gasGapIdentifierForMap(const MuonFixedId& id, bool includeMeasuresPhi = false) const;

    private:
        MuonFixedIdManipulator m_manipulator;
    };

}  // namespace MuonCalib

#endif

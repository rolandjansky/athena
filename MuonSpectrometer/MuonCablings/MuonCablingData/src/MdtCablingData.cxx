/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/

#include "MuonCablingData/MdtCablingData.h"

std::ostream& operator<<(std::ostream& ostr, const MdtCablingOffData& obj) {
    ostr << "StationIndex: " << (int)obj.stationIndex << " eta: " << (int)obj.eta << " phi: " << (int)obj.phi
         << " multilayer: " << (int)obj.multilayer;
    return ostr;
}
std::ostream& operator<<(std::ostream& ostr, const MdtCablingOnData& obj) {
    ostr << " subdetector id: " << (int)obj.subdetectorId << " MROD: " << (int)obj.mrod << " CSM: " << (int)obj.csm;
    return ostr;
}
std::ostream& operator<<(std::ostream& ostr, const MdtCablingData& obj) {
    ostr << static_cast<const MdtCablingOffData&>(obj) << " tube-layer: " << (int)obj.layer << " tube: " << (int)obj.tube << "  ---- "
         << static_cast<const MdtCablingOnData&>(obj) << " TDC: " << (int)obj.tdcId << " mezzanine-type: " << (int)obj.mezzanine_type
         << " tdc-channel: " << (int)obj.channelId;
    return ostr;
}

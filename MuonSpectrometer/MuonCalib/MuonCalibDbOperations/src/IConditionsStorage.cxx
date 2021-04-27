/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "MuonCalibDbOperations/IConditionsStorage.h"

// MuonCalibIdentifier
#include "MuonCalibIdentifier/MuonFixedId.h"

namespace MuonCalib {

    IConditionsStorage::TubeId::TubeId(int id_num) {
        MuonFixedId id(id_num);
        ml = id.mdtMultilayer();
        ly = id.mdtTubeLayer();
        tb = id.mdtTube();
    }

    bool IConditionsStorage::TubeId::operator<(const IConditionsStorage::TubeId &other) const {
        if (ml < other.ml) return true;
        if (ml > other.ml) return false;
        if (ly < other.ly) return true;
        if (ly > other.ly) return false;
        if (tb < other.tb) return true;
        return false;
    }

    bool IConditionsStorage::TubeId::operator>(const IConditionsStorage::TubeId &other) const {
        if (ml > other.ml) return true;
        if (ml < other.ml) return false;
        if (ly > other.ly) return true;
        if (ly < other.ly) return false;
        if (tb > other.tb) return true;
        return false;
    }

}  // namespace MuonCalib

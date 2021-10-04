/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/
#include "TrkDriftCircleMath/MdtStationId.h"

namespace TrkDriftCircleMath {

    std::ostream& operator<<(std::ostream& os, const TrkDriftCircleMath::MdtStationId& id) {
        if (id.isBarrel())
            os << " BARREL";
        else
            os << " ENDCAP";
        os << " stName " << id.stName() << " eta " << id.eta() << " phi " << id.phi();
        if (id.isSmallMdt()) os << " sMDT";
        return os;
    }

}  // namespace TrkDriftCircleMath

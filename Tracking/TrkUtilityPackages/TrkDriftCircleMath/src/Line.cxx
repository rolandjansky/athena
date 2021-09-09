/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "TrkDriftCircleMath/Line.h"

namespace TrkDriftCircleMath {

    std::ostream& operator<<(std::ostream& os, const TrkDriftCircleMath::Line& l) {
        os << " phi " << l.phi() << " pos " << l.position();
        //     << " pos " << l.position() << " dir " << l.direction();
        return os;
    }

}  // namespace TrkDriftCircleMath

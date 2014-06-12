/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef PANTAUEVENT_SEEDTRUTHMAP_H
#define PANTAUEVENT_SEEDTRUTHMAP_H

#include <map>
//#include "TauTrackEvent/TruthTau.h"
//#include "PanTauEvent/TauSeed.h"

namespace TauID {
    class TruthTau;
}
namespace PanTau {
  class TauSeed;
  typedef std::multimap<const PanTau::TauSeed*, const TauID::TruthTau*> SeedTruthMap;
}


#endif //PANTAUEVENT_SEEDTRUTHMAP_H

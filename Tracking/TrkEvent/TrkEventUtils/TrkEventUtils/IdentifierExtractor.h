/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRKEVENTUTILS_IDEXTRACTOR_H
#define TRKEVENTUTILS_IDEXTRACTOR_H

#include <vector>
#include "TrkRIO_OnTrack/RIO_OnTrack.h"

namespace Trk {

  class MeasurementBase;

  /** @class IdentifierExtractor
      @brief Small utility to get hit Identifiers out of MeasurementBase (either in a vector or single).

      If MB object is of an abstract flavour -- i.e. not on a detector element -- it is skipped.

      @author Wolfgang Liebig
   */

  class IdentifierExtractor {
  public:
    static void extract(std::vector<Identifier>& ids, const std::vector<const MeasurementBase*>& measurements);
    static Identifier extract(const MeasurementBase*);
  };

}
#endif


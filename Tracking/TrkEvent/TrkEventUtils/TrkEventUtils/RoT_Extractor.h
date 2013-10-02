/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef ROT_EXTRACTOR_H
#define ROT_EXTRACTOR_H

#include <vector>

namespace Trk {

  class RIO_OnTrack;
  class MeasurementBase;
  
  /** @class RoT_Extractor
      @brief Small utility to cast MeasurementBase (either in a vector or single)
             into RIO_OnTrack.

      CompetingRIOsOnTrack objects are understood (ROT with max. assign-prob taken).
      If MB object is of different flavour than ROT, it is skipped.

      @author Edward Moyse
   */

  class RoT_Extractor {
  public:
    static void extract(std::vector<const RIO_OnTrack*>& rots, const std::vector<const MeasurementBase*>& measurements, bool convertCompRots=false);
    static void extract(const RIO_OnTrack*& rot, const MeasurementBase* meas);
  };

}

#endif



/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/

#ifndef LARGEOH62004ALGS_EXCLUDERCONSTRUCTIONH62004_H
#define LARGEOH62004ALGS_EXCLUDERCONSTRUCTIONH62004_H

#include "GeoModelKernel/GeoFullPhysVol.h"
#include "GeoModelKernel/GeoPhysVol.h"

namespace LArGeo {


   class ExcluderConstructionH62004 {
   public:
     ExcluderConstructionH62004(int which = 0);
     virtual ~ExcluderConstructionH62004();

     GeoVFullPhysVol*  GetEnvelope();

   private:
     int m_which;
};

} // namespace LArGeo

#endif // LARGEOH62004ALGS_EXCLUDERCONSTRUCTIONH62004_H

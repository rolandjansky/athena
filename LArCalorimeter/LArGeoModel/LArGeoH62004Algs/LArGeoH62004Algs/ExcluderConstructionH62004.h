/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef __ExcluderConstructionH62004_H__
#define __ExcluderConstructionH62004_H__

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

#endif // __ExcluderConstructionH62004_H__

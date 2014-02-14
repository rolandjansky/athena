/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MUONCALIB_LOCALTOPRECISION_H
#define MUONCALIB_LOCALTOPRECISION_H

#include "GeoPrimitives/GeoPrimitives.h"

namespace MuonCalib {

  /** Provides transformation between local and presicion reference frames*/
  class LocalToPrecision {
  public:
    static Amg::Vector3D precision( const Amg::Vector3D& p ) { return Amg::Vector3D( p.y(), p.z(), p.x() ); }
    static Amg::Vector3D local( const Amg::Vector3D& p ) { return Amg::Vector3D( p.z(), p.x(), p.y() ); }
    
  };

}

#endif

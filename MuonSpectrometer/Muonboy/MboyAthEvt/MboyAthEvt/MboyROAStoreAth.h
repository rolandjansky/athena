/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MboyROAStoreAth_H
#define MboyROAStoreAth_H

#include "MboyAthEvt/MboyAthEvtMisc.h"

#include "MboyCoreEvt/MboyROAStore.h"

#include "GeoPrimitives/GeoPrimitives.h"

namespace Trk {
 class TrackRoad;
}

  /**
   @class MboyROAStoreAth

  @author samusog@cern.ch
  
  */

class MboyROAStoreAth: public MboyROAStore{
public:
   MboyROAStoreAth();
   virtual ~MboyROAStoreAth();

public:
///////////////////////////////////
 
   /**Add a MboyROA */
   void Add(const Amg::Vector3D& gpos , const Amg::Vector3D& gdir) ;
   void Add(const Trk::TrackRoad& road ) ;

   /**Get a MboyROA as */
   Trk::TrackRoad       GetMboyROAasTrackRoad(int MboyROANber) const;
   Amg::Vector3D        GetMboyROAasGlobalPosition(int MboyROANber) const ;
   Amg::Vector3D        GetMboyROAasGlobalDirection(int MboyROANber) const ;
   
};
#endif

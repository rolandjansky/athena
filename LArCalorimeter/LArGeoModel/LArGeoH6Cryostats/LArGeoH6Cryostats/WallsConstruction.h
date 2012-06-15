/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// Construction of the walls in front of the H1 Cryostat
// 
// March 2007 M.Fincke

#ifndef __WallsConstruction_H__
#define __WallsConstruction_H__

#include "GeoModelKernel/GeoPhysVol.h"
#include "GeoModelKernel/GeoFullPhysVol.h"
class IRDBAccessSvc;
class IGeoModelSvc;
class MsgStream;
 
namespace LArGeo {

  class WallsConstruction 
    {
    public:
    
    WallsConstruction();
    virtual ~WallsConstruction();
    
    // Get the envelope containing this detector:
    virtual GeoVPhysVol* GetEnvelope();

    MsgStream *msg;
 
      
  private:


    GeoPhysVol          *WallsPhysical;


    IRDBAccessSvc       *pAccessSvc;
    IGeoModelSvc        *geoModelSvc;
    
  };

} // namespace LArGeo

#endif // __WallsConstruction_H__

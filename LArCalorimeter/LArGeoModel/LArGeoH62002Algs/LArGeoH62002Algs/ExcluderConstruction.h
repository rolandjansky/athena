/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// ExcluderConstruction
// 
// 
// July-2006 JP Archambault, Mohsen Khakzad

#ifndef __ExcluderConstruction_H__
#define __ExcluderConstruction_H__

#include "GeoModelKernel/GeoPhysVol.h"
#include "GeoModelKernel/GeoFullPhysVol.h"
#include "GeoModelKernel/GeoVFullPhysVol.h"

class IRDBAccessSvc;
class IGeoModelSvc;

namespace LArGeo {

  class ExcluderConstruction 
    {
    public:
    
    ExcluderConstruction();
    virtual ~ExcluderConstruction();
    
    // Get the envelope containing this detector.
    virtual GeoPhysVol* GetEnvelope();
/*     virtual GeoVFullPhysVol* GetEnvelope(); */
/*     GeoPhysVol* GetLArPhysical(); */

      
  private:

    //GeoFullPhysVol*     createEnvelope();

    GeoPhysVol          *PhysExcluder;
/*     GeoPhysVol          *cryoEnvelopePhysical; */
/*     GeoFullPhysVol      *cryoMotherPhysical; */
/*     GeoPhysVol          *cryoLArPhys; */

    IRDBAccessSvc       *pAccessSvc;
    IGeoModelSvc        *geoModelSvc;
    
  };

}    // namespace LArGeo

#endif // __ExcluderConstruction_H__

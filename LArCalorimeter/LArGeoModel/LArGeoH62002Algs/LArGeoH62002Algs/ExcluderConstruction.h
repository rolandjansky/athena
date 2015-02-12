/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// ExcluderConstruction
// 
// 
// July-2006 JP Archambault, Mohsen Khakzad

#ifndef LARGEOH62002ALGS_EXCLUDERCONSTRUCTION_H
#define LARGEOH62002ALGS_EXCLUDERCONSTRUCTION_H

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

    //GeoPhysVol          *m_PhysExcluder;
/*     GeoPhysVol          *cryoEnvelopePhysical; */
/*     GeoFullPhysVol      *cryoMotherPhysical; */
/*     GeoPhysVol          *cryoLArPhys; */
    
  };

}    // namespace LArGeo

#endif // LARGEOH62002ALGS_EXCLUDERCONSTRUCTION_H

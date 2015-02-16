/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// H6CryostatConstruction
// Return an envelope that contains the H6 LAr Cryostat.
// Should be common to H62002, H62003, H62004 
// Feb-2006 JP Archambault, Mohsen Khakzad

#ifndef LARGEOH6CRYOSTATS_H6CRYOSTATCONSTRUCTION_H
#define LARGEOH6CRYOSTATS_H6CRYOSTATCONSTRUCTION_H

#include "GeoModelKernel/GeoPhysVol.h"
#include "GeoModelKernel/GeoFullPhysVol.h"
#include "GeoModelKernel/GeoVFullPhysVol.h"

class IRDBAccessSvc;
class IGeoModelSvc;

namespace LArGeo {

  class H6CryostatConstruction 
    {
    public:
    
    H6CryostatConstruction();
    virtual ~H6CryostatConstruction();
    
    // Get the envelope containing this detector.
    virtual GeoVFullPhysVol* GetEnvelope();
    GeoPhysVol* GetLArPhysical();

      
  private:

    //GeoFullPhysVol*     createEnvelope();

    GeoFullPhysVol      *m_cryoMotherPhysical;
    GeoPhysVol          *m_cryoLArPhys;
  };

}    // namespace LArGeo

#endif // LARGEOH6CRYOSTATS_H6CRYOSTATCONSTRUCTION_H


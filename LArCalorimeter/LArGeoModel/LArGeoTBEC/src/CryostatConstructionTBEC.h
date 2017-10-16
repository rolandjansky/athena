/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// CryostatConstructionTBEC
// Return an envelope that contains the TBEC LAr Cryostat.
// Dec-2005 V. Niess
// from CryostatConstructionH62003 and LArG4TBECCryostatConstruction.

#ifndef __CryostatConstructionTBEC_H__
#define __CryostatConstructionTBEC_H__

#include "GeoModelKernel/GeoPhysVol.h"
#include "GeoModelKernel/GeoFullPhysVol.h"
namespace LArGeo {

  class CryostatConstructionTBEC 
    {
    public:
    
    CryostatConstructionTBEC();
    virtual ~CryostatConstructionTBEC();
    
    // Get the envelope containing this detector.
    virtual GeoVFullPhysVol* GetEnvelope();
    
    // Get the LAr physical volume.
    GeoPhysVol* GetLArPhysical();
      
  private:

    GeoFullPhysVol      *m_cryoEnvelopePhysical;
    GeoPhysVol		*m_LArPhysical;
  };

} // namespace LArGeo

#endif // __CryostatConstructionTBEC_H__

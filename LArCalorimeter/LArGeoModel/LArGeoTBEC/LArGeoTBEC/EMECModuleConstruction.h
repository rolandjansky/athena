/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// EMECModuleConstruction
// Return an envelope that contains the TB Module of the EMEC.
// 9-Dec-2005 V.N.
// copied from EMECConstruction.h and ModuleConstruction in LArG4EC

#ifndef __EMECModuleConstruction_H__
#define __EMECModuleConstruction_H__

#include "GeoModelKernel/GeoVFullPhysVol.h"

namespace LArGeo {

  class EMECModuleConstruction 
  {
  public:
    
    EMECModuleConstruction( bool itbb = true, bool iw = true, bool ow = true ); // changed initialisation to select
                                                                                // inner or outer wheel only.
                                                                                // Default is TB position, with both wheels.
    virtual ~EMECModuleConstruction();
    
    // Get the envelope containing this detector.
    virtual GeoVFullPhysVol* GetEnvelope();

  private:

    bool m_isTB;
    bool m_isInnerWheel;
    bool m_isOuterWheel;
  };

} // namespace LArGeo

#endif // __EMECModuleConstruction_H__

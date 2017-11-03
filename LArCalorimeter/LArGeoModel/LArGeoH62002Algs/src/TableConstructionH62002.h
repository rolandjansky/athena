/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// TableConstructionH62002
// Return an envelope that contains the LArH62002 Beam Instrumentation.
// Apr-2006 mgf


#ifndef LARGEOH62002ALGS_TABLECONSTRUCTIONH62002_H
#define LARGEOH62002ALGS_TABLECONSTRUCTIONH62002_H

#include "GeoModelKernel/GeoPhysVol.h"

class LArDetDescrManager;

namespace LArGeo {

  class TableConstructionH62002 
  {
  public:
    
    TableConstructionH62002();
    virtual ~TableConstructionH62002();
    
    // Get the envelope containing this detector.
    virtual GeoVPhysVol* GetEnvelope();
    void SetManager(LArDetDescrManager* /*mgr*/){}
    
      
  private:
    GeoPhysVol          *m_H62002TablePhysical;
  };

} // namespace LArGeo

#endif // LARGEOH62002ALGS_TABLECONSTRUCTIONH62002_H

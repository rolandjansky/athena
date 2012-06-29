/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// TableConstructionH62002
// Return an envelope that contains the LArH62002 Beam Instrumentation.
// Apr-2006 mgf


#ifndef __TableConstructionH62002_H__
#define __TableConstructionH62002_H__

#include "GeoModelKernel/GeoPhysVol.h"
#include "GeoModelKernel/GeoFullPhysVol.h"
class IRDBAccessSvc;
class IGeoModelSvc;
class LArDetDescrManager;

namespace LArGeo {

  class TableConstructionH62002 
    {
    public:
    
    TableConstructionH62002();
    virtual ~TableConstructionH62002();
    
    // Get the envelope containing this detector.
    virtual GeoVPhysVol* GetEnvelope();
    void SetManager(LArDetDescrManager* mgr){_detectorManager = mgr;}

      
  private:

    GeoPhysVol          *H62002TablePhysical;
    LArDetDescrManager  *_detectorManager;

    IRDBAccessSvc       *pAccessSvc;
    IGeoModelSvc        *geoModelSvc;
    
  };

} // namespace LArGeo

#endif // __TableConstructionH62002_H__

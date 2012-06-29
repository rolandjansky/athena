/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// FrontBeamConstructionH62002
// Return an envelope that contains the LArH62002 Beam Instrumentation.
// Apr-2006 mgf

#ifndef __FrontBeamConstructionH62002_H__
#define __FrontBeamConstructionH62002_H__

#include "GeoModelKernel/GeoPhysVol.h"
#include "GeoModelKernel/GeoFullPhysVol.h"
class IRDBAccessSvc;
class IGeoModelSvc;
class LArDetDescrManager;

namespace LArGeo {

  class FrontBeamConstructionH62002 
    {
    public:
    
    FrontBeamConstructionH62002();
    virtual ~FrontBeamConstructionH62002();
    
    // Get the envelope containing this detector.
    virtual GeoVPhysVol* GetEnvelope();
    void SetManager(LArDetDescrManager* mgr){_detectorManager = mgr;}

    //void SetAxisVisState(bool state) {_axisVisState=state;}
      
  private:


    GeoPhysVol          *H62002FrontBeamPhysical;
    LArDetDescrManager  *_detectorManager;


    IRDBAccessSvc       *pAccessSvc;
    IGeoModelSvc        *geoModelSvc;
    
  };

} // namespace LArGeo

#endif // __FrontBeamConstructionH62002_H__

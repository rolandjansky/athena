/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// LArDetectorConstructionH62003
// Return an envelope that contains the LArH62003 Test Bench.
// Jan-2005 JPA
// from LArDetectorConstructionTBEC

#ifndef __LArDetectorConstructionH62003_H__
#define __LArDetectorConstructionH62003_H__

#include "GeoModelKernel/GeoPhysVol.h"
#include "GeoModelKernel/GeoFullPhysVol.h"
class IRDBAccessSvc;
namespace LArGeo {

  class LArDetectorConstructionH62003 
    {
    public:
      
      LArDetectorConstructionH62003();
      virtual ~LArDetectorConstructionH62003();
      
      // Get the envelope containing this detector.
      virtual GeoVPhysVol* GetEnvelope();
      
      // Set a vis limit for the EC
      void SetFCALVisLimit(int limit) {_fcalVisLimit=limit;}
      void SetAxisVisState(bool state) {_axisVisState=state;}
      
    private:
      
      void  createAxis(GeoFullPhysVol* H62003MotherPhysical, GeoMaterial* mat);
      
      GeoFullPhysVol*      H62003EnvelopePhysical;
      
      int                  _fcalVisLimit;
      bool                 _axisVisState;
      
      IRDBAccessSvc        *pAccessSvc;
      
    };
  
} // namespace LArGeo

#endif // __LArDetectorConstructionH62003_H__

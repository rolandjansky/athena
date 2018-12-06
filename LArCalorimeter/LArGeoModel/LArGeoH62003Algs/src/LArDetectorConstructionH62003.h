/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/

// LArDetectorConstructionH62003
// Return an envelope that contains the LArH62003 Test Bench.
// Jan-2005 JPA
// from LArDetectorConstructionTBEC

#ifndef LARGEOH62003ALGS_LARDETECTORCONSTRUCTIONH62003_H
#define LARGEOH62003ALGS_LARDETECTORCONSTRUCTIONH62003_H

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
      void SetFCALVisLimit(int limit) {m_fcalVisLimit=limit;}
      void SetAxisVisState(bool state) {m_axisVisState=state;}
      
    private:
      
      void  createAxis(GeoFullPhysVol* H62003MotherPhysical, const GeoMaterial* mat);
      
      GeoFullPhysVol*      m_H62003EnvelopePhysical;
      
      int                  m_fcalVisLimit;
      bool                 m_axisVisState;
      
      IRDBAccessSvc        *m_pAccessSvc;
      
    };
  
} // namespace LArGeo

#endif // LARGEOH62003ALGS_LARDETECTORCONSTRUCTIONH62003_H

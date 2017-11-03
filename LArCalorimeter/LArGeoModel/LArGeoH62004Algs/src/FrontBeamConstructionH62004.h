/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// FrontBeamConstructionH62004
// adopted from FrontBeamConstructionH62002
// Pavol S. May 2007

#ifndef __FrontBeamConstructionH62004_H__
#define __FrontBeamConstructionH62004_H__

#include "GeoModelKernel/GeoPhysVol.h"
#include "GeoModelKernel/GeoFullPhysVol.h"
class LArDetDescrManager;

namespace LArGeo {

  class FrontBeamConstructionH62004 
    {
    public:
    
    FrontBeamConstructionH62004();
    virtual ~FrontBeamConstructionH62004();
    
    // Get the envelope containing this detector.
    virtual GeoVPhysVol* GetEnvelope();
    void SetManager(LArDetDescrManager* mgr){m_detectorManager = mgr;}

    //void SetAxisVisState(bool state) {_axisVisState=state;}
      
  private:


    GeoPhysVol          *m_H62004FrontBeamPhysical;

    LArDetDescrManager  *m_detectorManager;
  };

} // namespace LArGeo

#endif // __FrontBeamConstructionH62004_H__

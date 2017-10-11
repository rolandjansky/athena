/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// MiddleBeamConstructionH62004
// adopted from FrontBeamConstructionH62004
// Pavol S. May 2007

#ifndef __MiddleBeamConstructionH62004_H__
#define __MiddleBeamConstructionH62004_H__

#include "GeoModelKernel/GeoPhysVol.h"
#include "GeoModelKernel/GeoFullPhysVol.h"
class LArDetDescrManager;

namespace LArGeo {

  class MiddleBeamConstructionH62004 
    {
    public:
    
    MiddleBeamConstructionH62004();
    virtual ~MiddleBeamConstructionH62004();
    
    // Get the envelope containing this detector.
    virtual GeoVPhysVol* GetEnvelope();
    void SetManager(LArDetDescrManager* mgr){m_detectorManager = mgr;}

    //void SetAxisVisState(bool state) {_axisVisState=state;}
      
  private:


    GeoPhysVol          *m_H62004MiddleBeamPhysical;
    LArDetDescrManager  *m_detectorManager;

  };

} // namespace LArGeo

#endif // __MiddleBeamConstructionH62004_H__

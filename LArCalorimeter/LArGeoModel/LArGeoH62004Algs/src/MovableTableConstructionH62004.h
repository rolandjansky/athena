/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// MovableBeamConstructionH62004
// adopted from MovableTableConstructionH62002
// Pavol S. May 2007

#ifndef __MovableTableConstructionH62004_H__
#define __MovableTableConstructionH62004_H__

#include "GeoModelKernel/GeoPhysVol.h"
#include "GeoModelKernel/GeoFullPhysVol.h"
class LArDetDescrManager;

namespace LArGeo {

  class MovableTableConstructionH62004 
    {
    public:
    
    MovableTableConstructionH62004();
    virtual ~MovableTableConstructionH62004();
    
    // Get the envelope containing this detector.
    virtual GeoVPhysVol* GetEnvelope();
    void SetManager(LArDetDescrManager* mgr){m_detectorManager = mgr;}

    //void SetAxisVisState(bool state) {_axisVisState=state;}
      
  private:


    GeoPhysVol          *m_H62004MovableTablePhysical;

    LArDetDescrManager  *m_detectorManager;
  };

} // namespace LArGeo

#endif // __MovableTableConstructionH62004_H__

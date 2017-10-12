/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef LArGeo_LArDetectorFactoryH62003_h
#define LArGeo_LArDetectorFactoryH62003_h

#include "GeoModelKernel/GeoVDetectorFactory.h"
#include "LArReadoutGeometry/LArDetectorManager.h"

namespace LArGeo {

  class LArDetectorFactoryH62003 : public GeoVDetectorFactory  {

  public:
  
    // Constructor:
    LArDetectorFactoryH62003();
  
    // Destructor:
    virtual ~LArDetectorFactoryH62003();
  
    // Creation of geometry:
    virtual void create(GeoPhysVol* world);
  
    // Access to the results:
    virtual const LArDetectorManager* getDetectorManager() const;
  
    // Set parameters:

    void setFCALVisLimit(int maxCell)       {_fcalVisLimit   = maxCell;}
    void setAxisVisState(bool axis)         {_axisVisState   = axis;}

  private:  
  
    // Illegal operations:
    const LArDetectorFactoryH62003 & operator=(const LArDetectorFactoryH62003 &right);
    LArDetectorFactoryH62003(const LArDetectorFactoryH62003 &right);
  
    // The manager:
    LArDetectorManager*       m_detectorManager;
    int                       _fcalVisLimit;
    bool                      _axisVisState;
  };

} // namespace LArGeo

#endif 


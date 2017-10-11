/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef LArGeo_LArDetectorFactoryTBEC_h
#define LArGeo_LArDetectorFactoryTBEC_h

#include "GeoModelKernel/GeoVDetectorFactory.h"
#include "LArReadoutGeometry/LArDetectorManager.h"

namespace LArGeo {

  class LArDetectorFactoryTBEC : public GeoVDetectorFactory  {

  public:
  
    // Constructor:
    LArDetectorFactoryTBEC();
  
    // Destructor:
    virtual ~LArDetectorFactoryTBEC();
  
    // Creation of geometry:
    virtual void create(GeoPhysVol* world);
  
    // Access to the results:
    virtual const LArDetectorManager* getDetectorManager() const;
  

    // Set parameters:

    void setECVisLimit(int maxCell)       {m_ecVisLimit   = maxCell;}

  private:  
  
    // Illegal operations:
    const LArDetectorFactoryTBEC & operator=(const LArDetectorFactoryTBEC &right);
    LArDetectorFactoryTBEC(const LArDetectorFactoryTBEC &right);
  
    // The manager:
    LArDetectorManager*       m_detectorManager;
    int                       m_ecVisLimit;

  };

} // namespace LArGeo

#endif 


/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef LArDetectorFactoryH62002_h
#define LArDetectorFactoryH62002_h 1
#include "GeoModelKernel/GeoVDetectorFactory.h"
#include "LArReadoutGeometry/LArDetectorManager.h"

class StoreGateSvc;
class LArDetDescrManager;
class LArMgrInitializer;

namespace LArGeo {

  class LArDetectorFactoryH62002 : public GeoVDetectorFactory  {

  public:
    
    // Constructor:
    LArDetectorFactoryH62002(StoreGateSvc *pDetStore);
    
    // Destructor:
    ~LArDetectorFactoryH62002();
    
    // Creation of geometry:
    virtual void create(GeoPhysVol *world);
    
    //
    virtual const LArDetectorManager * getDetectorManager() const;
    
    MsgStream *log ;
    
  private:  
    
    double m_cryoXpos;
    double m_tableYpos;

    void                getSimulationParameters();

    // Illegal operations:
    const LArDetectorFactoryH62002 & operator=(const LArDetectorFactoryH62002 &right);
    LArDetectorFactoryH62002(const LArDetectorFactoryH62002 &right);
    
    
    // The Detector
    StoreGateSvc             *m_detectorStore;

    // The manager:
    LArDetectorManager       *m_detectorManager;
    
  };
  
} // namespace LArGeo

#endif



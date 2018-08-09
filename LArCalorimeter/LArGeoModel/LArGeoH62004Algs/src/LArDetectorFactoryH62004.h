/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef LArDetectorFactoryH62004_h
#define LArDetectorFactoryH62004_h 1
#include "GeoModelKernel/GeoVDetectorFactory.h"
#include "LArReadoutGeometry/LArDetectorManager.h"

class StoreGateSvc;
class LArDetDescrManager;
class LArMgrInitializer;
class CaloCell_ID;

namespace LArGeo {

  class LArDetectorFactoryH62004 : public GeoVDetectorFactory  {

  public:
    
    // Constructor:
    LArDetectorFactoryH62004(StoreGateSvc *pDetStore);
    
    // Destructor:
    ~LArDetectorFactoryH62004();
    
    // Creation of geometry:
    virtual void create(GeoPhysVol *world);
    
    //
    virtual const LArDetectorManager * getDetectorManager() const;
    // Additional creation of CaloDDE for ColdTC
    StatusCode storeDDE();
    
    
  private:  
    
    double m_cryoXpos;
    double m_tableYpos;

    void                getSimulationParameters();

    // Illegal operations:
    const LArDetectorFactoryH62004 & operator=(const LArDetectorFactoryH62004 &right);
    LArDetectorFactoryH62004(const LArDetectorFactoryH62004 &right);
    
    
    // The Detector
    StoreGateSvc             *m_detectorStore;

    // The manager:
    LArDetectorManager       *m_detectorManager;

    // CaloCell_ID also needed
    const CaloCell_ID        *m_cell_id;
    
  };
  
} // namespace LArGeo

#endif



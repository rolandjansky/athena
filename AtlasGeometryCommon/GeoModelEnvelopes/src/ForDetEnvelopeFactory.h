/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/

#ifndef ForDetEnvelopeFactory_h
#define ForDetEnvelopeFactory_h 1

#include "GeoModelKernel/GeoVDetectorFactory.h"
#include "GeoModelEnvelopes/ForDetEnvelopeManager.h"

class StoreGateSvc;
class AbsMaterialManager;
class GeoShape;

class ForDetEnvelopeFactory : public GeoVDetectorFactory  
{
 public:
  
  // Constructor:
  ForDetEnvelopeFactory(StoreGateSvc *pDetStore);
  
  // Destructor:
  ~ForDetEnvelopeFactory();
  
  // Creation of geometry:
  virtual void create(GeoPhysVol *world);
  
  // Access to the results:
  virtual const ForDetEnvelopeManager * getDetectorManager() const;
  
 private:  

  // Illegal operations:
  const ForDetEnvelopeFactory & operator=(const ForDetEnvelopeFactory &right);
  ForDetEnvelopeFactory(const ForDetEnvelopeFactory &right);
  
  // The manager:
  ForDetEnvelopeManager*   m_detectorManager;
   
  const AbsMaterialManager* m_materialManager;
  StoreGateSvc*             m_detectorStore;
};

// Class ForDetEnvelopeFactory 
#endif



/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#ifndef GEOMODELENVELOPES_FORDETENVELOPEFACTORY_H
#define GEOMODELENVELOPES_FORDETENVELOPEFACTORY_H

#include "GeoModelKernel/GeoVDetectorFactory.h"
#include "GeoModelEnvelopes/ForDetEnvelopeManager.h"

class StoreGateSvc;
class StoredMaterialManager;
class GeoShape;

class ForDetEnvelopeFactory final : public GeoVDetectorFactory  
{
 public:
  
  // Constructor:
  ForDetEnvelopeFactory(StoreGateSvc *pDetStore);
  
  // Destructor:
  ~ForDetEnvelopeFactory();
  
  // Creation of geometry:
  virtual void create(GeoPhysVol *world) override;
  
  // Access to the results:
  virtual const ForDetEnvelopeManager * getDetectorManager() const override;
  
 private:  

  // Illegal operations:
  const ForDetEnvelopeFactory & operator=(const ForDetEnvelopeFactory &right);
  ForDetEnvelopeFactory(const ForDetEnvelopeFactory &right);
  
  // The manager:
  ForDetEnvelopeManager*   m_detectorManager;
   
  const StoredMaterialManager* m_materialManager;
  StoreGateSvc*             m_detectorStore;
};

// Class ForDetEnvelopeFactory 
#endif



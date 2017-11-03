/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef ToyDetectorFactory_h
#define ToyDetectorFactory_h 1
#include "GeoModelKernel/GeoVDetectorFactory.h"
#include "GeoModelExamples/ToyDetectorManager.h"
class StoreGateSvc;
class ToyDetectorFactory : public GeoVDetectorFactory  {

 public:
  
  // Constructor:
  ToyDetectorFactory(StoreGateSvc *pDetStore);
  
  // Destructor:
  ~ToyDetectorFactory();
  
  // Creation of geometry:
  virtual void create(GeoPhysVol *world);
  
  // Access to the results:
  virtual const ToyDetectorManager * getDetectorManager() const;
  
 private:  
  
  // Illegal operations:
  const ToyDetectorFactory & operator=(const ToyDetectorFactory &right);
  ToyDetectorFactory(const ToyDetectorFactory &right);
  
  // The manager:
  ToyDetectorManager       *detectorManager;
  
  StoreGateSvc             *detectorStore;
};

// Class ToyDetectorFactory 
#endif



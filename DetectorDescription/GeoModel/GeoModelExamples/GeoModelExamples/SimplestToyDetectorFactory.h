/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#ifndef SimplestToyDetectorFactory_h
#define SimplestToyDetectorFactory_h 1
#include "GeoModelKernel/GeoVDetectorFactory.h"
#include "GeoModelExamples/ToyDetectorManager.h"
class StoreGateSvc;
class SimplestToyDetectorFactory : public GeoVDetectorFactory  {

 public:

  // Constructor:
  SimplestToyDetectorFactory(StoreGateSvc *pDetStore);

  // Destructor:
  ~SimplestToyDetectorFactory();

  // Creation of geometry:
  virtual void create(GeoPhysVol *world);

  // Access to the results:
  virtual const ToyDetectorManager * getDetectorManager() const;

 private:

  // Illegal operations:
  const SimplestToyDetectorFactory & operator=(const SimplestToyDetectorFactory &right);
  SimplestToyDetectorFactory(const SimplestToyDetectorFactory &right);

  // The manager:
  ToyDetectorManager       *m_detectorManager;

  StoreGateSvc             *m_detectorStore;
};

// Class SimplestToyDetectorFactory
#endif

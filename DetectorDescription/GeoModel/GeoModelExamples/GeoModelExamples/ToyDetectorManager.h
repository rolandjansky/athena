/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef ToyDetectorManager_h
#define ToyDetectorManager_h 1
#include "GeoModelKernel/GeoVPhysVol.h"
#include "GeoModelKernel/GeoVDetectorManager.h"

class CentralScrutinizer;
class ForwardScrutinizer;

class ToyDetectorManager : public GeoVDetectorManager  {

 public:
  enum Type {CENTRAL, FORWARD};

  // Constructor
  ToyDetectorManager();

  // Destructor
  ~ToyDetectorManager();

  // Access to raw geometry:
  virtual unsigned int getNumTreeTops() const;
  
  // Access to raw geometry:
  virtual PVConstLink getTreeTop(unsigned int i) const;

  // Access to readout geometry:
  const ForwardScrutinizer * getForwardScrutinizer(unsigned int i) const;

  // Access to readout geometry:
  const CentralScrutinizer * getCentralScrutinizer(unsigned int i) const;
  
  // Access to readout geometry:
  unsigned int getNumScrutinizers(Type type) const;


  // Add a Tree top:
  void addTreeTop(PVLink);

  // Add a Central Scrutinizer:
  void addCentralScrutinizer(const CentralScrutinizer *);

  // Add a Forward Scrutinizer:
  void addForwardScrutinizer(const ForwardScrutinizer *); 


  private:  


  const ToyDetectorManager & operator=(const ToyDetectorManager &right);
  ToyDetectorManager(const ToyDetectorManager &right);
  
  
  std::vector<const CentralScrutinizer *> centralScrutinizer;
  std::vector<const ForwardScrutinizer *> forwardScrutinizer;
  std::vector<PVLink> volume;
};

// Class ToyDetectorManager 

#ifndef GAUDI_NEUTRAL
#include "CLIDSvc/CLASS_DEF.h"
CLASS_DEF(ToyDetectorManager, 9876, 1)
#endif



#endif



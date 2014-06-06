/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef ALFA_DetectorManager_h
#define ALFA_DetectorManager_h 1



#include "GeoModelKernel/GeoVPhysVol.h"
#include "GeoModelKernel/GeoVDetectorManager.h"

#include <vector>

class ALFA_DetectorManager : public GeoVDetectorManager  
{
 public:

  // Constructor
  ALFA_DetectorManager();

  // Destructor
  ~ALFA_DetectorManager();

  // Access to raw geometry:
  virtual unsigned int getNumTreeTops() const;
  
  // Access to raw geometry:
  virtual PVConstLink getTreeTop(unsigned int i) const;

  // Add a Tree top:
  void addTreeTop(PVLink);

 private:  

  const ALFA_DetectorManager & operator=(const ALFA_DetectorManager &right);
  ALFA_DetectorManager(const ALFA_DetectorManager &right);
  
  std::vector<PVLink> m_volume;  

}; 

#ifndef GAUDI_NEUTRAL
#include "CLIDSvc/CLASS_DEF.h" 
class ALFA_DetectorManager;
CLASS_DEF(ALFA_DetectorManager, 114886926, 1)
#endif

#endif

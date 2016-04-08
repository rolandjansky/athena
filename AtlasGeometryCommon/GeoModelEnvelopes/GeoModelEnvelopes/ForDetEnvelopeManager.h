/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef ForDetEnvelopeManager_h
#define ForDetEnvelopeManager_h 1

#include "GeoModelKernel/GeoVPhysVol.h"
#include "GeoModelKernel/GeoVDetectorManager.h"

#include <vector>

class ForDetEnvelopeManager : public GeoVDetectorManager  
{
 public:

  // Constructor
  ForDetEnvelopeManager();

  // Destructor
  ~ForDetEnvelopeManager();

  // Access to raw geometry:
  virtual unsigned int getNumTreeTops() const;
  
  // Access to raw geometry:
  virtual PVConstLink getTreeTop(unsigned int i) const;

  // Add a Tree top:
  void addTreeTop(PVLink);

 private:  

  const ForDetEnvelopeManager & operator=(const ForDetEnvelopeManager &right);
  ForDetEnvelopeManager(const ForDetEnvelopeManager &right);
  
  std::vector<PVLink> m_volume;  

};

#ifndef GAUDI_NEUTRAL
#include "CLIDSvc/CLASS_DEF.h"
CLASS_DEF(ForDetEnvelopeManager, 238515740, 1)
#endif

#endif



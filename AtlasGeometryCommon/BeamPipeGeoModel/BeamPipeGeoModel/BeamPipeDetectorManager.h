/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/

#ifndef BeamPipeDetectorManager_h
#define BeamPipeDetectorManager_h 1

#include "GeoModelKernel/GeoVPhysVol.h"
#include "GeoModelKernel/GeoVDetectorManager.h"

#include <vector>

class BeamPipeDetectorManager : public GeoVDetectorManager  
{
 public:

  // Constructor
  BeamPipeDetectorManager();

  // Destructor
  ~BeamPipeDetectorManager();

  // Access to raw geometry:
  virtual unsigned int getNumTreeTops() const;
  
  // Access to raw geometry:
  virtual PVConstLink getTreeTop(unsigned int i) const;

  // Add a Tree top:
  void addTreeTop(PVConstLink);

 private:  

  const BeamPipeDetectorManager & operator=(const BeamPipeDetectorManager &right);
  BeamPipeDetectorManager(const BeamPipeDetectorManager &right);
  
  std::vector<PVConstLink> m_volume;  

};

#ifndef GAUDI_NEUTRAL
#include "AthenaKernel/CLASS_DEF.h"
CLASS_DEF(BeamPipeDetectorManager, 119561103, 1)
#endif

#endif



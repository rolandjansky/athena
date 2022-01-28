/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/

//
// This is an implemetation of the GeoVDetectorManager which
// is needed by Geo2G4 to get the treetop of the geoetry to 
// have the conversion to G4. Only the basic methods are implemented
//


#ifndef INDETSERVMATGEOMODEL_INDETSERVMATMANAGER_H
#define INDETSERVMATGEOMODEL_INDETSERVMATMANAGER_H

#include "GeoModelKernel/GeoVPhysVol.h"
#include "GeoModelKernel/GeoVDetectorManager.h"
#include "AthenaKernel/CLASS_DEF.h"


namespace InDetDD {

class InDetServMatManager : public GeoVDetectorManager  {

 public:
  
  // Constructor
  InDetServMatManager( );

  // Destructor
  ~InDetServMatManager();

  // Access to raw geometry:
  virtual unsigned int getNumTreeTops() const;
  virtual PVConstLink getTreeTop(unsigned int i) const;

  // Add a Tree top:
  void addTreeTop(PVConstLink);

 private:  
  // prevent copy and assignment
  const InDetServMatManager & operator=(const InDetServMatManager &right);
  InDetServMatManager(const InDetServMatManager &right);

  // data members
  std::vector<PVConstLink> m_volume;
};

} // namespace InDetDD

CLASS_DEF(InDetDD::InDetServMatManager, 74623874, 1)

#endif






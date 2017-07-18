/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/


#ifndef SiDetectorElementMap_h
#define SiDetectorElementMap_h

#include "InDetReadoutGeometry/SiDetectorElement.h"

#include "InDetIdentifier/PixelID.h"
#include "StoreGate/StoreGateSvc.h"

class SiDetectorElementMap {
 public:
  SiDetectorElementMap();
  ~SiDetectorElementMap();
  
  void addEntry(const IdentifierHash, const InDetDD::SiDetectorElement*);
  const InDetDD::SiDetectorElement* readEntry(const IdentifierHash) const;

 private :
  std::map<const IdentifierHash , const InDetDD::SiDetectorElement* > m_detElementMap;

};

// Class PixelDetectorFactoryFastGeo 

#include "SGTools/CLASS_DEF.h"
CLASS_DEF( SiDetectorElementMap , 73688 , 1 )

#endif

/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#ifndef INDETSERVMATGEOMODEL_INDETSERVMATFACTORYSLHC_H
#define INDETSERVMATGEOMODEL_INDETSERVMATFACTORYSLHC_H

#include "GaudiKernel/ServiceHandle.h"
#include "InDetGeoModelUtils/InDetDetectorFactoryBase.h" 
#include "InDetServMatGeoModel/InDetServMatManager.h"
#include "RDBAccessSvc/IRDBAccessSvc.h"

class InDetServMatAthenaComps;
class IRDBRecordset;

class InDetServMatFactorySLHC : public InDetDD::DetectorFactoryBase {
 public:
  
  // Constructor:
  InDetServMatFactorySLHC(InDetServMatAthenaComps * athenaComps);

  // Destructor:
  ~InDetServMatFactorySLHC();
  
  // Creation of geometry:
  virtual void create(GeoPhysVol *world);
  // manager
  virtual const InDetDD::InDetServMatManager* getDetectorManager () const;

 private:  
 
  // Illegal operations:
  const InDetServMatFactorySLHC & operator=(const InDetServMatFactorySLHC &right);
  InDetServMatFactorySLHC(const InDetServMatFactorySLHC &right);

  void fetchTables();
  double envelopeLength() const; 
  bool simpleEnvelope() const; 
  bool oldEnvelope() const; 
  unsigned int envelopeNumPlanes() const; 
  double envelopeZ(int i = 0) const; 
  double envelopeRMin(int i = 0) const; 
  double envelopeRMax(int i = 0) const; 
  
  // Database tables (we name it same as table name):
  IRDBRecordset_ptr m_InDetServGenEnvelope;

  // Need acceess to derived version
  InDetServMatAthenaComps * m_athenaComps;

  // private data
  InDetDD::InDetServMatManager *m_manager;
};

#endif //  INDETSERVMATGEOMODEL_INDETSERVMATFACTORYSLHC_H



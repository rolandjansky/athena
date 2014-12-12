/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef SCT_SLHC_GeoModel_SCT_GeneralParameters_H
#define SCT_SLHC_GeoModel_SCT_GeneralParameters_H

#include "CLHEP/Geometry/Transform3D.h"
#include "SCT_SLHC_GeoModel/SCT_ParametersBase.h"

#include <string>

class TopLevelPlacements;
class SCT_DataBase;

#include "RDBAccessSvc/IRDBAccessSvc.h"

namespace InDetDDSLHC {

class SCT_DataBase;

class SCT_GeneralParameters : public SCT_ParametersBase {

public:

  SCT_GeneralParameters(const SCT_DataBase * sctdb, const SCT_GeoModelAthenaComps * athenaComps);
  ~SCT_GeneralParameters();
  //disable copy, assign
  SCT_GeneralParameters & operator=(const SCT_GeneralParameters &) = delete;
  SCT_GeneralParameters(const SCT_GeneralParameters &) = delete;

  // General
  double safety() const;

  const HepGeom::Transform3D & partTransform(const std::string & partName) const; 
  bool partPresent(const std::string & partName) const;
 
  double envelopeRMin() const; 
  double envelopeRMax() const; 
  double envelopeLength() const; 
  bool simpleEnvelope() const; 
  bool booleanEnvelope() const; 
  unsigned int envelopeNumPlanes() const; 
  double envelopeZ(int i) const; 
  double envelopeRMin(int i) const; 
  double envelopeRMax(int i) const; 
 
private:

  TopLevelPlacements * m_placements;
  IRDBRecordset_ptr SctEnvelope;

};
}

#endif // SCT_GeoModel_SCT_GeneralParameters_H

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef GeoDBUtils_h_
#define GeoDBUtils_h_
#include "RDBAccessSvc/IRDBAccessSvc.h"
#include "RDBAccessSvc/IRDBRecordset.h"
#include "RDBAccessSvc/IRDBRecord.h"
#include <string>
#include "GeoModelKernel/GeoDefinitions.h" 
#include "GaudiKernel/SystemOfUnits.h"


class GeoDBUtils {

 public:

  inline static const IRDBRecord *getTransformRecord(IRDBRecordset_ptr positionRecSet, const std::string & key) {
    for (unsigned int s=0;s<positionRecSet->size(); s++) {
      const IRDBRecord *currentRec= (*positionRecSet)[s];
      if (key == currentRec->getString("NAME")) {
	return currentRec;
      }
    }
    return NULL;
  }
  
  inline static GeoTrf::Transform3D getTransform (const IRDBRecord *currentRec) {
    double x             = currentRec->getDouble("TRANSX")*Gaudi::Units::mm;
    double y             = currentRec->getDouble("TRANSY")*Gaudi::Units::mm;
    double z             = currentRec->getDouble("TRANSZ")*Gaudi::Units::mm;
    double theta         = currentRec->getDouble("THETA")*Gaudi::Units::rad;
    double phi           = currentRec->getDouble("PHI")*Gaudi::Units::rad;
    double rotationAngle = currentRec->getDouble("ROTATIONANGLE")*Gaudi::Units::rad;
    
    GeoTrf::Vector3D axis(sin(theta)*cos(phi), sin(theta)*sin(phi),cos(theta));
    return GeoTrf::Translate3D(x,y,z)*GeoTrf::Rotate3D(rotationAngle,axis);
  } 

};
#endif

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef GeoDBUtils_h_
#define GeoDBUtils_h_
#include "RDBAccessSvc/IRDBAccessSvc.h"
#include "RDBAccessSvc/IRDBRecordset.h"
#include "RDBAccessSvc/IRDBRecord.h"
#include <string>
#include "CLHEP/Geometry/Transform3D.h" 

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
  
  inline static HepGeom::Transform3D getTransform (const IRDBRecord *currentRec) {
    double x             = currentRec->getDouble("TRANSX")*CLHEP::mm;
    double y             = currentRec->getDouble("TRANSY")*CLHEP::mm;
    double z             = currentRec->getDouble("TRANSZ")*CLHEP::mm;
    double theta         = currentRec->getDouble("THETA")*CLHEP::rad;
    double phi           = currentRec->getDouble("PHI")*CLHEP::rad;
    double rotationAngle = currentRec->getDouble("ROTATIONANGLE")*CLHEP::rad;
    
    CLHEP::Hep3Vector axis(sin(theta)*cos(phi), sin(theta)*sin(phi),cos(theta));
    return HepGeom::Translate3D(x,y,z)*HepGeom::Rotate3D(rotationAngle,axis);
  } 

};
#endif

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef GeoModelDetectorFacility_H
#define GeoModelDetectorFacility_H

#include "FadsGeometry/DetectorFacility.h"
#include "FadsUtilities/Tokenizer.h"
#include <iostream>

#include "Geo2G4/Geo2G4SvcAccessor.h"
#include "Geo2G4/Geo2G4SvcBase.h"
#include "Geo2G4/VolumeBuilder.h"

#include "SimHelpers/DetectorGeometryHelper.h"

class GeoModelDetectorFacility: public FADS::DetectorFacility {
public:
  GeoModelDetectorFacility(std::string detName): DetectorFacility(detName),blParamOn(false),blGetTopTransform(true)
  {
    FADS::Tokenizer t(":",detName);
    if (t.size()>1) GeoDetectorName=t[1];
    else
      {
        std::cout << "GeoModelDetectorFacility Warning! could not resolve the "<<
          " GeoModel detector name: detName= "<<detName<<std::endl;
        GeoDetectorName=detName;
      }
    DetectorGeometryHelper dg(t[0],GeoModel);

    Geo2G4SvcAccessor accessor;
    Geo2G4SvcBase *g=accessor.GetGeo2G4Svc();
    builderName=g->GetDefaultBuilder()->GetKey();
    blGetTopTransform = g->UseTopTransforms();
  }
  void BuildGeometry();
protected:
  std::string builderName;
  std::string GeoDetectorName;
  bool blParamOn;
  bool blGetTopTransform;
};
#endif


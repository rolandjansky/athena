/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// RALEmec
// Sep 5 JFB

// Fetch parameters from a NOVA database structure.

#include "LArGeoRAL/RALEmec.h"
#include "LArGeoCode/VDetectorParameters.h"

// Athena Service classes
#include "GaudiKernel/Bootstrap.h"
#include "GaudiKernel/ISvcLocator.h"
#include "StoreGate/StoreGateSvc.h"

#include "CLHEP/Units/PhysicalConstants.h"

#include "RDBAccessSvc/IRDBAccessSvc.h"
#include "RDBAccessSvc/IRDBRecord.h"
#include "RDBAccessSvc/IRDBRecordset.h"
#include "GeoModelInterfaces/IGeoModelSvc.h"
#include <iostream>
#include <string>
#include <stdlib.h>
#include <stdexcept>

class LArGeo::RALEmec::Clockwork {

public:

  Clockwork(){}
  ~Clockwork(){}
  

  IRDBRecordset_ptr emecCylLayer;
  IRDBRecordset_ptr emecGeometry;
  IRDBRecordset_ptr emecRapRegion;
  IRDBRecordset_ptr emecSamplingSep;
  IRDBRecordset_ptr emecWheelParameters;
  IRDBRecordset_ptr emecWhlOptParm;
  IRDBRecordset_ptr presamplerPosition;
};


LArGeo::RALEmec::RALEmec():
  c(new Clockwork())
{
  // First, fetch the Athena services.
  ISvcLocator* svcLocator = Gaudi::svcLocator(); // from Bootstrap.h


  StatusCode sc;
  IRDBAccessSvc *pAccessSvc;
  sc = svcLocator->service("RDBAccessSvc",pAccessSvc);
  if (sc != StatusCode::SUCCESS) {
    throw std::runtime_error ("Cannot locate RDBAccessSvc!!");
  }




  IGeoModelSvc *geoModel;
  sc = svcLocator->service ("GeoModelSvc",geoModel);
  if (sc != StatusCode::SUCCESS) {
    throw std::runtime_error ("Cannot locate GeoModelSvc!!");
  }

  
  std::string AtlasVersion = geoModel->atlasVersion();
  std::string LArVersion = geoModel->LAr_VersionOverride();

  std::string detectorKey  = LArVersion.empty() ? AtlasVersion : LArVersion;
  std::string detectorNode = LArVersion.empty() ? "ATLAS" : "LAr";



  c->emecCylLayer             = pAccessSvc->getRecordsetPtr("EmecCylLayer",detectorKey, detectorNode); 
  c->emecGeometry             = pAccessSvc->getRecordsetPtr("EmecGeometry",detectorKey, detectorNode); 
  c->emecRapRegion            = pAccessSvc->getRecordsetPtr("EmecRapRegion",detectorKey, detectorNode); 
  c->emecSamplingSep          = pAccessSvc->getRecordsetPtr("EmecSamplingSep",detectorKey, detectorNode); 
  c->emecWheelParameters      = pAccessSvc->getRecordsetPtr("EmecWheelParameters",detectorKey, detectorNode); 
  c->emecWhlOptParm           = pAccessSvc->getRecordsetPtr("EmecWhlOptParm",detectorKey, detectorNode); 
  c->presamplerPosition       = pAccessSvc->getRecordsetPtr("PresamplerPosition",AtlasVersion, "ATLAS"); 

}


LArGeo::RALEmec::~RALEmec()
{
  delete c;
}


double LArGeo::RALEmec::GetValue(const std::string& a_name,
                                 const int a0,
                                 const int a1,
                                 const int /*a2*/,
                                 const int /*a3*/,
                                 const int /*a4*/ ) 
{
  
  std::ostringstream A0STR;
  A0STR << "_" << a0;
  const std::string A0 = A0STR.str();

  std::ostringstream A1STR;
  A1STR << "_" << a1;
  const std::string A1 = A1STR.str();




  if ( a_name == "LArEMECZshift"                 ) return (*c->emecGeometry)[0]->getDouble("ZSHIFT")*CLHEP::cm;
  if ( a_name == "LArEMECRadiusMin"              ) return (*c->emecGeometry)[0]->getDouble("RMIN")*CLHEP::cm;
  if ( a_name == "LArEMECRadiusMax"              ) return (*c->emecGeometry)[0]->getDouble("RMAX")*CLHEP::cm;

  if ( a_name == "LArEMECAverageGap"             ) return (*c->emecGeometry)[0]->getDouble("GAP0")*CLHEP::cm;
  if ( a_name == "LArEMECStartAbsLength"         ) return (*c->emecGeometry)[0]->getDouble("SABL")*CLHEP::cm;
  if ( a_name == "LArEMECRefPoint"               ) return (*c->emecGeometry)[0]->getDouble("Z0")*CLHEP::cm;
  if ( a_name == "LArEMECNomLarOrig"             ) return (*c->emecGeometry)[0]->getDouble("Z1")*CLHEP::cm;
  if ( a_name == "LArEMECMaxRadiusActivePart"    ) return (*c->emecGeometry)[0]->getDouble("RLIMIT")*CLHEP::cm;
  if ( a_name == "LArEMECNumberRadialLayer"      ) return (*c->emecGeometry)[0]->getInt("NLAYER");

  if ( a_name == "LArEMECLArThickness"           ) return (*c->emecGeometry)[0]->getDouble("ETOT")*CLHEP::cm;
  if ( a_name == "LArEMECKaptonThickness"        ) return (*c->emecGeometry)[0]->getDouble("EKAP")*CLHEP::cm;
  if ( a_name == "LArEMECCopperThickness"        ) return (*c->emecGeometry)[0]->getDouble("ECU")*CLHEP::cm;

  if ( a_name == "LArEMECHalfCrack"              ) return (*c->emecGeometry)[0]->getDouble("DCRACK")*CLHEP::cm;

  if ( a_name == "LArEMECRegionEtaGranularity"   ) return (*c->emecGeometry)[0]->getDouble("DETA"+A0);
  if ( a_name == "LArEMECRegionPhiGranularity"   ) return (*c->emecGeometry)[0]->getDouble("DPHI"+A0);


  if ( a_name == "LArEMECLayerInnerRadius"       ) return (*c->emecCylLayer)[a0]->getDouble("RADIL")*CLHEP::cm;
  if ( a_name == "LArEMECLayerOuterRadius"       ) return (*c->emecCylLayer)[a0]->getDouble("RADOL")*CLHEP::cm;
  if ( a_name == "LArEMECLayerInnerAngle"        ) return (*c->emecCylLayer)[a0]->getDouble("ALFIL")*CLHEP::rad;
  if ( a_name == "LArEMECLayerOuterAngle"        ) return (*c->emecCylLayer)[a0]->getDouble("ALFOL")*CLHEP::rad;
  if ( a_name == "LArEMECAbsorberInnerThickness" ) return (*c->emecCylLayer)[a0]->getDouble("TCKIL")*CLHEP::cm;
  if ( a_name == "LArEMECAbsorberOuterThickness" ) return (*c->emecCylLayer)[a0]->getDouble("TCKOL")*CLHEP::cm;
  if ( a_name == "LArEMECRegionLowEta"           ) return (*c->emecRapRegion)[a0]->getDouble("ETA1");
  if ( a_name == "LArEMECRegionHighEta"          ) return (*c->emecRapRegion)[a0]->getDouble("ETA2");
  if ( a_name == "LArEMECRegionLowEtaDepth"      ) return (*c->emecRapRegion)[a0]->getDouble("ZMN"+A1)*CLHEP::cm;
  if ( a_name == "LArEMECRegionHighEtaDepth"     ) return (*c->emecRapRegion)[a0]->getDouble("ZMX"+A1)*CLHEP::cm;

  if ( a_name == "LArEMECLengthSeparationArrays" ) return (*c->emecSamplingSep)[0]->getInt("NSEP"+A0);
  if ( a_name == "LArEMECInnerWheelSeparation"   ) return (*c->emecSamplingSep)[0]->getDouble("ZIW"+A0)*CLHEP::cm;
  if ( a_name == "LArEMECZSeparation12"          ) return (*c->emecSamplingSep)[0]->getDouble("ZSEP12"+A0)*CLHEP::cm;
  if ( a_name == "LArEMECZSeparation23"          ) return (*c->emecSamplingSep)[0]->getDouble("ZSEP23"+A0)*CLHEP::cm;

  // In this case (since the number of wheels isn't likely to change), 
  // hard-code the inner versus the outer wheel.
  if ( a_name == "LArEMECInnerNumberAbsorbers"   ) return (*c->emecWheelParameters)[0]->getInt("NABS");
  if ( a_name == "LArEMECInnerNumberWaves"       ) return (*c->emecWheelParameters)[0]->getInt("NACC");
  if ( a_name == "LArEMECInnerMaxEta"            ) return (*c->emecWheelParameters)[0]->getDouble("ETAINT");
  if ( a_name == "LArEMECInnerMinEta"            ) return (*c->emecWheelParameters)[0]->getDouble("ETAEXT");
  if ( a_name == "LArEMECOuterNumberAbsorbers"   ) return (*c->emecWheelParameters)[1]->getInt("NABS");
  if ( a_name == "LArEMECOuterNumberWaves"       ) return (*c->emecWheelParameters)[1]->getInt("NACC");
  if ( a_name == "LArEMECOuterMaxEta"            ) return (*c->emecWheelParameters)[1]->getDouble("ETAINT");
  if ( a_name == "LArEMECOuterMinEta"            ) return (*c->emecWheelParameters)[1]->getDouble("ETAEXT");

  if ( a_name == "LArEMECInnerBeta"              ) return (*c->emecWhlOptParm)[0]->getDouble("BETX");
  if ( a_name == "LArEMECInnerLowerAngle"        ) return (*c->emecWhlOptParm)[0]->getDouble("ALFINT")*CLHEP::rad;
  if ( a_name == "LArEMECInnerUpperAngle"        ) return (*c->emecWhlOptParm)[0]->getDouble("ALFEXT")*CLHEP::rad;
  if ( a_name == "LArEMECInnerLeadThickness"     ) return (*c->emecWhlOptParm)[0]->getDouble("PBTH")*CLHEP::cm;
  if ( a_name == "LArEMECOuterBeta"              ) return (*c->emecWhlOptParm)[1]->getDouble("BETX");
  if ( a_name == "LArEMECOuterLowerAngle"        ) return (*c->emecWhlOptParm)[1]->getDouble("ALFINT")*CLHEP::rad;
  if ( a_name == "LArEMECOuterUpperAngle"        ) return (*c->emecWhlOptParm)[1]->getDouble("ALFEXT")*CLHEP::rad;
  if ( a_name == "LArEMECOuterLeadThickness"     ) return (*c->emecWhlOptParm)[1]->getDouble("PBTH")*CLHEP::cm;
 
  if ( a_name == "LArEMECPreNomPos"              ) return (*c->presamplerPosition)[0]->getDouble("ZPOS")*CLHEP::cm;
  if ( a_name == "LArEMECPreThickness"           ) return (*c->presamplerPosition)[0]->getDouble("TCK")*CLHEP::cm;  
  if ( a_name == "LArEMECPreMinRadius"           ) return (*c->presamplerPosition)[0]->getDouble("RMIN")*CLHEP::cm;
  if ( a_name == "LArEMECPreMaxRadius"           ) return (*c->presamplerPosition)[0]->getDouble("RMAX")*CLHEP::cm;




  // We didn't find a match.
  std::string errMessage = "RALEmec::GetValue: could not find a match for the key '" + a_name;
  std::cerr << errMessage << std::endl;
  throw std::runtime_error (errMessage.c_str());

  // This is a dummy statement to prevent a compiler warning. 
  return 0.;
}

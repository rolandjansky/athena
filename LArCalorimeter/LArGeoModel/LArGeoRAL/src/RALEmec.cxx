/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
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
#include "GeoModelInterfaces/IGeoDbTagSvc.h"

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
  m_c(new Clockwork())
{
  // First, fetch the Athena services.
  ISvcLocator* svcLocator = Gaudi::svcLocator();

  IGeoDbTagSvc* geoDbTagSvc{nullptr};
  StatusCode sc = svcLocator->service("GeoDbTagSvc",geoDbTagSvc);
  if (sc != StatusCode::SUCCESS) {
    throw std::runtime_error ("Cannot locate GeoDBTagSvc");
  }
  
  IRDBAccessSvc* pAccessSvc{nullptr};
  sc = svcLocator->service(geoDbTagSvc->getParamSvcName(),pAccessSvc);
  if (sc != StatusCode::SUCCESS) {
    throw std::runtime_error ("Cannot locate " + geoDbTagSvc->getParamSvcName());
  }

  std::string detectorKey;
  std::string detectorNode;
  std::string AtlasVersion;

  if(geoDbTagSvc->getSqliteReader()==nullptr) {
    // The geometry DB is used
    IGeoModelSvc* geoModel{nullptr};
    sc = svcLocator->service ("GeoModelSvc",geoModel);
    if (sc != StatusCode::SUCCESS) {
      throw std::runtime_error ("Cannot locate GeoModelSvc");
    }

    AtlasVersion = geoModel->atlasVersion();
    std::string LArVersion = geoModel->LAr_VersionOverride();

    detectorKey  = LArVersion.empty() ? AtlasVersion : LArVersion;
    detectorNode = LArVersion.empty() ? "ATLAS" : "LAr";
  }

  m_c->emecCylLayer             = pAccessSvc->getRecordsetPtr("EmecCylLayer",detectorKey, detectorNode); 
  m_c->emecGeometry             = pAccessSvc->getRecordsetPtr("EmecGeometry",detectorKey, detectorNode); 
  m_c->emecRapRegion            = pAccessSvc->getRecordsetPtr("EmecRapRegion",detectorKey, detectorNode); 
  m_c->emecSamplingSep          = pAccessSvc->getRecordsetPtr("EmecSamplingSep",detectorKey, detectorNode); 
  m_c->emecWheelParameters      = pAccessSvc->getRecordsetPtr("EmecWheelParameters",detectorKey, detectorNode); 
  m_c->emecWhlOptParm           = pAccessSvc->getRecordsetPtr("EmecWhlOptParm",detectorKey, detectorNode); 
  m_c->presamplerPosition       = pAccessSvc->getRecordsetPtr("PresamplerPosition",AtlasVersion, "ATLAS"); 

}


LArGeo::RALEmec::~RALEmec()
{
  delete m_c;
}


double LArGeo::RALEmec::GetValue(const std::string& a_name,
                                 const int a0,
                                 const int a1,
                                 const int /*a2*/,
                                 const int /*a3*/,
                                 const int /*a4*/ ) const
{
  
  std::ostringstream A0STR;
  A0STR << "_" << a0;
  const std::string A0 = A0STR.str();

  std::ostringstream A1STR;
  A1STR << "_" << a1;
  const std::string A1 = A1STR.str();




  if ( a_name == "LArEMECZshift"                 ) return (*m_c->emecGeometry)[0]->getDouble("ZSHIFT")*CLHEP::cm;
  if ( a_name == "LArEMECRadiusMin"              ) return (*m_c->emecGeometry)[0]->getDouble("RMIN")*CLHEP::cm;
  if ( a_name == "LArEMECRadiusMax"              ) return (*m_c->emecGeometry)[0]->getDouble("RMAX")*CLHEP::cm;

  if ( a_name == "LArEMECAverageGap"             ) return (*m_c->emecGeometry)[0]->getDouble("GAP0")*CLHEP::cm;
  if ( a_name == "LArEMECStartAbsLength"         ) return (*m_c->emecGeometry)[0]->getDouble("SABL")*CLHEP::cm;
  if ( a_name == "LArEMECRefPoint"               ) return (*m_c->emecGeometry)[0]->getDouble("Z0")*CLHEP::cm;
  if ( a_name == "LArEMECNomLarOrig"             ) return (*m_c->emecGeometry)[0]->getDouble("Z1")*CLHEP::cm;
  if ( a_name == "LArEMECMaxRadiusActivePart"    ) return (*m_c->emecGeometry)[0]->getDouble("RLIMIT")*CLHEP::cm;
  if ( a_name == "LArEMECNumberRadialLayer"      ) return (*m_c->emecGeometry)[0]->getInt("NLAYER");

  if ( a_name == "LArEMECLArThickness"           ) return (*m_c->emecGeometry)[0]->getDouble("ETOT")*CLHEP::cm;
  if ( a_name == "LArEMECKaptonThickness"        ) return (*m_c->emecGeometry)[0]->getDouble("EKAP")*CLHEP::cm;
  if ( a_name == "LArEMECCopperThickness"        ) return (*m_c->emecGeometry)[0]->getDouble("ECU")*CLHEP::cm;

  if ( a_name == "LArEMECHalfCrack"              ) return (*m_c->emecGeometry)[0]->getDouble("DCRACK")*CLHEP::cm;

  if ( a_name == "LArEMECRegionEtaGranularity"   ) return (*m_c->emecGeometry)[0]->getDouble("DETA"+A0);
  if ( a_name == "LArEMECRegionPhiGranularity"   ) return (*m_c->emecGeometry)[0]->getDouble("DPHI"+A0);


  if ( a_name == "LArEMECLayerInnerRadius"       ) return (*m_c->emecCylLayer)[a0]->getDouble("RADIL")*CLHEP::cm;
  if ( a_name == "LArEMECLayerOuterRadius"       ) return (*m_c->emecCylLayer)[a0]->getDouble("RADOL")*CLHEP::cm;
  if ( a_name == "LArEMECLayerInnerAngle"        ) return (*m_c->emecCylLayer)[a0]->getDouble("ALFIL")*CLHEP::rad;
  if ( a_name == "LArEMECLayerOuterAngle"        ) return (*m_c->emecCylLayer)[a0]->getDouble("ALFOL")*CLHEP::rad;
  if ( a_name == "LArEMECAbsorberInnerThickness" ) return (*m_c->emecCylLayer)[a0]->getDouble("TCKIL")*CLHEP::cm;
  if ( a_name == "LArEMECAbsorberOuterThickness" ) return (*m_c->emecCylLayer)[a0]->getDouble("TCKOL")*CLHEP::cm;
  if ( a_name == "LArEMECRegionLowEta"           ) return (*m_c->emecRapRegion)[a0]->getDouble("ETA1");
  if ( a_name == "LArEMECRegionHighEta"          ) return (*m_c->emecRapRegion)[a0]->getDouble("ETA2");
  if ( a_name == "LArEMECRegionLowEtaDepth"      ) return (*m_c->emecRapRegion)[a0]->getDouble("ZMN"+A1)*CLHEP::cm;
  if ( a_name == "LArEMECRegionHighEtaDepth"     ) return (*m_c->emecRapRegion)[a0]->getDouble("ZMX"+A1)*CLHEP::cm;

  if ( a_name == "LArEMECLengthSeparationArrays" ) return (*m_c->emecSamplingSep)[0]->getInt("NSEP"+A0);
  if ( a_name == "LArEMECInnerWheelSeparation"   ) return (*m_c->emecSamplingSep)[0]->getDouble("ZIW"+A0)*CLHEP::cm;
  if ( a_name == "LArEMECZSeparation12"          ) return (*m_c->emecSamplingSep)[0]->getDouble("ZSEP12"+A0)*CLHEP::cm;
  if ( a_name == "LArEMECZSeparation23"          ) return (*m_c->emecSamplingSep)[0]->getDouble("ZSEP23"+A0)*CLHEP::cm;

  // In this case (since the number of wheels isn't likely to change), 
  // hard-code the inner versus the outer wheel.
  if ( a_name == "LArEMECInnerNumberAbsorbers"   ) return (*m_c->emecWheelParameters)[0]->getInt("NABS");
  if ( a_name == "LArEMECInnerNumberWaves"       ) return (*m_c->emecWheelParameters)[0]->getInt("NACC");
  if ( a_name == "LArEMECInnerMaxEta"            ) return (*m_c->emecWheelParameters)[0]->getDouble("ETAINT");
  if ( a_name == "LArEMECInnerMinEta"            ) return (*m_c->emecWheelParameters)[0]->getDouble("ETAEXT");
  if ( a_name == "LArEMECOuterNumberAbsorbers"   ) return (*m_c->emecWheelParameters)[1]->getInt("NABS");
  if ( a_name == "LArEMECOuterNumberWaves"       ) return (*m_c->emecWheelParameters)[1]->getInt("NACC");
  if ( a_name == "LArEMECOuterMaxEta"            ) return (*m_c->emecWheelParameters)[1]->getDouble("ETAINT");
  if ( a_name == "LArEMECOuterMinEta"            ) return (*m_c->emecWheelParameters)[1]->getDouble("ETAEXT");

  if ( a_name == "LArEMECInnerBeta"              ) return (*m_c->emecWhlOptParm)[0]->getDouble("BETX");
  if ( a_name == "LArEMECInnerLowerAngle"        ) return (*m_c->emecWhlOptParm)[0]->getDouble("ALFINT")*CLHEP::rad;
  if ( a_name == "LArEMECInnerUpperAngle"        ) return (*m_c->emecWhlOptParm)[0]->getDouble("ALFEXT")*CLHEP::rad;
  if ( a_name == "LArEMECInnerLeadThickness"     ) return (*m_c->emecWhlOptParm)[0]->getDouble("PBTH")*CLHEP::cm;
  if ( a_name == "LArEMECOuterBeta"              ) return (*m_c->emecWhlOptParm)[1]->getDouble("BETX");
  if ( a_name == "LArEMECOuterLowerAngle"        ) return (*m_c->emecWhlOptParm)[1]->getDouble("ALFINT")*CLHEP::rad;
  if ( a_name == "LArEMECOuterUpperAngle"        ) return (*m_c->emecWhlOptParm)[1]->getDouble("ALFEXT")*CLHEP::rad;
  if ( a_name == "LArEMECOuterLeadThickness"     ) return (*m_c->emecWhlOptParm)[1]->getDouble("PBTH")*CLHEP::cm;
 
  if ( a_name == "LArEMECPreNomPos"              ) return (*m_c->presamplerPosition)[0]->getDouble("ZPOS")*CLHEP::cm;
  if ( a_name == "LArEMECPreThickness"           ) return (*m_c->presamplerPosition)[0]->getDouble("TCK")*CLHEP::cm;  
  if ( a_name == "LArEMECPreMinRadius"           ) return (*m_c->presamplerPosition)[0]->getDouble("RMIN")*CLHEP::cm;
  if ( a_name == "LArEMECPreMaxRadius"           ) return (*m_c->presamplerPosition)[0]->getDouble("RMAX")*CLHEP::cm;




  // We didn't find a match.
  std::string errMessage = "RALEmec::GetValue: could not find a match for the key '" + a_name;
  std::cerr << errMessage << std::endl;
  throw std::runtime_error (errMessage.c_str());

  // Unreached
}

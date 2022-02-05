/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

// RALHec
// Sep 5 JFB

// Fetch parameters from a NOVA database structure.

#include "LArGeoRAL/RALHec.h"
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

class LArGeo::RALHec::Clockwork {

public:

  Clockwork() {}
  ~Clockwork() {}
  
  IRDBRecordset_ptr hadronicEndcap;
  IRDBRecordset_ptr hecGridValues;
  IRDBRecordset_ptr hecLongitudinalBlock;
};


LArGeo::RALHec::RALHec():
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

  if(geoDbTagSvc->getSqliteReader()==nullptr) {
    // The geometry DB is used
    IGeoModelSvc* geoModel{nullptr};
    sc = svcLocator->service ("GeoModelSvc",geoModel);
    if (sc != StatusCode::SUCCESS) {
      throw std::runtime_error ("Cannot locate GeoModelSvc");
    }

    std::string AtlasVersion = geoModel->atlasVersion();
    std::string LArVersion = geoModel->LAr_VersionOverride();

    detectorKey  = LArVersion.empty() ? AtlasVersion : LArVersion;
    detectorNode = LArVersion.empty() ? "ATLAS" : "LAr";
  }


  m_c->hadronicEndcap             = pAccessSvc->getRecordsetPtr("HadronicEndcap",detectorKey, detectorNode); 
  m_c->hecGridValues              = pAccessSvc->getRecordsetPtr("HecGridValues",detectorKey, detectorNode); 
  m_c->hecLongitudinalBlock       = pAccessSvc->getRecordsetPtr("HecLongitudinalBlock",detectorKey, detectorNode); 

}


LArGeo::RALHec::~RALHec()
{
  delete m_c;
}


double LArGeo::RALHec::GetValue(const std::string& a_name,
                                const int a0,
                                const int /*a1*/,
                                const int /*a2*/,
                                const int /*a3*/,
                                const int /*a4*/ ) const
{
  
  std::ostringstream A0STR;
  A0STR << "_" << a0;
  const std::string A0 = A0STR.str();


  if ( a_name == "LArHECmoduleNumber"  ) return (*m_c->hadronicEndcap)[0]->getInt("NSCT");
  if ( a_name == "LArHECzStart"        ) return (*m_c->hadronicEndcap)[0]->getDouble("ZSTART")*CLHEP::cm;
  if ( a_name == "LArHECgapSize"       ) return (*m_c->hadronicEndcap)[0]->getDouble("LARG")*CLHEP::cm;
  if ( a_name == "LArHECbetweenWheel"  ) return (*m_c->hadronicEndcap)[0]->getDouble("GAPWHL")*CLHEP::cm;
  if ( a_name == "LArHECcopperPad"     ) return (*m_c->hadronicEndcap)[0]->getDouble("COPPER")*CLHEP::cm;
  if ( a_name == "LArHECplateWidth0"   ) return (*m_c->hadronicEndcap)[0]->getDouble("PLATE_0")*CLHEP::cm;
  if ( a_name == "LArHECplateWidth1"   ) return (*m_c->hadronicEndcap)[0]->getDouble("PLATE_1")*CLHEP::cm;
  if ( a_name == "LArHECkaptonPosition") return (*m_c->hadronicEndcap)[0]->getDouble("KPTPOS"+A0)*CLHEP::cm;
  if ( a_name == "LArHECkaptonWidth"   ) return (*m_c->hadronicEndcap)[0]->getDouble("KPTWID"+A0)*CLHEP::cm;
  if ( a_name == "LArHECrodPosY"       ) return (*m_c->hadronicEndcap)[0]->getDouble("RODPOSR"+A0)*CLHEP::cm;
  if ( a_name == "LArHECrodPosX"       ) return (*m_c->hadronicEndcap)[0]->getDouble("RODPOSX"+A0)*CLHEP::cm;
  if ( a_name == "LArHECrodDim"        ) return (*m_c->hadronicEndcap)[0]->getDouble("RODDIM"+A0)*CLHEP::cm;
  if ( a_name == "LArHECspcDim"        ) return (*m_c->hadronicEndcap)[0]->getDouble("SPCDIM"+A0)*CLHEP::cm;

  if ( a_name == "LArHECsliceIndex"    ) return (*m_c->hecLongitudinalBlock)[a0]->getDouble("IBLC");
  if ( a_name == "LArHECmoduleRinner1" ) return (*m_c->hecLongitudinalBlock)[0]->getDouble("BLRMN")*CLHEP::cm;
  if ( a_name == "LArHECmoduleRinner2" ) return (*m_c->hecLongitudinalBlock)[1]->getDouble("BLRMN")*CLHEP::cm;
  if ( a_name == "LArHECmoduleRouter"  ) return (*m_c->hecLongitudinalBlock)[0]->getDouble("BLRMX")*CLHEP::cm;
  if ( a_name == "LArHECdepthZ"        ) return (*m_c->hecLongitudinalBlock)[a0]->getDouble("BLDPTH")*CLHEP::cm;
  if ( a_name == "LArHECfirstAbsorber" ) return (*m_c->hecLongitudinalBlock)[a0]->getDouble("PLATE0")*CLHEP::cm;
  if ( a_name == "LArHECgapNumber"     ) return (*m_c->hecLongitudinalBlock)[a0]->getDouble("BLMOD");

  // We didn't find a match.  
  std::string errMessage = "RALHec::GetValue: could not find a match for the key '" + a_name;
  std::cerr << errMessage << std::endl;
  throw std::runtime_error (errMessage.c_str());

  // Unreached.
}

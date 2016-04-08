/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// RALExperimentalHall
// Sep 5 JFB

// Fetch parameters from a NOVA database structure.

#include "LArGeoRAL/RALExperimentalHall.h"
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

class LArGeo::RALExperimentalHall::Clockwork {

public:

  Clockwork(){}
  ~Clockwork(){}
  
  IRDBRecordset_ptr atlasMother;

};


LArGeo::RALExperimentalHall::RALExperimentalHall():
  m_c(new Clockwork())
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
  m_c->atlasMother             = pAccessSvc->getRecordsetPtr("AtlasMother",AtlasVersion, "ATLAS"); 
}


LArGeo::RALExperimentalHall::~RALExperimentalHall()
{
  delete m_c;
}


double LArGeo::RALExperimentalHall::GetValue(const std::string& a_name,
                                             const int /*a0*/,
                                             const int /*a1*/,
                                             const int /*a2*/,
                                             const int /*a3*/,
                                             const int /*a4*/ ) 
{
  
  if ( a_name == "LArExpHallOutOfTimeCut"     ) return 2.5*CLHEP::ns;
  if ( a_name == "LArExpHallInnerRadius"      ) return (*m_c->atlasMother)[1]->getDouble("RMIN")*CLHEP::cm;
  if ( a_name == "LArExpHallOuterRadius"      ) return (*m_c->atlasMother)[1]->getDouble("RMAX")*CLHEP::cm;
  if ( a_name == "LArExpHallZmax"             ) return (*m_c->atlasMother)[1]->getDouble("ZMAX")*CLHEP::cm;

  // We didn't find a match.
  std::string errMessage = "RALExperimentalHall::GetValue: could not find a match for the key '" + a_name;
  std::cerr << errMessage << std::endl;
  throw std::runtime_error (errMessage.c_str());

  // Unreached
}

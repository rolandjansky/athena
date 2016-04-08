/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// LArHECCalculator
// Revision history:

// 31-May-2002 : Karapetian Gaiane

// 13-Jan-2004 WGS: Now uses LArG4::HEC::Geometry for the Identifier calculations.

#undef DEBUG_HITS

#include "LArG4HEC/LArHECCalculator.h"
#include "LArG4HEC/Geometry.h"

#include "LArG4Code/LArG4Identifier.h"

#include "G4ThreeVector.hh"
#include "G4StepPoint.hh"
#include "G4Step.hh"

#include "RDBAccessSvc/IRDBAccessSvc.h"
#include "RDBAccessSvc/IRDBRecord.h"
#include "RDBAccessSvc/IRDBRecordset.h"
#include "GeoModelInterfaces/IGeoModelSvc.h"
#include "GaudiKernel/ISvcLocator.h"
#include "GaudiKernel/Bootstrap.h"
#include "StoreGate/StoreGateSvc.h"
// Standard implementation of a singleton pattern.

LArHECCalculator* LArHECCalculator::m_instance = 0;

LArHECCalculator* LArHECCalculator::GetCalculator()
{
  if (m_instance == 0) 
    {
      m_instance = new LArHECCalculator();
    }
  return m_instance;
}


LArHECCalculator::LArHECCalculator()
{


  StatusCode status;
  // Access the GeoModelSvc:
  ISvcLocator *svcLocator = Gaudi::svcLocator();
  IGeoModelSvc *geoModel;
  status = svcLocator->service ("GeoModelSvc",geoModel);
  if (status != StatusCode::SUCCESS) {
    throw std::runtime_error ("Cannot locate GeoModelSvc!!");
  }
  
  // Access the geometry database:
  IRDBAccessSvc *pAccessSvc;
  status=svcLocator->service("RDBAccessSvc",pAccessSvc);
  if (status != StatusCode::SUCCESS) {
    throw std::runtime_error ("Cannot locate RDBAccessSvc!!");
  }
  
  // Obtain the geometry version information:
  
  std::string AtlasVersion = geoModel->atlasVersion();
  std::string LArVersion = geoModel->LAr_VersionOverride();
  
  std::string detectorKey  = LArVersion.empty() ? AtlasVersion : LArVersion;
  std::string detectorNode = LArVersion.empty() ? "ATLAS" : "LAr";
  
  
  IRDBRecordset *emecGeometry             = const_cast<IRDBRecordset*> (pAccessSvc->getRecordset("EmecGeometry",detectorKey, detectorNode)); 
   if (emecGeometry->size()==0) {
     throw std::runtime_error("Cannot find the EmecGeometry Table");
   }

  // Constructor initializes the geometry.

  // Make sure we don't have any undefined values.
  //m_identifier = LArG4Identifier();
  //m_time = 0.;
  //m_energy = 0.;
  m_isInTime = false;

  // Initialize the geometry calculator.
  m_geometry = LArG4::HEC::Geometry::GetInstance();

  // By default, assume no charge-collection.
  m_chargeCollection = false;

  // Get the endcap shift and out-of-time cut from the detector
  // description routine.
  //get the endcap shift
  m_zShift = (*emecGeometry)[0]->getDouble("ZSHIFT")*CLHEP::cm;
  m_OOTcut = 2.5*CLHEP::ns;
}

G4bool LArHECCalculator::Process(const G4Step* a_step, std::vector<LArHitData>& hdata)
{
  // Given a G4Step, find the sampling, region, eta bin, and phi bin
  // in the LAr HEC associated with that point.

  // Note that we don't do any range checking; it is assumed that p
  // lies within the LAr endcap.  But we do have debug checks via asserts.

  // 29-Mar-2002 WGS: this method now returns a boolean.  If it's
  // true, the hit is valid; if it's false, there was some problem
  // with the hit and it should be ignored.

  // Make sure that vector is cleared
  hdata.clear();
  // First, get the energy.
  hdata[0].energy = a_step->GetTotalEnergyDeposit();

  // Find out how long it took the energy to get here.
  G4StepPoint* pre_step_point = a_step->GetPreStepPoint();
  G4StepPoint* post_step_point = a_step->GetPostStepPoint();
  G4double timeOfFlight = (pre_step_point->GetGlobalTime() + 
                           post_step_point->GetGlobalTime()) * 0.5;
  G4ThreeVector startPoint = pre_step_point->GetPosition();
  G4ThreeVector endPoint   = post_step_point->GetPosition();
  G4ThreeVector p = (startPoint + endPoint) * 0.5;
					 
  hdata[0].time = timeOfFlight/CLHEP::ns - p.mag()/CLHEP::c_light/CLHEP::ns;
  if (hdata[0].time > m_OOTcut)
    m_isInTime = false;
  else
    m_isInTime = true;

  // Calculate the identifier.
  hdata[0].id = m_geometry->CalculateIdentifier( a_step );

#ifdef DEBUG_HITS
  std::cout << "LArHECCalculator::Process                 "
	    << " ID=" << std::string(m_identifier)
	    << " energy=" << m_energy
	    << std::endl;
#endif

  // Check for any problems.
  if ( hdata[0].id == LArG4Identifier() )
    return false;

  return true;
}

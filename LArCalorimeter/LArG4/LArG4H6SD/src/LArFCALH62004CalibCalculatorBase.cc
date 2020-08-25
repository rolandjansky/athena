/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#include "LArFCALH62004CalibCalculatorBase.h"
//#include "LArG4FCAL/LArFCALCalculatorBase.h"

#include "LArG4Code/LArG4Identifier.h"

#include "GaudiKernel/ISvcLocator.h"
#include "GaudiKernel/Bootstrap.h"
#include "GaudiKernel/ServiceHandle.h"

#include "PathResolver/PathResolver.h"
#include "RDBAccessSvc/IRDBRecord.h"
#include "RDBAccessSvc/IRDBRecordset.h"
#include "GeoModelInterfaces/IGeoModelSvc.h"
#include "GeoModelUtilities/DecodeVersionKey.h"
#include "LArReadoutGeometry/FCAL_ChannelMap.h"

// Geant4 includes
#include "G4LogicalVolume.hh"
#include "G4VPhysicalVolume.hh"
#include "G4ThreeVector.hh"
#include "G4StepPoint.hh"
#include "G4Step.hh"
#include "globals.hh"
#include "G4TransportationManager.hh"
#include "G4TouchableHistory.hh"

#include <cstdlib>
#include <fstream>
#include <iomanip>

#include <algorithm>

// 03-Jan-2002 WGS: For 'copysign'.
#include <cmath>
#include <limits.h>
#include "StoreGate/StoreGateSvc.h"
#undef DEBUG_FCAL_IO
// Forward declaractions:
class G4Step;

// Forward declaration for namespace CaloG4.
namespace CaloG4 {
  class SimulationEnergies;
}

namespace {

  inline
  G4int etaToBin1 (G4double eta, G4double eta0)
  {
    return static_cast<G4int> ((eta - eta0) * (1./0.1));
  }


  inline
  G4int etaToBin2 (G4double eta, G4double eta0)
  {
    return static_cast<G4int> ((eta - eta0) * (1./0.2));
  }

}


// constructor
//

LArFCALH62004CalibCalculatorBase::LArFCALH62004CalibCalculatorBase(const std::string& name, ISvcLocator * pSvcLocator)
  : LArCalibCalculatorSvcImp(name, pSvcLocator)
  , m_deltaX(0.0)
  , m_deltaY(0.0)
  , m_FCalSampling(0)
  , m_ChannelMap(nullptr)
  , m_Zshift(0.0)
{
  declareProperty("deltaX"            , m_deltaX);
  declareProperty("deltaY"            , m_deltaY);
  declareProperty("FCalSampling"      , m_FCalSampling);
  //declareProperty("Zshift"            , m_Zshift);
}

StatusCode LArFCALH62004CalibCalculatorBase::initialize()
{
  ServiceHandle<StoreGateSvc> detStore ("DetectorStore" ,"LArFCALH62004CalibCalculatorBase");
  ATH_CHECK(detStore->retrieve(m_ChannelMap));

  ISvcLocator  *svcLocator = Gaudi::svcLocator();
  IRDBAccessSvc* rdbAccess;
  IGeoModelSvc * geoModel;

  if(svcLocator->service ("GeoModelSvc",geoModel) == StatusCode::FAILURE)
    throw std::runtime_error("Error in FCALConstruction, cannot access GeoModelSvc");
  if(svcLocator->service ("RDBAccessSvc",rdbAccess) == StatusCode::FAILURE)
    throw std::runtime_error("Error in FCALConstruction, cannot access RDBAccessSvc");
  DecodeVersionKey larVersionKey(geoModel, "LAr");

  m_fcalMod = rdbAccess->getRecordsetPtr("FCalMod", larVersionKey.tag(),larVersionKey.node());
  if (m_fcalMod->size()==0) {
    m_fcalMod=rdbAccess->getRecordsetPtr("FCalMod", "FCalMod-00");
    if (m_fcalMod->size()==0) {
      throw std::runtime_error("Error getting FCAL Module parameters from database");
    }
  }
  double startZFCal1 = (*m_fcalMod)[0]->getDouble("STARTPOSITION"); //466.85 * cm;
  double startZFCal3 = (*m_fcalMod)[2]->getDouble("STARTPOSITION"); //560.28 * cm;
  double depthZFCal3 = (*m_fcalMod)[2]->getDouble("FULLMODULEDEPTH");
  double stopZFCal3  = startZFCal3 + depthZFCal3;
  double halfDepth   = (stopZFCal3 - startZFCal1)/2.;
  m_Zshift = startZFCal1 + halfDepth;

  return StatusCode::SUCCESS;
}

LArFCALH62004CalibCalculatorBase::~LArFCALH62004CalibCalculatorBase() {
}


G4bool LArFCALH62004CalibCalculatorBase::Process(const G4Step* a_step, LArG4Identifier & identifier,
                                                 std::vector<G4double> & energies,
                                                 const LArG4::eCalculatorProcessing a_process) const {
  // First, get the energy.

  energies.clear();
  if ( a_process == LArG4::kEnergyAndID  ||  a_process == LArG4::kOnlyEnergy )
    {
      m_energyCalculator.Energies( a_step, energies );
    }
  else
    for (unsigned int i=0; i != 4; i++) energies.push_back( 0. );

  // find the space point for this deposit
  G4StepPoint* pre_step_point = a_step->GetPreStepPoint();
  G4StepPoint* post_step_point = a_step->GetPostStepPoint();
  G4ThreeVector startPoint = pre_step_point->GetPosition();
  G4ThreeVector endPoint   = post_step_point->GetPosition();
  G4ThreeVector p = (startPoint + endPoint) * 0.5;

  // S.M.: this does not work - there are some volumes inside the
  // FCAL which have their own local coordinate system and
  // just result in (~0,~0) local coordinates. I think they should
  // be treated as absorbers ... so I take the original coordinates
  // instead ...

  // P.S. We sould go to FCAL mother volume, and work there

  // S.M.      // get LOCAL (u,v) coordinates!
  // S.M.      G4Navigator* theNavigator =
  // S.M.          G4TransportationManager::GetTransportationManager()->
  // S.M.          GetNavigatorForTracking();
  // S.M.      G4ThreeVector theLocalPoint = theNavigator->
  // S.M.            GetGlobalToLocalTransform().
  // S.M.          TransformPoint(p);

  // Find a transformation to a module coordinate system
  G4TouchableHistory* theTouchable =
    (G4TouchableHistory*) (pre_step_point->GetTouchable());

  G4String volumeName = theTouchable->GetVolume()->GetName() ;
  int modVol=theTouchable->GetHistoryDepth();
  // Assume Gap as a most often case - get it through copy number
  if(volumeName[volumeName.size() - 3] == 'G') {
    modVol -= 2;
  } else if(volumeName[volumeName.size() - 8] == 'A') { // second most often volume
    modVol -= 1;
  } else if(volumeName[volumeName.size() - 11] == 'C') {
    modVol -= 2;
  } else if(volumeName[volumeName.size() - 3] == 'R') {
    modVol -= 3;
  } else {
    std::cout<<"Unknown volume ?? "<< volumeName << std::endl;
  }

  const G4AffineTransform moduleTransform= theTouchable->GetHistory()->GetTransform(modVol);
  G4ThreeVector theLocalPoint = moduleTransform.TransformPoint(p);


  // we've shifted the endcap, we have to project the geometry to a
  // different point).
  G4double pointZ = p.z();

  int etaIndex,phiIndex;
  float x = theLocalPoint.x(),y=theLocalPoint.y();
  int sampling = m_FCalSampling;
  bool ok = m_ChannelMap->getTileID(sampling,
                                    x,
                                    y,
                                    etaIndex,
                                    phiIndex);


  G4ThreeVector pointShift(x, y, theLocalPoint.z() + m_Zshift);
  // zSide is negative if z<0.
  G4int zSide = 2;
  if (pointZ < 0.) zSide = -zSide;

  identifier.clear();
  if (ok) {
    // we have a active or non-active hit inside the FCal
    G4int sampling = m_FCalSampling;


    // Append the values to the empty identifier.
    identifier << 4          // LArCalorimeter
               << 3          // LArFCAL
               << zSide      // EndCap
               << sampling   // FCal Module #
               << etaIndex
               << phiIndex;

    //	std::cout << "LArG4FCAL/LArFCALH62004CalibCalculatorBase: 4/3/"
    //            <<zSide<<"/"
    //            <<sampling<<"/"
    //            <<etaIndex<<"/"
    //            <<phiIndex<<": "
    //            << energies[0] << ", "
    //            << energies[1] << ", "
    //            << energies[2] << ", "
    //            << energies[3]
    //            << ", i=" << i
    //            << ", j=" << j
    //            << ", l.x=" << theLocalPoint.x()
    //            << ", l.y=" << theLocalPoint.y()
    //            << ", p.x=" << p.x()
    //            << ", p.y=" << p.y()
    //            << ", deltaX=" << m_deltaX
    //            << ", deltaY=" << m_deltaY
    //            << std::endl;
  }
  else {
    // S.M.: we have a hit which fails the electrode identifier
    // calculation.  It's likely just a the border of the FCAL
    // (outer or inner) and not really dead material ... Lacking a
    // better method of assigning it to a correct cell I make a
    // dead material identifier for it ...

    /*
      int dubina=theTouchable->GetHistoryDepth();
      std::cout<<"----------------------"<<std::endl;
      for( int kk=0; kk<=dubina; ++kk ) {
      std::cout<<theTouchable->GetHistory()->GetVolume(kk)->GetName()<<" "<<theTouchable->GetHistory()->GetVolume(kk)->GetCopyNo()<<std::endl;
      }
    */
    G4int sampling(3); // FCAL leakage
    G4int type(1); // FCAL leakage
    G4double eta = fabs( pointShift.pseudoRapidity() );
    G4double phi = pointShift.phi();
    G4double rho = theLocalPoint.perp();
    // since the outer edge of the FCal has a irregular shape we
    // take the innermost position of the outermost tubes as min
    // radius - this is about 3 cm from the outermost tube
    // position ...
    const G4double fcal_edge_approx = 415.0*CLHEP::mm;
    G4int subdet; // LAr
    if ( pointZ < 0. )
      subdet = -4;
    else
      subdet = 4;
    if (phi < 0) phi += 2*M_PI;
    G4int etaIndex;
    G4int phiIndex;
    G4int region;
    if ( eta < 5 ) {
      if(rho > fcal_edge_approx) {
        type = 2;
        sampling = m_FCalSampling;
        region = 4;
        etaIndex = 0;
        phiIndex = G4int(phi*(32/M_PI));
      } else {
        region = 0;
        etaIndex = etaToBin1 (eta, 1.7);
        phiIndex = G4int(phi*(32/M_PI));
      }
    }
    else if ( eta < 8 ) {
      region = 1;
      etaIndex = etaToBin2 (eta, 5.0);
      phiIndex = G4int(phi*(32/M_PI));
    }
    else {
      region = 2;
      etaIndex = 0;
      phiIndex = 0;
    }

    // Append the values to the empty identifier.
    identifier << 10          // Calorimeter Dead Material
               << subdet      // LAr
               << type
               << sampling
               << region
               << etaIndex
               << phiIndex;
    /*
      std::cout << "LArG4FCAL/LArFCALH62004CalibCalculatorBase: 10/4/"
      <<type<<"/"
      <<sampling<<"/"
      <<region<<"/"
      <<etaIndex<<"/"
      <<phiIndex<<": "
      //                  << energies[0] << ", "
      //                  << energies[1] << ", "
      //                  << energies[2] << ", "
      //                  << energies[3]
      //                  << ", i=" << i
      //                  << ", j=" << j
      << std::fixed
      << ", l.r=" << rho
      << ", l.x=" << theLocalPoint.x()
      << ", l.y=" << theLocalPoint.y()
      << ", l.z=" << theLocalPoint.z()<< std::endl;

      std::cout << ", p.x=" << p.x()
      << ", p.y=" << p.y()
      << ", p.z=" << p.z()
      << ", l.zshift "<< theLocalPoint.z() + m_Zshift
      << ", deltaX=" << m_deltaX
      << ", deltaY=" << m_deltaY
      << std::endl;
    */
  }

  return true;
}

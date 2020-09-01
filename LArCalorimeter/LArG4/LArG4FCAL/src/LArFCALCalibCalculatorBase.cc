/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

//-----------------------------------------------------------------------------
//
// Base class describing common behaviour in FCal G4 calculator.
//
//----------------------------------------------------------------------------

// ATLAS LAr includes
#include "LArFCALCalibCalculatorBase.h"
#include "LArFCALCalculatorBase.h"

#include "LArG4Code/LArG4Identifier.h"
//#include "LArG4Code/SimulationEnergies.h"
#include "CaloG4Sim/SimulationEnergies.h"
#include "LArReadoutGeometry/FCAL_ChannelMap.h"

// For reading the data files in Athena.
#ifndef LARG4_STAND_ALONE
#include "PathResolver/PathResolver.h"
#endif

// Geant4 includes
#include "G4LogicalVolume.hh"
#include "G4VPhysicalVolume.hh"
#include "G4ThreeVector.hh"
#include "G4StepPoint.hh"
#include "G4Step.hh"
#include "globals.hh"
#include "G4TransportationManager.hh"

#include <cstdlib>
#include <fstream>

#include <algorithm>

#include "GeoModelInterfaces/IGeoModelSvc.h"
#include "GaudiKernel/ISvcLocator.h"
#include "GaudiKernel/Bootstrap.h"
#include "GaudiKernel/ServiceHandle.h"
#include "RDBAccessSvc/IRDBAccessSvc.h"
#include "RDBAccessSvc/IRDBRecordset.h"
#include "RDBAccessSvc/IRDBRecord.h"

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

// constructor
//
namespace LArG4 {

  namespace FCAL {


    LArFCALCalibCalculatorBase::LArFCALCalibCalculatorBase(const std::string& name, ISvcLocator *pSvcLocator)
      : LArCalibCalculatorSvcImp(name, pSvcLocator)
      , m_deltaX(0.)
      , m_deltaY(0.)
      , m_FCalSampling(0)
      , m_zShift(0.)
      , m_ChannelMap(nullptr)
    {
      declareProperty("FCALdeltaX",m_deltaX);
      declareProperty("FCALdeltaY",m_deltaY);
      declareProperty("FCALSampling",m_FCalSampling);
      //m_FCalSampling.verifier().setUpper(3); //Would need to make m_FCalSampling an IntegerProperty for this to work. Overkill?
    }

    StatusCode LArFCALCalibCalculatorBase::initialize() {
      // Initialize the energy calculators.
      //      m_electrode.GetElectrodes();
      ////     m_ElectrodeFile = "FCal1-electrodes.sorted.dat";
      ////m_deltaX=7.5;
      ////m_deltaY=7.5*sin(60.0);

      ServiceHandle<StoreGateSvc> detStore ("DetectorStore" ,"LArFCALCalibCalculatorBase");
      ATH_CHECK(detStore.retrieve() );
      if (detStore->retrieve(m_ChannelMap)==StatusCode::FAILURE)
        throw std::runtime_error ("LArFCALCalibCalculatorBase ERROR: Cannot retrieve FCAL Channel Map!");

      // get GeoModelSvc & RDBAccessSvc
      ISvcLocator *svcLocator = Gaudi::svcLocator();
      IGeoModelSvc *geoModel(0);
      StatusCode status = svcLocator->service ("GeoModelSvc",geoModel);
      if (status != StatusCode::SUCCESS)
        throw std::runtime_error ("LArFCALCalibCalculatorBase ERROR: Cannot locate GeoModelSvc!");

      // Access the geometry database:
      IRDBAccessSvc *pAccessSvc(0);
      status=svcLocator->service("RDBAccessSvc",pAccessSvc);
      if (status != StatusCode::SUCCESS)
        throw std::runtime_error ("LArFCALCalibCalculatorBase ERROR: Cannot locate RDBAccessSvc!");

      // Obtain the geometry version information:

      std::string AtlasVersion = geoModel->atlasVersion();
      std::string LArVersion = geoModel->LAr_VersionOverride();

      std::string detectorKey  = LArVersion.empty() ? AtlasVersion : LArVersion;
      std::string detectorNode = LArVersion.empty() ? "ATLAS" : "LAr";

      IRDBRecordset_ptr emecGeoPtr = pAccessSvc->getRecordsetPtr("EmecGeometry",detectorKey,detectorNode);
      if (emecGeoPtr->size()==0)
        throw std::runtime_error ("LArFCALCalibCalculatorBase ERROR: Cannot find the EmecGeometry Table");

      m_zShift = (*emecGeoPtr)[0]->getDouble("ZSHIFT")*CLHEP::cm;
      return StatusCode::SUCCESS;
    }

    G4bool LArFCALCalibCalculatorBase::Process(const G4Step* step, LArG4Identifier & identifier,
                                               std::vector<G4double> & energies,
                                               const eCalculatorProcessing process) const
    {
      // First, get the energy.
      if ( process == kEnergyAndID  ||  process == kOnlyEnergy )
        {
          m_energyCalculator.Energies( step, energies );
        }
      else
        for (unsigned int i=0; i != 4; i++) energies.push_back( 0. );

      // find the space point for this deposit
      G4StepPoint* pre_step_point = step->GetPreStepPoint();
      G4StepPoint* post_step_point = step->GetPostStepPoint();
      G4ThreeVector startPoint = pre_step_point->GetPosition();
      G4ThreeVector endPoint   = post_step_point->GetPosition();
      G4ThreeVector p = (startPoint + endPoint) * 0.5;

      // S.M.: this does not work - there are some volumes inside the
      // FCAL which have their own local coordinate system and
      // just result in (~0,~0) local coordinates. I think they should
      // be treated as absorbers ... so I take the original coordinates
      // instead ...

      // S.M.      // get LOCAL (u,v) coordinates!
      // S.M.      G4Navigator* theNavigator =
      // S.M.      G4TransportationManager::GetTransportationManager()->
      // S.M.      GetNavigatorForTracking();
      // S.M.      G4ThreeVector theLocalPoint = theNavigator->
      // S.M.        GetGlobalToLocalTransform().
      // S.M.      TransformPoint(p);


      G4ThreeVector theLocalPoint(p);

      // we've shifted the endcap, we have to project the geometry to a
      // different point).
      G4double pointZ = p.z();

      G4double pointZshift = pointZ - copysign(m_zShift,pointZ);

      G4ThreeVector pointShift(p.x(),
                               p.y(),
                               pointZshift);

      int etaIndex,phiIndex;
      float x = theLocalPoint.x(),y=theLocalPoint.y();
      int sampling = m_FCalSampling;
      bool ok = m_ChannelMap->getTileID(sampling,
                                        x,
                                        y,
                                        etaIndex,
                                        phiIndex);


      // zSide is negative if z<0.
      G4int zSide = 2;
      if (pointZshift < 0.) zSide = -zSide;

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

        //	std::cout << "LArG4FCAL/LArFCALCalibCalculatorBase: 4/3/"
        //                <<zSide<<"/"
        //                <<sampling<<"/"
        //                <<etaIndex<<"/"
        //                <<phiIndex<<": "
        //                << energies[0] << ", "
        //                << energies[1] << ", "
        //                << energies[2] << ", "
        //                << energies[3]
        //                << ", i=" << i
        //                << ", j=" << j
        //                << ", l.x=" << theLocalPoint.x()
        //                << ", l.y=" << theLocalPoint.y()
        //                << ", p.x=" << p.x()
        //                << ", p.y=" << p.y()
        //                << ", deltaX=" << m_deltaX
        //                << ", deltaY=" << m_deltaY
        //                << std::endl;
      }
      else {
        // S.M.: we have a hit which fails the electrode identifier
        // calculation.  It's likely just a the border of the FCAL
        // (outer or inner) and not really dead material ... Lacking a
        // better method of assigning it to a correct cell I make a
        // dead material identifier for it ...
        G4int sampling(3); // FCAL leakage
        G4int type(1); // FCAL leakage
        G4double eta = fabs( pointShift.pseudoRapidity() );
        G4double phi = pointShift.phi();
        G4double rho = pointShift.perp();
        // since the outer edge of the FCal has a irregular shape we
        // take the innermost position of the outermost tubes as min
        // radius - this is about 3 cm from the outermost tube
        // position ...
        const G4double fcal_edge_approx = 415.0*CLHEP::mm;
        G4int subdet; // LAr
        if ( pointZshift < 0. )
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
            etaIndex = G4int((eta-1.7)/0.1);
            phiIndex = G4int(phi*(32/M_PI));
          }
        }
        else if ( eta < 8 ) {
          region = 1;
          etaIndex = G4int((eta-5)/0.2);
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

        //	std::cout << "LArG4FCAL/LArFCALCalibCalculatorBase: 10/4/"
        //                <<type<<"/"
        //                <<sampling<<"/"
        //                <<region<<"/"
        //                <<etaIndex<<"/"
        //                <<phiIndex<<": "
        //                << energies[0] << ", "
        //                << energies[1] << ", "
        //                << energies[2] << ", "
        //                << energies[3]
        //                << ", i=" << i
        //                << ", j=" << j
        //                << ", l.x=" << theLocalPoint.x()
        //                << ", l.y=" << theLocalPoint.y()
        //                << ", p.x=" << p.x()
        //                << ", p.y=" << p.y()
        //                << ", deltaX=" << m_deltaX
        //                << ", deltaY=" << m_deltaY
        //                << std::endl;
      }

      return true;
    }

  } // namespace FCAL

} // namespace LArG4

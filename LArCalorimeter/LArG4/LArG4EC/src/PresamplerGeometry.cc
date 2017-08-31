/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// LArG4::EC::PresamplerGeometry
// 17-Aug-2004 William Seligman

// 2-July-2003 Mikhail Leltchouk: local coordinates for determination
// of etaBin, phiBin at any Endcap Presamplerposition.

#include "LArG4EC/PresamplerGeometry.h"

#include "LArG4Code/LArG4Identifier.h"
#include "LArG4Code/LArVG4DetectorParameters.h"

#include "G4ThreeVector.hh"
#include "G4StepPoint.hh"
#include "G4Step.hh"
#include "G4AffineTransform.hh"
#include "G4NavigationHistory.hh"
#include "G4VTouchable.hh"
#include "G4TouchableHistory.hh"


#include "RDBAccessSvc/IRDBAccessSvc.h"
#include "RDBAccessSvc/IRDBRecord.h"
#include "RDBAccessSvc/IRDBRecordset.h"
#include "GeoModelInterfaces/IGeoModelSvc.h"
#include "GaudiKernel/ISvcLocator.h"
#include "GaudiKernel/Bootstrap.h"
#include "StoreGate/StoreGateSvc.h"

#include "globals.hh"

#include <cmath>

namespace LArG4 {

  namespace EC {


    struct  PresamplerGeometry::Clockwork {
      double rMinEndcap;
      double rMaxEndcap;
      double zEndcapFrontFace;
      double zEndcapBackFace;
      double halfThickness;
      double zPosInMother;
    };

    // Standard implementation of a singleton pattern.

    const PresamplerGeometry* PresamplerGeometry::GetInstance()
    {
      static PresamplerGeometry instance; // used as const after initialization
      return &instance;
    }

    PresamplerGeometry::PresamplerGeometry():
      m_c(new Clockwork())
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
      
      const std::string AtlasVersion = geoModel->atlasVersion();
      const std::string LArVersion = geoModel->LAr_VersionOverride();
      
      const std::string detectorKey  = LArVersion.empty() ? AtlasVersion : LArVersion;
      const std::string detectorNode = LArVersion.empty() ? "ATLAS" : "LAr";
      
      // Note Presampler Lives In DB under "cryostats"..
      IRDBRecordset_ptr presamplerPosition = pAccessSvc->getRecordsetPtr("PresamplerPosition",AtlasVersion,"ATLAS");
      if (presamplerPosition->size()==0) {
	presamplerPosition = pAccessSvc->getRecordsetPtr("PresamplerPosition","PresamplerPosition-00");
	if (presamplerPosition->size()==0) {
	  throw std::runtime_error("Cannot find the PresamplerPosition Table");
	}
      }
      
      m_c->rMinEndcap=(*presamplerPosition)[0]->getDouble("RMIN")*CLHEP::cm;
      m_c->rMaxEndcap=(*presamplerPosition)[0]->getDouble("RMAX")*CLHEP::cm;
      m_c->halfThickness=(*presamplerPosition)[0]->getDouble("TCK")*CLHEP::cm/2;;
      m_c->zEndcapFrontFace=(*presamplerPosition)[0]->getDouble("ZPOS")*CLHEP::cm-m_c->halfThickness;
      m_c->zEndcapBackFace =(*presamplerPosition)[0]->getDouble("ZPOS")*CLHEP::cm+m_c->halfThickness;
      
      m_c->zPosInMother=30.5*CLHEP::mm;
      
    }
    
    PresamplerGeometry::~PresamplerGeometry()
    {
      delete m_c;
    }


    G4double PresamplerGeometry::GetValue(const kValue a_valueType) const
    {
      // Look up a value based on name.
	  switch (a_valueType) {
		case rMinEndcapPresampler:
		  //return 1231.74 * mm;
		  return  m_c->rMinEndcap;
		  break;
		case rMaxEndcapPresampler:
		  //return 1701.98 * mm;
		  return  m_c->rMaxEndcap;
		  break;
		  // At nominal (zShift=0) endcap position absolute z-coordinates:
		  // of the faces of the EndcapPresampler
		case zEndcapPresamplerFrontFace:
		  //return 3622. * mm;
		  return m_c->zEndcapFrontFace;
		  break;
		case zEndcapPresamplerBackFace:
		  //return 3626. * mm;
		  return m_c->zEndcapBackFace;
		  break;
		case EndcapPresamplerHalfThickness:
		  //return ( GetValue(zEndcapPresamplerBackFace) - GetValue(zEndcapPresamplerFrontFace) ) / 2.;
		  return  m_c->halfThickness;
		  break;
		case EndcapPresamplerZpositionInMother:
		  // between cold wall center and presampler center which is at
		  // 3624 mm nominal (zShift=0) absolute position
		  return m_c->zPosInMother;
		  break;
		default:
		  G4cerr << "LArEndcapPresamplerCalculator::GetValue -- type '"
			<< a_valueType
			<< "' not recognized; using zero" << G4endl;
		  return 0.;
	  }
    }


    // The static structures that describe the endcap geometry.  Mostly these
    // were copied from the ATLAS G3 code.
    // Someday, the following table should be determined from the detector
    // database and not hard-coded.

    typedef struct {
      G4int    zSide;      // +- 3 for inner wheel, +- 2 for outer wheel, z determines sign
      G4int    sampling;
      G4int    region;
      G4double etaScale;   // 1/deta
      G4double etaOffset;  // set so that the range of etaBin starts at zero for each compartment
      G4int    maxEta;     // the maximum value of etaBin in this compartment
      G4int    gapsPerBin; // number of phi gaps (in LArWheelSolid) for each cell bin.
      G4int    maxPhi;     // the maximum value of phiBin in this compartment
    } geometry_t;

    static const geometry_t geometry =
    // zSide sampling region etaScale etaOffset maxEta gapsPerBin maxPhi
      {   2,      0,      0,      40,     60  ,    11,       0,     63 };


    LArG4Identifier PresamplerGeometry::CalculateIdentifier(const G4Step* a_step) const
    {
      // Given a G4Step, find the sampling, region, eta bin, and phi bin
      // in the LAr EMEC presampler associated with that point.

      // Note that we don't do any range checking; it is assumed that p
      // lies within the LAr endcap presampler.  But we do have debug
      // checks via asserts.

      // Find the mid-point of the current step.
      const G4StepPoint* pre_step_point = a_step->GetPreStepPoint();
      const G4StepPoint* post_step_point = a_step->GetPostStepPoint();
      const G4ThreeVector startPoint = pre_step_point->GetPosition();
      const G4ThreeVector endPoint   = post_step_point->GetPosition();
      const G4ThreeVector p = (startPoint + endPoint) * 0.5;

      // Get local coordinates in the Endcap Presampler system
      // independently how it was positioned into
      // the cryostat front cold wall and shifted with it
      // see ref.:http://geant4-hn.slac.stanford.edu:5090/
      //               HyperNews/public/get/geometry/17/1.html
      // and J.Toth's GetTopTransform in LArWheelCalculator.cc

      const G4AffineTransform transformation = pre_step_point->GetTouchable()->GetHistory()->GetTopTransform();

      const G4ThreeVector startPointinLocal = transformation.TransformPoint(startPoint);
      const G4ThreeVector   endPointinLocal = transformation.TransformPoint  (endPoint);
      const G4ThreeVector          pinLocal =(startPointinLocal+endPointinLocal)*0.5;

      const G4ThreeVector pForCell(pinLocal.x(), pinLocal.y(), pinLocal.z()
                             + GetValue(zEndcapPresamplerFrontFace)
                             + GetValue(EndcapPresamplerHalfThickness));
      //  pForCell.z() > 0 by definition (pinLocal.z() from -2 to +2)
      const G4double eta=pForCell.pseudoRapidity();
      G4double phi=pForCell.phi();
      if (phi < 0.) phi += 2.*M_PI;

      // zSide is negative if z<0.
      G4int zSide = geometry.zSide;
      if (p.z() < 0.) zSide = -zSide;

      // For the presampler, the 'sampling' and 'region' are always zero.
      static const G4int sampling = geometry.sampling;
      static const G4int region   = geometry.region;

      // calculate constants once:
      static const G4double phiScale = (geometry.maxPhi + 1) / (2.*M_PI);
      static const G4int maxPhi1 = geometry.maxPhi + 1;
      static const G4int halfMaxPhi1 = (geometry.maxPhi-1)/2;

      G4int etaBin = G4int( eta * geometry.etaScale - geometry.etaOffset );
      //  28-Apr-2004 ML: Bug fix
      if ( etaBin == 12  &&  eta < 1.801 )
	{
	  etaBin = 11;
	}
      G4int phiBin = G4int( phi * phiScale );

      if( zSide < 0 )
	{
	  phiBin = halfMaxPhi1 - phiBin;
	  if(phiBin < 0 ) phiBin += maxPhi1;
	}

      // Consistency asserts:
      assert ( etaBin >= 0 );
      assert ( phiBin >= 0 );
      //  assert ( etaBin <= geometry[c].maxEta );

	  // Start with a blank identifier.
      LArG4Identifier identifier;
  
      if ( etaBin > geometry.maxEta )
	{
	  G4cerr << "LArG4::EC::PresamplerGeometry::CalculateIdentifier: invalid hit, etaBin="
		 << etaBin
		 << " > geometry.maxEta="
		 << geometry.maxEta
		 << G4endl;
	  return identifier;
	}
      assert ( phiBin <= geometry.maxPhi );

      // Append the values to the empty identifier.
      identifier << 4          // LArCalorimeter
		 << 1          // LArEM
		 << zSide
		 << sampling
		 << region
		 << etaBin
		 << phiBin;

      return identifier;
    }

  } // namespace EC

} // namespace LArG4

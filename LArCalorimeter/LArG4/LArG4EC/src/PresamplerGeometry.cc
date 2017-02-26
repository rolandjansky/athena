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

    PresamplerGeometry::PresamplerGeometry(const std::string& name, ISvcLocator * pSvcLocator)
      : AthService(name, pSvcLocator)
      , m_zEndcapFrontFace(3622.*CLHEP::mm)
      , m_halfThickness(2.0*CLHEP::mm)
      // The following variables describe the endcap geometry.  Mostly
      // these were copied from the ATLAS G3 code.  FIXME Someday,
      // they should be determined from the detector database and not
      // hard-coded.
      , m_zSide(2)
      , m_sampling(0)
      , m_region(0)
      , m_etaScale(40.0)
      , m_etaOffset(60.0)
      , m_maxEta(11)
      //, m_gapsPerBin(0) //unused
      , m_maxPhi(63)
      // derived constants:
      , m_phiScale((m_maxPhi + 1) / (2.*M_PI))
      , m_maxPhi1(m_maxPhi + 1)
      , m_halfMaxPhi1((m_maxPhi-1)/2)
    {

    }

    StatusCode PresamplerGeometry::initialize()
    {
      // Access the GeoModelSvc:
      ISvcLocator *svcLocator = Gaudi::svcLocator();
      IGeoModelSvc *geoModel(nullptr);
      ATH_CHECK(svcLocator->service ("GeoModelSvc",geoModel));
      // Access the geometry database:
      IRDBAccessSvc *pAccessSvc(nullptr);
      ATH_CHECK(svcLocator->service("RDBAccessSvc",pAccessSvc));
      // Obtain the geometry version information:
      const std::string AtlasVersion = geoModel->atlasVersion();
      const std::string LArVersion = geoModel->LAr_VersionOverride();
      const std::string detectorKey  = LArVersion.empty() ? AtlasVersion : LArVersion;
      const std::string detectorNode = LArVersion.empty() ? "ATLAS" : "LAr";
      pAccessSvc->connect();
      // Note Presampler lives under "cryostats" in DB..
      const IRDBRecordset *presamplerPosition = pAccessSvc->getRecordset("PresamplerPosition",AtlasVersion,"ATLAS");
      if (presamplerPosition->size()==0)
        {
          presamplerPosition = pAccessSvc->getRecordset("PresamplerPosition","PresamplerPosition-00");
          if (presamplerPosition->size()==0)
            {
              ATH_MSG_ERROR("Cannot find the PresamplerPosition Table");
              return StatusCode::FAILURE;
            }
        }
      pAccessSvc->disconnect();

      m_halfThickness=(*presamplerPosition)[0]->getDouble("TCK")*CLHEP::cm/2; //FIXME divide by 2.0 instead?
      m_zEndcapFrontFace=(*presamplerPosition)[0]->getDouble("ZPOS")*CLHEP::cm-m_halfThickness;

      return StatusCode::SUCCESS;
    }

    PresamplerGeometry::~PresamplerGeometry()
    {
    }

    StatusCode PresamplerGeometry::queryInterface( const InterfaceID & riid,  void** ppvInterface )
    {
      if ( IECPresamplerGeometry::interfaceID().versionMatch(riid) )
        {
          *ppvInterface = dynamic_cast<IECPresamplerGeometry*>(this);
          addRef();
          return StatusCode::SUCCESS;
        }
      // Interface is not directly available : try out a base class
      return AthService::queryInterface(riid, ppvInterface);
    }

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

      const G4ThreeVector pForCell(pinLocal.x(),
                                   pinLocal.y(),
                                   pinLocal.z() + m_zEndcapFrontFace + m_halfThickness);
      //  pForCell.z() > 0 by definition (pinLocal.z() from -2 to +2)
      const G4double eta=pForCell.pseudoRapidity();
      G4double phi=pForCell.phi();
      if (phi < 0.) phi += 2.*M_PI;

      // zSide is negative if z<0.
      const int zSide = (p.z() < 0.)? -m_zSide : m_zSide;

      G4int etaBin = G4int( eta * m_etaScale - m_etaOffset );
      //  28-Apr-2004 ML: Bug fix
      if ( etaBin == 12  &&  eta < 1.801 )
        {
          etaBin = 11;
        }
      G4int phiBin = G4int( phi * m_phiScale );

      if( zSide < 0 )
        {
          phiBin = m_halfMaxPhi1 - phiBin;
          if(phiBin < 0 ) phiBin += m_maxPhi1;
        }

      // Consistency asserts:
      assert ( etaBin >= 0 ); //FIXME assert statement ignored in opt builds.
      assert ( phiBin >= 0 ); //FIXME assert statement ignored in opt builds.
      //  assert ( etaBin <= geometry[c].maxEta );

      // Start with a blank identifier.
      LArG4Identifier identifier;

      if ( etaBin > m_maxEta )
        {
          G4cerr << "LArG4::EC::PresamplerGeometry::CalculateIdentifier: invalid hit, etaBin="
                 << etaBin
                 << " > m_maxEta="
                 << m_maxEta
                 << G4endl;
          return identifier;
        }
      assert ( phiBin <= m_maxPhi ); //FIXME assert statement ignored in opt builds.

      // Append the values to the empty identifier.
      identifier << 4          // LArCalorimeter
                 << 1          // LArEM
                 << zSide
                 << m_sampling
                 << m_region
                 << etaBin
                 << phiBin;

      return identifier;
    }

  } // namespace EC

} // namespace LArG4

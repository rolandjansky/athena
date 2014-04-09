/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// class header include
#include "G4DetectorEnvelopes/DefSvcEnvelope.h"

#include "G4LogicalVolume.hh"
#include "G4VisAttributes.hh"
#include "G4PVPlacement.hh"
#include "G4UserLimits.hh"
#include "G4Polycone.hh"

#include "FadsGeometry/DetectorFacilityBaseT.h"

// stl includes
#include <iostream>
#include <vector>

// framework includes
#include "GaudiKernel/Bootstrap.h"
#include "GaudiKernel/ISvcLocator.h"
#include "GaudiKernel/MsgStream.h"
#include "AthenaBaseComps/AthMsgStreamMacros.h"

// Geant4 includes
#include "G4Polycone.hh"
#include "CLHEP/Units/PhysicalConstants.h"

// EnvelopeDefinitionService
#include "SubDetectorEnvelopes/IEnvelopeDefSvc.h"

static FADS::DetectorFacilityBaseT<DefSvcEnvelope> b("EnvelopeDefSvc");

DefSvcEnvelope::DefSvcEnvelope(G4String s):
  DetectorFacility(s),
  m_msgStream(0),
  m_envelopeSvcName("G4EnvelopeDefSvc"),
  m_envelopeSvc(),
  m_envelopeName(),
  m_volume(0),
  m_material("Air")
{
}


void DefSvcEnvelope::SetEnvelopeSvcName(std::string name) {
  m_envelopeSvcName = name;
}

void DefSvcEnvelope::SetEnvelopeName(std::string name) {
  m_envelopeName   = name;
}

MsgStream &DefSvcEnvelope::msg() const {
  return *m_msgStream;
}

MsgStream &DefSvcEnvelope::msg (const MSG::Level lvl) const {
  return *m_msgStream << lvl;
}

bool DefSvcEnvelope::msgLvl (const MSG::Level lvl) const {
  if ( m_msgStream->level() <= lvl) {
    *m_msgStream << lvl;
    return true;
  } else {
    return false;
  }
}


bool DefSvcEnvelope::setupServices() {
  // the service locator from Bootstrap
  ISvcLocator *svcLocator = Gaudi::svcLocator();

  // setup the message service
  IMessageSvc* msgSvc = 0;
  if ( svcLocator->service("MessageSvc", msgSvc).isFailure() || !msgSvc)
    std::cout << "DefSvcEnvelope: Trouble getting the message service. Should never happen. Will crash now." << std::endl;
  m_msgStream = new MsgStream(msgSvc, "DefSvcEnvelope");

  // retrieve the Athena service providing the envelope definitions
  if ( svcLocator->service( m_envelopeSvcName, m_envelopeSvc).isFailure()) {
    ATH_MSG_ERROR( "Could not locate envelope definition service, unable to set up '"
                   << m_envelopeName << "' envelope.");
    return true;
  }

  // return successful
  return false;
}


void DefSvcEnvelope::BuildGeometry() {

  // setup and retrieve MSG, DDDB and GeoModel services
  // if return status != 0  --> error
  if ( setupServices()) return;

  ATH_MSG_INFO("Building '" << m_envelopeName << "' Envelope using the envelope definition service '"
               <<m_envelopeSvcName << "'.");

  //
  // @TODO: support read-in of more than one volume from DB (via VOLUME_ID)
  //        see: AtlasOffline:Simulation/ISF/ISF_Geant4/ISF_Geant4Services/GeoIDSvc.cxx
  //        ( using m_envelopeSvc->get..NumVols() )

  // retrieve the vector of (r,z) values from the EnvelopeDefSvc
  const RZPairVector *rz = 0;
  if ( m_envelopeName == "BeamPipe")
    rz = &( m_envelopeSvc->getBeamPipeRZBoundary() );
  else if ( m_envelopeName == "IDET")
    rz = &( m_envelopeSvc->getInDetRZBoundary() );
  else if ( m_envelopeName == "CALO")
    rz = &( m_envelopeSvc->getCaloRZBoundary() );
  else if ( m_envelopeName == "MUONQ02")
    rz = &( m_envelopeSvc->getMuonRZBoundary() );
  else {
    ATH_MSG_ERROR("Unknown envelope volume name '"<< m_envelopeName << "'. Unable to build envelope.");
    return;
  }


  // check vector size
  size_t numPoints = rz->size();
  if ( !numPoints) {
    ATH_MSG_ERROR("No entries for " << m_envelopeName << " envelope in envelope definition service. Unable to build envelope.");
    return;
  }

  // arrays of (r,z) coordinates for the envelope
  double *z = new double[numPoints];
  double *r = new double[numPoints];

  // convert vector of pairs into two arrays
  RZPairVector::const_iterator rzIt = rz->begin();
  for ( size_t i=0; i<numPoints; ++i) {
    r[i] = rzIt->first;
    z[i] = rzIt->second;

    ++rzIt;
  }
  
  // create the G4 Volume
  double phimin    = 0.;
  double deltaphi  = 360.*CLHEP::deg;
	G4Polycone *pcone = new G4Polycone( m_envelopeName, phimin, deltaphi, numPoints, r, z);
	m_volume = new G4LogicalVolume( pcone, 0, m_envelopeName);
	SetMaterial( m_material);
	SetDetectorEnvelope( m_volume);

  // cleanup
  delete[] r;
  delete[] z;
}

// TODO: is there a reason for this include to be in the middle of the file?
#include "FadsMaterial/MaterialManager.h"
void DefSvcEnvelope::SetMaterial(std::string &mat) {
	if ( m_volume) {
		FADS::MaterialManager *mm=FADS::MaterialManager::GetMaterialManager();
		G4Material *gm=mm->GetMaterial(mat);
		m_volume->SetMaterial(gm);
	}
}


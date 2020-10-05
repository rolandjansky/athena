/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

// Base class
#include "G4AtlasTools/DetectorGeometryBase.h"
#include "G4AtlasTools/EnvelopeTool.h"

#include "SubDetectorEnvelopes/IEnvelopeDefSvc.h"

// Geant4 includes used in functions
#include "G4NistManager.hh"
#include "G4LogicalVolume.hh"
#include "G4PVPlacement.hh"
#include "G4SystemOfUnits.hh"
#include "G4Polycone.hh"

EnvelopeTool::EnvelopeTool(const std::string& type, const std::string& name, const IInterface* parent)
  : DetectorGeometryBase(type,name,parent),m_envelopeDefSvc("AtlasGeometry_EnvelopeDefSvc",name)
{
  ATH_MSG_INFO( "EnvelopeTool being created: " << name );
}

StatusCode EnvelopeTool::initialize()
{
  //envelope definition service
  if (m_envelopeDefSvc.retrieve().isFailure() )
    {
      ATH_MSG_ERROR( "Could not retrieve " << m_envelopeDefSvc );
      return StatusCode::FAILURE;
    }

  return StatusCode::SUCCESS;
}

// This method is taken almost verbatim from DefSvcEnvelope.cxx

void EnvelopeTool::BuildGeometry()
{
  ATH_MSG_INFO( "this is EnvelopeTool::BuildGeometry: " );

  ATH_MSG_INFO("Building '" << m_detectorName.value() << "' Envelope using the envelope definition service '");

  // retrieve the vector of (r,z) values from the EnvelopeDefSvc
  const RZPairVector *rz = 0;
  if ( m_detectorName.value() == "BeamPipe")
    rz = &( m_envelopeDefSvc->getBeamPipeRZBoundary() );
  else if ( m_detectorName.value() == "IDET")
    rz = &( m_envelopeDefSvc->getInDetRZBoundary() );
  else if ( m_detectorName.value() == "CALO")
    rz = &( m_envelopeDefSvc->getCaloRZBoundary() );
  else if ( m_detectorName.value() == "MUON")
    rz = &( m_envelopeDefSvc->getMuonRZBoundary() );
  else
    {
      ATH_MSG_ERROR("Unknown envelope volume name '"<< m_detectorName.value() << "'. Unable to build envelope.");
      return;
    }


  // check vector size
  size_t numPoints = rz->size();
  if ( !numPoints)
    {
      ATH_MSG_ERROR("No entries for " << m_detectorName.value() << " envelope in envelope definition service. Unable to build envelope.");
      return;
    }

  // arrays of (r,z) coordinates for the envelope
  double *z = new double[numPoints];
  double *r = new double[numPoints];

  // convert vector of pairs into two arrays
  RZPairVector::const_iterator rzIt = rz->begin();
  for ( size_t i=0; i<numPoints; ++i)
    {
      r[i] = rzIt->first;
      z[i] = rzIt->second;

      ++rzIt;
    }

  // create the G4 Volume
  double phimin    = 0.;
  double deltaphi  = 360.*CLHEP::deg;
  G4Polycone *pcone = new G4Polycone( m_detectorName.value(), phimin, deltaphi, numPoints, r, z);
  G4LogicalVolume* logVolume = new G4LogicalVolume( pcone, 0, m_detectorName.value());

  G4NistManager* nist = G4NistManager::Instance();

  G4Material* mat = nist->FindOrBuildMaterial("Air");

  logVolume->SetMaterial(mat);

  m_envelope.theEnvelope=logVolume;

  // cleanup
  delete[] r;
  delete[] z;
}

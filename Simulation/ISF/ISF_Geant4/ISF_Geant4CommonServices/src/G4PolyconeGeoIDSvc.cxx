/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// G4PolyconeGeoIDSvc.cxx, (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////

// class header include
#include "G4PolyconeGeoIDSvc.h"

// C includes
#include <math.h>

// framework includes
#include "GaudiKernel/Bootstrap.h"
#include "GaudiKernel/ISvcLocator.h"

// STL includes
#include <limits>
#include <iostream>

// ISF includes
#include "ISF_Event/ISFParticle.h"

// Geant4 includes
#include "geomdefs.hh"
#include "G4Polycone.hh"
#include "G4UnionSolid.hh"

// EnvelopeDefinitionService
#include "SubDetectorEnvelopes/IEnvelopeDefSvc.h"

// DetectorDescription
#include "AtlasDetDescr/AtlasRegion.h"
#include "AtlasDetDescr/AtlasRegionHelper.h"

/** Constructor **/
ISF::G4PolyconeGeoIDSvc::G4PolyconeGeoIDSvc(const std::string& name,ISvcLocator* svc) :
  AthService(name,svc),
  m_envDefSvc("AtlasGeometry_EnvelopeDefSvc", name),
  m_volume(),
  m_typeConverter()
{
  declareProperty("EnvelopeDefSvc"            , m_envDefSvc );
}


/** Destructor **/
ISF::G4PolyconeGeoIDSvc::~G4PolyconeGeoIDSvc()
{
}


/** Query the interfaces. */
StatusCode ISF::G4PolyconeGeoIDSvc::queryInterface(const InterfaceID& riid, void** ppvInterface)
{
  if ( ISF::IID_IGeoIDSvc == riid )
    {
      *ppvInterface = (IGeoIDSvc*)this;
      addRef();
      return StatusCode::SUCCESS;
    }
  // Interface is not directly available: try out a base class
  return AthService::queryInterface(riid, ppvInterface);
}


// Athena algtool's Hooks
StatusCode  ISF::G4PolyconeGeoIDSvc::initialize()
{
  ATH_MSG_INFO("initialize()");

  // retrieve envelope definition service
  ATH_CHECK( m_envDefSvc.retrieve() );

  // create internal volume representations for the given dimensions
  for (int geoID=AtlasDetDescr::fFirstAtlasRegion; geoID<AtlasDetDescr::fNumAtlasRegions; ++geoID)
    {
      if ( createVolume( AtlasDetDescr::AtlasRegion(geoID)).isFailure())
        {
          ATH_MSG_ERROR("Unable to create volume representation for geoID="<<geoID);
        }
    }

  // setup the converter for: G4 enum to ISF::InsideType
  m_typeConverter[kOutside] = ISF::fOutside;
  m_typeConverter[kSurface] = ISF::fSurface;
  m_typeConverter[kInside]  = ISF::fInside;

  ATH_MSG_INFO("initialize() successful");
  return StatusCode::SUCCESS;
}


StatusCode  ISF::G4PolyconeGeoIDSvc::finalize()
{
  ATH_MSG_INFO("finalize() successful");
  return StatusCode::SUCCESS;
}


ISF::InsideType ISF::G4PolyconeGeoIDSvc::inside(const Amg::Vector3D &pos, AtlasDetDescr::AtlasRegion geoID) const
{
  // initialized with default return value
  InsideType where = ISF::fOutside;

  // check for volume to be present
  G4VSolid *curVol = m_volume[geoID];
  if (curVol)
    {
      G4ThreeVector posG4( pos.x(), pos.y(), pos.z() );
      EInside g4Where = curVol->Inside( posG4 );

      // use this trick to quickly convert G4 type to ISF::InsideType
      where = m_typeConverter[g4Where];
    }

  // hand back the position information
  return where;
}


AtlasDetDescr::AtlasRegion ISF::G4PolyconeGeoIDSvc::identifyGeoID(const Amg::Vector3D &pos) const
{
  // loop over geoIDs
  for (unsigned short geoID=AtlasDetDescr::fFirstAtlasRegion; geoID<AtlasDetDescr::fNumAtlasRegions; ++geoID)
    {
      // point inside given geoID -> return geoID
      if ( inside(pos, AtlasDetDescr::AtlasRegion(geoID))==ISF::fInside)
        {
          return AtlasDetDescr::AtlasRegion(geoID);
        }
    }
  // point is not inside any of the registered geoIDs
  return AtlasDetDescr::fUndefinedAtlasRegion;
}


AtlasDetDescr::AtlasRegion ISF::G4PolyconeGeoIDSvc::identifyNextGeoID(const Amg::Vector3D &pos,
                                                                      const Amg::Vector3D &dir) const
{

  // if position inside a volume -> return that volume as the next geoID
  AtlasDetDescr::AtlasRegion geoID = identifyGeoID(pos);
  if validAtlasRegion(geoID)
    {
      return geoID;
    }
  //not inside - make a step towards the p direction and check if inside again
  const Amg::Vector3D &dirUnit = dir.unit();
  const Amg::Vector3D &posStep = pos+dirUnit;
  AtlasDetDescr::AtlasRegion closestGeoID = identifyGeoID(posStep);
  if validAtlasRegion(closestGeoID)
    {
      return closestGeoID;
    }
  return AtlasDetDescr::fUndefinedAtlasRegion;
}


StatusCode ISF::G4PolyconeGeoIDSvc::createVolume(AtlasDetDescr::AtlasRegion geoID)
{

  // ensure a proper numeric value for geoID
  assertAtlasRegion( geoID);

  ATH_MSG_INFO( "Building envelope volume for '" << AtlasDetDescr::AtlasRegionHelper::getName(geoID)
                << "' (GeoID="<< geoID << ").");

  // resetting volume for current geoID
  m_volume[geoID] = 0;

  // volume definitions
  const double phimin    = 0.;
  const double deltaphi  = 360.*CLHEP::deg;
  const std::string volumeName( AtlasDetDescr::AtlasRegionHelper::getName(geoID));

  {
    // retrieve the vector of (r,z) values from the EnvelopeDefSvc
    const RZPairVector &rz = m_envDefSvc->getRZBoundary( geoID );

    // entries in the database table
    size_t curVolNumPoints = rz.size();
    if ( !curVolNumPoints)
      {
        ATH_MSG_ERROR("No entries for " << AtlasDetDescr::AtlasRegionHelper::getName(geoID) << " envelope in envelope definition service. Unable to build envelope.");
        // resources cleanup
        return StatusCode::FAILURE;
      }

    // arrays of (r,z) coordinates of the current envelope volume
    double *z = new double[curVolNumPoints];
    double *r = new double[curVolNumPoints];

    // convert vector of pairs into two arrays
    RZPairVector::const_iterator rzIt = rz.begin();
    for ( size_t i=0; i<curVolNumPoints; ++i)
      {
        r[i] = rzIt->first;
        z[i] = rzIt->second;

        ++rzIt;
      }

    // create a G4Polycone volume with the given dimensions
    std::stringstream curName;
    curName << volumeName;
    // store it in the m_volume array
    m_volume[geoID] = new G4Polycone( curName.str(), phimin, deltaphi, curVolNumPoints, r, z);
    // set the volume name
    m_volume[geoID]->SetName( volumeName);

    // cleanup
    delete[] r;
    delete[] z;
  }

  return StatusCode::SUCCESS;
}

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// GenericParticleOrderingTool.cxx, (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////

// class header include
#include "GenericParticleOrderingTool.h"

// ISF includes
#include "ISF_Event/ISFParticle.h"

/** Constructor **/
ISF::GenericParticleOrderingTool::GenericParticleOrderingTool(const std::string& t, const std::string& n, const IInterface* p) :
  AthAlgTool(t,n,p),
  m_geoIDorder(),
  m_simIDorder()
{
  declareInterface<ISF::IParticleOrderingTool>(this);

  declareProperty( "OrderID",
                   m_geoIDorder[AtlasDetDescr::fAtlasID]=1,
                   "Order for Inner Detector particles");
  declareProperty( "OrderBeamPipe",
                   m_geoIDorder[AtlasDetDescr::fAtlasForward]=1,
                   "Order for Beam Pipe / Forward particles");
  declareProperty( "OrderCalo",
                   m_geoIDorder[AtlasDetDescr::fAtlasCalo]=1,
                   "Order for Calorimeter particles");
  declareProperty( "OrderMS",
                   m_geoIDorder[AtlasDetDescr::fAtlasMS]=1,
                   "Order for Muon Spectrometer particles");
  declareProperty( "OrderCavern",
                   m_geoIDorder[AtlasDetDescr::fAtlasCavern]=1,
                   "Order for Cavern particles");
}

/** Destructor **/
ISF::GenericParticleOrderingTool::~GenericParticleOrderingTool()
{

}

/** Athena algtool's Hooks */
StatusCode  ISF::GenericParticleOrderingTool::initialize()
{
  ATH_MSG_INFO("initialize() ...");

  ATH_MSG_INFO("initialize() successful");
  return StatusCode::SUCCESS;
}


/** Athena algtool's Hooks */
StatusCode  ISF::GenericParticleOrderingTool::finalize()
{
  ATH_MSG_INFO("finalize() ...");

  ATH_MSG_INFO("finalize() successful");
  return StatusCode::SUCCESS;
}


/** does the particle pass the this filter? */
ISF::ParticleOrder ISF::GenericParticleOrderingTool::computeOrder(const ISFParticle &p) const {
  // return order set for the current particle geoID
  AtlasDetDescr::AtlasRegion geoID = p.nextGeoID();
  ISF::SimSvcID simID = p.nextSimID();

  // NB: fUndefinedSimID(==0) will get order 0
  return m_geoIDorder[geoID]*simID;
}

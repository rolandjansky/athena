/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// ParticleHelper.cxx, (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////

// class header include
#include "ParticleHelper.h"

// ISF includes
#include "ISF_Event/ISFParticle.h"

// Barcode
#include "BarcodeInterfaces/IBarcodeSvc.h"


/** Constructor **/
ISF::ParticleHelper::ParticleHelper(const std::string& t, const std::string& n, const IInterface* p) :
  base_class(t,n,p),
  m_barcodeSvc("BarcodeSvc",n)
{
  // different options
  declareProperty("BarcodeSvc",               m_barcodeSvc);

}


// Athena algtool's Hooks
StatusCode  ISF::ParticleHelper::initialize()
{
  ATH_MSG_VERBOSE("Initializing ...");
  return StatusCode::SUCCESS;
}


/** Create a new particle */
ISF::ISFParticle* ISF::ParticleHelper::createParticle(double x, double y, double z,
                                                      double px, double py, double pz,
                                                      double pMass,
                                                      double pCharge,
                                                      int pPdgId,
                                                      double pTime,
                                                      const ISFParticle &parent,
                                                      Barcode::ParticleBarcode bc,
                                                      TruthBinding* tBinding) const {
  return new ISF::ISFParticle( Amg::Vector3D(x,y,z),
                               Amg::Vector3D(px,py,pz),
                               pMass,
                               pCharge,
                               pPdgId,
                               pTime,
                               parent,
                               bc,
                               tBinding);
}


/** Create a new particle */
ISF::ISFParticle* ISF::ParticleHelper::createParticle( const Amg::Vector3D& x,
                                                       const Amg::Vector3D& p,
                                                       double pMass,
                                                       double pCharge,
                                                       int pPdgId,
                                                       double pTime,
                                                       const ISFParticle &parent,
                                                       Barcode::ParticleBarcode bc,
                                                       TruthBinding* tBinding) const {
  return new ISF::ISFParticle( x,
                               p,
                               pMass,
                               pCharge,
                               pPdgId,
                               pTime,
                               parent,
                               bc,
                               tBinding);
}

/** Create a new particle */
ISF::ISFParticle* ISF::ParticleHelper::createParticle( const HepGeom::Point3D<double>& pos,
                                                       const HepGeom::Vector3D<double>& mom,
                                                       double pMass,
                                                       double pCharge,
                                                       int pPdgId,
                                                       double pTime,
                                                       const ISFParticle &parent,
                                                       Barcode::ParticleBarcode bc,
                                                       TruthBinding* tBinding) const {
  return createParticle( pos.x(), pos.y(), pos.z(),
                         mom.x(), mom.y(), mom.z(),
                         pMass,
                         pCharge,
                         pPdgId,
                         pTime,
                         parent,
                         bc,
                         tBinding );
}

/** Returns the Particle Stack, should register truth */
ISF::ISFParticle* ISF::ParticleHelper::updatedParticle(const ISFParticle& origIsp,
                                                       const Amg::Vector3D& updatedPos,
                                                       const Amg::Vector3D& updatedMom,
                                                       double deltaTime) const
{
  return new ISF::ISFParticle( updatedPos,
                               updatedMom,
                               origIsp.mass(),
                               origIsp.charge(),
                               origIsp.pdgCode(),
                               origIsp.timeStamp()+deltaTime,
                               origIsp,
                               origIsp.barcode(),
                               origIsp.getTruthBinding() ? new TruthBinding(*origIsp.getTruthBinding()) : nullptr);
}

/** Returns the Particle Stack, should register truth */
ISF::ISFParticle* ISF::ParticleHelper::updatedParticle(const ISFParticle& origIsp,
                                                       const HepGeom::Point3D<double>&  updatedPos,
                                                       const HepGeom::Vector3D<double>& updatedMom,
                                                       double deltaTime) const
{
  return new ISF::ISFParticle( Amg::Vector3D( updatedPos.x(), updatedPos.y(), updatedPos.z()),
                               Amg::Vector3D( updatedMom.x(), updatedPos.x(), updatedPos.z()),
                               origIsp.mass(),
                               origIsp.charge(),
                               origIsp.pdgCode(),
                               origIsp.timeStamp()+deltaTime,
                               origIsp,
                               origIsp.barcode(),
                               origIsp.getTruthBinding() ? new TruthBinding(*origIsp.getTruthBinding()) : nullptr);
}


StatusCode  ISF::ParticleHelper::finalize()
{
  ATH_MSG_VERBOSE("Finalizing ...");
  return StatusCode::SUCCESS;
}

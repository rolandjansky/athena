/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "xAODEgamma/EgammaxAODHelpers.h"

#include "xAODEgamma/Egamma.h"
#include "xAODEgamma/Electron.h"
#include "xAODEgamma/Photon.h"

#include "xAODEgamma/ElectronContainer.h"
#include "xAODEgamma/PhotonContainer.h"


#include "xAODTracking/Vertex.h"
#include "xAODTracking/TrackParticle.h"
#include "xAODCaloEvent/CaloCluster.h"

bool xAOD::EgammaHelpers::isElectron(const xAOD::Egamma *eg){
  return ( (eg->type()==xAOD::Type::Electron) && 
	   (!eg->author(xAOD::EgammaParameters::AuthorFwdElectron)));
}


bool xAOD::EgammaHelpers::isFwdElectron(const xAOD::Egamma *eg){
  return eg->author(xAOD::EgammaParameters::AuthorFwdElectron);
}

bool xAOD::EgammaHelpers::isPhoton(const xAOD::Egamma *eg){
  return (eg->type()==xAOD::Type::Photon);
}

bool xAOD::EgammaHelpers::isConvertedPhoton(const xAOD::Egamma *eg){
  const xAOD::Photon *ph = dynamic_cast<const xAOD::Photon*>(eg);
  return (ph && (ph->nVertices()>0) );
}

// ==================================================================
size_t xAOD::EgammaHelpers::numberOfSiHits(const xAOD::TrackParticle *tp)
{
  if (!tp) return 0;
  
  uint8_t dummy(0), nSiHits(0);
  if (tp->summaryValue(dummy, xAOD::numberOfPixelHits))
    nSiHits += dummy;
  if (tp->summaryValue(dummy, xAOD::numberOfSCTHits))
    nSiHits += dummy;
  return nSiHits;
}  

// ==================================================================
bool xAOD::EgammaHelpers::isBarrel(const xAOD::Egamma *eg)
{
  return (eg ? isBarrel(eg->caloCluster()) : false);
}

bool xAOD::EgammaHelpers::isBarrel(const xAOD::CaloCluster *cluster)
{
  if (cluster->inBarrel() &&  cluster->inEndcap()){
    return  cluster->eSample(CaloSampling::EMB2) >= cluster->eSample(CaloSampling::EME2);
  }
  return cluster->inBarrel();
}

// ==================================================================
const std::set<const xAOD::TrackParticle*> xAOD::EgammaHelpers::getTrackParticles(const xAOD::Egamma *eg, 
  bool useBremAssoc /* = true */, bool allParticles /* = true */)
{  
  if (!eg) return std::set<const xAOD::TrackParticle*>();

  if (eg->type()==xAOD::Type::Electron) {
    const xAOD::Electron* el = static_cast<const xAOD::Electron*> (eg);
    if (el) return getTrackParticles(el, useBremAssoc, allParticles);
  }
  if (eg->type()==xAOD::Type::Photon) {
    const xAOD::Photon* ph = static_cast<const xAOD::Photon*> (eg);  
    if (ph) return getTrackParticles(ph, useBremAssoc);
  }

  std::cerr << "egamma object not electron nor photon" << std::endl;
  return std::set<const xAOD::TrackParticle*>();
}

// ==================================================================
const std::set<const xAOD::TrackParticle*> xAOD::EgammaHelpers::getTrackParticles(const xAOD::Electron* el, 
  bool useBremAssoc /* = true */, bool allParticles /* = true */)
{
  std::set<const xAOD::TrackParticle*> tps;
  for (unsigned int i = 0; i < el->nTrackParticles(); ++i)
  {
    const xAOD::TrackParticle* tp = el->trackParticle(i);
    if (useBremAssoc) tp = xAOD::EgammaHelpers::getOriginalTrackParticleFromGSF(tp);
    if (tp) tps.insert( tp );
    if (!allParticles) break; // break after first particle
  }
  return tps;
}

// ==================================================================
const std::set<const xAOD::TrackParticle*> xAOD::EgammaHelpers::getTrackParticles(const xAOD::Photon* ph,
  bool useBremAssoc /* = true */)
{
  std::set<const xAOD::TrackParticle*> tps;
  if (!ph) return tps;
  const xAOD::Vertex* vx = ph->vertex();
  for (unsigned int i=0; vx && i < vx->nTrackParticles(); ++i)
  {
    const xAOD::TrackParticle *tp = vx->trackParticle(i);
    tps.insert( useBremAssoc ? xAOD::EgammaHelpers::getOriginalTrackParticleFromGSF(tp) : tp );
  }
  return tps;
}

// ==================================================================

const xAOD::TruthParticle* xAOD::EgammaHelpers::getTruthParticle(const xAOD::IParticle* particle, bool debug)
{
  return getLink<xAOD::TruthParticle>(particle, "truthParticleLink", debug);
}

const xAOD::Electron* xAOD::EgammaHelpers::getRecoElectron(const xAOD::TruthParticle* particle, bool debug)
{
  return getLink<xAOD::Electron>(particle, "recoElectronLink", debug);
}

const xAOD::Photon* xAOD::EgammaHelpers::getRecoPhoton(const xAOD::TruthParticle* particle, bool debug)
{
  return getLink<xAOD::Photon>(particle, "recoPhotonLink", debug);
}


/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "xAODEgamma/EgammaxAODHelpers.h"

#include "xAODEgamma/Egamma.h"
#include "xAODEgamma/Photon.h"
#include "xAODEgamma/Electron.h"

#include "xAODEgamma/ElectronContainer.h"
#include "xAODEgamma/PhotonContainer.h"

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

const xAOD::TruthParticle* xAOD::EgammaHelpers::getTruthParticle(const xAOD::IParticle* particle, bool debug /* =false */)
{
  return getLink<xAOD::TruthParticle>(particle, "truthParticleLink", debug);
}


int xAOD::EgammaHelpers::getParticleTruthType(const xAOD::IParticle* particle){
  static SG::AuxElement::Accessor<int> tT("truthType") ;
  if (!tT.isAvailable(*particle)) return 0;
  return tT(*particle);
}


int xAOD::EgammaHelpers::getParticleTruthOrigin(const xAOD::IParticle* particle){
  static SG::AuxElement::Accessor<int> tO("truthOrigin") ;
  if (!tO.isAvailable(*particle)) return 0;
  return tO(*particle);
}

const xAOD::Electron* xAOD::EgammaHelpers::getRecoElectron(const xAOD::TruthParticle* particle, bool debug /* =false */)
{
  return getLink<xAOD::Electron>(particle, "recoElectronLink", debug);
}

const xAOD::Photon* xAOD::EgammaHelpers::getRecoPhoton(const xAOD::TruthParticle* particle, bool debug /* =false */)
{
  return getLink<xAOD::Photon>(particle, "recoPhotonLink", debug);
}


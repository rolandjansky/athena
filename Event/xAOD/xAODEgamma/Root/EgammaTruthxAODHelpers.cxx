/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "xAODEgamma/EgammaTruthxAODHelpers.h"
#include "xAODEgamma/Egamma.h"
#include "xAODEgamma/Photon.h"
#include "xAODEgamma/Electron.h"
#include "xAODTruth/TruthVertex.h"
#include "xAODTruth/TruthParticle.h"

#include "xAODTruth/TruthParticleContainer.h"
#include "xAODEgamma/ElectronContainer.h"
#include "xAODEgamma/PhotonContainer.h"

// ==================================================================
const xAOD::TruthParticle* xAOD::EgammaHelpers::getTruthParticle(const xAOD::IParticle* particle, bool debug /* =false */){
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

// ==================================================================

const xAOD::Electron* xAOD::EgammaHelpers::getRecoElectron(const xAOD::TruthParticle* particle, bool debug /* =false */){
  return getLink<xAOD::Electron>(particle, "recoElectronLink", debug);
}

const xAOD::Photon* xAOD::EgammaHelpers::getRecoPhoton(const xAOD::TruthParticle* particle, bool debug /* =false */){
  return getLink<xAOD::Photon>(particle, "recoPhotonLink", debug);
}

// ==================================================================

//Is the object matched to a true converted photon with R < maxRadius
bool xAOD::EgammaHelpers::isTrueConvertedPhoton(const xAOD::Photon* ph, float maxRadius /* = 800. */){
  const xAOD::TruthParticle *truthPh = xAOD::EgammaHelpers::getTruthParticle(ph);
  if (!truthPh) {return false;}

  // In older versions egammaTruthParticles did not have a decay vertex associated
  // so we look for the original truth particle
  if (truthPh->hasDecayVtx()){ 
    return isTrueConvertedPhoton(truthPh, maxRadius);
  }
  const xAOD::TruthParticle *orgTruthPh = xAOD::EgammaHelpers::getTruthParticle(truthPh);
  if (!orgTruthPh){ return false;}
  return xAOD::EgammaHelpers::isTrueConvertedPhoton(orgTruthPh, maxRadius);
}

//Is the true object a converted photon with R < maxRadius
bool xAOD::EgammaHelpers::isTrueConvertedPhoton(const xAOD::TruthParticle* truthPh, float maxRadius /*= 800.*/){
  return (truthPh->pdgId() == 22 && truthPh->hasDecayVtx() 
	  && truthPh->decayVtx()->perp() < maxRadius); 
}

// ==================================================================
const xAOD::TruthParticle* xAOD::EgammaHelpers::getMotherElectron(const xAOD::Electron* el){
  
  const xAOD::TruthParticle *truthel = xAOD::EgammaHelpers::getTruthParticle(el);
  return getMotherElectron(truthel);

}
const xAOD::TruthParticle* xAOD::EgammaHelpers::getMotherElectron(const xAOD::TruthParticle* truthel){
  if (!truthel || truthel->absPdgId()!=11){ //Truth must exist and be an electron
    return 0;
  }

  for  (size_t i =0;i < truthel->nParents();++i){ //Loop over the truth electron parents
    const xAOD::TruthParticle *elparent = truthel->parent(i);
    if(elparent->absPdgId() ==11){ //e->gamma+e case
       return elparent;
    }
    else if(elparent->pdgId() ==22) { //Photon parent, lets check if it comes from an electron
      //Loop over the truth  photon parents (electron grandparents)
      for  (size_t j =0 ;j<elparent->nParents();++j){

	const xAOD::TruthParticle *elgrandparent = elparent->parent(j);
	if(elgrandparent->absPdgId() ==11){ //Photon comes from an electron
	  return elgrandparent;
	}
      }//Loop over grandparents
    } // Parent is Photon
  }//Loop over parents

  return 0;
}
// ==================================================================

/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#include "xAODEgamma/EgammaTruthxAODHelpers.h"
#include "xAODEgamma/Egamma.h"
#include "xAODEgamma/Photon.h"
#include "xAODEgamma/Electron.h"
#include "xAODTruth/TruthVertex.h"
#include "xAODTruth/TruthParticle.h"
#include "xAODTruth/xAODTruthHelpers.h"
#include "xAODTruth/TruthParticleContainer.h"
#include "xAODEgamma/ElectronContainer.h"
#include "xAODEgamma/PhotonContainer.h"

// ==================================================================

/// Accessor for the "recoElectronLink" dynamic variable
///
/// It is declared outside of the @c xAOD::EgammaHelpers::getRecoElectron(...)
/// call to make sure that the auxiliary ID registry would know about this type
/// as soon as the library holding this code is loaded.
///
static const SG::AuxElement::Accessor< ElementLink< xAOD::ElectronContainer > >
   recoElectronLinkAcc( "recoElectronLink" );

const xAOD::Electron*
xAOD::EgammaHelpers::getRecoElectron( const xAOD::TruthParticle* particle ) {

   if( ! recoElectronLinkAcc.isAvailable( *particle ) ) {
      return nullptr;
   }
   const ElementLink< xAOD::ElectronContainer >& link =
      recoElectronLinkAcc( *particle );
   if( ! link.isValid() ) {
      return nullptr;
   }
   return *link;
}

/// Accessor for the "recoPhotonLink" dynamic variable
///
/// It is declared outside of the @c xAOD::EgammaHelpers::getRecoPhoton(...)
/// call to make sure that the auxiliary ID registry would know about this type
/// as soon as the library holding this code is loaded.
///
static const SG::AuxElement::Accessor< ElementLink< xAOD::PhotonContainer > >
   recoPhotonLinkAcc( "recoPhotonLink" );

const xAOD::Photon*
xAOD::EgammaHelpers::getRecoPhoton( const xAOD::TruthParticle* particle ) {

   if( ! recoPhotonLinkAcc.isAvailable( *particle ) ) {
      return nullptr;
   }
   const ElementLink< xAOD::PhotonContainer >& link =
      recoPhotonLinkAcc( *particle );
   if( ! link.isValid() ) {
      return nullptr;
   }
   return *link;
}
// ==================================================================

//Is the object matched to a true converted photon with R < maxRadius
bool xAOD::EgammaHelpers::isTrueConvertedPhoton(const xAOD::Photon* ph, float maxRadius /* = 800. */){
  const xAOD::TruthParticle *truthPh = xAOD::TruthHelpers::getTruthParticle(*ph);
  if (!truthPh) {return false;}
  // In older versions egammaTruthParticles did not have a decay vertex associated
  // so we look for the original truth particle
  if (truthPh->hasDecayVtx()){ 
    return isTrueConvertedPhoton(truthPh, maxRadius);
  }
  const xAOD::TruthParticle *orgTruthPh = xAOD::TruthHelpers::getTruthParticle(*truthPh);
  if (!orgTruthPh){ return false;}
  return xAOD::EgammaHelpers::isTrueConvertedPhoton(orgTruthPh, maxRadius);
}

//Is the true object a converted photon with R < maxRadius
bool xAOD::EgammaHelpers::isTrueConvertedPhoton(const xAOD::TruthParticle* truthPh, float maxRadius /*= 800.*/){
  return (truthPh->pdgId() == 22 && truthPh->hasDecayVtx() 
	  && truthPh->decayVtx()->perp() < maxRadius); 
}

// ==================================================================

const xAOD::TruthParticle* xAOD::EgammaHelpers::getBkgElectronMother(const xAOD::Electron* el,  const int barcodecut/*=0*/){ 
  const xAOD::TruthParticle *truthel = xAOD::TruthHelpers::getTruthParticle(*el);
  return getBkgElectronMother(truthel,barcodecut);
}

const xAOD::TruthParticle* xAOD::EgammaHelpers::getBkgElectronMother(const xAOD::TruthParticle* truthel, const  int barcodecut/*=0*/){

  std::vector<const xAOD::TruthParticle*>  vec = xAOD::EgammaHelpers::getBkgElectronLineage(truthel,barcodecut);
  if(!vec.empty()){ 
    return vec.back();
  }
  return nullptr;
}

std::vector<const xAOD::TruthParticle*> 
xAOD::EgammaHelpers::getBkgElectronLineage(const xAOD::Electron* el,const int barcodecut/*=0*/){
  const xAOD::TruthParticle *truthel = xAOD::TruthHelpers::getTruthParticle(*el);
  return getBkgElectronLineage(truthel,barcodecut);
}

//The actual implementation code 
std::vector<const xAOD::TruthParticle*> 
xAOD::EgammaHelpers::getBkgElectronLineage(const xAOD::TruthParticle* truthel,const int barcodecut/*=0*/){
  std::vector<const xAOD::TruthParticle*> vec;
  //Truth must exist and be an electron
  if (!truthel || truthel->absPdgId()!=11){ 
    return vec;
  }
  vec.push_back(truthel); //push its self back as first entry

  // The first parent has to exist
  if ( !truthel->nParents() || abs(truthel->barcode())<barcodecut ){
    return vec;
  }
  //And has to be a photon or electron
  const xAOD::TruthParticle* parent = truthel->parent();
  if(parent->absPdgId() !=22 && parent->absPdgId() !=11){
    return vec;
  }
  
  vec.push_back(parent); //push in the parent as the second entry

  //Loop over the generations
  while (parent->nParents() && abs(parent->barcode())>=barcodecut){ 
    //Find the next parent
    const xAOD::TruthParticle* tmp = parent->parent();
    //You want to see an electron or a photon 
    if((tmp->absPdgId() ==22 || tmp->absPdgId() ==11) ){
      parent=tmp;
    }
    else{ // if we do not see any more electron and photons we stop
      break; 
    }
    vec.push_back(parent); //push in the parent
  }
  return vec;
}


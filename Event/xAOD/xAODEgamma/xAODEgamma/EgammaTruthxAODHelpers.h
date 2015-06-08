// Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: EgammaTruthxAODHelpers.h 668505 2015-05-19 19:23:05Z christos $
#ifndef XAOD_EGAMMATRUTHXAODHELPERS_H
#define XAOD_EGAMMATRUTHXAODHELPERS_H

#include "xAODEgamma/EgammaFwd.h"
#include "xAODEgamma/ElectronFwd.h"
#include "xAODEgamma/PhotonFwd.h"
#include "xAODTracking/TrackParticleFwd.h"
#include "xAODTruth/TruthParticleContainer.h"
#include "xAODTruth/TruthParticle.h"

#include <cstddef>
#include <set>

namespace xAOD {

  namespace EgammaHelpers{
    
    /// @brief Access to element link to object of type T stored in auxdata
    template<class T>
    const T* getLink(const xAOD::IParticle* particle, std::string name){
      if (!particle) return 0;
      typedef ElementLink< DataVector<T> > Link_t;
      
      if (!particle->isAvailable< Link_t >(name) ) { 
	  return 0; 
	}  
      const Link_t link = particle->auxdata<Link_t>(name);
      if (!link.isValid()) { 
	  return 0; 
	}
      return *link;
    }
    
    ///@brief return the reco electron associated to the given TruthParticle (if any)
    const xAOD::Electron* getRecoElectron(const xAOD::TruthParticle*);
    
    ///@brief return the reco photon associated to the given TruthParticle (if any)
    const xAOD::Photon* getRecoPhoton(const xAOD::TruthParticle* particle);
        
    ///@brief is the object matched to a true converted photon with R < maxRadius
    bool isTrueConvertedPhoton(const xAOD::Photon* ph, float maxRadius = 800.);

    ///@brief is the true object a converted photon with R < maxRadius
    bool isTrueConvertedPhoton(const xAOD::TruthParticle* truePh, float maxRadius = 800.);

    ///@brief Helper wrapper function for calling the function below that accepts truth input
    /// via a reco object
    const xAOD::TruthParticle* getBkgElectronMother(const xAOD::Electron* el);


    ///@brief Helper function for getting the True "Mother" electron for an existing electron.
    ///There are cases when an electron has  a  photon (or electron) mother, that in turn comes
    /// from another particle (possible leading to a Z or W etc).
    /// This method will navigate back up to the last electron or photon it can find
    /// in the lineage of the original true electron.
    /// Then the user just needs to check the mother of that last electron/photon, returned from the method.
    /// To see if we have a loose matched isolated electron (its mother would be Z,W etc)
    /// or background.
    ///If it fails returns null pointer.
    const xAOD::TruthParticle* getBkgElectronMother(const xAOD::TruthParticle* truthel);


        
  }// EgammaHelpers

} // namespace xAOD
  
#endif // XAOD_EGAMMAXAODHELPERS_H

// Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: EgammaTruthxAODHelpers.h 636327 2014-12-16 16:34:24Z christos $
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
    const T* getLink(const xAOD::IParticle* particle, std::string name, bool debug=false){
      if (!particle) return 0;
      typedef ElementLink< DataVector<T> > Link_t;
      
      if (!particle->isAvailable< Link_t >(name) ) 
	{ 
	  if (debug) std::cerr<< "Link not available" << std::endl; 
	  return 0; 
	}  
      const Link_t link = particle->auxdata<Link_t>(name);
      if (!link.isValid()) 
	{ 
	  if (debug) std::cerr << "Invalid link" << std::endl; 
	  return 0; 
	}
      return *link;
    }
    
    ///@brief return the truthParticle associated to the given IParticle (if any)
    const xAOD::TruthParticle* getTruthParticle(const xAOD::IParticle*, bool debug = false);

    ///@brief return the  particle's truth Type (as defined by the MC Truth Classifier) 
    int getParticleTruthType(const xAOD::IParticle* particle);
    
    ///@brief return the  particle's Truth Origin (as defined by the MC Truth Classifier) 
    int getParticleTruthOrigin(const xAOD::IParticle* particle);
    
    ///@brief return the reco electron associated to the given TruthParticle (if any)
    const xAOD::Electron* getRecoElectron(const xAOD::TruthParticle*, bool debug = false);
    
    ///@brief return the reco photon associated to the given TruthParticle (if any)
    const xAOD::Photon* getRecoPhoton(const xAOD::TruthParticle* particle, bool debug = false);
        
    ///@brief is the object matched to a true converted photon with R < maxRadius
    bool isTrueConvertedPhoton(const xAOD::Photon* ph, float maxRadius = 800.);

    ///@brief is the true object a converted photon with R < maxRadius
    bool isTrueConvertedPhoton(const xAOD::TruthParticle* truePh, float maxRadius = 800.);

    ///@brief Helper function for getting the Truth "Mother" electron for an existing electron
    ///i.e for e->photon->e cases (brem) , or e->gamma+e (fsr),  
    ///where we have matched the second electron as the best.
    ///If the input is not a true electron or no Electron Mother is found
    ///return null pointer.
    const xAOD::TruthParticle* getMotherElectron(const xAOD::Electron* el);

    ///@brief Helper function for getting the Truth "Mother" electron for a truth electron
    ///i.e for e->photon->e cases (brem) , or e->gamma+e (fsr),  
    ///where we have matched the second electron as the best.
    ///If the input is not a true electron or no Electron Mother is found
    /// return null pointer.
    const xAOD::TruthParticle* getMotherElectron(const xAOD::TruthParticle* truthel);

        
  }// EgammaHelpers

} // namespace xAOD
  
#endif // XAOD_EGAMMAXAODHELPERS_H

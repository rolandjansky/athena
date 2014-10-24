// Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: EgammaxAODHelpers.h 617854 2014-09-20 18:49:22Z christos $
#ifndef XAOD_EGAMMAXAODHELPERS_H
#define XAOD_EGAMMAXAODHELPERS_H

#include "xAODEgamma/EgammaFwd.h"
#include "xAODCaloEvent/CaloClusterFwd.h"

//Truth Includes
#include "xAODTruth/TruthParticleContainer.h"
#include "xAODTruth/TruthParticle.h"

// Include helpers for electrons and photons: the user only needs to include this file
#include "xAODEgamma/ElectronxAODHelpers.h"
#include "xAODEgamma/PhotonxAODHelpers.h"

#include <cstddef>
#include <set>

namespace xAOD {

  namespace EgammaHelpers{
    


    ///@brief is the object an electron (not Fwd) 
    bool isElectron(const xAOD::Egamma *eg);

    ///@brief is the object a Fwd  electron
    bool isFwdElectron(const xAOD::Egamma *eg);

    ///@brief  is the object a photon
    bool isPhoton(const xAOD::Egamma *eg);    

    ///@brief is the object a converted photon
    bool isConvertedPhoton(const xAOD::Egamma *eg);
    
    
    ///@brief return true if the cluster is in the barrel
    bool isBarrel(const xAOD::Egamma *eg);
    
    ///@brief return true if the cluster (or the majority of its energy) is in the barrel
    bool isBarrel(const xAOD::CaloCluster *cluster);

    /** Return a list of all or only the best TrackParticle associated to the object. 
      * (used for track isolation)
      * If useBremAssoc is set, get the original TrackParticle **/
    const std::set<const xAOD::TrackParticle*> getTrackParticles(const xAOD::Egamma *eg,
      bool useBremAssoc = true, bool allParticles = true);
    
    // @brief Access to element link to object of type T stored in auxdata
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
        
  }// EgammaHelpers

} // namespace xAOD
  
#endif // XAOD_EGAMMAXAODHELPERS_H

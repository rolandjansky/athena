// Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: EgammaxAODHelpers.h 788453 2016-12-07 15:40:30Z christos $
#ifndef XAOD_EGAMMAXAODHELPERS_H
#define XAOD_EGAMMAXAODHELPERS_H

#include "xAODEgamma/EgammaFwd.h"
#include "xAODCaloEvent/CaloClusterFwd.h"
#include "xAODTracking/TrackParticle.h"
#include "xAODCaloEvent/CaloClusterContainer.h"
// Include all other helpers: The user only needs to include this file
#include "xAODEgamma/ElectronxAODHelpers.h"
#include "xAODEgamma/PhotonxAODHelpers.h"
#include "xAODEgamma/EgammaTruthxAODHelpers.h"

//other includes
#include <cstddef>
#include <set>
#include <vector>


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

    ///@brief Return a vector of all the elementlinks to the topo clusters associated with the egamma cluster
    const std::vector< ElementLink< xAOD::CaloClusterContainer > > getAssociatedTopoClustersLinks(const xAOD::CaloCluster *cluster); 
    
    ///@brief Return a vector of all the topo clusters associated with the egamma cluster
    const std::vector<const xAOD::CaloCluster*> getAssociatedTopoClusters(const xAOD::CaloCluster *cluster);

    ///@brief Return a list of all or only the best TrackParticle associated to the object. 
    ///If useBremAssoc is set, get the original TrackParticle 
    ///This is useful when a std::set of the original track Particles is required, which is mainly the case for the 
    //isolation interface,
    ///as it will re-order the elements in pointer order and not best match.
    const std::set<const xAOD::TrackParticle*> getTrackParticles(const xAOD::Egamma *eg,
								 bool useBremAssoc = true, bool allParticles = true);

    ///@brief Return a list of all or only the best TrackParticle associated to the object. 
    ///If useBremAssoc is set, get the original TrackParticle 
    ///This one returns a vector so as to be more "user friendly", as it retains the original
    ///best match ordering
    const std::vector<const xAOD::TrackParticle*> getTrackParticlesVec(const xAOD::Egamma *eg,
								       bool useBremAssoc = true, bool allParticles = true);


    ///@brief return the summary value for a TrackParticle or default value (-999)
    /// (to be used mostly in python where uint8_t is converted to char and the Tracking does not provide unprotected methods)
    int summaryValueInt(const xAOD::TrackParticle& tp, const xAOD::SummaryType& info, int deflt = -999);
    
    ///@brief return the summary value for a TrackParticle or default value (-999)
    float summaryValueFloat(const xAOD::TrackParticle& tp, const xAOD::SummaryType& info, int deflt = -999.);
    
  }// EgammaHelpers

} // namespace xAOD
  
#endif // XAOD_EGAMMAXAODHELPERS_H

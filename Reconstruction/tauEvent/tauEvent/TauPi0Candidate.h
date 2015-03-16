/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**
 * @brief Tau Pi0 Candidate 
 *
 * (detailed description)
 *
 * @author Felix Friedrich 
 * @author Will Davey
 * @author Benedict Winter
 */

#ifndef __TAU_Pi0_CANDIDATE_H__
#define __TAU_Pi0_CANDIDATE_H__

#include <iostream>

//#include "CaloEvent/CaloCluster.h"
//#include "CaloEvent/CaloClusterContainer.h"
//#include "DataModel/ElementLink.h"
//#include "DataModel/ElementLinkVector.h"
#include "tauEvent/TauPi0Cluster.h"

namespace Analysis {

  class TauPi0Candidate {

    public:
    /// default constructor
    TauPi0Candidate();
    
    /// copy constructor
    /// @param rhs TauPi0Candidate instance to assign from
    TauPi0Candidate( const TauPi0Candidate& rhs );
    
    /// destructor (virtual)
    virtual ~TauPi0Candidate();

    /// assignment operator
    /// @param rhs TauPi0Candidate instance to assign from
    TauPi0Candidate& operator = ( const TauPi0Candidate& rhs );


    /// Equality operator
    /// @param rhs right hand side TauPi0Candidate instance to compare 
    /// @return true if equal, false otherwise
    bool operator==( const TauPi0Candidate& rhs ) const;

    /// Inequality operator
    /// @param rhs TauPi0Candidate instance to compare to
    /// @return !operator==
    bool operator!=( const TauPi0Candidate& rhs ) const;


    //////////////////////////////////////////////
    // public member accessors
    /////////////////////////////////////////////
    public:
    void dump(); //just for testing

    //@} --------------------------------------------------------------
    /// @name Accessors for clusters (copied from TauCommonDetails)
    /// @copydoc Analysis::TauPi0Candidate::m_pi0
    //@{ --------------------------------------------------------------

    /// Get pi0 cluster vector (const)
    /// @return (const) pi0 clusters vector
    const std::vector<TauPi0Cluster*>& pi0ClusterVec() const;
    
    /// Get pi0 cluster vector
    /// @return pi0 clusters vector
    std::vector<TauPi0Cluster*>& pi0ClusterVec();
        
    /// Get pi0 cluster
    /// Concider using direct access through clusterLinkVec() instead
    /// @param i index
    /// @return cluster
    const TauPi0Cluster* pi0Cluster(
			   unsigned int i ) const;

    /// Get length of cluster vector
    /// Concider using direct access through pi0LinkVec() instead
    /// @return length
    unsigned int nPi0Cluster() const;

    // Get HepLorentzVector of Pi0 Canidate (pi0 mass hypothesis)
    CLHEP::HepLorentzVector hlv() const;
    
    /// Add cluster to pi0 cluster vector
    /// Concider using direct access through pi0LinkVec() instead
    /// @param pi0 cluster
    /// @param cont cluster container
    void addPi0Cluster(
		TauPi0Cluster* pi0Cluster );

    //////////////////////////////////////////////
    // private data
    /////////////////////////////////////////////
    private:
    
    /// pi0 cluster vector
    std::vector<TauPi0Cluster*> m_pi0ClusterVector;
    
  };
} //end of namespace analysis

/// inline member function declarations
#include "TauPi0Candidate.icc"

#endif // __TAU_Pi0_CANDIDATE_H__

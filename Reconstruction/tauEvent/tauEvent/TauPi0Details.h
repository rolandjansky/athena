/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**
 * @brief Tau Pi0 Candidate Details Container
 *
 * (detailed description)
 *
 * @author Felix Friedrich <felix.friedrich@cern.ch>
 * @author Will Davey
 * @author Benedict Winter 
 *
 * TODO: implement correct constness!
 */


#ifndef __TAU_Pi0_DETAILS_H__
#define __TAU_Pi0_DETAILS_H__

#include <iostream>

#include "tauEvent/TauDetails.h"
#include "tauEvent/TauPi0Candidate.h"
#include "tauEvent/TauPi0Cluster.h"
#include "tauEvent/TauShot.h"
// remove this because the file will be taken out of CaloUtils
/* #include "CaloUtils/CaloClusterVertexCorrection.h" */

// forward declaration
//class Analysis::TauPi0Candidate;

namespace Analysis {
  
  class TauPi0Details : public TauDetails {
    
    public:
    /// default constructor
    TauPi0Details();
    
    /// Copy constructor
    /// @param rhs TauPi0CandidateDetails instance to assign from
    TauPi0Details( const TauPi0Details& rhs );
    
    /// destructor (virtual)
    virtual ~TauPi0Details();

    /// Assignment operator
    /// @param rhs TauPi0CandidateDetails instance to assign from
    /// @return assigned object
    TauPi0Details& operator = ( const TauPi0Details& rhs );

    /// Get name of class
    /// @return name of class
    virtual const std::string& className() const;

    
    //////////////////////////////////////////////
    // public member accessors
    /////////////////////////////////////////////
    public:    
    const std::vector<TauPi0Candidate*>&  pi0CandidateVector() const;
    std::vector<TauPi0Candidate*>&  pi0CandidateVector();
    void addPi0Candidate( TauPi0Candidate* );
    int nPi0Candidates() const;

    const std::vector<TauPi0Cluster*>&    pi0ClusterVector() const;
    std::vector<TauPi0Cluster*>&    pi0ClusterVector();
    void addPi0Cluster( TauPi0Cluster* );
    int nPi0Clusters() const;

    const std::vector<TauShot*>&    shotVector() const;
    std::vector<TauShot*>&          shotVector();
    void addShot( TauShot* );
    int nShots() const;

    //@} --------------------------------------------------------------
    /// @name Accessors for visTauhlv 
    /// @copydoc Ananlysis::TauPi0Details::m_visTauhlv
    //@{ --------------------------------------------------------------

    void setVisTauhlv(CLHEP::HepLorentzVector visTauhlv);
    CLHEP::HepLorentzVector visTauhlv() const;

    int nPi0s() const;
    CLHEP::HepLorentzVector sumPi0hlv() const;


    //////////////////////////////////////////////
    // function to do vertex correction
    //////////////////////////////////////////////

    // removed because caloclustervertexcorrection header was removed
    /* void doVertexCorrection(Amg::Vector3D* tauJetVertex) const; */

    //////////////////////////////////////////////
    // private data
    /////////////////////////////////////////////
    private:

    static std::string s_className;

    //TauPi0CandidateContainer m_pi0CandContainer;

    // owner of the TauPi0Candidates and TauPi0Clusters! Will delete them in the destructor
    std::vector<TauPi0Candidate*> m_pi0CandContainer;
    std::vector<TauPi0Cluster*>   m_pi0ClusContainer;
    std::vector<TauShot*>         m_shotContainer;
    CLHEP::HepLorentzVector m_visTau_hlv;
  };

} //end of namespace Analysis

/// inline member function declarations
#include "TauPi0Details.icc"


#endif //__TAU_Pi0_DETAILS_H__

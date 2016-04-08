/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**
 * @brief Tau Pi0 Cluster 
 *
 * (detailed description)
 *
 * @author Felix Friedrich 
 * @author Will Davey
 * @author Benedict Winter
 */

#ifndef TAUEVENT_TAUPI0CLUSTER_H
#define TAUEVENT_TAUPI0CLUSTER_H

#include <iostream>

//#include "CaloEvent/CaloCluster.h"
#include "CaloEvent/CaloClusterContainer.h"
#include "DataModel/ElementLink.h"
#include "tauEvent/TauShot.h"
//#include "DataModel/ElementLinkVector.h"

namespace Analysis {

  class TauPi0Cluster {

    public:
    /// default constructor
    TauPi0Cluster();
    
    /// copy constructor
    /// @param rhs TauPi0Cluster instance to assign from
    TauPi0Cluster( const TauPi0Cluster& rhs );
    
    /// destructor (virtual)
    virtual ~TauPi0Cluster();

    /// assignment operator
    /// @param rhs TauPi0Cluster instance to assign from
    TauPi0Cluster& operator = ( const TauPi0Cluster& rhs );

    /// Equality operator
    /// @param rhs right hand side TauPi0Cluster instance to compare 
    /// @return true if equal, false otherwise
    bool operator==( const TauPi0Cluster& rhs ) const;

    /// Inequality operator
    /// @param rhs TauPi0Cluster instance to compare to
    /// @return !operator==
    bool operator!=( const TauPi0Cluster& rhs ) const;

    //////////////////////////////////////////////
    // public member accessors
    /////////////////////////////////////////////
    public:
    void dump(); //just for testing

    //@} --------------------------------------------------------------
    /// @name Accessors for deltaPhi
    /// @copydoc Ananlysis::TauPi0Cluster::m_DELTA_PHI
    //@{ --------------------------------------------------------------    

    /// Get deltaPhi
    /// @return cluster deltaPhi
    float deltaPhi() const;

    /// Set deltaPhi
    /// @param deltaPhi cluster deltaPhi 
    void setDeltaPhi(
    float deltaPhi);

    //@} --------------------------------------------------------------
    /// @name Accessors for deltaTheta
    /// @copydoc Ananlysis::TauPi0Cluster::m_DELTA_THETA
    //@{ --------------------------------------------------------------    

    /// Get deltaTheta 
    /// @return cluster deltaTheta
    float deltaTheta() const;

    /// Set deltaTheta
    /// @param deltaTheta cluster deltaTheta 
    void setDeltaTheta(
    float deltaTheta);

    //@} --------------------------------------------------------------
    /// @name Accessors for engFracEM
    /// @copydoc Ananlysis::TauPi0Cluster::m_ENG_FRAC_EM
    //@{ --------------------------------------------------------------    

    /// Get engFracEM
    /// @return cluster engFracEM
    float engFracEM() const;

    /// Set engFracEM 
    /// @param engFracEM cluster engFracEM 
    void setEngFracEM(
    float engFracEM);

    //@} --------------------------------------------------------------
    /// @name Accessors for engFracMAX
    /// @copydoc Ananlysis::TauPi0Cluster::m_ENG_FRAC_MAX
    //@{ --------------------------------------------------------------    

    /// Get engFracMAX
    /// @return cluster engFracMAX
    float engFracMAX() const;

    /// Set engFracMAX 
    /// @param engFracMAX cluster engFracMAX 
    void setEngFracMAX(
    float engFracMAX);

    //@} --------------------------------------------------------------
    /// @name Accessors for engFracCore
    /// @copydoc Ananlysis::TauPi0Cluster::m_ENG_FRAC_CORE
    //@{ --------------------------------------------------------------    

    /// Get engFracCore
    /// @return cluster engFracCore
    float engFracCore() const;

    /// Set engFracCore
    /// @param engFracCore cluster engFracCore 
    void setEngFracCore(
    float engFracCore);

    //@} --------------------------------------------------------------
    /// @name Accessors for secondEngDens
    /// @copydoc Ananlysis::TauPi0Cluster::m_SECOND_ENG_DENS
    //@{ --------------------------------------------------------------    

    /// Get secondEngDens
    /// @return cluster secondEngDens
    float secondEngDens() const;

    /// Set secondEngDens
    /// @param secondEngDens cluster secondEngDens 
    void setSecondEngDens(
    float secondEngDens);

    //@} --------------------------------------------------------------
    /// @name Accessors for LATERAL
    /// @copydoc Ananlysis::TauPi0Cluster::m_LATERAL
    //@{ --------------------------------------------------------------    

    /// Get LATERAL
    /// @return cluster LATERAL
    float LATERAL() const;

    /// Set LATERAL
    /// @param LATERAL cluster LATERAL 
    void setLATERAL(
    float LATERAL);

    //@} --------------------------------------------------------------
    /// @name Accessors for LONGITUDINAL
    /// @copydoc Ananlysis::TauPi0Cluster::m_LONGITUDINAL
    //@{ --------------------------------------------------------------    

    /// Get LONGITUDINAL
    /// @return cluster LONGITUDINAL
    float LONGITUDINAL() const;

    /// Set LONGITUDINAL
    /// @param LONGITUDINAL cluster LONGITUDINAL 
    void setLONGITUDINAL(
    float LONGITUDINAL); 

    //@} --------------------------------------------------------------
    /// @name Accessors for EM1CoreFrac
    /// @copydoc Ananlysis::TauPi0Cluster::m_EM1CoreFrac
    //@{ --------------------------------------------------------------    

    /// Get EM1CoreFrac
    /// @return cluster EM1CoreFrac
    float EM1CoreFrac() const;

    /// Set EM1CoreFrac
    /// @param EM1CoreFrac cluster EM1CoreFrac
    void setEM1CoreFrac(
    float EM1CoreFrac);

    //@} --------------------------------------------------------------
    /// @name Accessors for asymmetryInEM1WRTTrk
    /// @copydoc Ananlysis::TauPi0Cluster::m_asymmetryInEM1WRTTrk
    //@{ --------------------------------------------------------------    

    /// Get asymmetryInEM1WRTTrk
    /// @return cluster asymmetryInEM1WRTTrk
    float asymmetryInEM1WRTTrk() const;

    /// Set asymmetryInEM1WRTTrk
    /// @param asymmetryInEM1WRTTrk cluster asymmetryInEM1WRTTrk 
    void setAsymmetryInEM1WRTTrk(
    float asymmetryInEM1WRTTrk);

    //@} --------------------------------------------------------------
    /// @name Accessors for NHitsInEM1
    /// @copydoc Ananlysis::TauPi0Cluster::m_NHitsInEM1
    //@{ --------------------------------------------------------------    

    /// Get NHitsInEM1
    /// @return cluster NHitsInEM1 
    int NHitsInEM1() const;

    /// Set NHitsInEM1
    /// @param NHitsInEM1 cluster NHitsInEM1 
    void setNHitsInEM1(
    int NHitsInEM1);

    //@} --------------------------------------------------------------
    /// @name Accessors for NPosECells_PS
    /// @copydoc Ananlysis::TauPi0Cluster::m_NPosECells_PS
    //@{ --------------------------------------------------------------    

    /// Get NPosECells_PS
    /// @return cluster NPosECells_PS
    int NPosECells_PS () const;

    /// Set NPosECells_PS
    /// @param NPosECells_PS cluster NPosECells_PS 
    void setNPosECells_PS(
    int NPosECells_PS);

    //@} --------------------------------------------------------------
    /// @name Accessors for NPosECells_EM1
    /// @copydoc Ananlysis::TauPi0Cluster::m_NPosECells_EM1
    //@{ --------------------------------------------------------------    

    /// Get NPosECells_EM1
    /// @return cluster NPosECells_EM1
    int NPosECells_EM1 () const;

    /// Set NPosECells_EM1
    /// @param NPosECells_EM1 cluster NPosECells_EM1 
    void setNPosECells_EM1(
    int NPosECells_EM1);

    //@} --------------------------------------------------------------
    /// @name Accessors for NPosECells_EM2
    /// @copydoc Ananlysis::TauPi0Cluster::m_NPosECells_EM2
    //@{ --------------------------------------------------------------    

    /// Get NPosECells_EM2
    /// @return cluster NPosECells_EM2
    int NPosECells_EM2 () const;

    /// Set NPosECells_EM2
    /// @param NPosECells_EM2 cluster NPosECells_EM2 
    void setNPosECells_EM2(
    int NPosECells_EM2);


    //@} --------------------------------------------------------------
    /// @name Accessors for firstEtaWRTClusterPosition_EM1 
    /// @copydoc Ananlysis::TauPi0Cluster::m_firstEtaWRTClusterPosition_EM1
    //@{ --------------------------------------------------------------    

    /// Get firstEtaWRTClusterPosition_EM1
    /// @return cluster firstEtaWRTClusterPosition_EM1
    float firstEtaWRTClusterPosition_EM1() const;

    /// Set firstEtaWRTClusterPosition_EM1
    /// @param firstEtaWRTClusterPosition_EM1 cluster firstEtaWRTClusterPosition_EM1 
    void setFirstEtaWRTClusterPosition_EM1(
    float firstEtaWRTClusterPosition_EM1);


    //@} --------------------------------------------------------------
    /// @name Accessors for firstEtaWRTClusterPosition_EM2 
    /// @copydoc Ananlysis::TauPi0Cluster::m_firstEtaWRTClusterPosition_EM2
    //@{ --------------------------------------------------------------    

    /// Get firstEtaWRTClusterPosition_EM2
    /// @return cluster firstEtaWRTClusterPosition_EM2
    float firstEtaWRTClusterPosition_EM2() const;

    /// Set firstEtaWRTClusterPosition_EM2
    /// @param firstEtaWRTClusterPosition_EM2 cluster firstEtaWRTClusterPosition_EM2 
    void setFirstEtaWRTClusterPosition_EM2(
    float firstEtaWRTClusterPosition_EM2);


    //@} --------------------------------------------------------------
    /// @name Accessors for secondEtaWRTClusterPosition_EM1 
    /// @copydoc Ananlysis::TauPi0Cluster::m_secondEtaWRTClusterPosition_EM1
    //@{ --------------------------------------------------------------    

    /// Get secondEtaWRTClusterPosition_EM1
    /// @return cluster secondEtaWRTClusterPosition_EM1
    float secondEtaWRTClusterPosition_EM1() const;

    /// Set secondEtaWRTClusterPosition_EM1
    /// @param secondEtaWRTClusterPosition_EM1 cluster secondEtaWRTClusterPosition_EM1 
    void setSecondEtaWRTClusterPosition_EM1(
    float secondEtaWRTClusterPosition_EM1);


    //@} --------------------------------------------------------------
    /// @name Accessors for secondEtaWRTClusterPosition_EM2 
    /// @copydoc Ananlysis::TauPi0Cluster::m_secondEtaWRTClusterPosition_EM2
    //@{ --------------------------------------------------------------    

    /// Get secondEtaWRTClusterPosition_EM2
    /// @return cluster secondEtaWRTClusterPosition_EM2
    float secondEtaWRTClusterPosition_EM2() const;

    /// Set secondEtaWRTClusterPosition_EM2
    /// @param secondEtaWRTClusterPosition_EM2 cluster secondEtaWRTClusterPosition_EM2 
    void setSecondEtaWRTClusterPosition_EM2(
    float secondEtaWRTClusterPosition_EM2);

    //@} --------------------------------------------------------------
    /// @name Accessors for BDTScore 
    /// @copydoc Ananlysis::TauPi0Cluster::m_BDTScore
    //@{ --------------------------------------------------------------

    /// Get BDTScore
    /// @return cluster BDTScore
    float BDTScore() const;

    /// Set BDTScore
    /// @param BDTScore cluster BDTScore 
    void setBDTScore(
    float BDTScore); 
      

    
    /// setter
    void setExternalVars(float DELTA_PHI, float DELTA_THETA, float ENG_FRAC_EM, float ENG_FRAC_MAX,float ENG_FRAC_CORE, float SECOND_ENG_DENS, float LATERAL, float LONGITUDINAL,
    float EM1CoreFrac, float asymmetryInEM1WRTTrk, int NHitsInEM1 , int NPosECells_PS, int NPosECells_EM1, int NPosECells_EM2, 
    float firstEtaWRTClusterPosition_EM1, float firstEtaWRTClusterPosition_EM2, float secondEtaWRTClusterPosition_EM1, float secondEtaWRTClusterPosition_EM2, float BDTScore);
    
    /// @name Accessors for cluster (copied from TauCommonDetails)
    /// @copydoc Analysis::TauPi0Cluster::m_cluster
    //@{ --------------------------------------------------------------

    /// Get (const) cluster link
    /// @return (const) cluster 
    const ElementLink<CaloClusterContainer>& clusterLink() const;
    
    /// Get cluster link 
    /// @return pi0 clusters vector
    ElementLink<CaloClusterContainer>& clusterLink();
        
    /// Get cluster
    /// Consider using direct access through clusterLink() instead
    /// @param i index
    /// @return cluster
    const CaloCluster* cluster() const;

    /// Set cluster to pi0 cluster vector
    /// Consider using direct access through clusterLink() instead
    /// @param cluster
    /// @param cont cluster container
    void setCluster(
		const CaloCluster *cluster,
		const CaloClusterContainer *cont );

    
    /// Get shot vector (const)
    /// @return (const) shot vector
    const std::vector<TauShot*>& tauShotVec() const;

    /// Get shot vector
    /// @return shot vector
    std::vector<TauShot*>& tauShotVec();

    /// Add shot to shot vector
    void addTauShot(
           TauShot *tauShot );
    

    // alternative setCluster method
    // which can call addCluster(CaloCluster*, CaloClusterContainer*)
    
    void setPi0Cluster(
		const CaloCluster *cluster,
		const CaloClusterContainer *cont,
    std::vector<TauShot*> tauShotVec,
    float DELTA_PHI, 
    float DELTA_THETA, 
    float ENG_FRAC_EM,
    float ENG_FRAC_MAX, 
    float ENG_FRAC_CORE, 
    float SECOND_ENG_DENS,
    float LATERAL,
    float LONGITUDINAL,
		float EM1CoreFrac, 
    float asymmetryInEM1WRTTrk,
    int   NHitsInEM1, 
    int   NPosECells_PS, 
    int   NPosECells_EM1, 
    int   NPosECells_EM2,
    float firstEtaWRTClusterPosition_EM1, 
    float firstEtaWRTClusterPosition_EM2,
    float secondEtaWRTClusterPosition_EM1, 
    float secondEtaWRTClusterPosition_EM2,
    float BDTScore
    );
    
    

    //////////////////////////////////////////////
    // private data
    /////////////////////////////////////////////
    private:
    
    /// cluster
    ElementLink<CaloClusterContainer> m_cluster;

    // shots
    std::vector<TauShot*> m_tauShotVector;
    
    /// cluster external properties
    float m_DELTA_PHI;
    float m_DELTA_THETA; 
    float m_ENG_FRAC_EM;
    float m_ENG_FRAC_MAX;
    float m_ENG_FRAC_CORE;
    float m_SECOND_ENG_DENS;
    float m_LATERAL;
    float m_LONGITUDINAL;
    float m_EM1CoreFrac; 
    float m_asymmetryInEM1WRTTrk;
    int   m_NHitsInEM1;
    int   m_NPosECells_PS;
    int   m_NPosECells_EM1;
    int   m_NPosECells_EM2;
    float m_firstEtaWRTClusterPosition_EM1;
    float m_firstEtaWRTClusterPosition_EM2;
    float m_secondEtaWRTClusterPosition_EM1;
    float m_secondEtaWRTClusterPosition_EM2;
    float m_BDTScore;
  };

} //end of namespace analysis

/// inline member function declarations
#include "TauPi0Cluster.icc"

#endif // TAUEVENT_TAUPI0CLUSTER_H

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/*!
 * file: TauPi0Clusters.h
 *
 * This class provides a handle on the Pi0Finder algorithm, and pass the resultin data to storegate
 *
 * Author: Michel Trottier-McDonald (mtm-AT-cern-dot-ch)
 */

#ifndef TAUPI0CLUSTERS_H
#define TAUPI0CLUSTERS_H

#include "TauDiscriminant/TauDetails.h"
#include "xAODTau/TauJet.h"
#include "TauDiscriminant/Pi0Finder.h"

class TauPi0Clusters
{
public:

    static const float LOW_NUMBER;
    
    //!< Constructor
    TauPi0Clusters(const xAOD::TauJet& tauJet);
    
    //!< Destructor
    ~TauPi0Clusters() {}

    //Getters
    float get_cl1_Pt()  const {return m_cl1_Pt;}
    float get_cl1_Eta() const {return m_cl1_Eta;}
    float get_cl1_Phi() const {return m_cl1_Phi;}

    float get_cl2_Pt()  const {return m_cl2_Pt;}
    float get_cl2_Eta() const {return m_cl2_Eta;}
    float get_cl2_Phi() const {return m_cl2_Phi;}

    float get_tau_vis_Pt()  const {return m_tau_vis_Pt;}
    float get_tau_vis_Eta() const {return m_tau_vis_Eta;}
    float get_tau_vis_Phi() const {return m_tau_vis_Phi;}
    float get_tau_vis_M()   const {return m_tau_vis_M;}

    
private:
    
    // Data members to return

    // Cluster 1 4-vector
    float m_cl1_Pt;
    float m_cl1_Eta;
    float m_cl1_Phi;
    
    // Cluster 2 4-vector
    float m_cl2_Pt;
    float m_cl2_Eta;
    float m_cl2_Phi;

    // Tau 4-vector from pi0s and tracks
    float m_tau_vis_Pt;
    float m_tau_vis_Eta;
    float m_tau_vis_Phi;
    float m_tau_vis_M;

    // Run Pi0Finder
    void runPi0Finder(const xAOD::TauJet& tauJet);

};

#endif

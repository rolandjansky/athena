/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef PI0FINDER_H
#define PI0FINDER_H

//*******************************************************//
// Name: Pi0Finder.h                                     //
// Author: Michel Trottier-McDonald <mtm@cern.ch>        //
// Description: Main class to find which clusters are    //
// the pi0s in tau decays involving neutral pions        //
//*******************************************************//

#include <TLorentzVector.h>
#include "TauDiscriminant/EMFCluster.h"
#include <vector>

class Pi0Finder
{
public:
    // Constructors, Destructors
    Pi0Finder();

    Pi0Finder(const std::vector<TLorentzVector>& tracks,
	      const std::vector<EMFCluster>& clusters,
	      bool twoPi0s = true,
	      double resImportance = 1.0,
	      double turnOnPoint = 1.05,
	      double turnOnRate = 9,
	      double PSSFactor = 1.4,
	      double EM2Factor = 0.4,
	      double twoPi0Strength = 0.4,
	      bool usePseudoHADF = true,
	      bool effClusters = false);

    Pi0Finder(const std::vector<TLorentzVector>& tracks,
	      const std::vector<TLorentzVector>& clusters,
	      const std::vector<float>& PSSFs,
	      const std::vector<float>& EM2Fs,
	      const std::vector<float>& EM3Fs,
	      bool twoPi0s = true,
	      double resImportance = 1.0,
	      double turnOnPoint = 1.05,
	      double turnOnRate = 9,
	      double PSSFactor = 1.4,
	      double EM2Factor = 0.4,
	      double twoPi0Strength = 0.4,
	      bool usePseudoHADF = true,
	      bool effClusters = false);

    ~Pi0Finder();

    //Getters
    TLorentzVector visTauTLV() const; //Returns the visible tau
    TLorentzVector pi0TLV1() const {return m_selEMFCluster1.TLV();}
    TLorentzVector pi0TLV2() const {return m_selEMFCluster2.TLV();}
    TLorentzVector pi0NotCorrTLV1() const {return m_selEMFClusterNotCorr1.TLV();}
    TLorentzVector pi0NotCorrTLV2() const {return m_selEMFClusterNotCorr2.TLV();}
    EMFCluster pi0EMFCluster1() const {return m_selEMFCluster1;}
    EMFCluster pi0EMFCluster2() const {return m_selEMFCluster2;}
    double doubleCounting() const {return m_doubleCountingE;}
    bool corrected1() const {return m_applyCorrCluster1;}
    bool corrected2() const {return m_applyCorrCluster2;}
    bool noMatch() const {return m_noMatch;}

private:

    //The candidate clusters (after noise correction)
    std::vector<EMFCluster> m_EMFClusters;

    //the tracks
    std::vector<TLorentzVector> m_tracks;

    //General parameters
    bool m_usePseudoHADF;
    bool m_twoPi0s;

    //Selection parameters
    double m_resImportance; // Importance of the resolution of the cluster energy compared to calo energy - track energy

    //Contamination correction parameters
    double m_turnOnPoint; // Value of energy double counting at which the correction turns on.
    double m_turnOnRate;  // How quickly the correction turns on after the turn on point
    double m_PSSFactor;   // How many times the PreSampler-Strip Energy when finding the corrected energy
    double m_EM2Factor;   // How many times the EMLayer 2 Energy when finding the corrected energy
    double m_twoPi0Strength; //Biases the preference for finding 1 or 2 clusters

    //Internal transient parameters
    double m_caloE;             // total calorimeter energy
    double m_caloHADE;          // total hadronic (pseudo-hadronic) calorimeter energy
    double m_trkE;              // total track system energy
    double m_trkHADF;           // energy in the hadronic (pseudo-hadronic) calorimeter divided by the track energy
    double m_doubleCountingE;   // Values above 1 roughly indicates energy double counting, used to decide to correct cluster energy or not
    bool m_applyCorrCluster1;   // Apply correction to cluster 1
    bool m_applyCorrCluster2;   // Apply correction to cluster 2
    bool m_keepCluster1;        // Keep Cluster1
    bool m_keepCluster2;        // Keep Cluster2
    bool m_noMatch;             // True if no pi0 has been selected

    TLorentzVector m_trkSys;

    //Results
    EMFCluster m_selEMFCluster1;
    EMFCluster m_selEMFCluster2;
    EMFCluster m_selEMFClusterNotCorr1;
    EMFCluster m_selEMFClusterNotCorr2;

    //Internal methods
    void preSelParameters();  // Calculate pre-selection internal transient parameters
    void postSelParameters(); // Calculate post-selection internal transient parameters
    void select();            // Select one pi0 cluster
    void select2();           // Select two pi0 clusters
    EMFCluster correct(const EMFCluster& cl);   // Correct the energy of one contaminated cluster
    EMFCluster giveMass(const EMFCluster& cl);  // Give the pi0 mass to uncorrected clusters
    void execute();           // Execute the pi0 finding
    std::vector<EMFCluster> convertToEMFClusters(const std::vector<TLorentzVector>& clusters,
						 const std::vector<float>& PSSFs,
						 const std::vector<float>& EM2Fs,
						 const std::vector<float>& EM3Fs);
};

#endif

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef EMFCluster_H
#define EMFCluster_H

//*******************************************************//
// Name: EMFCluster.h                                    //
// Author: Michel Trottier-McDonald <mtm@cern.ch>        //
// Description: A simple class to house cluster          //
// TLorentzVectors and their associated fractions of     //
// energy in different calorimeter layers                //
//*******************************************************//

#include <TLorentzVector.h>

class EMFCluster
{
public:
    //Constructor, Destructor
    EMFCluster();
    EMFCluster(double pt);
    EMFCluster(const TLorentzVector& inCluster,
	       double inPSSF,
	       double inEM2F,
	       double inEM3F);
    ~EMFCluster();

    //Getters
    TLorentzVector TLV() const {return m_cluster;}

    double PSSF() const {return m_PSSF;}
    double EM2F() const {return m_EM2F;}
    double EM3F() const {return m_EM3F;}
    double HADF() const {return m_HADF;}
    double pseudoHADF() const {return m_pseudoHADF;}

    double PSSE() const {return m_PSSE;}
    double EM2E() const {return m_EM2E;}
    double EM3E() const {return m_EM3E;}
    double HADE() const {return m_HADE;}
    double pseudoHADE() const {return m_pseudoHADE;}

    //Comparison operator
    bool operator< (const EMFCluster& rhs) const
    { return (m_cluster.E() < rhs.TLV().E()); }

private:
    //Cluster 4-vector
    TLorentzVector m_cluster;

    //Layer related info
    double m_PSSF; // Fraction on energy in the PreSampler and Strip layers
    double m_EM2F; // Fraction of energy in layer 2 of the EM calorimeter
    double m_EM3F; // Fraction of energy in layer 3 of the EM calorimeter
    double m_HADF; // Fraction of energy in the hadronic calorimeter
    double m_pseudoHADF; // Fraction of energy in layer 3 of the EM calorimeter and hadronic calorimeter

    double m_PSSE; // Energy in the PreSampler and Strip layers
    double m_EM2E; // Energy in layer 2 of the EM calorimeter
    double m_EM3E; // Energy in layer 3 of the EM calorimeter
    double m_HADE; // Energy in the hadronic calorimeter
    double m_pseudoHADE; // Energy in layer 3 of the EM calorimeter and hadronic calorimeter

    //Calculate the hadronic fractions and energies
    void update();
};

#endif

/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#ifndef CALOCLUSTERVARIABLES_H
#define CALOCLUSTERVARIABLES_H

#include <vector>
#include "xAODCaloEvent/CaloVertexedTopoCluster.h"
#ifndef XAOD_ANALYSIS
#include "CaloUtils/CaloVertexedCluster.h"
typedef xAOD::CaloVertexedCluster CaloVertexedClusterType;
#else
typedef xAOD::CaloVertexedTopoCluster CaloVertexedClusterType;
#endif
#include "CxxUtils/fpcompare.h"
#include "xAODTau/TauJet.h"

#include "TLorentzVector.h"


/** Provide calculations of cluster based variables using the clusters associated to the jet seed of the tau candidate. */
class CaloClusterVariables {
public:

    static const double DEFAULT;

    CaloClusterVariables();

    ~CaloClusterVariables() {
    }

    bool update(const xAOD::TauJet& pTau); //!< update the internal variables for the given tau

    void setVertexCorrection(bool flag) {m_doVertexCorrection=flag;}
    void setIncSub(bool flag) {m_incShowerSubtr=flag;}

    // ID Variables
    unsigned int numConstituents() { return (unsigned int) m_numConstit; }

    double totalMass()     { return m_totMass; }
    double effectiveMass() { return m_effMass; }
    
    double effectiveNumConstituents()  { return m_effNumConstit; }
    int effectiveNumConstituents_int() { return m_effNumConstit_int; }

    double averageEffectiveRadius() { return m_aveEffRadius; }
    double averageRadius()          { return m_aveRadius; }

    // Energy Variables
    double totalEnergy()     { return m_totEnergy; }
    double effectiveEnergy() { return m_effEnergy; }

private:
    int m_numConstit;
    int m_effNumConstit_int;
    double m_effNumConstit;
    double m_aveRadius;
    double m_aveEffRadius;
    double m_totMass;
    double m_effMass;
    double m_totEnergy;
    double m_effEnergy;

    // Calculate the geometrical center of the tau constituents
    TLorentzVector calculateTauCentroid(int nConst, const std::vector<CaloVertexedClusterType>& constituents);

    // Enable cell origin correction.
    // Eta and phi of the cells are corrected wrt to the origin of the tau vertex
    bool m_doVertexCorrection;

    // use shower subtracted clusters with PFlow jet seeds
    bool m_incShowerSubtr;

};

//-------------------------------------------------------------------------
//! Descending order by energy
//-------------------------------------------------------------------------
struct CaloClusterCompare { 
  bool operator()(const CaloVertexedClusterType& left, const CaloVertexedClusterType& right) {
    //volatile double leftE = left.e();
    //volatile double rightE = right.e();
    return CxxUtils::fpcompare::greater (left.e(),right.e());
  }
};

struct DefCaloClusterCompare { 
  bool operator()(const xAOD::CaloCluster* left, const xAOD::CaloCluster* right) {
    //volatile double leftE = left.e();
    //volatile double rightE = right.e();
    return CxxUtils::fpcompare::greater (left->e(),right->e());
  }
};

#endif

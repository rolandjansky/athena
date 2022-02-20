/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TAURECTOOLS_CALOCLUSTERVARIABLES_H
#define TAURECTOOLS_CALOCLUSTERVARIABLES_H

#include "xAODTau/TauJet.h"
#include "CxxUtils/fpcompare.h"

#include "TLorentzVector.h"
#include <vector>


/** Provide calculations of cluster based variables using the clusters associated to the jet seed of the tau candidate. */
class CaloClusterVariables {
public:

    static const double DEFAULT;

    CaloClusterVariables();

    ~CaloClusterVariables() {
    }

    bool update(const xAOD::TauJet& pTau); //!< update the internal variables for the given tau

    // ID Variables
    unsigned int numConstituents() const { return (unsigned int) m_numConstit; }

    double totalMass()     const { return m_totMass; }
    double effectiveMass() const { return m_effMass; }
    
    double effectiveNumConstituents()  const { return m_effNumConstit; }
    int effectiveNumConstituents_int() const { return m_effNumConstit_int; }

    double averageEffectiveRadius() const { return m_aveEffRadius; }
    double averageRadius()          const { return m_aveRadius; }

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
    TLorentzVector calculateTauCentroid(int nConst, const std::vector<TLorentzVector>& clusterP4Vector) const;
};

//-------------------------------------------------------------------------
//! Descending order by energy
//-------------------------------------------------------------------------
struct CaloClusterCompare { 
  bool operator()(const TLorentzVector& left, const TLorentzVector& right) const {
    //volatile double leftE = left.e();
    //volatile double rightE = right.e();
    return CxxUtils::fpcompare::greater (left.E(),right.E());
  }
};

struct DefCaloClusterCompare { 
  bool operator()(const xAOD::CaloCluster* left, const xAOD::CaloCluster* right) const {
    //volatile double leftE = left.e();
    //volatile double rightE = right.e();
    return CxxUtils::fpcompare::greater (left->e(),right->e());
  }
};

#endif // TAURECTOOLS_CALOCLUSTERVARIABLES_H

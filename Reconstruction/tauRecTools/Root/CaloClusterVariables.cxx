/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#include "tauRecTools/CaloClusterVariables.h"
#include "tauRecTools/HelperFunctions.h"
#include <cmath>

const double CaloClusterVariables::DEFAULT = -1111.;

//****************************************
// constructor
//****************************************

CaloClusterVariables::CaloClusterVariables() :
m_numConstit((int) DEFAULT),
m_effNumConstit_int((int) DEFAULT),
m_effNumConstit(DEFAULT),
m_aveRadius(DEFAULT),
m_aveEffRadius(DEFAULT),
m_totMass(DEFAULT),
m_effMass(DEFAULT),
m_totEnergy(DEFAULT),
m_effEnergy(DEFAULT),
m_incShowerSubtr(true){
}

//*******************************************
// update/fill the cluster based variables
//*******************************************

bool CaloClusterVariables::update(const xAOD::TauJet& pTau, const ToolHandle<ITauVertexCorrection>& tauVertexCorrection) {
    
    if (! pTau.jetLink().isValid()) return false;

    const xAOD::Jet* jetSeed = pTau.jet();
    const xAOD::Vertex* jetVertex = tauVertexCorrection->getJetVertex(*jetSeed);

    const xAOD::Vertex* tauVertex = nullptr;
    if (pTau.vertexLink().isValid()) tauVertex = pTau.vertex();
 
    std::vector<const xAOD::CaloCluster*> clusterList;
    StatusCode sc = tauRecTools::GetJetClusterList(jetSeed, clusterList, m_incShowerSubtr);

    std::vector<TLorentzVector> clusterP4Vector;
    for (const xAOD::CaloCluster* cluster : clusterList) {
      TLorentzVector clusterP4 = tauVertexCorrection->getVertexCorrectedP4(cluster, tauVertex, jetVertex);
      clusterP4Vector.push_back(clusterP4);
    }

    this->m_numConstit = (int) clusterP4Vector.size();

    // Order constituents by energy
    sort(clusterP4Vector.begin(), clusterP4Vector.end(), CaloClusterCompare());

    //****************************************
    // Looping over all constituents
    //****************************************

    double sum_px = 0;
    double sum_py = 0;
    double sum_pz = 0;
    double sum_e = 0;
    double sum_of_E2 = 0;
    double sum_radii = 0;
    TLorentzVector centroid = calculateTauCentroid(this->m_numConstit, clusterP4Vector);

    for (const TLorentzVector& clusterP4 : clusterP4Vector) {
        double energy = clusterP4.E();
        sum_of_E2 += energy*energy;

        double px = clusterP4.Px();
        double py = clusterP4.Py();
        double pz = clusterP4.Pz();
        double dr = clusterP4.DeltaR(centroid);
        sum_radii += dr;
        sum_e += energy;
        sum_px += px;
        sum_py += py;
        sum_pz += pz;
    }

    // Sum up the energy for constituents
    this->m_totEnergy = sum_e;

    // Calculate the mass of the constituents
    if (this->m_numConstit < 2) this->m_totMass = DEFAULT;
    else {
        double mass2 = sum_e * sum_e - (sum_px * sum_px + sum_py * sum_py + sum_pz * sum_pz);
        this->m_totMass = mass2 > 0 ? std::sqrt(mass2) : -std::sqrt(-mass2);
    }

    // Calculate the average radius of the constituents wrt the tau centroid
    this->m_aveRadius = this->m_numConstit > 0 ? sum_radii / this->m_numConstit : DEFAULT;

    // Effective number of constituents
    this->m_effNumConstit = sum_of_E2 > 0 ? (sum_e * sum_e) / (sum_of_E2) : DEFAULT;

    this->m_effNumConstit_int = int(ceil(this->m_effNumConstit));

    // A problem!
    if (this->m_effNumConstit_int > this->m_numConstit) return false;

    // Avoid segfault, happens when we try to iterate below if sum_of_E2 was 0 or negative
    if (this->m_effNumConstit_int < 0) return false;

    //****************************************
    // Now: Looping over effective constituents
    //****************************************

    sum_px = 0;
    sum_py = 0;
    sum_pz = 0;
    sum_e = 0;
    sum_of_E2 = 0;
    sum_radii = 0;
    centroid = calculateTauCentroid(this->m_effNumConstit_int, clusterP4Vector);

    int icount = this->m_effNumConstit_int;
    for (const TLorentzVector& clusterP4 : clusterP4Vector) {
      if (icount <= 0) break;
      --icount;

        double energy = clusterP4.E();
        double px = clusterP4.Px();
        double py = clusterP4.Py();
        double pz = clusterP4.Pz();
        double dr = clusterP4.DeltaR(centroid);
        sum_radii += dr;

        sum_e += energy;
        sum_px += px;
        sum_py += py;
        sum_pz += pz;
    }

    // Sum up the energy for effective constituents
    this->m_effEnergy = sum_e;

    // Calculate the mass of the constituents
    if (this->m_effNumConstit_int < 2) this->m_effMass = DEFAULT;
    else {
        double mass2 = sum_e * sum_e - (sum_px * sum_px + sum_py * sum_py + sum_pz * sum_pz);
        this->m_effMass = mass2 > 0 ? std::sqrt(mass2) : -std::sqrt(-mass2);
    }

    // Calculate the average radius of the constituents wrt the tau centroid
    this->m_aveEffRadius = this->m_effNumConstit_int > 0 ? sum_radii / this->m_effNumConstit_int : DEFAULT;

    return true;
}


//***********************************************************
// Calculate the geometrical center of the tau constituents
//***********************************************************
TLorentzVector CaloClusterVariables::calculateTauCentroid(int nConst, const std::vector<TLorentzVector>& clusterP4Vector) {

    double px = 0;
    double py = 0;
    double pz = 0;
    double current_px, current_py, current_pz, modulus;

    for (const TLorentzVector& clusterP4: clusterP4Vector) {
      if (nConst <= 0) break;
      --nConst;
        current_px = clusterP4.Px();
        current_py = clusterP4.Py();
        current_pz = clusterP4.Pz();
        modulus = sqrt(current_px * current_px + current_py * current_py + current_pz * current_pz);
        px += current_px / modulus;
        py += current_py / modulus;
        pz += current_pz / modulus;
    }
    TLorentzVector centroid(px, py, pz, 1);
    return centroid;
}

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
m_doVertexCorrection(false),
m_incShowerSubtr(true){
}

//*******************************************
// update/fill the cluster based variables
//*******************************************

bool CaloClusterVariables::update(const xAOD::TauJet& pTau) {

    const xAOD::Jet* pSeed = *pTau.jetLink();
    if(!pSeed) return false;

    // Loop through jets, get links to clusters
    std::vector<const xAOD::CaloCluster*> clusterList;
    StatusCode sc = tauRecTools::GetJetClusterList(pSeed, clusterList, m_incShowerSubtr);

    std::vector<CaloVertexedClusterType> constituents;
    for (auto pCluster : clusterList){
      // correct cluster
      if (pTau.vertexLink() && m_doVertexCorrection)
	constituents.emplace_back (*pCluster, (*pTau.vertexLink())->position());
      else
        constituents.emplace_back (*pCluster);
    }

    this->m_numConstit = (int) constituents.size();

    // Order constituents by energy
    sort(constituents.begin(), constituents.end(), CaloClusterCompare());

    //****************************************
    // Looping over all constituents
    //****************************************

    double sum_px = 0;
    double sum_py = 0;
    double sum_pz = 0;
    double sum_e = 0;
    double sum_of_E2 = 0;
    double sum_radii = 0;
    TLorentzVector centroid = calculateTauCentroid(this->m_numConstit, constituents);

    for (const CaloVertexedClusterType& c : constituents) {
        double energy = c.e();
        sum_of_E2 += energy*energy;

        double px = c.p4().Px();
        double py = c.p4().Py();
        double pz = c.p4().Pz();
        // FF: phi is wrong in case px,py AND pz is negative when using HepLorentzVector(px,py,pz,1)
        // because phi = atan(py/px)
        // in trigger many clusters have negative energies/momentum
        // negative values of px and py are only treated correctly if pz is positive.
        // Otherwise px and py must be taken as positive values
        // so using cluster eta/phi directly instead of creating a HLV.
        //CLHEP::HepLorentzVector constituentHLV(px, py, pz, 1);
        //sum_radii += centroid.deltaR(constituentHLV);
        double dr = std::sqrt( std::pow(c.eta() - centroid.Eta(),2) + std::pow(c.phi() - centroid.Phi(),2));
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
        this->m_totMass = mass2 > 0 ? sqrt(mass2) : -sqrt(-mass2);
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
    centroid = calculateTauCentroid(this->m_effNumConstit_int, constituents);

    int icount = this->m_effNumConstit_int;
    for (const CaloVertexedClusterType& c : constituents) {
      if (icount <= 0) break;
      --icount;

        double energy = c.e();
        double px = c.p4().Px();
        double py = c.p4().Py();
        double pz = c.p4().Pz();
        double dr = std::sqrt( std::pow(c.eta() - centroid.Eta(),2) + std::pow(c.phi() - centroid.Phi(),2));
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
        this->m_effMass = mass2 > 0 ? sqrt(mass2) : -sqrt(-mass2);
    }

    // Calculate the average radius of the constituents wrt the tau centroid
    this->m_aveEffRadius = this->m_effNumConstit_int > 0 ? sum_radii / this->m_effNumConstit_int : DEFAULT;

    return true;
}


//***********************************************************
// Calculate the geometrical center of the tau constituents
//***********************************************************
TLorentzVector CaloClusterVariables::calculateTauCentroid(int nConst, const std::vector<CaloVertexedClusterType>& constituents) {

    double px = 0;
    double py = 0;
    double pz = 0;
    double current_px, current_py, current_pz, modulus;

    for (const CaloVertexedClusterType& c : constituents) {
      if (nConst <= 0) break;
      --nConst;
        current_px = c.p4().Px();
        current_py = c.p4().Py();
        current_pz = c.p4().Pz();
        modulus = sqrt(current_px * current_px + current_py * current_py + current_pz * current_pz);
        px += current_px / modulus;
        py += current_py / modulus;
        pz += current_pz / modulus;
    }
    TLorentzVector centroid(px, py, pz, 1);
    return centroid;
}

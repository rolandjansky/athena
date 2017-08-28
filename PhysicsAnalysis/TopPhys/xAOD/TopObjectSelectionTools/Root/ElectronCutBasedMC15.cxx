/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "TopObjectSelectionTools/ElectronCutBasedMC15.h"

namespace top {

ElectronCutBasedMC15::ElectronCutBasedMC15(double ptcut, bool vetoCrack, const std::string& quality, const std::string& qualityLoose, IsolationBase* isolation) :
    m_ptcut(ptcut),
    m_vetoCrack(vetoCrack),
    m_quality(quality),
    m_qualityLoose(qualityLoose),
    m_isolation(isolation)
{
}

bool ElectronCutBasedMC15::passSelection(const xAOD::Electron& el) const {

    if (!passSelectionNoIsolation(el, m_quality))
      return false;

    if (m_isolation && !m_isolation->passSelection(el))
      return false;
  
    return true;
}

bool ElectronCutBasedMC15::passSelectionLoose(const xAOD::Electron& el) const {

    if (!passSelectionNoIsolation(el, m_qualityLoose))
      return false;

    if (m_isolation && !m_isolation->passSelectionLoose(el))
      return false;

    return true;
}

bool ElectronCutBasedMC15::passSelectionNoIsolation(const xAOD::Electron& el, const std::string& quality) const {
    // if (el.author() != xAOD::EgammaParameters::AuthorElectron && el.author() != xAOD::EgammaParameters::AuthorSofte)
    //     return false;

    if (el.pt() < m_ptcut)
        return false;

    // This will be replaced with the proper AsgElectronIsEMSelector
    // Once the calib files are on afs (cvmfs?) - not there yet.....
    if (!el.passSelection(quality))
        return false;

    //WARNING: Not all electrons keep clusters in the derivation
    //i.e. bad electrons (which is why we moved the check on the quality
    //before the check on the calo cluster)
    //Good electrons should always have a cluster, if not then crash to warn us
    //Better than checking and silently doing nothing...
    //This stops a crash
    if (std::fabs(el.caloCluster()->etaBE(2)) > 2.47)
         return false;

    if (m_vetoCrack && std::fabs(el.caloCluster()->etaBE(2)) > 1.37 && std::fabs(el.caloCluster()->etaBE(2)) < 1.52)
        return false;

    return true;
}

void ElectronCutBasedMC15::print(std::ostream& os) const {
    os << "ElectronCutBasedMC15\n";
    os << "    * pT > " << m_ptcut << "\n";
    os << "    * Currently disabled --- |cluster_eta| < 2.47 \n";
    os << "    * Veto 1.37 < |cluster_eta| < 1.52? " << std::boolalpha << m_vetoCrack << "\n";
    os << "    * Quality " << m_quality << "\n";

    if (!m_isolation)
        os << "    * No isolation requirement\n";
    else
        m_isolation->print(os);
}

}

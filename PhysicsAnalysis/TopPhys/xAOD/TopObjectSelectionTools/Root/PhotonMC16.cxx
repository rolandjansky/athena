/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#include "TopObjectSelectionTools/PhotonMC16.h"

// Include what you use!
#include <map>
#include <string>

#include "TopEvent/EventTools.h"

namespace top {

  PhotonMC16::PhotonMC16(double ptcut, double etamax, IsolationBase* isolation, bool usePhotonShowerShapeVariables) :
  m_ptcut(ptcut),
  m_etamax(etamax),
  m_photon_selection("DFCommonPhotonsIsEMTight"),
  m_loose_photon_selection("DFCommonPhotonsIsEMLoose"),
  m_isolation(isolation),
  m_usePhotonShowerShapeVariables(usePhotonShowerShapeVariables) {
}

PhotonMC16::PhotonMC16(double ptcut, double etamax,
                       const std::string& tightID,
                       const std::string& looseID,
                       IsolationBase* isolation,
		       bool usePhotonShowerShapeVariables) :
  m_ptcut(ptcut),
  m_etamax(etamax),
  m_photon_selection(tightID),
  m_loose_photon_selection(looseID),
  m_isolation(isolation),
  m_usePhotonShowerShapeVariables(usePhotonShowerShapeVariables) {
  // Make a map of shortcuts e.g "Tight = DFCommonPhotonsIsEMTight"
  std::map<std::string, std::string> id_map;
  id_map["Tight"] = "DFCommonPhotonsIsEMTight";
  id_map["Loose"] = "DFCommonPhotonsIsEMLoose";
  id_map["None"] = "None";
  // If ID in map then set it to full name, else keep as is.
  if (id_map.find(tightID) != id_map.end())
    m_photon_selection = id_map[tightID];
  if (id_map.find(looseID) != id_map.end())
    m_loose_photon_selection = id_map[looseID];
}

bool PhotonMC16::passSelection(const xAOD::Photon& ph) const {
  if (!passSelectionNoIsolation(ph, m_photon_selection))
    return false;

  if (m_isolation && !m_isolation->passSelection(ph))
    return false;

  return true;
}

bool PhotonMC16::passSelectionLoose(const xAOD::Photon& ph) const {
  if (!passSelectionNoIsolation(ph, m_loose_photon_selection))
    return false;

  if (m_isolation && !m_isolation->passSelectionLoose(ph))
    return false;

  return true;
}

bool PhotonMC16::passSelectionNoIsolation(const xAOD::Photon& ph,
                                          const std::string& photon_selection) const {
  // Photon author : AuthorPhoton/AuthorAmbiguous.
  // Also, for special-case recovery of soft photons:
  // xAOD::EgammaParameters::AuthorCaloTopo35.
  if (ph.author() != xAOD::EgammaParameters::AuthorPhoton
      && ph.author() != xAOD::EgammaParameters::AuthorAmbiguous)
    return false;

  if (ph.pt() < m_ptcut)
    return false;

  // removing bad photon cluser [http://cern.ch/go/kp8F]
  if (!ph.isGoodOQ(xAOD::EgammaParameters::BADCLUSPHOTON))
    return false;

  //  Photon selection using derivation decorations
  if (photon_selection != "None")
    if (!ph.auxdataConst<char>(photon_selection))
      return false;

  if (!ph.caloCluster()) return false;
  float abs_eta = std::abs(ph.caloCluster()->etaBE(2));
  if (abs_eta > m_etamax)
    return false;

  // Remove crack region
  if (abs_eta > 1.37 && abs_eta < 1.52)
    return false;

  // Photon cleaning [http://cern.ch/go/8RdK]
  // <tom.neep@cern.ch>: This is madness!! I *think* this is the right way
  // to do this but there should really be a helper function supplied by egamma!
  // <iconnell@cern.ch>: Updating the cleaning for R21, using similar options...
  if (m_usePhotonShowerShapeVariables) {
    try {
      if( (ph.OQ()&1073741824)!=0 ||
	  ( ( ph.OQ()&134217728)!=0 &&
	    ( ph.showerShapeValue(xAOD::EgammaParameters::Reta) >0.98
	      || ph.showerShapeValue(xAOD::EgammaParameters::f1) > 0.4
	      || (ph.OQ()&67108864)!=0) ) )
	return false;
    }
    catch (const SG::ExcBadAuxVar &e) {
      std::cout << "WARNING - Didn't find the necessary photon shower shapes variables for photon cleaning! This might be a derivation issue, skipping from now on." << std::endl;
      m_usePhotonShowerShapeVariables = false;
    }
  }

  return true;
}

void PhotonMC16::print(std::ostream& os) const {
  os << "PhotonMC16\n";
  os << "    * pT > " << m_ptcut << "\n";
  os << "    * |eta| < " << m_etamax << "\n";
  os << "    * Photon PID: " << m_photon_selection << "\n";
  os << "    * Loose Photon PID: " << m_loose_photon_selection << "\n";
  if (!m_isolation)
    os << "    * No isolation requirement\n";
  else
    m_isolation->print(os);
}
}  // namespace top

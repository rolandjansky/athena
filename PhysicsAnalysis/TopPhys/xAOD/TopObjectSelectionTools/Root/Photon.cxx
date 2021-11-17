/*
   Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
 */

#include "TopObjectSelectionTools/Photon.h"

// Include what you use!
#include <map>
#include <string>

#include "TopEvent/EventTools.h"

#include "TopObjectSelectionTools/MsgCategory.h"
using namespace TopObjectSelectionTools;

namespace top {
  Photon::Photon(double ptcut, double etamax, IsolationBase* isolation) :
    m_ptcut(ptcut),
    m_etamax(etamax),
    m_photon_selection("DFCommonPhotonsIsEMTight"),
    m_loose_photon_selection("DFCommonPhotonsIsEMLoose"),
    m_isolation(isolation) {
  }

  Photon::Photon(double ptcut, double etamax,
                 const std::string& tightID,
                 const std::string& looseID,
                 IsolationBase* isolation) :
    m_ptcut(ptcut),
    m_etamax(etamax),
    m_photon_selection(tightID),
    m_loose_photon_selection(looseID),
    m_isolation(isolation) {
    // Make a map of shortcuts e.g "Tight = DFCommonPhotonsIsEMTight"
    std::map<std::string, std::string> id_map;
    id_map["Tight"] = "DFCommonPhotonsIsEMTight";
    id_map["Loose"] = "DFCommonPhotonsIsEMLoose";
    id_map["None"] = "None";
    // If ID in map then set it to full name, else keep as is.
    if (id_map.find(tightID) != id_map.end()) m_photon_selection = id_map[tightID];
    if (id_map.find(looseID) != id_map.end()) m_loose_photon_selection = id_map[looseID];

    m_deadHVTool.setTypeAndName("AsgDeadHVCellRemovalTool/deadHVTool");
    top::check(m_deadHVTool.retrieve(), "Failed to setup Egamma DeadHVCellRemovalTool");
  }

  bool Photon::passSelection(const xAOD::Photon& ph) const {
    if (!passSelectionNoIsolation(ph, m_photon_selection)) return false;

    if (m_isolation && !m_isolation->passSelection(ph)) return false;

    return true;
  }

  bool Photon::passSelectionLoose(const xAOD::Photon& ph) const {
    if (!passSelectionNoIsolation(ph, m_loose_photon_selection)) return false;

    if (m_isolation && !m_isolation->passSelectionLoose(ph)) return false;

    return true;
  }

  bool Photon::passSelectionNoIsolation(const xAOD::Photon& ph,
                                            const std::string& photon_selection) const {
    // Photon author : AuthorPhoton/AuthorAmbiguous.
    // Also, for special-case recovery of soft photons:
    // xAOD::EgammaParameters::AuthorCaloTopo35.
    if (ph.author() != xAOD::EgammaParameters::AuthorPhoton
        && ph.author() != xAOD::EgammaParameters::AuthorAmbiguous) return false;

    if (ph.pt() < m_ptcut) return false;

    // removing bad photon cluser [http://cern.ch/go/kp8F]
    if (!ph.isGoodOQ(xAOD::EgammaParameters::BADCLUSPHOTON)) return false;

    //  Photon selection using derivation decorations
    if (photon_selection != "None")
      if (!ph.auxdataConst<char>(photon_selection)) return false;

    if (!ph.caloCluster()) return false;

    float abs_eta = std::abs(ph.caloCluster()->etaBE(2));
    if (abs_eta > m_etamax) return false;

    // Remove crack region
    if (abs_eta > 1.37 && abs_eta < 1.52) return false;

    // Photon cleaning
    static const SG::AuxElement::ConstAccessor<char> accDFCommonPhotonsCleaning("DFCommonPhotonsCleaning");
    static const SG::AuxElement::ConstAccessor<char> accDFCommonPhotonsCleaningNoTime("DFCommonPhotonsCleaningNoTime");

    if (!accDFCommonPhotonsCleaning(ph)) return false;
    if (!accDFCommonPhotonsCleaningNoTime(ph)) return false;
    
    // removing photon cluster in EMEC bad HV regions
    // https://twiki.cern.ch/twiki/bin/view/AtlasProtected/EGammaIdentificationRun2#Removal_of_Electron_Photon_clust
    if (!m_deadHVTool->accept(ph)) return false;


    return true;
  }

  void Photon::print(std::ostream& os) const {
    os << "Photon\n";
    os << "    * pT > " << m_ptcut << "\n";
    os << "    * |eta| < " << m_etamax << "\n";
    os << "    * Photon PID: " << m_photon_selection << "\n";
    os << "    * Loose Photon PID: " << m_loose_photon_selection << "\n";
    if (!m_isolation) os << "    * No isolation requirement\n";
    else m_isolation->print(os);
  }
}  // namespace top

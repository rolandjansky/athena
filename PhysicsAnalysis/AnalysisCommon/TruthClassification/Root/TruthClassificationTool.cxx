/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#include <set>

#include <MCTruthClassifier/MCTruthClassifierDefs.h>
#include <xAODTruth/TruthParticle.h>
#include <xAODTruth/xAODTruthHelpers.h>

#include "TruthClassification/TruthClassificationDefs.h"
#include "TruthClassification/TruthClassificationTool.h"

namespace
{
  bool isInSet(int origin, const std::set<int> &s)
  {
    return s.find(origin) != s.end();
  }
}


TruthClassificationTool::TruthClassificationTool(const std::string &type)
  : asg::AsgTool(type)
{
  declareProperty ("separateChargeFlipElectrons", m_separateChargeFlipElectrons, "separate prompt charge-flipped electrons");
}


StatusCode TruthClassificationTool::classify(const xAOD::IParticle &particle,
                                             unsigned int &classification) const
{
  Truth::Type type = Truth::Type::Unknown;
  ANA_CHECK(classify(particle, type));
  classification = static_cast<int>(type);
  return StatusCode::SUCCESS;
}


StatusCode TruthClassificationTool::classify(const xAOD::IParticle &particle,
                                             Truth::Type &classification) const
{
  if (dynamic_cast<const xAOD::Electron *> (&particle))
  {
    ANA_CHECK(classify(*dynamic_cast<const xAOD::Electron *> (&particle), classification));
  }
  else if (dynamic_cast<const xAOD::Muon *> (&particle))
  {
    ANA_CHECK(classify(*dynamic_cast<const xAOD::Muon *> (&particle), classification));
  }
  else
  {
    ANA_MSG_ERROR("Only electrons and muons are supported.");
    return StatusCode::FAILURE;
  }

  return StatusCode::SUCCESS;
}


StatusCode TruthClassificationTool::classify(const xAOD::Electron &electron,
                                             Truth::Type &classification) const
{
  namespace MC = MCTruthPartClassifier;

  if (!m_truthPdgId.isAvailable(electron))
  {
    ANA_MSG_ERROR("Electron does not have the 'truthPdgId' decoration.");
    return StatusCode::FAILURE;
  }

  if (!m_firstMotherTruthType.isAvailable(electron)
    || !m_firstMotherTruthOrigin.isAvailable(electron)
    || !m_firstMotherPdgId.isAvailable(electron))
  {
    ANA_MSG_ERROR("Electron does not have one or more 'firstEgMother' decorations.");
    return StatusCode::FAILURE;
  }

  int type = m_truthType(electron);
  int origin = m_truthOrigin(electron);
  int pdgId = m_truthPdgId(electron);
  int firstMotherType = m_firstMotherTruthType(electron);
  int firstMotherOrigin = m_firstMotherTruthOrigin(electron);
  int firstMotherPdgId = m_firstMotherPdgId(electron);
  // not in the smart slimming list, thus only in few derivations
  int lastMotherType = m_lastMotherTruthType.isAvailable(electron) ? m_lastMotherTruthType(electron) : -1;
  int lastMotherOrigin = m_lastMotherTruthOrigin.isAvailable(electron) ? m_lastMotherTruthOrigin(electron) : -1;
  int lastMotherPdgId = m_lastMotherPdgId.isAvailable(electron) ? m_lastMotherPdgId(electron) : -1;
  // fallback recorations
  int fallbackType{-1};
  if (m_fallbackTruthType.isAvailable(electron) && m_fallbackDR.isAvailable(electron))
  {
    fallbackType = m_fallbackDR(electron) < 0.05 ? m_fallbackTruthType(electron) : -1;
  }

  // !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
  // Prompt Photon Conversions
  // !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!

  // gamma -> e+ e-
  if (type == MC::BkgElectron
    && (origin == MC::PhotonConv || origin == MC::ElMagProc)
    && firstMotherType == MC::IsoPhoton && firstMotherOrigin == MC::PromptPhot)
  {
    classification = Truth::Type::PromptPhotonConversion;
    return StatusCode::SUCCESS;
  }

  // H -> gamma gamma, gamma -> e+ e-
  if (type == MC::BkgElectron && origin == MC::PhotonConv
    && firstMotherType == MC::IsoPhoton && firstMotherOrigin == MC::Higgs)
  {
    classification = Truth::Type::PromptPhotonConversion;
    return StatusCode::SUCCESS;
  }

  // bkg electrons from bkg photons
  if (type == MC::BkgElectron && origin == MC::PhotonConv
    && firstMotherType == MC::UnknownPhoton && firstMotherOrigin == MC::NonDefined)
  {
    classification = Truth::Type::PromptPhotonConversion;
    return StatusCode::SUCCESS;
  }

  // bkg photon from UndrPhot; (Here there is a generator level photon (not gen electron ) that later converts)
  if (type == MC::BkgElectron
    && (origin == MC::PhotonConv || origin == MC::ElMagProc )
    && firstMotherType == MC::BkgPhoton && firstMotherOrigin == MC::UndrPhot)
  {
    classification = Truth::Type::PromptPhotonConversion;
    return StatusCode::SUCCESS;
  }

  // type = 16 and origin = 38 (again, this is a photon)
  if (type == MC::BkgPhoton && origin == MC::UndrPhot)
  {
    classification = Truth::Type::PromptPhotonConversion;
    return StatusCode::SUCCESS;
  }

  // Is an isolated photon
  if (type == MC::IsoPhoton && pdgId == 22)
  {
    classification = Truth::Type::PromptPhotonConversion;
    return StatusCode::SUCCESS;
  }

  // electrons from ElMagProc
  // when FSR, a better classification can be made with the fall back vars
  if (type == MC::BkgElectron && origin == MC::ElMagProc
    && firstMotherType == MC::UnknownPhoton && firstMotherOrigin == MC::NonDefined)
  {
    classification = Truth::Type::PromptPhotonConversion;
    return StatusCode::SUCCESS;
  }
  if (type == MC::BkgElectron && origin == MC::ElMagProc
    && firstMotherType == MC::NonIsoPhoton && firstMotherOrigin == MC::FSRPhot)
  {
    classification = Truth::Type::PromptPhotonConversion;
    return StatusCode::SUCCESS;
  }

  // TODO: Message from Otilia: """
  // but it's not clear if these electrons are really
  // "fakes" or they should go in the real category (we don't know from where
  // this photon is coming...). I would say more truth studies should be done.
  // """
  // Hence the warning message...
  if (type == MC::BkgElectron && origin == MC::PhotonConv
    && firstMotherType == MC::Unknown && firstMotherOrigin == MC::ZBoson)
  {
    ANA_MSG_WARNING("Electron identified as from a PromptPhotonConversion, "
                    "but this type of electron needs further study!");
    classification = Truth::Type::PromptPhotonConversion;
    return StatusCode::SUCCESS;
  }

  // when always a photon (last mum is a photon, even if the truth PDG is 11 and first mum PDG is 11 ):
  // very likely these are internal conversions;  last_mum_pdgId == 22 important as the cases with last_mum_pdgId == 11 were found to be quite often close to a true electron
  if (type == MC::BkgElectron && firstMotherType == MC::BkgElectron
    && origin == MC::PhotonConv && firstMotherOrigin == MC::PhotonConv
    && std::abs(firstMotherPdgId) == 11 && std::abs(firstMotherPdgId) == 11 )
  {  // electron
    if(lastMotherType == -1 || (lastMotherType == MC::GenParticle && (lastMotherPdgId == 22 || std::abs(lastMotherPdgId) == 11)))
    {
      // lastMotherType == -1 ==>  when the last mother info is not stored in the derivations
      classification = Truth::Type::PromptPhotonConversion;
      return StatusCode::SUCCESS;
    }
  }


  // !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
  // Is muon reco as electron or ele radiated by muons
  // !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!

  if ((type == MC::NonIsoElectron || type == MC::NonIsoPhoton)
    && (origin == MC::Mu || firstMotherOrigin == MC::Mu))
  {
    classification = Truth::Type::ElectronFromMuon;
    return StatusCode::SUCCESS;
  }

  if (type == MC::BkgElectron && firstMotherOrigin == MC::Mu)
  {
    classification = Truth::Type::ElectronFromMuon;
    return StatusCode::SUCCESS;
  }

  if (type == MC::IsoMuon || type == MC::NonIsoMuon || type == MC::BkgMuon || type == MC::UnknownMuon) {
    classification = Truth::Type::ElectronFromMuon;
    return StatusCode::SUCCESS;
  }


  // !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
  // Tau decays
  // !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!

  // Non-isolated electron/photon from tau decay
  if ((type == MC::NonIsoElectron || type == MC::NonIsoPhoton)
    && origin == MC::TauLep)
  {
    classification = Truth::Type::TauDecay;
    return StatusCode::SUCCESS;
  }

  // tau -> tau gamma, gamma -> e+ e-, etc
  if ((firstMotherType == MC::NonIsoElectron || firstMotherType == MC::NonIsoPhoton)
    && firstMotherOrigin == MC::TauLep)
  {
    classification = Truth::Type::TauDecay;
    return StatusCode::SUCCESS;
  }


  // !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
  // Light hadron sources
  // !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!

  if (type == MC::Hadron || fallbackType == MC::Hadron)
  {
    classification = Truth::Type::LightFlavorDecay;
    return StatusCode::SUCCESS;
  }

  if (firstMotherType == MC::BkgElectron)
  {
    if ((origin == MC::DalitzDec || origin == MC::ElMagProc)
      && (hasLightHadronOrigin(origin) || hasLightHadronOrigin(firstMotherOrigin)))
    {
      classification = Truth::Type::LightFlavorDecay;
      return StatusCode::SUCCESS;
    }
  }

  if (type == MC::BkgElectron)
  {
    if (origin == MC::DalitzDec || firstMotherOrigin == MC::DalitzDec) {
      classification = Truth::Type::LightFlavorDecay;
      return StatusCode::SUCCESS;
    }
    if (hasLightHadronOrigin(origin) || hasLightHadronOrigin(firstMotherOrigin)) {
      classification = Truth::Type::LightFlavorDecay;
      return StatusCode::SUCCESS;
    }
  }

  if (type == MC::BkgPhoton
    && (hasLightHadronOrigin(origin) || hasLightHadronOrigin(firstMotherOrigin)))
  {
    classification = Truth::Type::LightFlavorDecay;
    return StatusCode::SUCCESS;
  }


  // !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
  // From B hadron
  // !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!

  if (hasBHadronOrigin(origin) || hasBHadronOrigin(firstMotherOrigin))
  {
    classification = Truth::Type::BHadronDecay;
    return StatusCode::SUCCESS;
  }

  // !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
  // From C hadron
  // !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!

  if (type != MC::IsoElectron && (hasCHadronOrigin(origin) || hasCHadronOrigin(firstMotherOrigin)))
  {
    classification = Truth::Type::CHadronDecay;
    return StatusCode::SUCCESS;
  }


  // !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
  // Prompt / Isolated electrons
  // !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!

  if (isPromptElectron(electron))
  {
    if (m_separateChargeFlipElectrons && isChargeFlipElectron(electron))
    {
      classification = Truth::Type::ChargeFlipIsoElectron;
      return StatusCode::SUCCESS;
    }

    classification = Truth::Type::IsoElectron;
    return StatusCode::SUCCESS;
  }


  // !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
  // Unknown & known Unknown
  // !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!

  // TODO: See if we want this or not. Now we check if this is something we are
  // able to classify or not. Note that this might be a bit dangerous because
  // the reasons for not having origin and status codes might be complex. The
  // main idea is to weed out things we don't have a hope of classifying due to
  // missing or unknown information.
  const xAOD::TruthParticle *truthParticle = xAOD::TruthHelpers::getTruthParticle(electron);
  int status = (truthParticle != nullptr && truthParticle->isAvailable<int>("status")) ? truthParticle->status() : 0;
  bool stable = (status == 1);

  if (origin == MC::NonDefined && firstMotherOrigin == MC::NonDefined)
  {
    if (!stable)
    {
      if ((type == MC::Unknown || type == MC::UnknownPhoton) && firstMotherType == MC::Unknown)
      {
        classification = Truth::Type::KnownUnknown;
        return StatusCode::SUCCESS;
      }
    } else {
      if ((type == MC::Unknown && firstMotherType == MC::Unknown)
        || (type == MC::UnknownElectron && firstMotherType == MC::UnknownElectron))
      {
        classification = Truth::Type::KnownUnknown;
        return StatusCode::SUCCESS;
      }

      if (type == MC::UnknownPhoton)
      {
        classification = Truth::Type::KnownUnknown;
        return StatusCode::SUCCESS;
      }
    }
  }

  // non-iso photons with no info available to classify
  if (type == MC::NonIsoPhoton && origin == MC::FSRPhot && pdgId == 22
    && firstMotherType == 0 && firstMotherOrigin == 0 && firstMotherPdgId == 0)
  {
    if (lastMotherType == -1 || (lastMotherType == 0 && lastMotherOrigin == 0 && lastMotherPdgId == 0))
    { // last_firstMotherType == -1 ==>  when the last_mum info is not stored in the derivations
      classification = Truth::Type::KnownUnknown;
      return StatusCode::SUCCESS;
    }
  }

  ANA_MSG_WARNING("Electron type unknown: type = " << type << ", origin = " << origin);

  // debug printout
  if (truthParticle != nullptr)
  {
    const xAOD::TruthParticle *parent = truthParticle;
    ATH_MSG_DEBUG("Unknown particle decay chain:");
    std::string out = "\t";
    while (parent != nullptr)
    {
      out.append(std::to_string(parent->pdgId()));
      parent = parent->parent();
      if (parent) out.append(" -> ");
    }
    ATH_MSG_DEBUG(out);
  }

  classification = Truth::Type::Unknown;
  return StatusCode::SUCCESS;
}


StatusCode TruthClassificationTool::classify(const xAOD::Muon &muon,
                                             Truth::Type &classification) const
{
  namespace MC = MCTruthPartClassifier;

  int type = m_truthType(muon);
  int origin = m_truthOrigin(muon);
  // fallback recorations
  int fallbackType{-1};
  if (m_fallbackTruthType.isAvailable(muon) && m_fallbackDR.isAvailable(muon))
  {
    fallbackType = m_fallbackDR(muon) < 0.05 ? m_fallbackTruthType(muon) : -1;
  }

  // !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
  // muons from taus
  // !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!

  if (type == MC::NonIsoMuon && origin == MC::TauLep)
  {
    classification = Truth::Type::TauDecay;
    return StatusCode::SUCCESS;
  }


  // !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
  // Light hadron sources
  // !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!

  if (type == MC::BkgMuon && hasLightHadronOrigin(origin))
  {
    classification = Truth::Type::LightFlavorDecay;
    return StatusCode::SUCCESS;
  }

  if (type == MC::Hadron || fallbackType == MC::Hadron )
  {
    classification = Truth::Type::LightFlavorDecay;
    return StatusCode::SUCCESS;
  }


  // !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
  // From B hadron
  // !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!

  if (hasBHadronOrigin(origin))
  {
    classification = Truth::Type::BHadronDecay;
    return StatusCode::SUCCESS;
  }

  // !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
  // From C hadron
  // !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!

  if (type != MC::IsoMuon && hasCHadronOrigin(origin))
  {
    classification = Truth::Type::CHadronDecay;
    return StatusCode::SUCCESS;
  }
  // TODO:: There is a comment in the example code about J/psi but there is a
  // separate origin code for that: `MC::JPsi == 28.` --> this might not be in all samples/generators?


  // !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
  // prompt muons
  // !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
  // Check if the type of muon is IsoMuon(6) and whether the origin
  // of the muon is from a prompt source
  static const std::set<int> promptOrigin({
      MC::SingleMuon,  // Single muon (origin = 2) from muon twiki
      MC::top,
      MC::WBoson,
      MC::ZBoson,
      MC::Higgs,
      MC::HiggsMSSM,
      MC::SUSY,
      MC::DiBoson,
      MC::CCbarMeson, // PromptQuarkoniumDecay
      MC::BBbarMeson,
  });
  if (type == MC::IsoMuon && isInSet(origin, promptOrigin))
  {
    classification = Truth::Type::PromptMuon;
    return StatusCode::SUCCESS;
  }


  // !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
  // Unknown & known Unknown
  // !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!

  if (type == MC::UnknownMuon && origin == MC::NonDefined)
  {
    classification = Truth::Type::KnownUnknown;
    return StatusCode::SUCCESS;
  }

  const xAOD::TruthParticle *truthParticle = xAOD::TruthHelpers::getTruthParticle(muon);
  int status = (truthParticle != nullptr && truthParticle->isAvailable<int>("status")) ? truthParticle->status() : 0;
  bool stable = (status == 1);

  if (!stable) {
    if (type == MC::Unknown && origin == MC::NonDefined)
    { // Data
      classification = Truth::Type::KnownUnknown;
      return StatusCode::SUCCESS;
    }
    if (type == -99999 && origin == -99999)
    { // MC - no status = 1 truth particle associated with the primary track
      classification = Truth::Type::KnownUnknown;
      return StatusCode::SUCCESS;
    }
  }

  ATH_MSG_WARNING("Muon type unknown: type = " << type << ", origin = " << origin);

  classification = Truth::Type::Unknown;
  return StatusCode::SUCCESS;
}


bool TruthClassificationTool::isPromptElectron(const xAOD::Electron &electron) const
{
  namespace MC = MCTruthPartClassifier;

  // Electron is IsoElectron - return true
  int type = m_truthType(electron);
  if (type == MC::IsoElectron)
  {
    return true;
  }

  int origin = m_truthOrigin(electron);
  int pdgId = m_truthPdgId(electron);
  int firstMotherType = m_firstMotherTruthType(electron);
  int firstMotherOrigin = m_firstMotherTruthOrigin(electron);

  // Adding these cases from ElectronEfficiencyHelpers
  if (firstMotherType == MC::IsoElectron && std::abs(m_firstMotherPdgId(electron)) == 11)
  {
    return true;
  }

  // FSR photons from electrons
  if (origin == MC::FSRPhot && type == MC::NonIsoPhoton && std::abs(pdgId) == 11)
  {
    return true;
  }

  if (type == MC::BkgElectron && origin == MC::PhotonConv
    && firstMotherType == MC::NonIsoPhoton && firstMotherOrigin == MC::FSRPhot
    && std::abs(pdgId) == 11)
  {
    return true;
  }

  // If we reach here then it is not a prompt electron
  return false;
}



bool TruthClassificationTool::isChargeFlipElectron(const xAOD::Electron &electron) const
{
  namespace MC = MCTruthPartClassifier;

  int type = m_truthType(electron);
  int origin = m_truthOrigin(electron);
  int pdgId = m_truthPdgId(electron);
  int firstMotherType = m_firstMotherTruthType(electron);
  int firstMotherOrigin = m_firstMotherTruthOrigin(electron);
  int firstMotherPdgId = m_firstMotherPdgId(electron);

  // not consider FSR photons from electrons (the photon has no charge)
  if (origin == MC::FSRPhot && type == MC::NonIsoPhoton && std::abs(pdgId) == 11)
  {
    return false;
  }
  if (type == MC::BkgElectron && origin == MC::PhotonConv
    && firstMotherType == MC::NonIsoPhoton && firstMotherOrigin == MC::FSRPhot
    && std::abs(pdgId) == 11)
  {
    return false;
  }

  // bkg electrons with no additional info to help us classify them FSR -- not in the charge flip category
  if (type == MC::BkgElectron && origin == MC::PhotonConv
    && firstMotherType == MC::BkgElectron && firstMotherOrigin == MC::PhotonConv
    && std::abs(pdgId) == 11)
  {
	  return false;
  }

  if (electron.charge() != 0) {
    return (firstMotherPdgId * electron.charge()) > 0;
  }

  return (firstMotherPdgId * (-pdgId)) > 0;
}


bool TruthClassificationTool::hasBHadronOrigin(int origin) const
{
  namespace MC = MCTruthPartClassifier;
  static const std::set<int> b_hadrons({
    MC::BottomMeson,
    MC::BBbarMeson,
    MC::BottomBaryon,
  });
  return isInSet(origin, b_hadrons);
}

bool TruthClassificationTool::hasCHadronOrigin(int origin) const {
  namespace MC = MCTruthPartClassifier;
  static const std::set<int> c_hadrons({
    MC::CharmedMeson,
    MC::CCbarMeson,
    MC::CharmedBaryon,
  });
  return isInSet(origin, c_hadrons);
}

bool TruthClassificationTool::hasLightHadronOrigin(int origin) const {
  namespace MC = MCTruthPartClassifier;
  static const std::set<int> light_source({
    MC::PiZero,
    MC::LightMeson,
    MC::StrangeMeson,
    MC::LightBaryon,
    MC::StrangeBaryon,
    MC::PionDecay,
    MC::KaonDecay,
  });
  return isInSet(origin, light_source);
}

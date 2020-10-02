/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

// EDM include(s):
#include <AsgTools/MessageCheck.h>
#include <AsgTools/AnaToolHandle.h>

// Project dependent include(s)
#ifdef XAOD_STANDALONE
#include <xAODRootAccess/Init.h>
#include <xAODRootAccess/TEvent.h>
#else
#include <POOLRootAccess/TEvent.h>
#endif

// Tool interface(s):
#include <AsgAnalysisInterfaces/IClassificationTool.h>
#include <MuonAnalysisInterfaces/IMuonSelectionTool.h>

// EDM include(s):
#include <xAODEventInfo/EventInfo.h>
#include <xAODEgamma/ElectronContainer.h>
#include <xAODMuon/MuonContainer.h>
#include <xAODTruth/TruthParticle.h>
#include <xAODTruth/xAODTruthHelpers.h>

// ROOT include(s):
#include <TFile.h>

// std
#include <iomanip>
#include <memory>

// messaging
ANA_MSG_HEADER(Test)
ANA_MSG_SOURCE(Test, "TruthClassification")
using namespace Test;

int main(int argc, char* argv[])
{
  ANA_CHECK_SET_TYPE (int); // makes ANA_CHECK return ints if exiting function

  // Initialise the application:
#ifdef XAOD_STANDALONE
  StatusCode::enableFailure();
  ANA_CHECK(xAOD::Init());
#else
  IAppMgrUI *app = POOL::Init();
#endif

  // Use a default MC file for testing if none is provided
  TString fileName = "$ASG_TEST_FILE_MC";
  if (argc < 2) {
    ANA_MSG_WARNING("No file name received, using ASG_TEST_FILE_MC");
  } else {
    fileName = argv[1]; // use the user provided file
  }

  // The map of class names
  std::map<int, std::string> classNames {
    {0, "Unknown"},
    {1, "KnownUnknown"},
    {2, "IsoElectron"},
    {3, "ChargeFlipIsoElectron"},
    {4, "PromptMuon"},
    {5, "PromptPhotonConversion"},
    {6, "ElectronFromMuon"},
    {7, "TauDecay"},
    {8, "BHadronDecay"},
    {9, "CHadronDecay"},
    {10, "LightFlavorDecay"}
  };

  // Initialise TEvent reading
#ifdef XAOD_STANDALONE
  std::unique_ptr<TFile> ptrFile(TFile::Open(fileName, "READ"));
  xAOD::TEvent event(xAOD::TEvent::kClassAccess);
  ANA_CHECK(event.readFrom(ptrFile.get()));
#else
  POOL::TEvent event(POOL::TEvent::kClassAccess);
  event.readFrom(fileName);
#endif
  ANA_MSG_INFO("Opened file: " << fileName);

  // Decide how many events to run over:
  uint64_t entries = event.getEntries();
  if (argc > 2) {
    const uint64_t e = atoll(argv[2]);
    if (e < entries) {
      entries = e;
    }
  }

  bool do_electrons = true;
  bool do_muons = true;

  // Create the truth classification tool:
  ANA_MSG_INFO("Creating TruthClassificationTool...");
  asg::AnaToolHandle< CP::IClassificationTool > tool("TruthClassificationTool/TruthClassificationTool");
  ANA_CHECK(tool.retrieve());

  const SG::AuxElement::ConstAccessor<int> truthTypeAcc("truthType");
  const SG::AuxElement::ConstAccessor<int> truthOriginAcc("truthOrigin");
  const SG::AuxElement::ConstAccessor<int> truthPdgIdAcc("truthPdgId");
  const SG::AuxElement::ConstAccessor<int> firstMotherTruthTypeAcc("firstEgMotherTruthType");
  const SG::AuxElement::ConstAccessor<int> firstMotherTruthOriginAcc("firstEgMotherTruthOrigin");
  const SG::AuxElement::ConstAccessor<int> firstMotherPdgIdAcc("firstEgMotherPdgId");
  const SG::AuxElement::ConstAccessor<int> lastMotherTruthTypeAcc("lastEgMotherTruthType");
  const SG::AuxElement::ConstAccessor<int> lastMotherTruthOriginAcc("lastEgMotherTruthOrigin");
  const SG::AuxElement::ConstAccessor<int> lastMotherPdgIdAcc("lastEgMotherPdgId");
  // Fallback variables
  const SG::AuxElement::ConstAccessor<int> fallbackTruthTypeAcc("TruthClassifierFallback_truthType");
  const SG::AuxElement::ConstAccessor<int> fallbackTruthOriginAcc("TruthClassifierFallback_truthOrigin");
  const SG::AuxElement::ConstAccessor<float> fallbackDRAcc("TruthClassifierFallback_dR");
  // Ambiguity flag
  const SG::AuxElement::ConstAccessor<int> ambiguityAcc("DFCommonAddAmbiguity");
  const SG::AuxElement::ConstAccessor<char> passDFLooseAcc("DFCommonElectronsLHLoose");
  const SG::AuxElement::ConstAccessor<char> passDFLooseBLAcc("DFCommonElectronsLHLooseBL");
  const SG::AuxElement::ConstAccessor<char> passDFTightAcc("DFCommonElectronsLHTight");

  // Muon selection tool
  asg::AnaToolHandle< CP::IMuonSelectionTool > muonSelectionTool("CP::MuonSelectionTool/MuonSelectionTool");
  // Tight: 0, Med: 1, Loose: 2, VeryLoose: 3
  ANA_CHECK(muonSelectionTool.setProperty("MuQuality", 1));
  ANA_CHECK(muonSelectionTool.setProperty("MaxEta", 2.4));
  ANA_CHECK(muonSelectionTool.retrieve());

  std::map<int, int> el_counter;
  std::map<int, int> mu_counter;
  std::map<int, std::map<int, int>> unknown_el_type_origin_counter;
  std::map<int, int> unknown_mu_origin_counter;

  std::map<int, std::map<int, std::map<int, std::map<int, int>>>> ambiguity_map;

  // Loop over the events:
  for (uint64_t entry = 0; entry < entries; ++entry)
  {
    // Tell the object which entry to look at:
    event.getEntry(entry);

    // Electrons
    const xAOD::ElectronContainer *electrons{};
    ANA_CHECK(event.retrieve(electrons, "Electrons"));

    if (do_electrons)
    {
      for (const xAOD::Electron *el : *electrons)
      {
        // Look at electrons with pT > 1 GeV, |eta| < 2.47, LHLooseBL
        if (el->pt() < 4500 || std::abs(el->eta()) > 2.47)
        {
          continue;
        }

        if ((passDFLooseBLAcc.isAvailable(*el) && !passDFLooseBLAcc(*el))
          || (passDFLooseAcc.isAvailable(*el) && !passDFLooseAcc(*el)))
        {
          continue;
        }

        unsigned int classification{};
        ANA_CHECK(tool->classify(*el, classification));

        el_counter[classification]++;

        int ambiguity = ambiguityAcc.isAvailable(*el) ? ambiguityAcc(*el) : -99;
        if (classification == 0)
        {
          /// not in the smart slimming list, thus only in few derivations
          int type = truthTypeAcc(*el);
          int origin = truthOriginAcc(*el);
          int lastMotherType = lastMotherTruthTypeAcc.isAvailable(*el) ? lastMotherTruthTypeAcc(*el) : -1;
          int lastMotherOrigin = lastMotherTruthOriginAcc.isAvailable(*el) ? lastMotherTruthOriginAcc(*el) : -1;
          int lastMotherPdgId = lastMotherPdgIdAcc.isAvailable(*el) ? lastMotherPdgIdAcc(*el) : -1;
          int fallbackType = (fallbackTruthTypeAcc.isAvailable(*el) && fallbackDRAcc(*el) < 0.05) ? fallbackTruthTypeAcc(*el) : -1;
          int fallbackOrigin = (fallbackTruthOriginAcc.isAvailable(*el) && fallbackDRAcc(*el) < 0.05) ? fallbackTruthOriginAcc(*el) : -1;
          float fallbackDR = fallbackDRAcc.isAvailable(*el) ? fallbackDRAcc(*el) : -1;

          const xAOD::TruthParticle *truthParticle = xAOD::TruthHelpers::getTruthParticle(*el);
          int status = (truthParticle != nullptr && truthParticle->isAvailable<int>("status")) ? truthParticle->status() : -1;
          int pdgId = truthPdgIdAcc.isAvailable(*el)
            ? truthPdgIdAcc(*el)
            : (truthParticle != nullptr && truthParticle->isAvailable<int>("pdgId")) ? truthParticle->pdgId() : 0;

          ANA_MSG_WARNING("Unknown electron passing loose selection with "
            << "status = " << status
            << ", type = " << type
            << ", origin = " << origin
            << ", PDG ID = " << pdgId
            << ", first mother type = " << firstMotherTruthTypeAcc(*el)
            << ", first mother origin = " << firstMotherTruthOriginAcc(*el)
            << ", first mother PDG ID = " << firstMotherPdgIdAcc(*el)
            << ", last mother type = " << lastMotherType
            << ", last mother origin = " << lastMotherOrigin
            << ", last mother PDG ID = " << lastMotherPdgId
            << ", fallback type = " << fallbackType
            << ", fallback origin = " << fallbackOrigin
            << ", fallback DR= " << fallbackDR
            << ", ambiguity = " << ambiguity
            << ", pt = " << el->pt());

          unknown_el_type_origin_counter[type][origin]++;

          if (truthParticle == nullptr) {
            ANA_MSG_WARNING("Unknown electron is NOT truth matched!");
          }
        }

        /// How ambiguity works (from
        /// https://gitlab.cern.ch/atlas/athena/merge_requests/24852/)
		    /// https://indico.cern.ch/event/862748/contributions/3635039/attachments/1946503/3235131/conversionsIFF2019.11.18.pdf, page 13
		    /// "It is meant to be used together with ambiguityType to tighten the conversion veto by requiring (ambiguityType == 0 && DFCommonAddAmbiguity<=0)"
        /// Cuts to define the various types :
        /// -1 : no other track,
        /// 0 : other track exists but no good gamma reco,
        /// 1 : gamma*,
        /// 2 : material conversion
        if (ambiguityAcc.isAvailable(*el))
        {
          if (ambiguity >= -1) {
            ambiguity_map[classification][ambiguity][truthTypeAcc(*el)][truthOriginAcc(*el)]++;
          }
        }
      }
    }

    // Muons
    if (do_muons)
    {
      const xAOD::EventInfo *eventInfo{};
      ANA_CHECK(event.retrieve(eventInfo, "EventInfo"));

      const xAOD::MuonContainer *muons{};
      ANA_CHECK(event.retrieve(muons, "Muons"));

      for (const xAOD::Muon *mu : *muons)
      {
        if (mu->pt() < 3000 || std::abs(mu->eta()) > 2.5)
        {
          continue;
        }

        if (!muonSelectionTool->accept(*mu)) {
          continue;
        }

	      unsigned int classification{};
        ANA_CHECK(tool->classify(*mu, classification));

        mu_counter[classification]++;

        if (classification == 0)
        {
          int type = truthTypeAcc(*mu);
          int origin = truthOriginAcc(*mu);
          int fallbackType = (fallbackTruthTypeAcc.isAvailable(*mu) && fallbackDRAcc(*mu) < 0.05) ? fallbackTruthTypeAcc(*mu) : -1;
          int fallbackOrigin = (fallbackTruthOriginAcc.isAvailable(*mu) && fallbackDRAcc(*mu) < 0.05) ? fallbackTruthOriginAcc(*mu) : -1;
          float fallbackDR = (fallbackDRAcc.isAvailable(*mu)) ? fallbackDRAcc(*mu) : -1;

          const xAOD::TruthParticle *truthParticle = xAOD::TruthHelpers::getTruthParticle(*mu);
          int status = (truthParticle != nullptr && truthParticle->isAvailable<int>("status")) ? truthParticle->status() : -1;
          int pdgId = truthPdgIdAcc.isAvailable(*mu)
            ? truthPdgIdAcc(*mu)
            : (truthParticle != nullptr && truthParticle->isAvailable<int>("pdgId")) ? truthParticle->pdgId() : 0;

		      ANA_MSG_WARNING("Unknown muon passing Medium selection with "
            << "status = " << status
            << ", type = " << type
            << ", origin = " << origin
            << ", PDG ID = " << pdgId
            << ", fallback type = " << fallbackType
            << ", fallback origin = " << fallbackOrigin
            << ", fallback DR= " << fallbackDR
            << ", pt = " << mu->pt());
          
          unknown_mu_origin_counter[origin]++;

          if (truthParticle == nullptr) {
            ANA_MSG_WARNING("Unknown muon is NOT truth matched!");
          }
        }
      }
    }
  }  // Event loop

  // Use standard c++ stdout for nice log
  std::cout << std::endl << std::endl;

  std::cout << "Electron summary:" << std::endl;
  for (const auto&[classification, count] : el_counter)
  {
    std::cout << "  " << std::setw(25) << (classNames.at(classification) + ": ") << std::right << std::setw(6) << count << std::endl;
  }
  std::cout << std::endl;

  std::cout << "Muon summary:" << std::endl;
  for (const auto&[classification, count] : mu_counter)
  {
    std::cout << "  " << std::setw(25) << (classNames.at(classification) + ": ") << std::right << std::setw(6) << count << std::endl;
  }
  std::cout << std::endl;

  std::cout << "Unknown electron origin:" << std::endl;
  for (const auto&[type, map] : unknown_el_type_origin_counter)
  {
    for (const auto&[origin, count] : map)
    {
      std::cout << "  " << std::setw(2) << type << ", " << std::setw(2) << origin << ": " << std::setw(6) << count << std::endl;
    }
  }
  std::cout << std::endl;

  std::cout << "Unknown muon origin:" << std::endl;
  for (const auto&[origin, count] : unknown_mu_origin_counter)
  {
    std::cout << "  " << std::setw(2) << origin << ": " << std::setw(6) << count << std::endl;
  }
  std::cout << std::endl;

  std::cout << "Electron ambiguity map:" << std::endl;
  std::cout << "           Classification | Amb. | Type | Origin | Count" << std::endl;
  std::cout << "-----------------------------------------------------------" << std::endl;
  for (const auto&[classification, map1] : ambiguity_map) {
    for (const auto&[ambiguity, map2] : map1) {
      for (const auto&[type, map3] : map2) {
        for (const auto&[origin, count] : map3) {
          std::cout << "  " << std::setw(23) << classNames.at(classification)
            << " | " << std::setw(4) << ambiguity
            << " | " << std::setw(4) << type
            << " | " << std::setw(6) << origin
            << " | " << std::setw(6) << count
            << std::endl;
        }
      }
    }
  }
  std::cout << std::endl;

  // trigger finalization of all services and tools created by the Gaudi Application
#ifndef XAOD_STANDALONE
  app->finalize();
#endif

  return 0;
}

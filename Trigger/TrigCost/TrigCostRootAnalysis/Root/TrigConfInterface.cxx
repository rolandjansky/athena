// Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// -------------------------------------------------------------
//  author: Tim Martin <Tim.Martin@cern.ch>
// -------------------------------------------------------------

// STL include(s):
#include <iostream>
#include <fstream>
#include <assert.h>
#include <algorithm> //std::replace

// ROOT include(s):
#include <TChain.h>
#include <TTree.h>
#include <TFile.h>
#include <TError.h>

// Local include(s):
#include "../TrigCostRootAnalysis/TrigConfInterface.h"
#include "../TrigCostRootAnalysis/Utility.h"
#include "../TrigCostRootAnalysis/Config.h"

// RootCore TrigRootAnalysis include(s):
#include "TrigRootAnalysis/TrigDecisionToolD3PD.h"
#include "TrigRootAnalysis/TrigConfigSvcD3PD.h"

namespace TrigCostRootAnalysis {
  // Initialise statics
  D3PD::TrigDecisionToolD3PD* TrigConfInterface::m_tdt;
  D3PD::TrigConfigSvcD3PD* TrigConfInterface::m_tct;

  Bool_t TrigConfInterface::m_isConfigured = kFALSE;

  Bool_t TrigConfInterface::m_usingNtupleMetadata = kTRUE;
  Bool_t TrigConfInterface::m_usingNtuplePrescales = kTRUE;

  DBKey TrigConfInterface::m_key(0, 0, 0);
  std::set<DBKey> TrigConfInterface::m_seenKeys;
  std::map<UInt_t, DBKey> TrigConfInterface::m_lumiToKeyMap;
  std::map<std::string, Bool_t> TrigConfInterface::m_mainChains;

  /**
   * Link tool to trigger meta data. Call once per chain.
   * @param inputChain TChain pointer to input files
   * @return Returns kTRUE upon successful configuration.
   */
  Bool_t TrigConfInterface::configure(TChain* inputChain) {
    TFile* file = 0;
    TChain* chain = dynamic_cast< TChain* >(inputChain);

    if (chain) {
      // We are running locally...
      Info("TrigConfInterface::configure", "Opening a new file locally...");
      file = chain->GetFile();
    } else {
      // We are running on PROOF:
      Info("TrigConfInterface::configure", "Opening a new file on PROOF...");
      file = inputChain->GetCurrentFile();
    }

    TTree* confTree = dynamic_cast< TTree* >(file->Get(Config::config().getStr(kConfigPrefix).c_str()));
    if (!confTree) {
      Error("TrigConfInterface::configure", "Couldn't retrieve configuration metadata tree!");
      m_isConfigured = kFALSE;
      return kFALSE;
    }

    if (m_tdt) delete m_tdt;
    m_tdt = new D3PD::TrigDecisionToolD3PD();

    if (!m_tdt->SetEventTree(inputChain)) {
      Error("TrigConfInterface::configure", "Problems with setting the event tree to the TrigDecisionTool.");
      m_isConfigured = kFALSE;
      return kFALSE;
    }
    if (!m_tdt->SetConfigTree(confTree)) {
      Error("TrigConfInterface::configure", "Problems setting the trigger config tree to the TrigDecisionTool.");
      m_isConfigured = kFALSE;
      return kFALSE;
    }
    m_isConfigured = kTRUE;


    Info("TrigConfInterface::configure", "Available configs:");
    getTCT()->PrintSummary();

    m_isConfigured = kTRUE;
    return m_isConfigured;
  }

  /**
   * @return When configured, returns the super-master key for the currently processing event.
   */
  Int_t TrigConfInterface::getCurrentSMK() {
    // XXX this is temporary
    Int_t smk = getTDT()->GetSMK();

    //if (smk == 65535) return 0;
    return smk;
  }

  /**
   * @return When configured, returns the level 1 prescale key for the currently processing event.
   */
  Int_t TrigConfInterface::getCurrentL1PSK() {
    return getTDT()->GetL1PSK();
  }

  /**
   * @return When configured, returns the HLT prescale key for the currently processing event.
   */
  Int_t TrigConfInterface::getCurrentHLTPSK() {
    return getTDT()->GetHLTPSK();
  }

  /**
   * @return When configured, returns the current HLT keyset in small wrapper object.
   */
  const DBKey& TrigConfInterface::getCurrentDBKey() {
    m_key.set(getCurrentSMK(), getCurrentL1PSK(), getCurrentHLTPSK());
    return m_key;
  }

  /**
   * Check if we have seen this configuration before - and if we need to dump it
   */
  void TrigConfInterface::newEvent(UInt_t lb) {
    Bool_t seen = (m_seenKeys.count(getCurrentDBKey()) == 1);

    if (m_lumiToKeyMap.count(lb) == 0) m_lumiToKeyMap[ lb ] = getCurrentDBKey();
    if (seen) return;

    m_seenKeys.insert(getCurrentDBKey());
    if (Config::config().getInt(kOutputMenus) == kTRUE) {
      Info("TrigConfInterface::newEvent", "Exporting trigger configuration JSON file for %s",
           getCurrentDBKey().name().c_str());
      TrigConfInterface::dump();
    }
  }

  /**
   * Look at which LB each keyset was being used in
   */
  void TrigConfInterface::populateLBPerKeysetStrings() {
    for (const auto keyset : m_seenKeys) {
      // Set of my LB
      std::set<UInt_t> myLB;
      for (const auto it : m_lumiToKeyMap) {
        if (it.second == keyset) {
          myLB.insert(it.first);
        }
      }
      // Now have a set of all LB, need to make a nice string
      std::stringstream ss;
      ss << keyset.name() << ":";
      Int_t previous = -1;
      Bool_t chainInProgress = kFALSE;
      for (const auto lumiBlock : myLB) {
        if (lumiBlock == (UInt_t) (previous + 1)) { // Chain
          chainInProgress = kTRUE;
        } else if (chainInProgress == kTRUE) { // Chain has just ended
          chainInProgress = kFALSE;
          ss << "-" << previous << ", " << lumiBlock;
        } else {
          if (previous != -1) ss << ", ";
          ss << lumiBlock;
        }
        previous = lumiBlock;
      }
      if (chainInProgress == kTRUE) ss << "-" << previous;
      Config::config().addVecEntry(kLBPerKeyset, ss.str());
    }
    return;
  }

  /**
   * Fetch the ID of L1 item from name.
   * @param name Const reference to name of L1 chain.
   * @return Integer CTPID for L1 chain.
   */
  Int_t TrigConfInterface::getCtpId(const std::string& name) {
    if (name == "NO_SEED") return -1;

    return getTCT()->GetCTPId(name);
  }

  /**
   * Fetch the name of a L1 item from its ID
   * @param name CPT ID of L1 chain.
   * @return Name of the L1 chain.
   */
  const std::string& TrigConfInterface::getNameFromCtpId(Int_t ctpId) {
    return getTCT()->GetNameFromCTPId(ctpId);
  }

  /**
   * Fetch the low chain name of a given chain, EF->L2, L2->L1, HLT->L1.
   * @param name Const reference to higher chain name.
   * @return Const reference to lower chain name.
   */
  const std::string& TrigConfInterface::getLowerChainName(const std::string& name) {
    return getTCT()->GetLowerChainName(name);
  }

  /**
   * Fetch the current passtrhough PS value for a given chain
   * @param name Const reference to higher chain name.
   * @return Passthrough value
   */
  Float_t TrigConfInterface::getPassthrough(const std::string& name) {
    return getTCT()->GetPassthrough(name);
  }

  /**
   * Explicitly load given entry in tree. This should not be needed.
   * @param entry Entry in tree to load.
   */
  void TrigConfInterface::getEntry(Long64_t /*entry*/) {
    assert(m_isConfigured);
    // This is not, it appears, needed.
    Warning("TrigConfInterface::getEntry", "This does not need to be called"); //TODO Remove this func
    return;
    //m_tdt->GetEntry( entry );
  }

  /**
   * Fetch HLT string name using currently configured access method. Supplying level helps but is not needed.
   * @param chainID HLT ID number of chain
   * @param level Chain level (2 for L2 or HLT, 3 for EF)
   * @return Const reference to chain name. Empty string if invalid ID or if using unsupported access mode.
   */
  const std::string TrigConfInterface::getHLTNameFromChainID(Int_t chainID, Int_t level) {
    if (getUsingNtupleMetadata() == kTRUE) {
      // Currently split into L2 and EF
      if (level == 2 || level == 0) { // If L2 (HLT responds as L2) or unspecified
        if (getTCT()->GetL2NameFromChainId(chainID) != "" && getTCT()->GetL2NameFromChainId(chainID) != "0") {
          //std::cout << chainID << ", THE NAME IS " << getTCT()->GetL2NameFromChainId( chainID ) << std::endl;
          return getTCT()->GetL2NameFromChainId(chainID);
        }
      }
      // Else try EF, unless we're running on HLT as there is no EF any more, then we can just return cannot find.
      if (Config::config().getInt(kDoHLT) == kTRUE) return Config::config().getStr(kBlankString);

      return getTCT()->GetEFNameFromChainId(chainID);
    } else {
      Error("TrigConfInterface::getHLTNameFromChainID", "XML based menu navigation not yet included.");
    }
    return Config::config().getStr(kBlankString);
  }

  /**
   * Fetch the number of groups a chain is registered under.
   * TODO check that we don't need to add extra code for L2 and EF here?
   * @param chainID HLT ID number of chain
   * @return Number of groups this chain belongs to
   */
  UInt_t TrigConfInterface::getNHLTGroupNamesFromChainID(Int_t chainID) {
    if (getUsingNtupleMetadata() == kTRUE) {
      UInt_t index = getTCT()->GetChainIndexFromCounter(chainID);
      if (index == UINT_MAX) {
        Warning("TrigConfInterface::getNHLTGroupNamesFromChainID", "Chain with Counter %i is not in the configuration.",
                chainID);
        return 0;
      }
      return getTCT()->GetChainGroupNameSize(index);
    } else {
      Error("TrigConfInterface::getNHLTGroupNamesFromChainID", "XML based menu navigation not yet included.");
    }
    return 0;
  }

  /**
   * Fetch the name of one of the groups this chain is registered under.
   * TODO check that we don't need to add extra code for L2 and EF here?
   * @param chainID HLT ID number of chain
   * @param group the index of the group to fetch (0 to getNHLTGroupNamesFromChainID() -1)
   * @return New'd string of the group name.
   */
  const std::string TrigConfInterface::getHLTGroupNameFromChainID(Int_t chainID, UInt_t group) {
    if (getUsingNtupleMetadata() == kTRUE) {
      UInt_t index = getTCT()->GetChainIndexFromCounter(chainID);
      if (index == UINT_MAX) {
        Warning("TrigConfInterface::getHLTGroupNameFromChainID", "Chain with Counter %i is not in the configuration.",
                chainID);
        return Config::config().getStr(kBlankString);
      }
      if (group >= getNHLTGroupNamesFromChainID(chainID)) {
        Warning("TrigConfInterface::getHLTGroupNameFromChainID", "Requested group %i out of range, size is %i.", group, getNHLTGroupNamesFromChainID(
                  chainID));
        return Config::config().getStr(kBlankString);
      }
      return getTCT()->GetChainGroupName(index, group);
    } else {
      Error("TrigConfInterface::getHLTGroupNameFromChainID", "XML based menu navigation not yet included.");
    }
    return Config::config().getStr(kBlankString);
  }

  /**
   * Fetch HLT sequence name from sequence index number. Uses advanced trigger configuration information.
   * @param index Index number of sequence.
   * @return Const reference to sequence name. Empty string if invalid ID or if using unsupported access mode.
   */
  const std::string& TrigConfInterface::getHLTSeqNameFromIndex(UInt_t index) {
    if (getUsingNtupleMetadata() == kTRUE) {
      return getTCT()->GetSequenceNameFromIndex(index);
    } else {
      Error("TrigConfInterface::getHLTSeqNameFromIndex", "XML based menu navigation not yet included.");
    }
    return Config::config().getStr(kBlankString);
  }

  /**
   * Fetch HLT algorithm name using the parents sequence index and position of algorithm within sequence.
   * @param index Index number of parent sequence.
   * @param position Position of algorithm within parent sequence.
   * @return Const reference to algorithm name. Empty string if invalid ID or if using unsupported access mode.
   */
  const std::string& TrigConfInterface::getHLTAlgNameFromSeqIDAndAlgPos(UInt_t index, UInt_t position) {
    if (getUsingNtupleMetadata() == kTRUE) {
      return getTCT()->GetAlgNameFromSeqIDAndAlgPos((int) index, (int) position);
    } else {
      Error("TrigConfInterface::getHLTAlgNameFromSeqIDAndAlgPos", "XML based menu navigation not yet included.");
    }
    return Config::config().getStr(kBlankString);
  }

  /**
   * Fetch hash of HLT algorithm name using the parents sequence index and position of algorithm within sequence.
   * @param index Index number of parent sequence.
   * @param position Position of algorithm within parent sequence.
   * @return Const reference to algorithm name. Empty string if invalid ID or if using unsupported access mode.
   */
  UInt_t TrigConfInterface::getHLTAlgNameIDFromSeqIDAndAlgPos(UInt_t index, UInt_t position) {
    if (getUsingNtupleMetadata() == kTRUE) {
      return getTCT()->GetAlgNameIDFromSeqIDAndAlgPos((int) index, (int) position);
    } else {
      Error("TrigConfInterface::getHLTAlgNameIDFromSeqIDAndAlgPos", "XML based menu navigation not yet included.");
    }
    return 0;
  }

  /**
   * Fetch HLT algorithm class type name using the parents sequence index and position of algorithm within sequence.
   * @param index Index number of parent sequence.
   * @param position Position of algorithm within parent sequence.
   * @return Const reference to algorithm class type name. Empty string if invalid ID or if using unsupported access
   *mode.
   */
  const std::string& TrigConfInterface::getHLTAlgClassNameFromSeqIDAndAlgPos(UInt_t index, UInt_t position) {
    if (getUsingNtupleMetadata() == kTRUE) {
      return getTCT()->GetAlgClassNameFromSeqIDAndAlgPos((Int_t) index, (Int_t) position);
    } else {
      Error("TrigConfInterface::getHLTAlgClassNameFromSeqIDAndAlgPos", "XML based menu navigation not yet included.");
    }
    return Config::config().getStr(kBlankString);
  }

  /**
   * Fetch hash of HLT algorithm class type name using the parents sequence index and position of algorithm within
   *sequence.
   * @param index Index number of parent sequence.
   * @param position Position of algorithm within parent sequence.
   * @return Hash of algorithm class type name. 0 if invalid ID or if using unsupported access mode.
   */
  UInt_t TrigConfInterface::getHLTAlgClassNameIDFromSeqIDAndAlgPos(UInt_t index, UInt_t position) {
    if (getUsingNtupleMetadata() == kTRUE) {
      return getTCT()->GetAlgClassNameIDFromSeqIDAndAlgPos((Int_t) index, (Int_t) position);
    } else {
      Error("TrigConfInterface::getHLTAlgClassNameIDFromSeqIDAndAlgPos", "XML based menu navigation not yet included.");
    }
    return 0;
  }

  /**
   * Fetch prescale factor for given chain name.
   * @param chainName Name of chain to fetch prescale for.
   * @return Floating point prescale factor for chain in current menu. 0 on fail.
   */
  Float_t TrigConfInterface::getPrescale(std::string chainName) {
    if (getUsingNtuplePrescales() == kTRUE) {
      if (chainName == Config::config().getStr(kAlwaysPassString)) return 1.;

      return getTCT()->GetPrescale(chainName);
    } else {
      Error("TrigConfInterface::getPrescale", "XML based menu navigation not yet included.");
    }
    return 0;
  }

  /**
   * Get number of metadata values in file
   * @return Number of string->string metadata pairs
   */
  UInt_t TrigConfInterface::getMetaStringN() {
    if (getUsingNtupleMetadata() == kTRUE) {
      return getTCT()->GetMetaStringN();
    }
    return 0;
  }

  /**
   * Fetch a string key from the config metadata
   * @param m Which metadata to fetch.
   * @return String key value for this metadata
   */
  std::string TrigConfInterface::getMetaStringKey(UInt_t m) {
    if (getUsingNtuplePrescales() == kTRUE) {
      return getTCT()->GetMetaStringKey(m);
    }
    return Config::config().getStr(kBlankString);
  }

  /**
   * Fetch a string payload from the config metadata
   * @param m Which metadata to fetch.
   * @return String payload value for this metadata
   */
  std::string TrigConfInterface::getMetaStringVal(UInt_t m) {
    if (getUsingNtuplePrescales() == kTRUE) {
      return getTCT()->GetMetaStringVal(m);
    }
    return Config::config().getStr(kBlankString);
  }

  /**
   * Fetch a string payload from the config metadata
   * @param key String key to fetch
   * @return String payload value for this metadata
   */
  std::string TrigConfInterface::getMetaStringVal(std::string key) {
    if (getUsingNtuplePrescales() == kTRUE) {
      for (UInt_t i = 0; i < getMetaStringN(); ++i) {
        if (getMetaStringKey(i) == key) return getMetaStringVal(i);
      }
    }
    return Config::config().getStr(kBlankString);
  }

  /**
   * Fetch a map which contains the bunch group information.
   * This may be fetched from the database or from the CTP
   * conf. Both are tried but only one will be returned.
   * @return Map of bunch group name -> number of BCIDs in bunch group
   */
  StringIntMap_t TrigConfInterface::getBunchGroupSetup() {
    StringIntMap_t BGDatabase, BGCTPConf;
    UInt_t BCIDCountDB = 0, BCIDCountCTPConf = 0;

    for (Int_t bg = 0; bg < Config::config().getInt(kNBunchGroups); ++bg) {
      std::stringstream ss, ssDB, ssCTPName, ssCTPSize;
      // Database
      ssDB << "DB:BGRP" << bg;
      std::string BGDBVal = getMetaStringVal(ssDB.str());
      if (BGDBVal != Config::config().getStr(kBlankString)) {
        ss << "BGRP" << bg;
        BGDatabase[ss.str()] = stringToInt(BGDBVal);
        BCIDCountDB += stringToInt(BGDBVal);
      }
      // CTP Config
      ssCTPName << "CTPConfig:NAME:BGRP" << bg;
      ssCTPSize << "CTPConfig:SIZE:BGRP" << bg;
      std::string CTPBGName = getMetaStringVal(ssCTPName.str());
      std::string CTPBGSize = getMetaStringVal(ssCTPSize.str());
      if (CTPBGName != Config::config().getStr(kBlankString) && CTPBGSize != Config::config().getStr(kBlankString)) {
        BGCTPConf[CTPBGName] = stringToInt(CTPBGSize);
        BCIDCountCTPConf += stringToInt(CTPBGSize);
      }
    }
    // Hopefully these agree, but let's assume if not that the one with the most active BCIDs
    if (BCIDCountDB > BCIDCountCTPConf) {
      return BGDatabase;
    } else {
      return BGCTPConf;
    }
  }

  /**
   * Designed for internal use, get a pointer to the TriggerDescisionTool object.
   * @return Pointer to configured D3PD::TrigDecisionToolD3PD.
   */
  D3PD::TrigDecisionToolD3PD* TrigConfInterface::getTDT() {
    if (m_isConfigured == kFALSE) {
      Error("TrigConfInterface::getTDT", "Configuration tool is not configured!");
      return 0;
    }
    return m_tdt;
  }

  /**
   * Designed for internal use, get a pointer to the TriggerDescisionTool object.
   * @return Const pointer to configured D3PD::TrigConfigSvcD3PD.
   */
  const D3PD::TrigConfigSvcD3PD* TrigConfInterface::getTCT() {
    assert(m_isConfigured);
    if (!m_tct) {
      m_tct = dynamic_cast<D3PD::TrigConfigSvcD3PD*>(&(m_tdt->GetConfigSvc()));
    }
    return m_tct;
  }

  D3PD::TrigConfigSvcD3PD* TrigConfInterface::getTCTNonConst() {
    assert(m_isConfigured);
    if (!m_tct) {
      m_tct = dynamic_cast<D3PD::TrigConfigSvcD3PD*>(&(m_tdt->GetConfigSvc()));
    }
    return m_tct;
  }

  /**
   * Export current trigger configuration to basic HTML page.
   */
  void TrigConfInterface::dump() {
    if (m_isConfigured == kFALSE) return;

    const std::string sm = intToString(getCurrentSMK());

    const std::string fileName = std::string(Config::config().getStr(kOutputDirectory)
                                              + "/TriggerConfiguration_"
                                              + getCurrentDBKey().name() + ".json");
    if (Config::config().debug()) Info("TrigConfInterface::dump", "Saving trigger configuration to %s",
                                       fileName.c_str());

    //std::ofstream foutHtml( std::string(fileName + ".htm").c_str() );
    std::ofstream foutJson(fileName.c_str());

    JsonExport* json = new JsonExport();

    //foutHtml << "<!DOCTYPE html><html><head><title>Menu Configuration</title></head><body>" << std::endl;
    //foutHtml << "<h1>Trigger Menu Configuration: " << getCurrentDBKey().name() << "</h1><p>" << std::endl;
    //foutHtml << "<ul>" << std::endl;

    json->addNode(foutJson, "Trigger", "TRG");
    json->addNode(foutJson, "LVL1", "LV1");     // BEGIN
    Bool_t jsonDoingL1 = kTRUE;
    // CHAIN
    for (UInt_t c = 0; c < getTCT()->GetChainN(); ++c) {
      Bool_t isL1 = kFALSE;
      //std::string counter = "</b>, Counter:<i>";
      if (getTCT()->GetChainLevel(c) == 1) {
        //counter = "</b>, CTPID:<i>";
        isL1 = kTRUE;
      } else if (jsonDoingL1 == kTRUE) {
        //switch over to HLT
        json->endNode(foutJson); //END L1
        json->addNode(foutJson, "HLT", "HLT");  // BEGINHLT
        jsonDoingL1 = kFALSE;
      }
      //foutHtml << "<hr>" << std::endl;
      //foutHtml << "<li>Trigger Chain: Name:<b>" << getTCT()->GetChainName(c) << counter <<
      // getTCT()->GetChainCounter(c) << "</i></li>" << std::endl;
      std::string chainName = getTCT()->GetChainName(c);
      json->addNode(foutJson, chainName + " (PS:" + floatToString(getTCT()->GetPrescale(chainName), 2) + ")", "C");
      // foutHtml << "<li>Prescale:<i>" << getPrescale( getTCT()->GetChainName(c) ) << "</i></li>" << std::endl;
      // if (getTCT()->GetChainGroupNameSize(c)) {
      //   foutHtml << "<li>Groups:<i>";
      //   for (UInt_t g = 0; g < getTCT()->GetChainGroupNameSize(c); ++g) {
      //     foutHtml << getTCT()->GetChainGroupName( c, g );
      //     if (g != getTCT()->GetChainGroupNameSize(c) - 1) foutHtml << ",";
      //   }
      //   foutHtml << "</i></li>" << std::endl;
      // }
      // if ( getTCT()->GetChainLevel(c) > 1) {
      //   foutHtml << "<li>Lower Chain Name:" << getTCT()->GetLowerChainName( getTCT()->GetChainName(c) ) << "</li>"
      // << std::endl;
      // }
      // if ( getTCT()->GetChainEBHypoNameSize(c) > 0) {
      //   std::cout << "<li>Enhanced Bias Hypo Items:<i>";
      //   foutHtml << "<li>Enhanced Bias Hypo Items:<i>";
      //   for (UInt_t g = 0; g < getTCT()->GetChainEBHypoNameSize(c); ++g) {
      //     foutHtml << getTCT()->GetChainEBHypoName( c, g );
      //     if (g != getTCT()->GetChainEBHypoNameSize(c) - 1) foutHtml << ",";
      //   }
      //   foutHtml << "</i></li>" << std::endl;
      // }
      //foutHtml << " <ol>" << std::endl;
      // SIGNATURE
      for (UInt_t s = 0; s < getTCT()->GetSigN(c); ++s) {
        UInt_t nOutputTEs = getTCT()->GetSigNOutputTE(c, s);
        if (!isL1) json->addNode(foutJson, getTCT()->GetSigLabel(c, s), "SI"); // SIG STEP
        //foutHtml << " <li>Signature Step: Label:<b>" << getTCT()->GetSigLabel(c, s) << "</b>, Logic:<i>" <<
        // getTCT()->GetSigLogic(c, s) << "</i></li>" << std::endl;
        //foutHtml << "  <ul>" << std::endl;
        // SIG -> SEQ
        for (Int_t t = nOutputTEs - 1; t >= 0; --t) {
          UInt_t outputTE = getTCT()->GetSigOutputTE(c, s, t);
          UInt_t seq = getTCT()->GetSequenceIndex(outputTE);
          if (seq != UINT_MAX) {
            //foutHtml << "  <li> Sequence: Name:<b>" << getTCT()->GetSeqName(seq) << "</b>, Index:<i>" <<
            // getTCT()->GetSeqIndex(seq) << "</i></li>" << std::endl;
            if (isL1) json->addLeaf(foutJson, getTCT()->GetSeqName(seq), "L1"); // L1 BG (does not need closing)
            else json->addNode(foutJson, getTCT()->GetSeqName(seq), "SE"); // SEQ STEP
            //foutHtml << "   <ol>" << std::endl;
            // ALG
            for (UInt_t a = 0; a < getTCT()->GetAlgN(seq); ++a) {
              json->addLeaf(foutJson, getTCT()->GetAlgName(seq, a), "A"); //ALG LEAF (does not need closing)
              //foutHtml << "   <li>Algorithm Instance/Class:<b>" << getTCT()->GetAlgName(seq, a) << " </b>/<b> " <<
              // getTCT()->GetAlgTypeName(seq, a) << "</b></li>" << std::endl;
            }
            //foutHtml << "   </ol>" << std::endl;
            if (!isL1) json->endNode(foutJson); // SEQ STEP
          }
        }
        //foutHtml << "  </ul>" << std::endl;
        if (!isL1) json->endNode(foutJson); // SIG STEP
      }
      //foutHtml << " </ol>" << std::endl;
      json->endNode(foutJson); // CHAIN
    }
    json->endNode(foutJson); //END HLT
    json->endNode(foutJson); //END TRIGGER
    foutJson.close();
    delete json;
    //foutHtml << "</ul>" << std::endl;
    //foutHtml << "</p></body></html>" << std::endl;
    //foutHtml.close();
  }

  /**
   * Reset the configuration interface, removing any TriggerDescisionTool object.
   */
  void TrigConfInterface::reset() {
    Info("TrigConfInterface::reset", "Clearing Memory.");
    delete m_tdt;
    m_tdt = 0;
    m_tct = 0;
    m_isConfigured = false;
  }

  /**
   * Perform diagnostic dumping of configuration information to the terminal
   */
  void TrigConfInterface::debug() {
    Info("TrigConfInterface::debug", " Printing Full Trigger Menu");
    for (UInt_t s = 0; s < getTCT()->GetSeqN(); ++s) {
      Info("TrigConfInterface::debug", "Sequence: ID=%i\tIndex=%i\tNAlgs=%i\tNInputTEs=%i\tName=%s",
           getTCT()->GetSeqID(s),
           getTCT()->GetSeqIndex(s),
           getTCT()->GetAlgN(s),
           getTCT()->GetSeqNInputTEs(s),
           getTCT()->GetSeqName(s).c_str());
      for (UInt_t t = 0; t < getTCT()->GetSeqNInputTEs(s); ++t) {
        Info("TrigConfInterface::debug", "\tSequence Input TE #%i: %u", t, getTCT()->GetSeqInputTE(s, t));
      }
      // Algorithm
      for (UInt_t a = 0; a < getTCT()->GetAlgN(s); ++a) {
        Info("TrigConfInterface::debug",
             "\tAlgorithm: Index=%i\tPosition=%i\tNameHash=%u\tTypeNameHash=%u\tName=%s\tTypeName=%s",
             getTCT()->GetAlgIndex(s, a),
             getTCT()->GetAlgPosition(s, a),
             getTCT()->GetAlgNameID(s, a),
             getTCT()->GetAlgTypeID(s, a),
             getTCT()->GetAlgName(s, a).c_str(),
             getTCT()->GetAlgTypeName(s, a).c_str());
      }
    }
  }

  UInt_t TrigConfInterface::getChainN() {
    return getTCT()->GetChainN();
  }

  UInt_t TrigConfInterface::getChainLevel(UInt_t c) {
    return getTCT()->GetChainLevel(c);
  }

  UInt_t TrigConfInterface::getChainCounter(UInt_t c) {
    return getTCT()->GetChainCounter(c);
  }

  std::string TrigConfInterface::getChainName(UInt_t c) {
    return getTCT()->GetChainName(c);
  }

  UInt_t TrigConfInterface::getChainEBHypoNameSize(UInt_t c) {
    return getTCT()->GetChainEBHypoNameSize(c);
  }

  std::string TrigConfInterface::getChainEBHypoName(UInt_t c, UInt_t h) {
    return getTCT()->GetChainEBHypoName(c, h);
  }

  UInt_t TrigConfInterface::getChainGroupsNameSize(UInt_t c) {
    return getTCT()->GetChainGroupNameSize(c);
  }

  std::string TrigConfInterface::getChainGroupName(UInt_t c, UInt_t g) {
    return getTCT()->GetChainGroupName(c, g);
  }

  UInt_t TrigConfInterface::getChainStreamNameSize(UInt_t c) {
    return getTCT()->GetChainStreamNameSize(c);
  }

  std::string TrigConfInterface::getChainStreamName(UInt_t c, UInt_t g) {
    return getTCT()->GetChainStreamName(c, g);
  }

  const std::vector<std::string>& TrigConfInterface::getChainRatesGroupNames(UInt_t c) { // Now with caching
    static std::map<UInt_t, std::vector<std::string> > groups;
    static std::vector<std::string> emptyVector;

    if (groups.count(c) == 0) { // Populate
      groups[c] = std::vector<std::string>();
      for (UInt_t group = 0; group < getTCT()->GetChainGroupNameSize(c); ++group) {
        std::string groupName = getTCT()->GetChainGroupName(c, group);
        if (groupName.find("Rate:") != std::string::npos || groupName.find("RATE:") != std::string::npos) {
          // Veto CPS groups - these have their own system
          if (groupName.find("CPS") != std::string::npos) continue;
          std::replace(groupName.begin(), groupName.end(), ':', '_'); // A ":" can cause issues in TDirectory naming
                                                                        // structure. "_" is safe.
          groups[c].push_back(groupName);
        }
      }
    }
    return groups[c];
  }

  std::vector<std::string> TrigConfInterface::getChainStreamNames(UInt_t c) {
    std::vector<std::string> streams;
    for (UInt_t stream = 0; stream < getTCT()->GetChainStreamNameSize(c); ++stream) {
      if (getTCT()->GetChainStreamName(c, stream) == "Main") continue; // These are handled on their own
      if (getTCT()->GetChainStreamName(c, stream) == "express") continue;
      std::stringstream streamName;
      streamName << "STREAM_" << getTCT()->GetChainStreamName(c, stream);
      streams.push_back(streamName.str());
    }
    return streams;
  }

  Bool_t TrigConfInterface::getChainIsMainStream(const std::string& name) {
    if (TrigConfInterface::m_mainChains.size() == 0) { // Buffer on first call
      for (UInt_t i = 0; i < TrigConfInterface::getChainN(); ++i) {
        TrigConfInterface::m_mainChains[ TrigConfInterface::getChainName(i) ] = TrigConfInterface::getChainIsMainStream( i );
        if (Config::config().debug()) {
          Info("TrigConfInterface::getChainIsMainStream", "Debug: %s is main? -> %i", TrigConfInterface::getChainName(i).c_str(),
             TrigConfInterface::getChainIsMainStream( i ));
        }
      }
    }
    std::map<std::string, Bool_t>::const_iterator it = TrigConfInterface::m_mainChains.find(name);
    if (it == TrigConfInterface::m_mainChains.end()) {
      Warning("TrigConfInterface::getChainIsMainStream", "Cannot find chain %s.", name.c_str());
      return kFALSE;
    }
    return (*it).second;
  }

  Bool_t TrigConfInterface::getChainIsMainStream(UInt_t c) {
    for (UInt_t s = 0; s < getChainStreamNameSize(c); ++s) {
      if (getChainStreamName(c, s) == "Main") return kTRUE;
    }
    return kFALSE;
  }

  std::string TrigConfInterface::getChainCPSGroup(UInt_t c) {
    std::string cpsGroup = "";
    for (UInt_t group = 0; group < getTCT()->GetChainGroupNameSize(c); ++group) {
      std::string groupName = getTCT()->GetChainGroupName(c, group);
      if (groupName.find("CPS") != std::string::npos) {
        std::replace(groupName.begin(), groupName.end(), ':', '_'); // A ":" can cause issues in TDirectory naming
                                                                      // structure. "_" is safe.
        if (cpsGroup == "") cpsGroup = groupName;
        else Warning("TrigConfInterface::getChainCPSGroup",
                     "Chain %s has more than one CPS group (%s, %s). This is not supported.", getChainName(
                       c).c_str(), cpsGroup.c_str(), groupName.c_str());
      }
    }
    return cpsGroup;
  }
} // namespace TrigCostRootAnalysis

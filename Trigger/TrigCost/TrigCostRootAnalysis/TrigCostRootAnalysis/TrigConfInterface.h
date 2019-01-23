// Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/

// -------------------------------------------------------------
//  author: Tim Martin <Tim.Martin@cern.ch>
// -------------------------------------------------------------
#ifndef TrigCostRootAnalysis_TrigConfInterface_H
#define TrigCostRootAnalysis_TrigConfInterface_H

// STL include(s):
#include <string>
#include <sstream>
#include <set>

// ROOT include(s):
#include <Rtypes.h>

//Local include(s):
#include "Utility.h" //Typedefs

// Forward declaration(s):
class TChain;
namespace D3PD {
  class TrigDecisionToolD3PD;
  class TrigConfigSvcD3PD;
}

namespace TrigCostRootAnalysis {
  /** @class DBKey
   * Small class to hold trigger DB key triplets
   */
  class DBKey {
  public:
    DBKey() : m_SMK(-1), m_L1PSK(-1), m_HLTPSK(-1), m_keyString() {}
    DBKey(Int_t SMK, Int_t L1PSK, Int_t HLTPSK) : m_SMK(-1), m_L1PSK(-1), m_HLTPSK(-1), m_keyString() {
      set(SMK, L1PSK, HLTPSK);
    }

    ~DBKey() = default;
    void set(Int_t SMK, Int_t L1PSK, Int_t HLTPSK) {
      if (m_SMK == SMK && m_L1PSK == L1PSK && m_HLTPSK == HLTPSK) return;

      m_SMK = SMK;
      m_L1PSK = L1PSK;
      m_HLTPSK = HLTPSK;
      std::stringstream ss;
      ss << "SMK_" << SMK << "_L1_" << L1PSK << "_HLT_" << HLTPSK;
      ss >> m_keyString;
    }

    Int_t SMK() const {
      return m_SMK;
    }

    Int_t L1PSK() const {
      return m_L1PSK;
    }

    Int_t HLTPSK() const {
      return m_HLTPSK;
    }

    const std::string& name() const {
      return m_keyString;
    }

    inline bool operator == (const DBKey& A) const {
      return(A.SMK() == SMK() && A.L1PSK() == L1PSK() && A.HLTPSK() == HLTPSK());
    }

    inline bool operator < (const DBKey& A) const {
      if (A.SMK() != SMK()) return A.SMK() < SMK();

      if (A.L1PSK() != L1PSK()) return A.L1PSK() < L1PSK();

      return A.HLTPSK() < HLTPSK();
    }

  private:
    Int_t m_SMK;
    Int_t m_L1PSK;
    Int_t m_HLTPSK;
    std::string m_keyString;
  }; //class DBKey

  /**
   * @class TrigConfInterface
   * Static interface with RootCore package TrigDescisionToolD3PD
   * Used to interpret basic and advanced menu information directly from ntuple.
   * Basic information may come from the vanilla TriggerD3PDMaker, while advanced
   * information is saved by the same tool from TrigMonConf EDM objects.
   */
  class TrigConfInterface {
  public:
    static Bool_t configure(TChain* chain);
    static void newEvent(UInt_t lb);
    static Int_t getCurrentSMK();
    static Int_t getCurrentL1PSK();
    static Int_t getCurrentHLTPSK();
    static const DBKey& getCurrentDBKey();
    static void getEntry(Long64_t entry); //TODO remove me
    static const std::string getHLTNameFromChainID(Int_t chainID, Int_t level = 0); //Note - copy string here (origin
                                                                                      // volatile)
    static UInt_t getNHLTGroupNamesFromChainID(Int_t chainID);
    static const std::string getHLTGroupNameFromChainID(Int_t chainID, UInt_t group); //Note - copy string here
                                                                                        // (origin volatile)
    static const std::string& getLowerChainName(const std::string& name);
    static const std::string& getHLTSeqNameFromIndex(const UInt_t index);
    static const std::string& getHLTAlgNameFromSeqIDAndAlgPos(const UInt_t index, const UInt_t position);
    static UInt_t getHLTAlgNameIDFromSeqIDAndAlgPos(const UInt_t index, const UInt_t position);
    static const std::string& getHLTAlgClassNameFromSeqIDAndAlgPos(const UInt_t index, const UInt_t position);
    static UInt_t getHLTAlgClassNameIDFromSeqIDAndAlgPos(const UInt_t index, const UInt_t position);
    static Int_t getCtpId(const std::string& name);
    static const std::string& getNameFromCtpId(Int_t ctpId);
    static Float_t getPrescale(std::string chainName);
    static Float_t getPassthrough(const std::string& name);
    static UInt_t getMetaStringN();
    static std::string getMetaStringKey(UInt_t m);
    static std::string getMetaStringVal(UInt_t m);
    static std::string getMetaStringVal(std::string key);
    static StringIntMap_t getBunchGroupSetup();
    static void populateLBPerKeysetStrings();


    static Bool_t getUsingNtupleMetadata() {
      return m_usingNtupleMetadata;
    }

    static Bool_t getUsingNtuplePrescales() {
      return m_usingNtuplePrescales;
    }

    static Bool_t isConfigured() {
      return m_isConfigured;
    }

    static void debug();
    static void dump();
    static void reset();

    //Direct calls
    // Chain
    static UInt_t getChainN();
    static UInt_t getChainLevel(UInt_t c);
    static UInt_t getChainCounter(UInt_t c);
    static std::string getChainName(UInt_t c);
    static UInt_t getChainEBHypoNameSize(UInt_t c);
    static std::string getChainEBHypoName(UInt_t c, UInt_t h);
    static UInt_t getChainGroupsNameSize(UInt_t c);
    static std::string getChainGroupName(UInt_t c, UInt_t g);
    static const std::vector<std::string>& getChainRatesGroupNames(UInt_t c);
    static std::string getChainCPSGroup(UInt_t c);
    static UInt_t getChainStreamNameSize(UInt_t c);
    static std::string getChainStreamName(UInt_t c, UInt_t g);
    static std::vector<std::string> getChainStreamNames(UInt_t c);
    static Bool_t getChainIsMainStream(UInt_t c);
    static Bool_t getChainIsMainStream(const std::string& name);

    // // Chain->Sig //todo gruntwork
    // UInt_t      GetSigN(UInt_t c) const;
    // UInt_t      GetSigCounter(UInt_t c, UInt_t s) const;
    // UInt_t      GetSigLogic(UInt_t c, UInt_t s) const;
    // std::string GetSigLabel(UInt_t c, UInt_t s) const;
    // UInt_t      GetSigNOutputTE(UInt_t c, UInt_t s) const;
    // UInt_t      GetSigOutputTE(UInt_t c, UInt_t s, UInt_t t) const;

    // // Seq
    // UInt_t       GetSeqN() const;
    // UInt_t       GetSeqID(UInt_t s) const;
    // UInt_t       GetSeqIndex(UInt_t s) const;
    // std::string  GetSeqName(UInt_t s) const;
    // UInt_t       GetSeqNInputTEs(UInt_t s) const;
    // UInt_t       GetSeqInputTE(UInt_t s, UInt_t t) const;
    // // Seq->Alg
    // UInt_t      GetAlgN(UInt_t s) const;
    // UInt_t      GetAlgIndex(UInt_t s, UInt_t a) const;
    // UInt_t      GetAlgPosition(UInt_t s, UInt_t a) const;
    // UInt_t      GetAlgNameID(UInt_t s, UInt_t a) const;
    // std::string GetAlgName(UInt_t s, UInt_t a) const;
    // UInt_t      GetAlgTypeID(UInt_t s, UInt_t a) const;
    // std::string GetAlgTypeName(UInt_t s, UInt_t a) const;
  private:
    // Not to be instanced, static use only
    TrigConfInterface();
    ~TrigConfInterface();

    static D3PD::TrigDecisionToolD3PD* getTDT();
    static const D3PD::TrigConfigSvcD3PD* getTCT();
    static D3PD::TrigConfigSvcD3PD* getTCTNonConst();


    static Bool_t m_usingNtupleMetadata; //!< Flag if configuration metadata is coming from the ntuple.
    static Bool_t m_usingNtuplePrescales; //!< Flag if prescale metadata is coming from the ntuple.
    static Bool_t m_isConfigured; //!< Flag if the tool has been successfully configured.
    static D3PD::TrigDecisionToolD3PD* m_tdt; //!< Pointer to the D3PD::TriggerDescisionTool object.
    static D3PD::TrigConfigSvcD3PD* m_tct; //!< Pointer to the D3PD::TrigConfigSvcD3PD object, held inside the TDT
                                           // object.

    static DBKey m_key; //!< HLT keyset of current event

    static std::set<DBKey> m_seenKeys; //!< Set of keys which we have seen in the processing
    static std::map<UInt_t, DBKey> m_lumiToKeyMap; //!< Which LB are using which keys

    static std::map<std::string, Bool_t> m_mainChains; //!< Cache of which chains are in the Main stream (by name)
  }; //class TrigConfInterface
} // namespace TrigCostRootAnalysis

#endif //TrigCostRootAnalysis_TrigConfInterface_H

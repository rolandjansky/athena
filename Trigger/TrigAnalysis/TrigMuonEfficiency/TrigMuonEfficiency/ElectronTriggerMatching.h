/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef __ElectronTriggerMatching_h__
#define __ElectronTriggerMatching_h__
/**
 * @author  Toyonobu Okuyama
 * @author  Takashi Matsushita
 */


#include <string>
#include <map>
#include <set>
#include "TTree.h"
#include "TLorentzVector.h"

#include "TrigMuonEfficiency/LeptonTriggerMatching.h"

/**
 *  This class implements method for electron trigger matching with navigation information in ntuple analysis.
 *
 *  Validated on NTUP_SMWZ. Should work on any ntuples which contains trigger navigation information
 */
class ElectronTriggerMatching : public LeptonTriggerMatching {
  public:
    struct DielectronChainInfo {
      public:
        std::string chain;
        std::pair<std::string, std::string> thresholds;
        std::string tightness;
        bool isSymmetric;
        bool isValid;

        DielectronChainInfo(const std::string& chain_="") :
          chain(chain_), thresholds(), tightness(), isSymmetric(false), isValid(false) { }

        
        void dump() const {
          std::cout << "DielectronChainInfo: chain = " << chain
                << " thresholds.first = " << thresholds.first << " thresholds.second = " << thresholds.second
                << " isSymmetric = " << isSymmetric << " isValid = " << isValid << "\n";
        }
    };

    /** default constructor
     *
     * @param d3pd [in] class object used for the given ntuple analysis
     * @param TrigConfTree [in] pointer of physicsMeta/TrigConfTree block [optional]
     */
    ElectronTriggerMatching(const MTT__D3PDEvent *d3pd,
                            TTree *TrigConfTree=0)
      : LeptonTriggerMatching(d3pd, TrigConfTree),
        m_DielectronChainMap()
    {
      if (TrigConfTree) {
        std::vector<std::string> skip;
        skip.push_back("EF_e");
        skip.push_back("EF_2e");
        readTrigConfTree(TrigConfTree, skip);

      } else {
        createChainIdMapFromFile();
      }
    }
    virtual ~ElectronTriggerMatching(){}



    /** look for matching EF electron trigger object
     *
     * @param eta [in] eta of an offline electron
     * @param phi [in] phi of an offline electron
     * @param chain [in] single electron EF chain name. ex) "EF_mu18"
     * @return true if matching EF object was found false otherwise.
     */
    bool match(const double eta,
               const double phi,
               const std::string& chain);

    /** look for matching EF di-electron trigger object
     *
     * @param electron1 [in] TLorentzVector of electron
     * @param electron2 [in] TLorentzVector of electron
     * @param chain [in] di-electron EF chain name. ex) "EF_2e12"
     * @param result1 [out] bool values indicating if electron1 passed the di-muon trigger.
     * @param result2 [out] bool values indicating if electron2 passed the di-muon trigger.
     * @return true if matching EF object was found false otherwise.
     */
    bool matchDielectron(const TLorentzVector& electron1,
                         const TLorentzVector& electron2,
                         const std::string& chain,
                         bool& result1,
                         bool& result2);

    bool matchDielectron(const TLorentzVector& electron1,
                         const TLorentzVector& electron2,
                         const std::string& chain,
                         std::pair<bool, bool>& result1,
                         std::pair<bool, bool>& result2);

    int matchDielectron_python(const TLorentzVector& electron1,
                               const TLorentzVector& electron2,
                               const std::string& chain)
    {
      std::pair<bool, bool> result1, result2;
      matchDielectron(electron1, electron2, chain, result1, result2);
      return (result1.second << 3 | result2.first << 2 | result2.second << 1 | result1.first << 0);
    }



    /** look for matching e/mu combined trigger object
     *
     * @param electron [in] TLorentzVector of electron
     * @param muon [in] TLorentzVector of muon
     * @param chain [in] electron-muon EF chain name. ex) "EF_e12TVh_medium1_mu8"
     * @return true if matching EF object was found false otherwise.
     */
    bool matchElectronMuon(const TLorentzVector& electron,
                           const TLorentzVector& muon,
                           const std::string& chain);



  private:
    std::map<std::string, DielectronChainInfo> m_DielectronChainMap; // [chain - <threshold1, threshold2>]

    double matchedTrackDetail(EFelectron& efElectronId,
                              const EFelectron& usedEfElectronId,
                              const double eta,
                              const double phi,
                              const std::string& chainEventTrigger,
                              const std::string& threshold = std::string());


    std::pair<bool, bool> matchDielectron(const TLorentzVector& electron1,
                                          const TLorentzVector& electron2,
                                          const DielectronChainInfo& chainInfo);

    bool decodeDielectronChain(DielectronChainInfo& chainInfo);

    bool decodeElectronMuonChain(const std::string& chain,
                                 std::string& threshold);

    void createChainIdMapFromFile(); //implementation in ElectronChainIdMap.h
};

#endif //__ElectronTriggerMatching_h__
// eof

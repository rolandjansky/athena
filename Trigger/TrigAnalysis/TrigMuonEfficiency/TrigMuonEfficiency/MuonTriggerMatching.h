/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef __MuonTriggerMatching_h__
#define __MuonTriggerMatching_h__
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
 *  This class implements method for muon trigger matching with navigation information in ntuple analysis.
 *
 *  Validated on NTUP_SMWZ. Should work on any ntuples which contains TrigMuonD3PDMaker block
 */
class MuonTriggerMatching : public LeptonTriggerMatching {
  public:
    /** default constructor
     *
     * @param d3pd [in] class object used for the given ntuple analysis
     * @param TrigConfTree [in] pointer of physicsMeta/TrigConfTree block [optional]
     */
    MuonTriggerMatching(const MTT__D3PDEvent *d3pd,
                        TTree *TrigConfTree=0)
      : LeptonTriggerMatching(d3pd, TrigConfTree),
        m_index(false),
        m_DimuonChainMap()
    {
      if (TrigConfTree) {
        std::vector<std::string> skip;
        skip.push_back("EF_mu");
        skip.push_back("EF_2mu");
        skip.push_back("L2_mu15");
        skip.push_back("L2_mu18");
        skip.push_back("L2_mu20");
        skip.push_back("L2_mu24");
        readTrigConfTree(TrigConfTree, skip);

      } else {
        createChainIdMapFromFile();
      }
    }
    virtual ~MuonTriggerMatching(){}


    /** look for matching EF muon trigger object
     *
     * @param eta [in] eta of an offline muon
     * @param phi [in] phi of an offline muon
     * @param chain [in] single muon EF chain name. ex) "EF_mu18"
     * @param l1item [in] optional: minimum level1 threshold required for seed of the matching EF chain ex) "L1_MU11"
     * @return true if matching EF object was found false otherwise.
     */
    bool match(const double eta,
               const double phi,
               const std::string& chain,
               const std::string& l1item="");


    /** look for matching EF muon trigger object
     *
     * @param eta [in] eta of an offline muon
     * @param phi [in] phi of an offline muon
     * @param chain [in] single muon EF chain name. ex) "EF_mu18"
     * @param EFmuon [out] to be used for accessing the matched EF trigger object
     * @param l1item [in] optional: minimum level1 threshold required for seed of the matching EF chain ex) "L1_MU11"
     * @return pair of int to access trigger object
     * 
    */
    std::pair<int, int>
    matchedTrack(const double eta,
                 const double phi,
                 const std::string& chain,
                 const std::string& l1item="");


    /** look for matching EF di-muon trigger object
     *
     * @param muon1 [in] TLorentzVector of muon
     * @param muon2 [in] TLorentzVector of muon
     * @param chain [in] di-muon EF chain name. ex) "EF_2mu10"
     * @param result1 [out] bool values indicating if muon1 passed the first/second thresholds of the di-muon trigger.
     * @param result2 [out] bool values indicating if muon2 passed the first/second thresholds of the di-muon trigger.
     * @return true if matching EF object was found false otherwise.
     */
    bool matchDimuon(const TLorentzVector& muon1,
                     const TLorentzVector& muon2,
                     const std::string& chain,
                     std::pair<bool, bool>& result1,
                     std::pair<bool, bool>& result2);


    int matchDimuon_python(const TLorentzVector& muon1,
                           const TLorentzVector& muon2,
                           const std::string& chain)
    {
      std::pair<bool, bool> result1, result2;
      matchDimuon(muon1, muon2, chain, result1, result2);
      return (result1.second << 3 | result2.first << 2 | result2.second << 1 | result1.first << 0);
    }



    // for developers
    void useIndex(bool x) {m_index = x;}


    /**
     *  stores information needed for di-muon trigger matching
     */
    struct DimuonChainInfo {
      public:
        std::string chain;
        std::string tightness;
        std::pair<std::string, std::string> thresholds;
        std::string l2seed;
        std::string efseed;
        bool is2011;
        bool isEFFS;
        bool isSymmetric;
        bool isValid;

        DimuonChainInfo(const std::string& chain_="") :
          chain(chain_), tightness(""), thresholds(), l2seed(""), efseed(""),
          is2011(false), isEFFS(false), isSymmetric(false), isValid(false) {}

        void dump() {
          std::cout << "DimuonChainInfo: chain = " << chain << " tightness = " << tightness
                << " thresholds.first = " << thresholds.first << " thresholds.second = " << thresholds.second
                << " l2seed = " << l2seed << " efseed = " << efseed << " is2011 = " << is2011
                << " isEFFS = " << isEFFS << " isSymmetric = " << isSymmetric << " isValid = " << isValid << "\n";
        }
    };

  private:
    bool m_index;
    std::map<std::string, DimuonChainInfo> m_DimuonChainMap; // [chain - <threshold1, threshold2>]
  
    double matchedTrackDetail_index(EFmuon& efMuonId,
                                    const EFmuon& usedEfMuonId,
                                    const double eta,
                                    const double phi,
                                    const std::string& chainForThisMuon,
                                    const std::string& l1item = std::string(),
                                    const std::string& chainEventTrigger = std::string());

    double matchedTrackDetail(EFmuon& efMuonId,
                              const EFmuon& usedEfMuonId,
                              const double eta,
                              const double phi,
                              const std::string& chainForThisMuon,
                              const std::string& l1item = std::string(),
                              const std::string& chainEventTrigger = std::string());

    std::pair<bool, bool> matchDimuon(const TLorentzVector& muon1,
                                      const TLorentzVector& muon2,
                                      const DimuonChainInfo& chain);

    void createChainIdMapFromFile(); //implementation in MuonChainIdMap.h

    bool decodeDimuonChain(DimuonChainInfo& chainInfo);

    bool checkSeedL2(const double eta,
                     const double phi,
                     const std::string& chain,
                     double l2dr=0.15,
                     double l1dr=0.20);

    bool checkSeedEF(const double eta,
                     const double phi,
                     const std::string& chain);

    bool checkSeed_index(const double eta,
                         const double phi,
                         const std::string& chain);
};


#endif //__MuonTriggerMatching_h__
// eof

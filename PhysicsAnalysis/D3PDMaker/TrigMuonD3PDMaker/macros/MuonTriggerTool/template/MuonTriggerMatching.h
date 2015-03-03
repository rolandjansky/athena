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


/**
 *  This class implements method for muon trigger matching with navigation information at ntuple analysis.
 *
 *  Validated on NTUP_SMWZ. Should work on any ntuples which contains TrigMuonD3PDMaker block
 */
class MuonTriggerMatching {
  public:
    enum chain_type {
      CB = 0, // TrigMuonEF combined chain
      SA = 1, // TrigMuonEF standalone chain
      MG = 2  // TrigMuGirl
    };


    /** default constructor
     *
     * @param d3pd [in] class object used for the given ntuple analysis
     * @param TrigConfTree [in] pointer of physicsMeta/TrigConfTree block [optional]
     */
    MuonTriggerMatching(const D3PDEvent *d3pd,
                        TTree *TrigConfTree=0)
      : m_d3pd(d3pd),
        m_matchingDr(0.1),
        m_MapOfChainIdMap(),
        m_NotFoundChain() {
      readTrigConfTree(TrigConfTree);
    }
    virtual ~MuonTriggerMatching(){}


    /** returns deltaR for offline muon and trigger object matching
     *
     * @return deltaR
     */
    double getDeltaR() const {return m_matchingDr;}


    /** set deltaR for offline muon and trigger object matching
     *
     * @param dr [in] matching distance for offline muon and trigger object matching
     */
    void setDeltaR(const double dr) {m_matchingDr = dr;}


    /** look for matching EF muon trigger object
     *
     * @param eta [in] eta of an offline muon
     * @param phi [in] phi of an offline muon
     * @param chain [in] EF chain name. ex) "EF_mu18"
     * @param L1Item [in] optional: minimum level1 threshold required for seed of the matching EF chain ex) "L1_MU11"
     * @return true if matching EF object was found false otherwise.
     */
    bool match(const double eta,
               const double phi,
               const std::string& chain,
               const std::string& L1Item="");


    /** look for matching EF muon trigger object
     *
     * @param eta [in] eta of an offline muon
     * @param phi [in] phi of an offline muon
     * @param chain [in] EF chain name. ex) "EF_mu18"
     * @param L1Item [in] optional: minimum level1 threshold required for seed of the matching EF chain ex) "L1_MU11"
     * @return pair of int which can be used to access the matched EF trigger object
     */
    std::pair<int, int>
    matchedTrack(const double eta,
                 const double phi,
                 const std::string& chain,
                 const std::string& L1Item="");

    void showSMKeys();// For Debug
    void dumpChainIdMap(const int SMK);// For Debug
    void createChainIdMapFile(const int SMK);


  private:
    const D3PDEvent* m_d3pd; 
    double m_matchingDr;
    std::map<int, std::map<std::string, int> > m_MapOfChainIdMap;// [SMK - [ChainName - ChainId] ]
    std::set<std::string> m_NotFoundChain;
  
    int getChainIndex(const std::string& chain);
    void readTrigConfTree(TTree* TrigConfTree);
    bool L1_isPassed(const int iRoI,
                     const std::string& L1Item);
    int getL1pt(const std::string& l1item);
    double dR(double eta1,
              double phi1,
              double eta2,
              double phi2);
    void createChainIdMapFromFile(); // implementation in ChainIdMap.hxx
    void tokenize(const std::string& str,
                  std::vector<std::string>& tokens,
                  const std::string& delimiters);
};


// defined in Trigger/TrigAnalaysis/TriggerMenuNtuple/ChainEntry.h
namespace ChainEntry {
  enum RoIType {
    kUnknown = 0,
    kMuonRoIType = 1,
    kElectronRoIType = 2,
    kTauRoIType = 3,
    kJetRoIType = 4,
    kBjetRoIType = 5,
    kMETRoIType = 6,
    kBphysicsRoIType = 7,

    kRoIType_L2_mu = 1001,
    kRoIType_L2_e = 1002,
    kRoIType_L2_tau = 1003,
    kRoIType_L2_j = 1004,
    kRoIType_L2_b = 1005,
    kRoIType_L2_xe = 1006,
    kRoIType_L2_DiMu = 1007,
    kRoIType_L2_TileMu = 1010,

    kRoIType_EF_mu = 2001,
    kRoIType_EF_e = 2002,
    kRoIType_EF_tau = 2003,
    kRoIType_EF_j = 2004,
    kRoIType_EF_b = 2005,
    kRoIType_EF_xe = 2006,
    kRoIType_EF_DiMu = 2007,
    kRoIType_EF_TileMu = 2010
  };
  const int kEFChainIdOffset = 10000;
}
#endif//__MuonTriggerMatching_h__


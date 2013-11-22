/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef __LeptonTriggerMatching_h__
#define __LeptonTriggerMatching_h__
/**
 * @author  Takashi Matsushita
 */


#include <string>
#include <map>
#include <set>
#include "TTree.h"
#include "TLorentzVector.h"

#define MUONEF_CB(x) m_d3pd->trig_EF_trigmuonef_track_CB_##x
#define MUONEF_SA(x) m_d3pd->trig_EF_trigmuonef_track_SA_##x
#define MUGIRL_CB(x) m_d3pd->trig_EF_trigmugirl_track_CB_##x

#include "TrigMuonEfficiency/TriggerNavigationVariables.h"
typedef TriggerNavigationVariables MTT__D3PDEvent;

/**
 *  This class implements method for lepton trigger matching with navigation information in ntuple analysis.
 *
 *  Validated on NTUP_SMWZ. Should work on any ntuples which contains TrigMuonD3PDMaker block
 */
class LeptonTriggerMatching {
  public:
    enum MuonChainType {
      CHAIN_CB = 0, // TrigMuonEF combined chain
      CHAIN_SA = 1, // TrigMuonEF standalone chain
      CHAIN_MG = 2  // TrigMuGirl
    };

    struct EFmuon {
      bool valid;
      bool isIso;
      int iso_index;
      int roi_index;
      unsigned int chain_type;
      std::pair<int, int> indicies;
      float charge;
      float pt;
      float eta;
      float phi;

      EFmuon() : valid(false), isIso(false), iso_index(-1), roi_index(-1), chain_type(CHAIN_CB), indicies(),
                 charge(0.), pt(-1.e30), eta(-1.e30), phi(-1.e30) {}
    };

    struct EFelectron {
      bool valid;
      bool useThreshold;
      float charge;
      int roi_index;
      float eta;
      float phi;
      std::string threshold;

      EFelectron() : valid(false), useThreshold(false), charge(0.), roi_index(-1), eta(-1.e30), phi(-1.e30), threshold() {}
    };



    /** default constructor
     *
     * @param d3pd [in] class object used for the given ntuple analysis
     * @param TrigConfTree [in] pointer of physicsMeta/TrigConfTree block [optional]
     */
    LeptonTriggerMatching(const MTT__D3PDEvent *d3pd,
                          TTree* /*TrigConfTree*/ = 0)
      : m_d3pd(d3pd),
        m_matchingDr(0.1),
        m_debug(false),
        m_MapOfChainIdMap(),
        m_NotFoundChain(),
        m_isIndexMatchUsed(false),
        m_isNavigationMatchUsed(false)
    {}
    virtual ~LeptonTriggerMatching(){}


    /** returns minimum distance required for offline lepton and trigger object matching
     *
     * @return m_matchingDr
     */
    double getDeltaR() const {return m_matchingDr;}


    /** set minimum distance required for offline lepton and trigger object matching
     *
     * @param dr [in] matching distance for offline lepton and trigger object matching
     */
    void setDeltaR(const double dr) {m_matchingDr = dr;}


    void setTriggerNavigationVariables(const MTT__D3PDEvent *d3pd) {m_d3pd = d3pd;}

    // for developers
    void setDebug(bool x) {m_debug = x;}
    void showSMKeys();
    void dumpChainIdMap(const int SMK);
    void createChainIdMapFile(const int SMK,
                              const std::string& fileName);



  protected:
    const MTT__D3PDEvent* m_d3pd; 
    double m_matchingDr;
    bool m_debug;
    std::map<int, std::map<std::string, int> > m_MapOfChainIdMap; // [SMK - [ChainName - ChainId] ]
    std::set<std::string> m_NotFoundChain;
    bool m_isIndexMatchUsed;
    bool m_isNavigationMatchUsed;
  
    
    double matchedTrackDetail(EFelectron& efElectronId,
                              const EFelectron& usedEfElectronId,
                              const double eta,
                              const double phi,
                              const std::vector<int>& container,
                              const std::vector<int>& status);

    double matchedTrackDetail(EFmuon& efMuonId,
                              const EFmuon& usedEfMuonId,
                              const std::string& threshold,
                              const double eta,
                              const double phi,
                              const std::vector<int>& container,
                              const std::vector<int>& status);

    double index_match(const double eta,
                       const double phi,
                       const std::string& chain,
                       const std::string& threshold,
                       EFmuon& efMuonId,
                       const EFmuon& usedEfMuonId);

    bool L1_isPassed(const int iRoI,
                     const std::string& l1item);

    int getL1pt(const std::string& l1item);

    int getChainIndex(const std::string& chain);

    void readTrigConfTree(TTree* TrigConfTree,
                          const std::vector<std::string>& skip);

    double dR(const double eta1,
              const double phi1,
              const double eta2,
              const double phi2);

    void tokenize(const std::string& str,
                  std::vector<std::string>& tokens,
                  const std::string& delimiters);

    bool isEqual(const double x,
                 const double y);
};
#endif //__LeptonTriggerMatching_h__
// eof

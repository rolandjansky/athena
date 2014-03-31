/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TrigHLTMonitoring_HLTJetMonTooL_H
#define TrigHLTMonitoring_HLTJetMonTooL_H

/*
 * @brief   HLTJetMonTool is the monitoring tool for HLT Jets
 *          Does basic HLT jet monitoring plots and also matches
 *          offline jets with L1, L2 and EF jets and makes histograms
 *          for matching objects. Also determines trigger efficiency
 *          w.r.t offline jets (currently only [L1,L2,EF]_JX chains)
 * @author  Venkat Kaushik, Xiaowen Lei, Ken Johns (University of Arizona)
 * @date    March 2008
 */

#include "TrigHLTMonitoring/IHLTMonTool.h"
#include "GaudiKernel/ToolHandle.h"
#include "AnalysisTriggerEvent/LVL1_ROI.h"

// STL includes
#include <string>
#include <vector>
#include <map>

// forward declarations
class TrigMatchTool;

class TrigT2JetContainer;
class Jet;
class JetCollection;

class TH1;

//namespace Trig {
//  class ChainGroup;
//}

class HLTJetMonTool : public IHLTMonTool {

  typedef std::vector<std::string>::const_iterator VectIter;

  typedef std::map<std::string, std::string> JetSigtype;
  typedef std::map<std::string, std::string>::const_iterator JetSigIter;
  
  typedef std::map<std::string, double> JetThrestype;
  typedef std::map<std::string, double>::const_iterator JetThresIter;


  public:

    // C'tor, D'tor
    HLTJetMonTool(const std::string & type, const std::string & name, const IInterface* parent);
    ~HLTJetMonTool();

    // from HLTMonTool
    // LS 24 Jan 2014 - Modified for new AthenaMonitoring base classes, managed monitor histograms
    StatusCode init();
    StatusCode book();
    StatusCode fill();
    StatusCode proc();

  private:


    // data members

    // binning for basic histograms
    std::vector<int> m_njnbins, m_jEtnbins, m_jetanbins, m_jphinbins, m_jDEtnbins, m_jDepnbins;
    std::vector<float> m_njperbin, m_jEtperbin, m_jetaperbin, m_jphiperbin, m_jDEtperbin, m_jDepperbin;
    std::vector<float> m_njbinlo, m_jEtbinlo, m_jetabinlo, m_jphibinlo, m_jDEtbinlo, m_jDepbinlo;
    std::vector<float> m_njbinhi, m_jEtbinhi, m_jetabinhi, m_jphibinhi, m_jDEtbinhi, m_jDepbinhi;

    // binning for trigger efficiency 
    std::vector<float> /*m_binlo, m_binhi, m_nperbin, */
                       m_l1binloEt, m_l1binhiEt, m_l1nperbinEt,    // for L1 trigger eff vs. Et
                       m_l2binloEt, m_l2binhiEt, m_l2nperbinEt,    // for L2 trigger eff vs. Et
                       m_efbinloEt, m_efbinhiEt, m_efnperbinEt;    // for EF trigger eff vs. Et

    JetThrestype m_l1EtThres, m_l2EtThres, m_efEtThres, m_ofEtThres;

    std::vector<int> /*m_nbins, */
      m_l1nbinsEt, // for L1 trigger eff vs. Et
      m_l2nbinsEt, // for L2 trigger eff vs. Et
      m_efnbinsEt; // for EF trigger eff vs. Et


    // keep track of monitoring groups
    JetSigtype m_monGroups;

    std::string m_monBase, m_L2dir, m_EFdir, m_OFpfx, m_Effdir;
    std::string m_L1JetKey, m_L2JetKey, m_EFJetKey;
    
    JetSigtype m_L1Items, m_L2Chains, m_EFChains;
    JetSigtype m_basicL2Trig, m_basicEFTrig;

    std::vector<std::string> /*m_OFJetKeys,*/ m_sampSelTriggers, m_chainsByRegexp;
    JetSigtype m_OFJetKeys;

    // jet selection, matching
    bool m_doL1TrigEff, m_doL2TrigEff, m_doEFTrigEff, m_doEvtSel, m_debuglevel;
    
    bool m_doselOFJets, m_doselOFBasicHists, m_reqMinPtCut, m_reqEtaCut, m_reqMaxNJetCut;
    bool m_reqP4State, /*m_reqEMFracCut, m_reqN90Cut, m_reqTimeCut,*/ m_reqBadQCut;

    float m_deltaRCut, m_emfracCut;
    //std::vector<float> m_ofemfracCut;

    float m_MinPtCut, m_EtaCut /*m_jetTimens, m_badQFrac*/ ;
    int m_MaxNJet /*, m_n90Cut*/ ;

    std::string m_p4State;

    ToolHandle<TrigMatchTool> m_trigMatchTool;
    //const Trig::ChainGroup *m_trjetL1Items, *m_trjetL2Chain, *m_trjetEFChain;

    // containers
    const LVL1_ROI *m_L1RoiC;
    const TrigT2JetContainer *m_L2JetC;
    const JetCollection *m_EFJetC;
    std::vector<const JetCollection *> m_OFJetC;

    // fill methods
    StatusCode fillJetHists();        // this method calls all other fill methods
    StatusCode fillBasicHists();      // basic hists
    StatusCode fillOfflineHists();    // offline jet + trigger efficiency hists

    // helpers for fill method
    void fillBasicL1forChain(const std::string& theChain, LVL1_ROI::jets_type& L1JetROI, double L1thr );
    void fillBasicL2forChain(const std::string& theChain, double L2thr );
    void fillBasicEFforChain(const std::string& theChain, double EFthr );

    // SG retrieval method
    StatusCode retrieveContainers();

    // book methods
    void bookJetHists();  // this method calls all other book methods
    void bookOfflineHists(JetSigtype& items, const std::string& ofjet);     // trigger efficiency hists
    //void bookCorrHists(const std::string& level2, const std::string& level1, const size_t& k ); // correlation hists
    void bookCorrHists(const std::string& level2, const std::string& level1,  const std::string& ofjAlg);
    void bookBasicHists(std::vector<std::string>& level, std::vector<std::string>& hists);     // basic hists

    void setHistProperties(TH1* h);
    int basicKineVar(const std::string& hist, std::vector<std::string>& m_kinevars);
    void clearVectors();
    
    // offline jet cuts
    bool selectJet(const Jet *jet);

    typedef enum {LooseBad, MediumBad, TightBad} JetCategorytype;
    bool isBadJet(JetCategorytype, double, double, double, double, double, double, double, double, double, double);
    JetCategorytype m_JetCategory;
    std::string m_JetQuality;

    // trigger object matching helpers  
    class ChainMatch {
      public: 
        // c'tor
        ChainMatch(const std::string& name, 
            const double& eta, const double& phi, 
            const double& pt, const double& et, bool match ) { 
          m_cname = name; 
          m_eta = eta; 
          m_phi = phi; 
          m_et = et;
          m_pt = pt; 
          m_chpass = match; 
        }
        // c'tor
        ChainMatch() { 
          m_cname =""; m_ctrigname = ""; m_eta = -9e9; m_phi = -9e9; 
          m_pt = -9e9; m_et = -9e9; m_chpass = false; 
        }
        // d'tor
        ~ChainMatch() {;}
        
        // data
        std::string m_cname, m_ctrigname;
        double m_eta, m_phi, m_pt, m_et;
        bool m_chpass;
    };
    bool passedChain( const Jet *jet, std::vector<ChainMatch>& mFound, const std::string& level);
    bool evtSelTriggersPassed();
    bool isChainActive(const std::string& theChain );
    double signed_delta_phi(double ph11, double phi2);
    double delta_r(double eta1, double phi1, double eta2, double phi2);
  
};

#endif 

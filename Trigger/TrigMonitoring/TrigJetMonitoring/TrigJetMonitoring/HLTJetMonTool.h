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
 *          Original Run 1 version by Venkat Kaushik, Xiaowen Lei, Ken Johns (University of Arizona)
 *          Substantially re-written for Run 2.
 * @author  Lee Sawyer (Louisiana Tech University)
 * @date    July 2014
 */

#include "TrigHLTMonitoring/IHLTMonTool.h"
#include "GaudiKernel/ToolHandle.h"
#include "AnalysisTriggerEvent/LVL1_ROI.h"
#include "AnalysisTriggerEvent/Jet_ROI.h"

#include "xAODTrigger/JetRoI.h"
#include "xAODTrigger/JetRoIContainer.h"

#include "xAODEventInfo/EventInfo.h"

#include "xAODJet/Jet.h"
#include "xAODJet/JetContainer.h"
#include "xAODJet/JetConstituentVector.h"

// STL includes
#include <string>
#include <vector>
#include <map>

// forward declarations
class TrigMatchTool;
//class TrigT2JetContainer;
//class Jet;
//class JetCollection;

class TH1;

//namespace Trig {
//  class ChainGroup;
//}

class HLTJetMonTool : public IHLTMonTool {

  typedef std::vector<std::string>::const_iterator VectIter;

  typedef std::map<std::string, std::string> JetSigtype;
  typedef std::map<std::string, std::string>::const_iterator JetSigIter;

  typedef std::map<std::string, std::string> JetContainertype;
  typedef std::map<std::string, std::string>::const_iterator JetContainerIter;
  
  typedef std::map<std::string, double> JetThrestype;
  typedef std::map<std::string, double>::const_iterator JetThresIter;

  typedef std::map<std::string, double> JetEtaHighThrestype;
  typedef std::map<std::string, double>::const_iterator JetEtaHighThresIter;

  typedef std::map<std::string, double> JetEtaLowThrestype;
  typedef std::map<std::string, double>::const_iterator JetEtaLowThresIter;

  typedef std::map<std::string, unsigned int> JetNtype;
  typedef std::map<std::string, unsigned int>::const_iterator JetNIter;

  public:

    // C'tor, D'tor
    HLTJetMonTool(const std::string & type, const std::string & name, const IInterface* parent);
    ~HLTJetMonTool();

    // from HLTMonTool
    StatusCode init();
    StatusCode book();
    StatusCode fill();
    StatusCode proc();

  private:


    // data members

    // binning for basic histograms
    std::vector<int>   m_njnbins,  m_jEtnbins,  m_jetanbins,  m_jphinbins,  m_jemfracnbins,  m_jhecfracnbins,  m_jDEtnbins,  m_jDepnbins,  m_jJVTnbins,  m_jSumPtTrk500nbins;
    std::vector<float> m_njperbin, m_jEtperbin, m_jetaperbin, m_jphiperbin, m_jemfracperbin, m_jhecfracperbin, m_jDEtperbin, m_jDepperbin, m_jJVTperbin, m_jSumPtTrk500perbin;
    std::vector<float> m_njbinlo,  m_jEtbinlo,  m_jetabinlo,  m_jphibinlo,  m_jemfracbinlo,  m_jhecfracbinlo,  m_jDEtbinlo,  m_jDepbinlo,  m_jJVTbinlo,  m_jSumPtTrk500binlo;
    std::vector<float> m_njbinhi,  m_jEtbinhi,  m_jetabinhi,  m_jphibinhi,  m_jemfracbinhi,  m_jhecfracbinhi,  m_jDEtbinhi,  m_jDepbinhi,  m_jJVTbinhi,  m_jSumPtTrk500binhi;

    // binning for trigger efficiency 
    std::vector<float> m_l1binloEt, m_l1binhiEt, m_l1nperbinEt,       // for L1 trigger eff vs. Et
                       m_hltbinloEt, m_hltbinhiEt, m_hltnperbinEt;    // for HLT trigger eff vs. Et

    JetThrestype m_l1EtThres, m_hltEtThres, m_ofEtThres;
    JetEtaHighThrestype m_hltEtaHighThres;
    JetEtaLowThrestype  m_hltEtaLowThres;
    JetEtaHighThrestype m_l1EtaHighThres;
    JetEtaLowThrestype  m_l1EtaLowThres;
    JetNtype m_hltJetn;

    std::vector<int> m_l1nbinsEt,  // for L1 trigger eff vs. Et
                     m_hltnbinsEt; // for EF trigger eff vs. Et


    // keep track of monitoring groups
    JetSigtype m_monGroups;

    // keep track of hlt containers
    JetContainertype m_hltContainers;

    std::string m_monBase, m_L1dir, m_HLTdir, m_HLTpfx, m_OFpfx, m_Effdir;
    std::string m_L1xAODJetKey; 
    
    // Default names for trigger items used by menu aware monitoring
    std::vector<std::string> m_monitoring_l1jet, m_monitoring_jet;
    std::vector<std::string> m_primary_l1jet, m_primary_jet;
   
    //Default names of triggers for DiJet histos 
    std::vector<std::string> m_l1_DijetChains;
    std::vector<std::string> m_hlt_DijetChains;
    std::vector<std::string> m_of_DijetChains;

    // INternal names used for basic monitornig triggers and triggers used in the efficiencies
    JetSigtype m_basicL1Trig, m_basicHLTTrig;
    JetSigtype m_L1Items, m_HLTChains;

    //std::vector<std::string> /*m_OFJetKeys,*/ m_sampSelTriggers, m_chainsByRegexp;
    JetSigtype m_HLTJetKeys;
    JetSigtype m_OFJetKeys;

    // jet selection, matching
    //bool m_doL1TrigEff, m_doHLTTrigEff, m_doOFJets, m_doEvtSel, m_debuglevel;
    bool /*m_doLumiWeight,*/ m_doL1TrigEff, m_doHLTTrigEff, m_doOFJets, m_debuglevel;
    
    bool m_doselOFJets, m_doselOFBasicHists, m_reqMinPtCut, m_reqEtaCut, m_reqMaxNJetCut;
    bool m_reqP4State, /*m_reqEMFracCut, m_reqN90Cut, m_reqTimeCut,*/ m_reqBadQCut;

    bool m_isPP, m_isHI, m_isCosmic, m_isMC/*m_isPPb*/; // collsion mode flags

    float m_deltaRCut, m_emfracCut;
    //std::vector<float> m_ofemfracCut;

    float m_MinPtCut, m_EtaCut /*m_jetTimens, m_badQFrac*/ ;
    int m_MaxNJet /*, m_n90Cut*/ ;

    std::vector<TLorentzVector> m_v_HLTjet;
    std::vector<TLorentzVector> m_v_L1jet;
    std::vector<TLorentzVector> m_v_OFjet;
    std::vector<int> m_v_HLTindex;
    std::vector<int> m_v_L1index;
    std::vector<int> m_v_OFindex;

    int m_n_index = 0;

    double m_lumi_weight = 0;
    int m_lumiBlock = -1;
    std::vector<int> m_v_lbn;

    std::string m_p4State;
   
    /// Handle to the TDT
    //ToolHandle< Trig::TrigDecisionTool > m_tdt;

    //const Trig::ChainGroup *m_trjetL1Items, *m_trjetL2Chain, *m_trjetEFChain;

    // Container Names
    //const LVL1_ROI *m_L1RoiC;
    const xAOD::JetRoIContainer* m_L1JetRoIC = nullptr;

    //const xAOD::JetContainer* m_EFJetC;
    std::vector<const xAOD::JetContainer*> m_HLTJetC;

    //std::vector<const JetCollection *> m_OFJetC;
    std::vector<const xAOD::JetContainer* > m_OFJetC;

    // fill methods
    StatusCode fillJetHists();        // this method calls all other fill methods
    StatusCode fillBasicHists();      // basic hists
    StatusCode fillOfflineHists();    // offline jet + trigger efficiency hists
    StatusCode fillDiJetHists();    // dijet monitoring hists

    // helpers for fill method
    void fillBasicL1forChain(const std::string& theChain, double L1thr); // No need to pass JetRoI - it is known Khaleesi
    void fillBasicHLTforChain(const std::string& theChain, double HLTthr, const std::string& theContainer);


    // SG retrieval method
    StatusCode retrieveContainers();
    int retrieveLumiBlock();

    // book methods
    void bookJetHists();  // this method calls all other book methods
    void bookOfflineHists(JetSigtype& items, const std::string& ofjet);     // trigger efficiency hists
    //void bookCorrHists(const std::string& level2, const std::string& level1,  const std::string& ofjAlg);
    void bookBasicHists(std::vector<std::string>& level, std::vector<std::string>& hists);     // basic hists
    void bookDijetHistos(const std::string& trigjet, const std::string& ofjet);

    void setHistProperties(TH1* h);
    void set2DHistProperties(TH2* h);
    int basicKineVar(const std::string& hist, std::vector<std::string>& kinevars);
    void clearVectors();
    
    // offline jet cuts
    //bool selectJet(const Jet *jet);

    //typedef enum {LooseBad, MediumBad, TightBad} JetCategorytype;
    bool isBadJet(const std::string& Category, double, double, double, double, double, double, double, double, double, double);
    std::string m_JetCategory;
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
  
    bool passedChainTest( const xAOD::Jet *jet, std::vector<std::string>& mFound,  std::vector<TLorentzVector>& mFoundJets, std::vector<std::string>& mUnmatched,std::vector<TLorentzVector>& mUnmatchedJets, const std::string& level);
    //bool evtSelTriggersPassed();

    TLorentzVector DeltaRMatching(const xAOD::Jet *jet, const std::string &ChainName, const std::string &ContainerName, const std::string& level, double thrHLT, float DRCut, bool& Pass);
    bool   isLeadingJet(const xAOD::Jet *jet, const xAOD::JetContainer *jetcoll,double EtaLow, double EtaHigh, int Jetn);
    bool   isChainActive(const std::string& theChain );
    double signed_delta_phi(double ph11, double phi2);
    double delta_r(double eta1, double phi1, double eta2, double phi2);
  
};

#endif 

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef HLTXAODBPHYSMONTOOL_H
#define HLTXAODBPHYSMONTOOL_H

// C++
#include <string>
#include <vector>
#include <map>

class TLorentzVector;
class TVector3;

// TrigHLTMonitoring interface
#include "TrigHLTMonitoring/IHLTMonTool.h"

#include "TrigDecisionTool/TrigDecisionTool.h"


namespace Trig { class chainGroup; }

#include "xAODTracking/VertexFwd.h"

namespace xAOD {
    class TrigBphys_v1;
    typedef TrigBphys_v1 TrigBphys; // use Fwd include when it exists
}

///Concrete monitoring tool derived from MonitorToolBase
class HLTXAODBphysMonTool : public IHLTMonTool
{
    
public:
    HLTXAODBphysMonTool(   const std::string & type,
                        const std::string & name,
                        const IInterface* parent);
    virtual ~HLTXAODBphysMonTool();
    
    // Standard monitoring methods
    StatusCode init() override;
    StatusCode book() override;
    StatusCode fill() override;
    StatusCode proc() override;
    
private:
    
    // Method to parse the chain list passed from DB to form the monitoring chain dictionaries
    StatusCode generateChainDicts();
    
    // booking and filling methods; helper methods for booking
    StatusCode bookTriggers(); //! book the general trigger hists
    StatusCode fillTriggers(); //! fill the general trigger hists

    StatusCode bookJpsiFinder(); //! book the hists with offline comparisons
    StatusCode fillJpsiFinder(); //! fill the hists with offline comparisons

    StatusCode bookJpsiFinderEfficiency();//! book the hists for the JpsiFinder efficiency
    StatusCode fillJpsiFinderEfficiency();//! fill the hists for the JpsiFinder efficiency 
    StatusCode fillJpsiFinderEfficiencyHelper(const std::string & groupName, const std::string & chainName); ////! helper method for JpsiFinder Efficiency 

    StatusCode bookContainers(); //! book the hists for containers
    StatusCode fillContainers(); //! fill the hists for containers

    StatusCode bookTriggerGroups(); //! book the hists for the triggered groups
    StatusCode fillTriggerGroups(); //! fill the hists for the triggered groups
    StatusCode fillTriggerGroup(const std::string & groupName, const std::string & chainName, const bool fullSetOfHists = true); //! helper method for dedicated trigger chains

    StatusCode bookEfficiencyGroups(); //! book the hists for the efficiency groups
    StatusCode bookEfficiencyGroup(const std::string & groupName); //! book the hists for the efficiency groups
    StatusCode fillEfficiencyGroups(); //! fill the hists for the efficiency groups
    StatusCode fillEfficiencyGroup(const std::string & groupName, const std::string & chainName); //! helper method for dedicated Efficiency chains

        
    void bookTrigBphysHists(const std::string & trigname,const std::string & prefix, const std::string & path, const std::string & chainName, const bool fullSetOfHists = true);
    void fillTrigBphysHists(const xAOD::TrigBphys *bphysItem, const std::string & trigItem,
                            const std::string & prefix, const std::string & path, const std::string & chainName, const bool fullSetOfHists = true);

    
    // member variables
    ToolHandle<Trig::TrigDecisionTool> m_tdt;
    const Trig::ChainGroup *m_all;
    //list for pair of muon track links for efficiency 
    std::list<std::pair<const xAOD::TrackParticle *,const xAOD::TrackParticle *>> m_efficiency_denomnoVtxOS_pairs;   
    //vector for pair of muon track links for JpsiFinder efficiency 
    std::vector<std::pair<const xAOD::TrackParticle *,const xAOD::TrackParticle *>> m_JpsiFinderEfficiency_denomnoVtxOS_pairs;
    // vector of muon pairs to check if already put into histograms
    std::vector<std::pair<const xAOD::TrackParticle *,const xAOD::TrackParticle *>> m_muon_pairs_processed;
    
    // property variables
    double m_muonMass; //! mass of muon
    std::string m_base_path_shifter; //!Base path to the shifter Bphys monitoring hists
    std::string m_base_path_expert;  //!Base path to the expert  Bphys monitoring hists
    std::string m_prefix;            //!Bphys histogram prefix name

    typedef std::map<std::string, std::string> Table_t;
    typedef std::pair<std::string, std::string> Pair_t;
    //typedef std::vector<std::pair<std::string, std::string> > Table_t;
    Table_t m_dedicated_chains;  //! menu-independent labels with matching menu-aware chain names for dedicated monitoring
    Table_t m_dedicatedL1Topo_chains;  //! menu-independent labels with matching menu-aware chain names for dedicated L1Topo monitoring
    Table_t m_efficiency_chains; //! menu-independent labels with matching menu-aware chain names for numerator efficiencies
    
    // patterns for the above, if generated on-the-fly from m_primary_chains list
    Table_t m_dedicated_chains_patterns;
    Table_t m_dedicatedL1Topo_chains_patterns;
    Table_t m_efficiency_chains_patterns;

    std::vector<std::string> m_containerList; //! Container list
    
    std::vector<std::string> m_monitored_chains; //! chaingroups for general monitoring
    std::vector<std::string> m_primary_chains; //! chaingroups for detailed monitoring
    
    bool m_GenerateChainDictsFromDB;

    std::string m_JpsiCandidatesKey; //! offline di-muon container name
    
    
    std::string m_trigchain_denomID; //! denominator trigger for ID efficiencies
    std::string m_trigchain_denomMS;
    std::string m_trigchain_denomL1;
    std::string m_trigchain_denomL2;
    std::string m_trigchain_denomEF;
    std::string m_trigchain_denomnoVtxOS;
    std::string m_trigchain_denomL1Topo;
    
    // patterns for the above, if generated on-the-fly from m_primary_chains list
    std::string m_trigchain_denomnoVtxOS_pattern;
    
    //property variables for histograms, etc.
    double m_mw_jpsi_forward_min;
    double m_mw_jpsi_forward_max ;
    double m_mw_upsi_forward_min;
    double m_mw_upsi_forward_max;
    double m_mw_jpsi_central_min;
    double m_mw_jpsi_central_max;
    double m_mw_upsi_central_min;
    double m_mw_upsi_central_max;
    double m_eta_min;
    double m_eta_max;
    double m_mu_pt_min;
    double m_mu_pt_max;
    double m_trk_pt_min;
    double m_trk_pt_max;
    double m_z0_min;
    double m_z0_max;
    double m_d0_min;
    double m_d0_max;
    double m_dphi_min;
    double m_dphi_max;
    double m_deta_min;
    double m_deta_max;
    double m_dr_min;
    double m_dr_max;
    double m_ptsum_min;
    double m_ptsum_max;
    double m_oniamass_min;
    double m_oniamass_max;
    double m_taumass_min;
    double m_taumass_max;
    double m_oniapt_min;
    double m_oniapt_max ;

    double m_lxy_min, m_lxy_max, m_lxyErr_min, m_lxyErr_max;
    double m_tau_min, m_tau_max, m_tauErr_min, m_tauErr_max;
    double m_pt_min, m_pt_max, m_ptErr_min, m_ptErr_max;
    double m_massErr_min, m_massErr_max;
    double m_EffMatchThresh_Eta=0.1;   
    double m_EffMatchThresh_phi=0.1; 
    double m_EffMatchThresh_pT=0.1;  

    // Method for managing the histogram divisions
    void divide(TH1 *num, TH1 *den, TH1 *quo);
    void divide2(TH2 *num, TH2 *den, TH2 *quo);

    void divide2WithRow(TH2 *num, TH2 *quo,int denom_row=1); // use one row as the denominator hist

    
    // The following methods will eventually be implemented in new "V0Tools". For now,
    // we will keep them here.
    TVector3       trackMomentum(const xAOD::Vertex * vxCandidate, uint trkIndex) const;
    TLorentzVector track4Momentum(const xAOD::Vertex * vxCandidate, int trkIndex, double mass) const;
    TVector3       origTrackMomentum(const xAOD::Vertex * vxCandidate, int trkIndex) const;
    TLorentzVector origTrack4Momentum(const xAOD::Vertex * vxCandidate, int trkIndex, double mass) const;
    //    double         invariantMassError(const xAOD::Vertex* vxCandidate, std::vector<double> masses) const;
    //    double         massErrorVKalVrt(const xAOD::Vertex * vxCandidate, std::vector<double> masses) const;
    double         trackCharge(const xAOD::Vertex * vxCandidate, int i) const;
    //    Amg::MatrixX*  convertVKalCovMatrix(int NTrk, const std::vector<float> & Matrix) const;
    float          cosMethod(const TLorentzVector & Mu1, const TLorentzVector & Mu2, int Mu1_q) const;
    float          phiMethod(const TLorentzVector & Mu1, const TLorentzVector & Mu2, int Mu1_q) const;
    float          deltaR( float eta_1, float phi_1, float eta_2, float phi_2 ) const;

    float          absDeltaPhi(float phi_1, float phi_2 ) const;
    float          absDeltaEta(float eta_1, float eta_2 ) const;
    
    std::string    stripHLT(const std::string& chain) {return chain.substr(4);} // remove "HLT_" from the name
    
};

#endif


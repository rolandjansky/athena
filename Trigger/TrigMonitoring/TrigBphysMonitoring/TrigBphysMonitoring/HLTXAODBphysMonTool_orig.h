/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**    @file HLTXAODBphysMonTool.h
 *   Class declaration for HLTXAODBphysMonTool
 *
 *
 *
 *    @author name (uid@cern.ch)
 *
 *
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

namespace Trig { class chainGroup; }

///Concrete monitoring tool derived from MonitorToolBase
class HLTXAODBphysMonTool : public IHLTMonTool
{
    
public:
    HLTXAODBphysMonTool(   const std::string & type,
                    const std::string & name,
                    const IInterface* parent);
    virtual ~HLTXAODBphysMonTool();
    // Standard monitoring methods
    StatusCode init() ;
    StatusCode book() ;
    StatusCode fill() ;
    
    // xAOD migrated methods
    StatusCode fillContainer   (const std::string& containerItem) ;
    StatusCode fillTriggerChain(const std::string& trigItem, const std::string& monGroup);
    StatusCode fillTriggerEfficiency();
    
    // Offline Di-muon finder
    StatusCode JpsiFinder() ;
    
    // Old Methods - yet to be implemeted
    StatusCode Triggers() ;
    StatusCode L2Tracks() ;
    StatusCode EFTracks() ;
    StatusCode L2DsPhiPiMon() ;
    StatusCode L2TopoDiMuMon() ;
    StatusCode L2TopoDiMuXMon() ;
    StatusCode L2DiMuMon() ;
    StatusCode L2DiMuXMon() ;
    StatusCode L2BJpsieeMon() ;
    StatusCode L2BJpsimumu();
    StatusCode EFDsPhiPiMon() ;
    StatusCode EFTopoDiMuMon() ;
    StatusCode EFDiMuMon() ;
    StatusCode EFBJpsimumu();
    StatusCode CompareDiMuL2EF();
    StatusCode CompareTopoDiMuL2EF();
    
    StatusCode proc(); // called by procHistograms
    
private:
    //James'
    void buildHist( const std::string & trigname,const std::string & prefix);
    void fillHist(const xAOD::TrigBphys *bphysItem, const std::string & trigItem,const std::string & prefix);
    void fillHistEff(const xAOD::TrigBphys *bphysItem, const std::string & trigItem,const std::string & prefix, int DenomOrNum, const std::string & trigEffHistName);

    
    ToolHandle<Trig::TrigDecisionTool> m_trigDec;
    
    // The following methods will eventually be implemented in new "V0Tools". For now,
    // we will keep them here.
    TVector3       trackMomentum(const xAOD::Vertex * vxCandidate, uint trkIndex) const;
    TLorentzVector track4Momentum(const xAOD::Vertex * vxCandidate, int trkIndex, double mass) const;
    TVector3       origTrackMomentum(const xAOD::Vertex * vxCandidate, int trkIndex) const;
    TLorentzVector origTrack4Momentum(const xAOD::Vertex * vxCandidate, int trkIndex, double mass) const;
    double         invariantMassError(const xAOD::Vertex* vxCandidate, std::vector<double> masses) const;
    double         massErrorVKalVrt(const xAOD::Vertex * vxCandidate, std::vector<double> masses) const;
    double         trackCharge(const xAOD::Vertex * vxCandidate, int i) const;
    Amg::MatrixX*  convertVKalCovMatrix(int NTrk, const std::vector<float> & Matrix) const;
    float          cosMethod(const TLorentzVector & Mu1, const TLorentzVector & Mu2, int Mu1_q) const;
    float          phiMethod(const TLorentzVector & Mu1, const TLorentzVector & Mu2, int Mu1_q) const;
    float          deltaR( float eta_1, float phi_1, float eta_2, float phi_2 ) const;
    
    
    double m_muonMass;
    
    
    std::vector<std::string> m_TrigNames;
    std::vector<bool> m_TrigNames1D;
    std::vector<std::string> m_bphysItems; //! Bphysics chains to test
    std::vector<std::string> m_monGroup; //! Monitoring groups for trigger
    std::vector<std::string> m_containerList; //! Container list
    std::vector<std::string> m_effNum;
    std::vector<std::string> m_effDenom;
    std::vector<std::string> m_effNumGroup;
    std::vector<std::string> m_effDenomGroup;
    
    std::string m_JpsiCandidatesKey;
    
    int L1passed;
    int L2passed;
    int EFpassed;
    
    int L1DiMu;
    int L2DiMu;
    int EFDiMu;
	
    //std::vector<std::string> TrigName;
    //std::vector<bool> TrigName1D;
    
    std::vector<std::string> EffNum;
    std::vector<std::string> EffDenom;
    std::map<std::string, TH1F*> EffHistMap;
    
    const Trig::ChainGroup *m_all;
    const Trig::ChainGroup *m_allL1;
    const Trig::ChainGroup *m_allL2; // presumed obsolete
    const Trig::ChainGroup *m_allEF; // presumed obsolete
    const Trig::ChainGroup *m_allHLT;
    
    
    // Method for managing the histogram divisions
    void divide(TH1 *num, TH1 *den, TH1 *quo);
    void divide2(TH2 *num, TH2 *den, TH2 *quo);
    

    //Job Option configurable parameters
    
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
    double m_oniapt_min;
    double m_oniapt_max ;
    
    
    
    
    
};

#endif

/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#ifndef METMonTool_H
#define METMonTool_H


// **********************************************************************
// METMonTool.cxx
// AUTHORS: Michele Consonni
// **********************************************************************


#include "AthenaMonitoring/ManagedMonitorToolBase.h"
#include "AthenaMonitoringKernel/GenericMonitoringTool.h"


#include "JetInterface/IJetSelector.h"
#include "xAODMissingET/MissingETContainer.h" 
#include "xAODJet/JetContainer.h"
#include "xAODEgamma/ElectronContainer.h"
#include "xAODMuon/MuonContainer.h"
#include <string>
#include <vector>

#include "TH1.h"
#include "TH2.h"
#include "TProfile.h"
#include "TProfile2D.h"
#include "TDirectory.h"
class IInterface;
class StatusCode;


/**						        			        
  * METMonTool class
  *
  * This class is finalized to Missing Et data quality monitoring
  *
  * @author Michele.Consonni@cern.ch
  */


class METMonTool : public ManagedMonitorToolBase
{

  public:

    METMonTool( const std::string& type, const std::string& name, const IInterface* parent );
    ~METMonTool();
    
    StatusCode initialize();

    StatusCode clearHistograms();
    
    StatusCode bookHistograms( );
    StatusCode bookSourcesHistograms( std::string& metName, MonGroup& met_mongroup, bool doProfiles );
    StatusCode bookCalosHistograms( MonGroup& met_calos );
    StatusCode bookRegionsHistograms( MonGroup& met_regions );
    StatusCode bookSummaryHistograms( MonGroup& met_summary );
    StatusCode bookProfileHistograms( std::string& metName, const char* objName, MonGroup& met_mongroup, int* index);
    
    StatusCode fillHistograms();
    StatusCode fillSourcesHistograms();
    StatusCode fillCalosHistograms();
    StatusCode fillRegionsHistograms();
    StatusCode fillProfileHistograms( float et, float phi, float objEta, float objPhi, int i );
    
    StatusCode procHistograms( );

  protected:

    ToolHandle<GenericMonitoringTool> m_genTool{ this, "GenTool",  "", "Generic monitoring tool" };

  
    StringProperty m_suffix{ this, "NameSuffix", "", "" };
    // TB this needs to be converted into the ReadHandles, for now doing c++11 move
    StringArrayProperty m_metKeys {this, "metKeys", {"MET_Base", "MET_Topo", "MET_Track"}};
    SG::ReadHandleKeyArray<xAOD::MissingETContainer> m_metKeysFull {this, "metKeysFull", {}, "Don't set this manually, please"};
    SG::ReadHandleKey<xAOD::MissingETContainer> m_metForCut {this, "metForCut", "MET_Reference_AntiKt4LCTopo", "MET object for cuts"};
    SG::ReadHandleKey<xAOD::EventInfo> m_eventInfoKey {this, "eventInfoKey", "EventInfo"};
    StringProperty m_metFinKey {this, "metFinKey", "MET_RefFinal"};
    StringProperty m_metCalKey {this, "metCalKey", "MET_Calo"};
    StringProperty m_metRegKey {this, "metRegKey", "", "Set to MET_TruthRegions if desired"};
    SG::ReadHandleKey<xAOD::JetContainer> m_jetColKey {this, "jetColKey", "AntiKt4LCTopoJets"};
    SG::ReadHandleKey<xAOD::ElectronContainer> m_eleColKey {this, "eleColKey", "Electrons"};
    SG::ReadHandleKey<xAOD::MuonContainer> m_muoColKey {this, "muoColKey", "Muons"};




    const size_t m_calIndices = 7;
    const size_t m_regIndices = 3;
    const std::vector<std::string>                   m_calStrings {
      "PEMB", "EMB", "PEME", "EME", "TILE", "HEC", "FCAL" };
    std::vector<std::string>                   m_regStrings {
      "Central", "EndCap", "Forward" };
    Gaudi::Property<bool> m_met_cut_80 { this, "doMetCut80", false, "Documentation?" };

    Gaudi::Property<int>   m_etabin{ this, "nEtaBins", 100, "Documentation?" };
    Gaudi::Property<int>   m_phibin{ this, "nPhiBins", 100, "Documentation?" };
    Gaudi::Property<int>   m_etbin { this, "nEtBins", 800, "Documentation?" };
    Gaudi::Property<float> m_met_cut{ this, "metCut", 0., "Documentation?" };
    Gaudi::Property<float> m_etrange{ this, "EtRange", 400., "Documentation?" };
    Gaudi::Property<float> m_etrangeSumFactor{ this, "SumEtRangeFactor", 10., "Documentation?" };

    Gaudi::Property<std::vector<float>> m_etrangeCalFactors{ this, "EtRangeCalFactors",  
	{ 0.2, 0.6, 0.1, 0.5, 0.5, 0.4, 0.3 }, "" };

    Gaudi::Property<std::vector<float>> m_etrangeRegFactors{ this, "EtRangeRegFactors", 
	{ 0.1, 0.1, 0.1 }, "" };

    Gaudi::Property<bool>  m_doFillNegativeSumEt{ this, "FillNegativeSumEt", false , "" };
    Gaudi::Property<float> m_tos{ this, "YaxisTitleOffset", 1.25, ""}; 
    float m_truncatedMean = 200.;
    
    std::vector<TH1*> m_et;
    std::vector<TH1*> m_ex;
    std::vector<TH1*> m_ey;
    std::vector<TH1*> m_phi;
    std::vector<TH1*> m_sumet;
    TProfile*         m_metVsSumEt;
    TProfile*         m_metVsMetPhi;

    std::vector<TProfile*>   m_metVsEta;
    //std::vector<TProfile*>   m_metParaVsEta;
    //std::vector<TProfile*>   m_metPerpVsEta;
    std::vector<TProfile*>   m_dphiVsEta;
    std::vector<TProfile*>   m_metVsPhi;
    //std::vector<TProfile*>   m_metParaVsPhi;
    //std::vector<TProfile*>   m_metPerpVsPhi;
    std::vector<TProfile*>   m_dphiVsPhi;
    std::vector<TProfile2D*> m_metVsEtaPhi;

    std::vector<TH1*> m_etCal;
    std::vector<TH1*> m_exCal;
    std::vector<TH1*> m_eyCal;
    std::vector<TH1*> m_phiCal;
    std::vector<TH1*> m_sumetCal;

    std::vector<TH1*> m_etReg;
    std::vector<TH1*> m_exReg;
    std::vector<TH1*> m_eyReg;
    std::vector<TH1*> m_phiReg;
    std::vector<TH1*> m_sumetReg;

    TProfile* m_exMean;
    TProfile* m_eyMean;
    TProfile* m_phiMean;

    TProfile* m_exCalMean;
    TProfile* m_eyCalMean;
    TProfile* m_phiCalMean;

    TProfile* m_exRegMean;
    TProfile* m_eyRegMean;
    TProfile* m_phiRegMean;
    
    int m_iJet, m_iEle, m_iMuo;

  

    Gaudi::Property<bool> m_doJetcleaning{ this, "doJetcleaning", false, "" };
    Gaudi::Property<bool> m_badJets{ this, "badJets", false, "" };
    
    /// Set Number of WARNINGs displayed for failure of container retrieval
    Gaudi::Property<int> m_maxNumContainerWarnings{ this, "maxNumContainerWarnings", 10, "" };

    int m_ContainerWarnings_Muon{}, m_ContainerWarnings_Ele{}, m_ContainerWarnings_Jet{}; 
    std::vector<int> m_ContainerWarnings_metKeys; 

    ToolHandle<IJetSelector> m_selTool{ this, "JetSelectorTool", "", ""}; /// used only if m_selType == FromTool

    ITHistSvc * m_thistSvc;   
};


#endif // METMonTool_H

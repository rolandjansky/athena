/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef METMonTool_H
#define METMonTool_H


// **********************************************************************
// METMonTool.cxx
// AUTHORS: Michele Consonni
// **********************************************************************


#include "AthenaMonitoring/ManagedMonitorToolBase.h"
#include "AthenaMonitoring/GenericMonitoringTool.h"

//#include "GaudiKernel/ITHistSvc.h"
#include "JetInterface/IJetSelector.h"
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

    ToolHandle<GenericMOnitoringTool> m_genTool{ this, "GenTool",  "", "Generic monitoring tool" };

  
    std::string m_suffix;
    
    std::vector<std::string> m_metKeys;
    std::string              m_metFinKey;
    std::string              m_metCalKey;
    std::string              m_metRegKey;
    std::string              m_jetColKey;
    std::string              m_eleColKey;
    std::string              m_muoColKey;
    
    std::vector<int>      m_calIndices;
    std::vector<int> m_regIndices;
    std::vector<std::string>                   m_calStrings;
    std::vector<std::string>                   m_regStrings;
    bool m_met_cut_80;

    int   m_etabin;
    int   m_phibin;
    int   m_etbin;
    float   m_met_cut;
    float              m_etrange;
    float              m_etrangeSumFactor;
    std::vector<float> m_etrangeCalFactors;
    std::vector<float> m_etrangeRegFactors;
    bool               m_doFillNegativeSumEt;
    float              m_tos;
    float              m_truncatedMean;
    
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

  

    bool m_doJetcleaning;
    bool m_badJets;
    
    /// Set Number of WARNINGs displayed for failure of container retrieval
    int m_maxNumContainerWarnings;
    int m_ContainerWarnings_Muon, m_ContainerWarnings_Ele, m_ContainerWarnings_Jet;
    std::vector<int> m_ContainerWarnings_metKeys; 

    ToolHandle<IJetSelector> m_selTool; /// used only if m_selType == FromTool

    //ITHistSvc * m_thistSvc;   // TB ServiceHandle? 
  private:

    float m_Pi;

};


#endif // METMonTool_H

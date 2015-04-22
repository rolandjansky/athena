/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/


/**********************************************************************
 * AsgTool: TrigEgammaNavZeeTPEff
 * Authors:
 *      Ryan Mackenzie White <ryan.white@cern.ch>
 *      Denis Damazio <denis.damazio@cern.ch>
 * Contributors:
 * Date: Feb 2015
 * Description:
 *      Derived class for measuring the trigger efficiency.
 *      Produces trigger efficiency distributions at each trigger level
 **********************************************************************/
#include "TrigEgammaAnalysisTools/TrigEgammaNavZeeTPEff.h"

//**********************************************************************

TrigEgammaNavZeeTPEff::
TrigEgammaNavZeeTPEff( const std::string& myname )
: TrigEgammaNavZeeTPBaseTool(myname) {
  declareProperty("DirectoryPath",m_dir="NavZeeTPEff");
}

//**********************************************************************
StatusCode TrigEgammaNavZeeTPEff::childInitialize(){

    m_eventCounter=0;
    std::vector <std::string> dirnames;
    for (int i = 0; i < (int) m_probeTrigList.size(); i++) {
        std::string probeTrigger = m_probeTrigList[i];
        ATH_MSG_INFO("Trigger " << probeTrigger);
        dirnames.push_back(m_dir+"/" + probeTrigger + "/L1CaloEfficiencies");
        dirnames.push_back(m_dir+"/" + probeTrigger + "/L2CaloEfficiencies");
        dirnames.push_back(m_dir+"/" + probeTrigger + "/L2Efficiencies");
        dirnames.push_back(m_dir+"/" + probeTrigger + "/EFCaloEfficiencies");
        dirnames.push_back(m_dir+"/" + probeTrigger + "/HLTEfficiencies");
    }

    for (int i = 0; i < (int) dirnames.size(); i++) {
        addDirectory(dirnames[i]);
        ATH_MSG_INFO(dirnames[i]);
        addHistogram(new TH1F("ZeeProbeEt", "Probe E_{T} for Z->e+e; E_{T} [GeV] ; Count", 50, 0., 100.));
        addHistogram(new TH1F("ZeeMatchProbeEt", "Match Probe E_{T} for Z->e+e; E_{T} [GeV] ; Count", 50, 0., 100.));
        addHistogram(new TH1F("ZeeProbeEffEt", "Probe Efficiency E_{T} for Z->e+e; E_{T} [GeV] ; #epsilon", 50, 0., 100.));
        addHistogram(new TH1F("ZeeProbeEta", "Probe #eta for Z->e+e; #eta ; Count", 50, -2.5, 2.5));
        addHistogram(new TH1F("ZeeMatchProbeEta", "Match Probe #eta for Z->e+e; #eta ; Count", 50, -2.5, 2.5));
        addHistogram(new TH1F("ZeeProbeEffEta", "Probe #eta for Z->e+e; #eta ; #epsilon", 50, -2.5, 2.5));
        addHistogram(new TH1F("ZeeProbePhi", "Probe #eta for Z->e+e; #phi ; Count", 50, -3.14, 3.14));
        addHistogram(new TH1F("ZeeMatchProbePhi", "Match Probe #eta for Z->e+e; #phi ; Count", 50, -3.14, 3.14));
        addHistogram(new TH1F("ZeeProbeEffPhi", "Probe #eta for Z->e+e; #phi ; #epsilon", 50, -3.14, 3.14));
    }
    return StatusCode::SUCCESS;
}

void TrigEgammaNavZeeTPEff::fillProbeHistos(const std::string dir,const float etthr,const float et, const float eta, const float phi){
    cd(dir);
    hist1("ZeeProbeEt")->Fill(et);
    if(et > etthr+1.0){
        hist1("ZeeProbeEta")->Fill(eta);
        hist1("ZeeProbePhi")->Fill(phi);
    }
}

void TrigEgammaNavZeeTPEff::fillMatchHistos(const std::string dir,const float etthr,const float et, const float eta, const float phi){
    cd(dir);
    hist1("ZeeMatchProbeEt")->Fill(et);
    if(et > etthr+1.0){
        hist1("ZeeMatchProbeEta")->Fill(eta);
        hist1("ZeeMatchProbePhi")->Fill(phi);
    }
}

StatusCode TrigEgammaNavZeeTPEff::childExecute()
{

    m_eventCounter++;

    // Event Wise Selection (independent of the required signatures)
    if ( !TrigEgammaNavZeeTPBaseTool::EventWiseSelection() ) return StatusCode::SUCCESS;

    for(unsigned int ilist = 0; ilist != m_probeTrigList.size(); ilist++) {
        std::string probeTrigger = m_probeTrigList.at(ilist);

        std::string type="";
        float etthr=0;
        float l1thr=0;
        std::string l1type="";
        std::string pidname="";
        bool perf=false;
        bool etcut=false;
        parseTriggerName(probeTrigger,"Loose",type,etthr,l1thr,l1type,pidname,perf,etcut); // Determines probe PID from trigger
        if ( executeTandP(probeTrigger).isFailure() )
            return StatusCode::FAILURE;

        // Just for counting
        for(unsigned int i=0;i<m_probeElectrons.size();i++){
            const xAOD::Electron* offEl = m_probeElectrons[i].first;
            float denOffEl_et = getEt(offEl)/1e3;//offEl->caloCluster()->et()/1e3;
            float denOffEl_eta = offEl->eta();
            float denOffEl_phi = offEl->phi();
            bool passedL1Calo=false;
            bool passedL2Calo=false;
            bool passedL2=false;
            bool passedEFCalo=false;
            bool passedEF=false;
            const HLT::TriggerElement* feat = m_probeElectrons[i].second;
            fillProbeHistos(m_dir+"/"+probeTrigger+"/L1CaloEfficiencies",etthr,denOffEl_et,denOffEl_eta,denOffEl_phi);
            fillProbeHistos(m_dir+"/"+probeTrigger+"/L2CaloEfficiencies",etthr,denOffEl_et,denOffEl_eta,denOffEl_phi);
            fillProbeHistos(m_dir+"/"+probeTrigger+"/L2Efficiencies",etthr,denOffEl_et,denOffEl_eta,denOffEl_phi);
            fillProbeHistos(m_dir+"/"+probeTrigger+"/EFCaloEfficiencies",etthr,denOffEl_et,denOffEl_eta,denOffEl_phi);
            fillProbeHistos(m_dir+"/"+probeTrigger+"/HLTEfficiencies",etthr,denOffEl_et,denOffEl_eta,denOffEl_phi);

            if ( feat ) {
                passedL1Calo=ancestorPassed<xAOD::EmTauRoI>(feat);
                passedL2Calo = ancestorPassed<xAOD::TrigEMCluster>(feat);
                passedL2 = ancestorPassed<xAOD::TrigElectronContainer>(feat);
                passedEFCalo = ancestorPassed<xAOD::CaloClusterContainer>(feat);
                passedEF = ancestorPassed<xAOD::ElectronContainer>(feat);
                if( passedL1Calo) fillMatchHistos(m_dir+"/"+probeTrigger+"/L1CaloEfficiencies",etthr,denOffEl_et,denOffEl_eta,denOffEl_phi);
                if( passedL2Calo ) fillMatchHistos(m_dir+"/"+probeTrigger+"/L2CaloEfficiencies",etthr,denOffEl_et,denOffEl_eta,denOffEl_phi);
                if( passedL2 ) fillMatchHistos(m_dir+"/"+probeTrigger+"/L2Efficiencies",etthr,denOffEl_et,denOffEl_eta,denOffEl_phi);
                if( passedEFCalo ) fillMatchHistos(m_dir+"/"+probeTrigger+"/EFCaloEfficiencies",etthr,denOffEl_et,denOffEl_eta,denOffEl_phi);
                if( passedEF ) fillMatchHistos(m_dir+"/"+probeTrigger+"/HLTEfficiencies",etthr,denOffEl_et,denOffEl_eta,denOffEl_phi);
            } // Features
        } // End loop over electrons
        clearProbeList(); // Clear Probes after each trigger
    } // End loop over trigger list


    return StatusCode::SUCCESS;

}

StatusCode TrigEgammaNavZeeTPEff::childFinalize()
{
    for(unsigned int ilist = 0; ilist != m_probeTrigList.size(); ilist++) {
        std::string probeTrigger = m_probeTrigList.at(ilist);
        cd(m_dir+"/"+probeTrigger+"/L1CaloEfficiencies");
        hist1("ZeeMatchProbeEt")->Sumw2();
        hist1("ZeeProbeEt")->Sumw2();
        hist1("ZeeProbeEffEt")->Divide(hist1("ZeeMatchProbeEt"),hist1("ZeeProbeEt"),1,1,"b");
        hist1("ZeeMatchProbeEta")->Sumw2();
        hist1("ZeeProbeEta")->Sumw2();
        hist1("ZeeProbeEffEta")->Divide(hist1("ZeeMatchProbeEta"),hist1("ZeeProbeEta"),1,1,"b");
        hist1("ZeeMatchProbePhi")->Sumw2();
        hist1("ZeeProbePhi")->Sumw2();
        hist1("ZeeProbeEffPhi")->Divide(hist1("ZeeMatchProbePhi"),hist1("ZeeProbePhi"),1,1,"b");
        cd(m_dir+"/"+probeTrigger+"/L2CaloEfficiencies");
        hist1("ZeeMatchProbeEt")->Sumw2();
        hist1("ZeeProbeEt")->Sumw2();
        hist1("ZeeProbeEffEt")->Divide(hist1("ZeeMatchProbeEt"),hist1("ZeeProbeEt"),1,1,"b");
        hist1("ZeeMatchProbeEta")->Sumw2();
        hist1("ZeeProbeEta")->Sumw2();
        hist1("ZeeProbeEffEta")->Divide(hist1("ZeeMatchProbeEta"),hist1("ZeeProbeEta"),1,1,"b");
        hist1("ZeeMatchProbePhi")->Sumw2();
        hist1("ZeeProbePhi")->Sumw2();
        hist1("ZeeProbeEffPhi")->Divide(hist1("ZeeMatchProbePhi"),hist1("ZeeProbePhi"),1,1,"b");
        cd(m_dir+"/"+probeTrigger+"/L2Efficiencies");
        hist1("ZeeMatchProbeEt")->Sumw2();
        hist1("ZeeProbeEt")->Sumw2();
        hist1("ZeeProbeEffEt")->Divide(hist1("ZeeMatchProbeEt"),hist1("ZeeProbeEt"),1,1,"b");
        hist1("ZeeMatchProbeEta")->Sumw2();
        hist1("ZeeProbeEta")->Sumw2();
        hist1("ZeeProbeEffEta")->Divide(hist1("ZeeMatchProbeEta"),hist1("ZeeProbeEta"),1,1,"b");
        hist1("ZeeMatchProbePhi")->Sumw2();
        hist1("ZeeProbePhi")->Sumw2();
        hist1("ZeeProbeEffPhi")->Divide(hist1("ZeeMatchProbePhi"),hist1("ZeeProbePhi"),1,1,"b");
        cd(m_dir+"/"+probeTrigger+"/EFCaloEfficiencies");
        hist1("ZeeMatchProbeEt")->Sumw2();
        hist1("ZeeProbeEt")->Sumw2();
        hist1("ZeeProbeEffEt")->Divide(hist1("ZeeMatchProbeEt"),hist1("ZeeProbeEt"),1,1,"b");
        hist1("ZeeMatchProbeEta")->Sumw2();
        hist1("ZeeProbeEta")->Sumw2();
        hist1("ZeeProbeEffEta")->Divide(hist1("ZeeMatchProbeEta"),hist1("ZeeProbeEta"),1,1,"b");
        hist1("ZeeMatchProbePhi")->Sumw2();
        hist1("ZeeProbePhi")->Sumw2();
        hist1("ZeeProbeEffPhi")->Divide(hist1("ZeeMatchProbePhi"),hist1("ZeeProbePhi"),1,1,"b");
        cd(m_dir+"/"+probeTrigger+"/HLTEfficiencies");
        hist1("ZeeMatchProbeEt")->Sumw2();
        hist1("ZeeProbeEt")->Sumw2();
        hist1("ZeeProbeEffEt")->Divide(hist1("ZeeMatchProbeEt"),hist1("ZeeProbeEt"),1,1,"b");
        hist1("ZeeMatchProbeEta")->Sumw2();
        hist1("ZeeProbeEta")->Sumw2();
        hist1("ZeeProbeEffEta")->Divide(hist1("ZeeMatchProbeEta"),hist1("ZeeProbeEta"),1,1,"b");
        hist1("ZeeMatchProbePhi")->Sumw2();
        hist1("ZeeProbePhi")->Sumw2();
        hist1("ZeeProbeEffPhi")->Divide(hist1("ZeeMatchProbePhi"),hist1("ZeeProbePhi"),1,1,"b");
    }
    return StatusCode::SUCCESS;
}

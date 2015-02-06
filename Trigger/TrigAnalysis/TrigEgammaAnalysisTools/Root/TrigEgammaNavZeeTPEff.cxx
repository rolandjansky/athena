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
        dirnames.push_back(m_dir+"/" + probeTrigger + "/L2CaloEfficiencies");
        dirnames.push_back(m_dir+"/" + probeTrigger + "/L2Efficiencies");
        dirnames.push_back(m_dir+"/" + probeTrigger + "/HLTEfficiencies");
    }

    for (int i = 0; i < (int) dirnames.size(); i++) {
        addDirectory(dirnames[i]);
        addHistogram(new TH1F("ZeeProbePt", "Probe P_{T} for Z->e+e; p_{T} [GeV] ; Count", 50, 0., 100.));
        addHistogram(new TH1F("ZeeMatchProbePt", "Match Probe P_{T} for Z->e+e; p_{T} [GeV] ; Count", 50, 0., 100.));
        addHistogram(new TH1F("ZeeProbeEffPt", "Probe Efficiency P_{T} for Z->e+e; p_{T} [GeV] ; #epsilon", 50, 0., 100.));
        addHistogram(new TH1F("ZeeProbeEta", "Probe #eta for Z->e+e; #eta ; Count", 50, -2.5, 2.5));
        addHistogram(new TH1F("ZeeMatchProbeEta", "Match Probe #eta for Z->e+e; #eta ; Count", 50, -2.5, 2.5));
        addHistogram(new TH1F("ZeeProbeEffEta", "Probe #eta for Z->e+e; #eta ; #epsilon", 50, -2.5, 2.5));
    }
    return StatusCode::SUCCESS;
}

StatusCode TrigEgammaNavZeeTPEff::childExecute()
{

    m_eventCounter++;

    // Event Wise Selection (independent of the required signatures)
    if ( !TrigEgammaNavZeeTPBaseTool::EventWiseSelection() ) return StatusCode::SUCCESS;

    for(unsigned int ilist = 0; ilist != m_probeTrigList.size(); ilist++) {
        std::string probeTrigger = m_probeTrigList.at(ilist);

        if ( executeTandP(probeTrigger).isFailure() )
            return StatusCode::FAILURE;

        // Just for counting
        for(unsigned int i=0;i<m_probeElectrons.size();i++){
            if ( m_probeElectrons[i].first->pt() < 24e3 ) continue;
            const xAOD::Electron* offEl = m_probeElectrons[i].first;
            float denOffEl_et = offEl->caloCluster()->et()/1e3;
            float denOffEl_eta = offEl->caloCluster()->eta();
            bool passedL2=false;
            bool passedEF=false;
            const HLT::TriggerElement* feat = m_probeElectrons[i].second;
            cd(m_dir+"/"+probeTrigger+"/L2CaloEfficiencies");
            hist1("ZeeProbePt")->Fill(denOffEl_et);
            hist1("ZeeProbeEta")->Fill(denOffEl_eta);
            cd(m_dir+"/"+probeTrigger+"/L2Efficiencies");
            hist1("ZeeProbePt")->Fill(denOffEl_et);
            hist1("ZeeProbeEta")->Fill(denOffEl_eta);
            cd(m_dir+"/"+probeTrigger+"/HLTEfficiencies");
            hist1("ZeeProbePt")->Fill(denOffEl_et);
            hist1("ZeeProbeEta")->Fill(denOffEl_eta);

            if ( feat ) {

                passedL2 = ancestorPassed<xAOD::TrigElectronContainer>(feat);
                passedEF = ancestorPassed<xAOD::ElectronContainer>(feat);
                if ( passedL2 ) {
                    cd(m_dir+"/"+probeTrigger+"/L2Efficiencies");
                    hist1("ZeeMatchProbePt")->Fill(denOffEl_et);
                    hist1("ZeeMatchProbeEta")->Fill(denOffEl_eta);
                }
                if ( passedEF ) {
                    cd(m_dir+"/"+probeTrigger+"/HLTEfficiencies");
                    hist1("ZeeMatchProbePt")->Fill(denOffEl_et);
                    hist1("ZeeMatchProbeEta")->Fill(denOffEl_eta);
                }
            }
        }
        clearProbeList(); // Clear Probes after each trigger
    } // End loop over trigger list


    return StatusCode::SUCCESS;

}

StatusCode TrigEgammaNavZeeTPEff::childFinalize()
{
    for(unsigned int ilist = 0; ilist != m_probeTrigList.size(); ilist++) {
        std::string probeTrigger = m_probeTrigList.at(ilist);
        cd(m_dir+"/"+probeTrigger+"/L2CaloEfficiencies");
        hist1("ZeeMatchProbePt")->Sumw2();
        hist1("ZeeProbePt")->Sumw2();
        hist1("ZeeProbeEffPt")->Divide(hist1("ZeeMatchProbePt"),hist1("ZeeProbePt"),1,1,"b");
        hist1("ZeeMatchProbeEta")->Sumw2();
        hist1("ZeeProbeEta")->Sumw2();
        hist1("ZeeProbeEffEta")->Divide(hist1("ZeeMatchProbeEta"),hist1("ZeeProbeEta"),1,1,"b");
        cd(m_dir+"/"+probeTrigger+"/L2Efficiencies");
        hist1("ZeeMatchProbePt")->Sumw2();
        hist1("ZeeProbePt")->Sumw2();
        hist1("ZeeProbeEffPt")->Divide(hist1("ZeeMatchProbePt"),hist1("ZeeProbePt"),1,1,"b");
        hist1("ZeeMatchProbeEta")->Sumw2();
        hist1("ZeeProbeEta")->Sumw2();
        hist1("ZeeProbeEffEta")->Divide(hist1("ZeeMatchProbeEta"),hist1("ZeeProbeEta"),1,1,"b");
        cd(m_dir+"/"+probeTrigger+"/HLTEfficiencies");
        hist1("ZeeMatchProbePt")->Sumw2();
        hist1("ZeeProbePt")->Sumw2();
        hist1("ZeeProbeEffPt")->Divide(hist1("ZeeMatchProbePt"),hist1("ZeeProbePt"),1,1,"b");
        hist1("ZeeMatchProbeEta")->Sumw2();
        hist1("ZeeProbeEta")->Sumw2();
        hist1("ZeeProbeEffEta")->Divide(hist1("ZeeMatchProbeEta"),hist1("ZeeProbeEta"),1,1,"b");
    }
    return StatusCode::SUCCESS;
}

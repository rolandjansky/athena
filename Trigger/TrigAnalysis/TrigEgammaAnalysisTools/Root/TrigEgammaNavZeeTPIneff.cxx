/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**********************************************************************
 * AsgTool: TrigEgammaNavZeeTPIneff
 * Authors:
 *      Ryan Mackenzie White <ryan.white@cern.ch>
 *      Denis Damazio <denis.damazio@cern.ch>
 * Contributors:
 * Date: Feb 2015
 * Description:
 *      Derived class to study bit-wise inefficiencies.
 *      Requires additional ToolHandleArray of the Online Selectors.
 *      Should be extended to study LH.
 **********************************************************************/
#include "TrigEgammaAnalysisTools/TrigEgammaNavZeeTPIneff.h"

//**********************************************************************

TrigEgammaNavZeeTPIneff::
TrigEgammaNavZeeTPIneff( const std::string& myname )
: TrigEgammaNavZeeTPBaseTool(myname) {
  declareProperty("DirectoryPath",m_dir="NavZeeTPIneff");
  declareProperty("ElectronOnlPPSelector", m_electronOnlPPCutIDTool);
  declareProperty("IneffLabels",m_labels);
  std::cout << "Building up" << std::endl;
}

StatusCode TrigEgammaNavZeeTPIneff::childInitialize(){

    m_eventCounter=0;
    m_probesTried=0;
    addDirectory(m_dir);
    std::vector <std::string> dirnames;
    for (int i = 0; i < (int) m_probeTrigList.size(); i++) {
        std::string probeTrigger = m_probeTrigList[i];
        dirnames.push_back(m_dir+"/" + probeTrigger);
    }
    m_labels.push_back("All");
    for (int i = 0; i < (int) dirnames.size(); i++) {
        addDirectory(dirnames[i]);
        addHistogram(new TH1F("IsEmFailLoose","IsEmFailLoose",33,0,33));
        addHistogram(new TH1F("IsEmFailMedium","IsEmFailMedium",33,0,33));
        addHistogram(new TH1F("IsEmFailTight","IsEmFailTight",33,0,33));
        addHistogram(new TH1F("IneffIsEmLoose","IsEmLoose",33,0,33));
        addHistogram(new TH1F("IneffIsEmMedium","IsEmMedium",33,0,33));
        addHistogram(new TH1F("IneffIsEmTight","IsEmTight",33,0,33));
        for(unsigned int i=0;i<m_labels.size();i++){
            hist1("IsEmFailLoose")->GetXaxis()->SetBinLabel(i+1,m_labels.at(i).c_str());
            hist1("IsEmFailMedium")->GetXaxis()->SetBinLabel(i+1,m_labels.at(i).c_str());
            hist1("IsEmFailTight")->GetXaxis()->SetBinLabel(i+1,m_labels.at(i).c_str());
            hist1("IneffIsEmLoose")->GetXaxis()->SetBinLabel(i+1,m_labels.at(i).c_str());
            hist1("IneffIsEmMedium")->GetXaxis()->SetBinLabel(i+1,m_labels.at(i).c_str());
            hist1("IneffIsEmTight")->GetXaxis()->SetBinLabel(i+1,m_labels.at(i).c_str());
        }
    }
    if ( m_electronOnlPPCutIDTool.retrieve().isFailure() ){
        std::cout << "retrieve failed for tools" << std::endl;
    }
   

    return StatusCode::SUCCESS;
}

StatusCode TrigEgammaNavZeeTPIneff::childExecute()
{

    m_eventCounter++;

    // Event Wise Selection (independent of the required signatures)
    if ( !TrigEgammaNavZeeTPBaseTool::EventWiseSelection() ) return StatusCode::SUCCESS;

    for(unsigned int ilist = 0; ilist != m_probeTrigList.size(); ilist++) {
        std::string probeTrigger = m_probeTrigList.at(ilist);
        cd(m_dir+"/"+probeTrigger);
        if ( executeTandP(probeTrigger).isFailure() )
            return StatusCode::FAILURE;

        // Just for counting
        for(unsigned int i=0;i<m_probeElectrons.size();i++){
            m_probesTried++;

            hist1("IneffIsEmTight")->Fill(32.5);
            hist1("IneffIsEmMedium")->Fill(32.5);
            hist1("IneffIsEmLoose")->Fill(32.5);
            const xAOD::Electron* offEl = m_probeElectrons[i].first;
            //float denOffEl_et = offEl->caloCluster()->et()/1e3;
            float denOffEl_eta = offEl->caloCluster()->eta();
            float denOffEl_phi = offEl->caloCluster()->phi();
            bool passedEF=false;
            const HLT::TriggerElement* feat = m_probeElectrons[i].second;
            const xAOD::Electron* selEF = NULL;

            if ( feat ) {
                 // Check if passed EFElectronHypo
                passedEF = ancestorPassed<xAOD::ElectronContainer>(feat);
                // Retrieve all electrons for ROI and trigger -- feature container before EFHypo
                const auto* EFEl = getFeature<xAOD::ElectronContainer>(feat);
                if ( EFEl != NULL )
                    for(unsigned int j=0;j<EFEl->size();j++){
                        float deltaEta = TMath::Abs(denOffEl_eta- EFEl->at(j)->eta() );
                        if ( deltaEta < 0.05 ) {
                            float deltaPhi = TMath::Abs(denOffEl_phi- EFEl->at(j)->phi() );
                            deltaPhi = TMath::Abs( TMath::Pi() - deltaPhi );
                            deltaPhi = TMath::Abs( TMath::Pi() - deltaPhi );
                            if ( deltaPhi < 0.05 ) {
                                selEF = EFEl->at(j);
                            } // deltaPhi
                        } // deltaEta
                    } // loop over EFEl

            }
            // If fails hypo, check the remaining nearest probes
            if ( !passedEF && (feat) && (selEF!=NULL) ) {
                m_electronOnlPPCutIDTool[2]->accept(selEF);
                m_electronOnlPPCutIDTool[1]->accept(selEF);
                m_electronOnlPPCutIDTool[0]->accept(selEF);
                unsigned int loose = m_electronOnlPPCutIDTool[2]->IsemValue();
                unsigned int medium= m_electronOnlPPCutIDTool[1]->IsemValue();
                unsigned int tight = m_electronOnlPPCutIDTool[0]->IsemValue();
                for(int ii=0;ii<32;ii++){
                    if ( (tight>>ii) & 0x1 ){
                        hist1("IsEmFailTight")->Fill(ii+0.5);
                        hist1("IneffIsEmTight")->Fill(ii+0.5);
                    }
                    if ( (medium>>ii) & 0x1 ){
                        hist1("IneffIsEmMedium")->Fill(ii+0.5);
                        hist1("IsEmFailMedium")->Fill(ii+0.5);
                    }
                    if ( (loose>>ii) & 0x1 ){
                        hist1("IneffIsEmLoose")->Fill(ii+0.5);
                        hist1("IsEmFailLoose")->Fill(ii+0.5);
                    }
                }
            }
        }
        clearProbeList();
    } // End loop over trigger lists


    return StatusCode::SUCCESS;

}

StatusCode TrigEgammaNavZeeTPIneff::childFinalize()
{
    float ineff = 0; 
    for(unsigned int ilist = 0; ilist != m_probeTrigList.size(); ilist++) {
        std::string probeTrigger = m_probeTrigList.at(ilist);
        cd(m_dir+"/"+probeTrigger);
        for(int ii=1;ii<=32;ii++){
            hist1("IneffIsEmTight")->Scale(1./hist1("IneffIsEmTight")->GetBinContent(32));
            hist1("IneffIsEmMedium")->Scale(1./hist1("IneffIsEmMedium")->GetBinContent(32));
            hist1("IneffIsEmLoose")->Scale(1./hist1("IneffIsEmLoose")->GetBinContent(32));
        }
    }

    return StatusCode::SUCCESS;
}

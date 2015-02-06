/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**********************************************************************
 * AsgTool: TrigEgammaNavZeeTPRes
 * Authors:
 *      Ryan Mackenzie White <ryan.white@cern.ch>
 *      Denis Damazio <denis.damazio@cern.ch>
 * Contributors:
 * Date: Feb 2015
 * Description:
 *      Derived class for resolution studies
 **********************************************************************/
#include "TrigEgammaAnalysisTools/TrigEgammaNavZeeTPRes.h"

//**********************************************************************

TrigEgammaNavZeeTPRes::
TrigEgammaNavZeeTPRes( const std::string& myname )
: TrigEgammaNavZeeTPBaseTool(myname) {
  declareProperty("DirectoryPath",m_dir="NavZeeTPRes");
}

//**********************************************************************
StatusCode TrigEgammaNavZeeTPRes::childInitialize(){
    ATH_MSG_DEBUG("Initialize NavZeeTPRes");

    m_eventCounter=0;
    std::vector <std::string> dirnames;
    for (int i = 0; i < (int) m_probeTrigList.size(); i++) {
        std::string probeTrigger = m_probeTrigList[i];
        dirnames.push_back(m_dir+"/" + probeTrigger + "/L2CaloResolutions");
        dirnames.push_back(m_dir+"/" + probeTrigger + "/L2Resolutions");
        dirnames.push_back(m_dir+"/" + probeTrigger + "/HLTResolutions");
    }

    for (int i = 0; i < (int) dirnames.size(); i++) {
        addDirectory(dirnames[i]);
        addHistogram(new TH1F("EtRes","EtRes",160,-40.0,40.0));
        addHistogram(new TH2F("EtRes_eta","EtRes_eta",50,-2.5,2.5,160,-40.0,40.0));
        addHistogram(new TH2F("EtRes_et","EtRes_et",80,0,80,160,-40.0,40.0));
    }
    return StatusCode::SUCCESS;
}

StatusCode TrigEgammaNavZeeTPRes::childExecute()
{

    m_eventCounter++;
    ATH_MSG_DEBUG("Execute NavZeeTPRes event " << m_eventCounter); 
    // Event Wise Selection (independent of the required signatures)
    if ( !TrigEgammaNavZeeTPBaseTool::EventWiseSelection() ) return StatusCode::SUCCESS;

    for(unsigned int ilist = 0; ilist != m_probeTrigList.size(); ilist++) {
        std::string probeTrigger = m_probeTrigList.at(ilist);
        ATH_MSG_DEBUG("Execute NavZeeTPRes:: executeTandP " << m_eventCounter);
        if ( executeTandP(probeTrigger).isFailure() )   
            return StatusCode::FAILURE;

        // Just for counting
        ATH_MSG_DEBUG("Execute NavZeeTPRes:: Probe Loop " << m_probeElectrons.size()); 
        for(unsigned int i=0;i<m_probeElectrons.size();i++){
            if ( m_probeElectrons[i].first->pt() < 24e3 ) continue;
            const xAOD::Electron* offEl = m_probeElectrons[i].first;
            //float denOffEl_et = offEl->caloCluster()->et()/1e3;
            float denOffEl_eta = offEl->caloCluster()->eta();
            float denOffEl_phi = offEl->caloCluster()->phi();
            bool passedL2=false;
            bool passedEF=false;
            const HLT::TriggerElement* feat = m_probeElectrons[i].second;

            const xAOD::Electron* selEF = NULL;
            const xAOD::TrigElectron* selL2 = NULL;
            if ( feat ) {

                passedL2 = ancestorPassed<xAOD::TrigElectronContainer>(feat);
                const xAOD::TrigElectronContainer* L2El = getFeature<xAOD::TrigElectronContainer>(feat);
                if (passedL2 && (L2El!=NULL) )
                    for(unsigned int j=0;j<L2El->size();j++){
                        float deltaEta = TMath::Abs(denOffEl_eta- L2El->at(j)->eta() );
                        if ( deltaEta < 0.05 ) {
                            float deltaPhi = TMath::Abs(denOffEl_phi- L2El->at(j)->phi() );
                            deltaPhi = TMath::Abs( TMath::Pi() - deltaPhi );
                            deltaPhi = TMath::Abs( TMath::Pi() - deltaPhi );
                            if ( deltaPhi < 0.05 ) {
                                selL2 = L2El->at(j);
                            } // deltaPhi
                        } // deltaEta
                    } // loop over L2El


                passedEF = ancestorPassed<xAOD::ElectronContainer>(feat);
                const xAOD::ElectronContainer* EFEl = getFeature<xAOD::ElectronContainer>(feat);
                if (passedEF && (EFEl!=NULL) )
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
            if ( (offEl!=NULL) && (selEF!=NULL) ) {
                float offET = offEl->e()/TMath::CosH(offEl->trackParticle()->eta());
                float onlET = selEF->e()/TMath::CosH(selEF->trackParticle()->eta());
                float resEF = 100.0*(offET - onlET)/offET;
                cd(m_dir+"/"+probeTrigger+"/L2Resolutions");
                hist1("EtRes")->Fill(resEF);
                hist2("EtRes_eta")->Fill(offEl->eta(),resEF);
                hist2("EtRes_et")->Fill(offET/1e3,resEF);
                ATH_MSG_DEBUG("Execute NavZeeTPRes:: resEF, eta, et " << resEF << " " << offEl->eta() << " " << offET); 
            }
            if ( (offEl!=NULL) && (selL2!=NULL) ) {
                float offET = offEl->e()/TMath::CosH(offEl->trackParticle()->eta());
                float onlET = selL2->pt();
                float resL2 = 100.0*(offET - onlET)/offET;

                cd(m_dir+"/"+probeTrigger+"/L2Resolutions");
                hist1("EtRes")->Fill(resL2);
                hist2("EtRes_eta")->Fill(offEl->eta(),resL2);
                hist2("EtRes_et")->Fill(offET/1e3,resL2);
            }
        }
        clearProbeList();
    } // End loop over trigger list


    return StatusCode::SUCCESS;

}

StatusCode TrigEgammaNavZeeTPRes::childFinalize()
{

return StatusCode::SUCCESS;
}

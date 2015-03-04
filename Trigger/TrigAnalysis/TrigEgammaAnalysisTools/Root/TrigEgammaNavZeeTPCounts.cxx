/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/


/**********************************************************************
 * AsgTool: TrigEgammaNavZeeTPCounts
 * Authors:
 *      Ryan Mackenzie White <ryan.white@cern.ch>
 *      Denis Damazio <denis.damazio@cern.ch>
 * Date: Feb 2015
 * Description:
 *      Derived class for counting, computing average trigger efficiency
 **********************************************************************/
#include "TrigEgammaAnalysisTools/TrigEgammaNavZeeTPCounts.h"

//**********************************************************************

TrigEgammaNavZeeTPCounts::
TrigEgammaNavZeeTPCounts( const std::string& myname )
: TrigEgammaNavZeeTPBaseTool(myname) {
  declareProperty("DirectoryPath",m_dir="");
}

StatusCode TrigEgammaNavZeeTPCounts::childInitialize(){

    m_eventCounter=0;
    std::cout << "child Initialize" << std::endl;
    for(unsigned int ilist = 0; ilist != m_probeTrigList.size(); ilist++) {
        std::string probeTrigger = m_probeTrigList.at(ilist);
        m_nProbes[probeTrigger]=0;
        m_nProbesPassedL2[probeTrigger]=0;
        m_nProbesPassedEF[probeTrigger]=0;
        m_nProbesPassedL2Calo[probeTrigger]=0;
        m_nProbesPassedEFCalo[probeTrigger]=0;
    }
    return StatusCode::SUCCESS;
}

StatusCode TrigEgammaNavZeeTPCounts::childExecute()
{

    ATH_MSG_DEBUG("Executing NavZeeCounts");
    m_eventCounter++;
    //return StatusCode::SUCCESS;

    // Event Wise Selection (independent of the required signatures)
    if ( !TrigEgammaNavZeeTPBaseTool::EventWiseSelection() ) return StatusCode::SUCCESS;
    for(unsigned int ilist = 0; ilist != m_probeTrigList.size(); ilist++) {
        std::string probeTrigger = m_probeTrigList.at(ilist);

        if ( executeTandP(probeTrigger).isFailure() )
            return StatusCode::FAILURE;

        for(unsigned int i=0;i<m_probeElectrons.size();i++){
            if ( m_probeElectrons[i].first->pt() < 24e3 ) continue;
            const HLT::TriggerElement* feat = m_probeElectrons[i].second;
            m_nProbes[probeTrigger]++;
            if ( feat ) {

                const xAOD::Electron* offEl = m_probeElectrons[i].first;
                ATH_MSG_DEBUG("Off El : " << offEl->pt());
                ATH_MSG_DEBUG("; eta : " << offEl->eta());
                ATH_MSG_DEBUG("; phi : " << offEl->phi());

                bool L2Cok=false;
                bool L2ok=false;
                bool EFCok=false;
                bool EFok=false;


                bool passedL2Calo = ancestorPassed<xAOD::TrigEMCluster>(feat); // L2 check
                const auto* l2ElCalo = getFeature<xAOD::TrigEMCluster>(feat);
                if ( l2ElCalo != NULL){
                    ATH_MSG_DEBUG("L2 El : " << l2ElCalo->et());
                    ATH_MSG_DEBUG("; eta : " << l2ElCalo->eta());
                    ATH_MSG_DEBUG("; phi : " << l2ElCalo->phi());
                    if ( passedL2Calo ){
                        L2Cok=true;
                        ATH_MSG_DEBUG(" passed");
                    }
                    else
                        ATH_MSG_DEBUG(" not passed");
                    std::cout << std::endl;
                }
                if (L2Cok) m_nProbesPassedL2Calo[probeTrigger]++;
                bool passedL2 = ancestorPassed<xAOD::TrigElectronContainer>(feat); // L2 check
                const auto* l2El = getFeature<xAOD::TrigElectronContainer>(feat);
                if ( l2El != NULL)
                    for(unsigned int j=0;j<l2El->size();j++){
                        ATH_MSG_DEBUG("L2 El : " << l2El->at(j)->pt());
                        ATH_MSG_DEBUG("; eta : " << l2El->at(j)->eta());
                        ATH_MSG_DEBUG("; phi : " << l2El->at(j)->phi());
                        if ( passedL2 ){
                            L2ok=true;
                            ATH_MSG_DEBUG(" passed");
                        }
                        else
                            ATH_MSG_DEBUG(" not passed");
                        std::cout << std::endl;
                    }
                if (L2ok) m_nProbesPassedL2[probeTrigger]++;
                   

                bool passedEFCalo = ancestorPassed<xAOD::CaloClusterContainer>(feat); // EF check
                const auto* EFElCalo = getFeature<xAOD::CaloClusterContainer>(feat);
                if ( EFElCalo != NULL)
                    for(unsigned int j=0;j<EFElCalo->size();j++){
                        ATH_MSG_DEBUG("EF El : " << EFElCalo->at(j)->et());
                        ATH_MSG_DEBUG("; eta : " << EFElCalo->at(j)->eta());
                        ATH_MSG_DEBUG("; phi : " << EFElCalo->at(j)->phi());
                        if ( passedEFCalo ){
                            EFCok=true;
                            ATH_MSG_DEBUG(" passed");
                        }
                        else
                            ATH_MSG_DEBUG(" not passed");
                    }
                if (EFCok) m_nProbesPassedEFCalo[probeTrigger]++;
                bool passedEF = ancestorPassed<xAOD::ElectronContainer>(feat); // EF check
                const auto* EFEl = getFeature<xAOD::ElectronContainer>(feat);
                if ( EFEl != NULL)
                    for(unsigned int j=0;j<EFEl->size();j++){
                        ATH_MSG_DEBUG("EF El : " << EFEl->at(j)->pt());
                        ATH_MSG_DEBUG("; eta : " << EFEl->at(j)->eta());
                        ATH_MSG_DEBUG("; phi : " << EFEl->at(j)->phi());
                        if ( passedEF ){
                            EFok=true;
                            ATH_MSG_DEBUG(" passed");
                        }
                        else
                            ATH_MSG_DEBUG(" not passed");
                    }
                if (EFok) m_nProbesPassedEF[probeTrigger]++;
            }
        }
        clearProbeList();
    }
    return StatusCode::SUCCESS;
}

StatusCode TrigEgammaNavZeeTPCounts::childFinalize()
{

    ATH_MSG_INFO("Final Number of Events executed : \t\t" << m_eventCounter);

    std::ofstream countsFile ("TPEfficiencies.txt");
    if(countsFile.is_open()){
        countsFile << "------------------------------ Estimated TP Counts and Efficiencies ------------------------------\n";
        for(unsigned int i = 0; i != m_probeTrigList.size(); i++) {
            std::string probeTrigger = m_probeTrigList.at(i);
            m_EffL2[probeTrigger] = 100.0*((float)m_nProbesPassedL2[probeTrigger]/(float)m_nProbes[probeTrigger]);
            m_EffHLT[probeTrigger] = 100.0*((float)m_nProbesPassedEF[probeTrigger]/(float)m_nProbes[probeTrigger]);
            m_EffL2Calo[probeTrigger] = 100.0*((float)m_nProbesPassedL2Calo[probeTrigger]/(float)m_nProbes[probeTrigger]);
            m_EffEFCalo[probeTrigger] = 100.0*((float)m_nProbesPassedEFCalo[probeTrigger]/(float)m_nProbes[probeTrigger]);
            countsFile << std::setw(50) << probeTrigger << " N Probes " << m_nProbes[probeTrigger] << " N HLT " <<
                std::setw(6) << m_nProbesPassedEF[probeTrigger] << " Eff " << std::setprecision(4) << m_EffHLT[probeTrigger] << " N EFCalo " <<
                std::setw(6) << m_nProbesPassedEFCalo[probeTrigger] << " Eff EFCalo " << std::setprecision(4) <<  m_EffEFCalo[probeTrigger] << " N L2 " <<
                std::setw(6) << m_nProbesPassedL2[probeTrigger] << " Eff L2 " << std::setprecision(4) <<  m_EffL2[probeTrigger] << " N L2Calo " <<
                std::setw(6) << m_nProbesPassedL2Calo[probeTrigger] << " Eff L2Calo " << std::setprecision(4) <<  m_EffL2Calo[probeTrigger] << " -----\n";
        }
    }
    countsFile.close();
    return StatusCode::SUCCESS;
}

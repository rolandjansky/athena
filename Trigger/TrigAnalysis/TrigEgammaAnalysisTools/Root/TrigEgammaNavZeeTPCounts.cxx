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
        m_nProbesL2[probeTrigger]=0;
        m_nProbesPassedL2[probeTrigger]=0;
        m_nProbesEF[probeTrigger]=0;
        m_nProbesPassedEF[probeTrigger]=0;
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
            m_nProbesL2[probeTrigger]++;
            m_nProbesEF[probeTrigger]++;
            if ( feat ) {

                const xAOD::Electron* offEl = m_probeElectrons[i].first;
                ATH_MSG_DEBUG("Off El : " << offEl->pt());
                ATH_MSG_DEBUG("; eta : " << offEl->eta());
                ATH_MSG_DEBUG("; phi : " << offEl->phi());

                //	bool L2cok=false;
                bool L2ok=false;
                bool EFok=false;

                /*
                 * bool passedL2c = ancestorPassed<xAOD::TrigEMCluster>(feat); // L2 check
                const xAOD::TrigEMCluster* l2cEl = getFeature<xAOD::TrigEMCluster>(feat);
                if ( l2cEl != NULL ) std::cout << "L2 EMCluster El : " << l2cEl->et() << " " << l2cEl->eta() << " " << l2cEl->phi() << std::endl;
                */

                bool passedL2 = ancestorPassed<xAOD::TrigElectronContainer>(feat); // L2 check
                const xAOD::TrigElectronContainer* l2El = getFeature<xAOD::TrigElectronContainer>(feat);
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
                   

                bool passedEF = ancestorPassed<xAOD::ElectronContainer>(feat); // EF check
                const xAOD::ElectronContainer* EFEl = getFeature<xAOD::ElectronContainer>(feat);
                if ( EFEl != NULL)
                    for(unsigned int j=0;j<EFEl->size();j++){
                        ATH_MSG_DEBUG("EF El : " << EFEl->at(j)->pt());
                        ATH_MSG_DEBUG("; eta : " << EFEl->at(j)->eta());
                        ATH_MSG_DEBUG("; phi : " << EFEl->at(j)->phi());
                        if ( passedEF ){
                            EFok=true;
                            ATH_MSG_DEBUG(" passed");
                            //m_nProbesPassed[1]++;
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

    for(unsigned int i = 0; i != m_probeTrigList.size(); i++) {
        std::string probeTrigger = m_probeTrigList.at(i);
        ATH_MSG_INFO("Checking average efficiency " << probeTrigger);
        ATH_MSG_INFO("HLT Probes " << m_nProbesEF[probeTrigger] << " HLT matches " << 
                m_nProbesPassedEF[probeTrigger] << " L2 matches " << m_nProbesPassedL2[probeTrigger]);
        ATH_MSG_INFO("EC : " << m_eventCounter << "; Prob : "
                << 100.0*((float)m_nProbesPassedL2[probeTrigger]/(float)m_nProbesL2[probeTrigger]) );
        ATH_MSG_INFO("EC : " << m_eventCounter << "; Prob : "
                << 100.0*((float)m_nProbesPassedEF[probeTrigger]/(float)m_nProbesEF[probeTrigger]) );

    }
    return StatusCode::SUCCESS;
}

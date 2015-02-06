/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**********************************************************************
 * AsgTool: TrigEgammaNavZeeTPPerf
 * Authors:
 *      Ryan Mackenzie White <ryan.white@cern.ch>
 *      Denis Damazio <denis.damazio@cern.ch>
 * Date: Feb 2015
 * Description:
 *      Derived class for studying performance triggers (e.g e0_perf)
 *      Can be used to emulate trigger selection with triggered objects.
 *      Or to study performance of new calibrations, selections, etc.
 **********************************************************************/
#include "TrigEgammaAnalysisTools/TrigEgammaNavZeeTPPerf.h"

//**********************************************************************

TrigEgammaNavZeeTPPerf::
TrigEgammaNavZeeTPPerf( const std::string& myname )
: TrigEgammaNavZeeTPBaseTool(myname) {
  declareProperty("DirectoryPath",m_dir="NavZeeTPPerf");
}

//**********************************************************************
StatusCode TrigEgammaNavZeeTPPerf::childInitialize(){

m_eventCounter=0;
m_nProbes[0]=0;
m_nProbesPassed[0]=0;
m_nProbes[1]=0;
m_nProbesPassed[1]=0;
m_nProbes[2]=0;
m_nProbesPassed[2]=0;
std::cout << "child Initialize" << std::endl;

return StatusCode::SUCCESS;
}

StatusCode TrigEgammaNavZeeTPPerf::childExecute()
{

    ATH_MSG_DEBUG("Executing NavZeePerf");
    m_eventCounter++;
    //return StatusCode::SUCCESS;

    // Event Wise Selection (independent of the required signatures)
    if ( !TrigEgammaNavZeeTPBaseTool::EventWiseSelection() ) return StatusCode::SUCCESS;

    for(unsigned int ilist = 0; ilist != m_probeTrigList.size(); ilist++) {
        std::string probeTrigger = m_probeTrigList.at(ilist);

        if ( executeTandP(probeTrigger).isFailure() )   
            return StatusCode::FAILURE;

        // Just for counting
        //std::cout << "number of probes to check : " << probeElectrons.size() << std::endl;
        for(unsigned int i=0;i<m_probeElectrons.size();i++){
            if ( m_probeElectrons[i].first->pt() < 24e3 ) continue;
            const HLT::TriggerElement* feat = m_probeElectrons[i].second;
            m_nProbes[0]++;
            m_nProbes[1]++;
            m_nProbes[2]++;
            if ( feat ) {

                const xAOD::Electron* offEl = m_probeElectrons[i].first;
                ATH_MSG_DEBUG("Off El : " << offEl->pt());
                ATH_MSG_DEBUG("; eta : " << offEl->eta());
                ATH_MSG_DEBUG("; phi : " << offEl->phi());

                //bool L2cok=false;
                bool L2ok=false;
                bool EFok=false;

                bool passedL2c = ancestorPassed<xAOD::TrigEMCluster>(feat); // L2 check
                const xAOD::TrigEMCluster* l2cEl = getFeature<xAOD::TrigEMCluster>(feat);
                if ( l2cEl != NULL ) std::cout << "L2 EMCluster El : " << l2cEl->et() << " " << l2cEl->eta() << " " << l2cEl->phi() << std::endl;
                if (passedL2c) m_nProbesPassed[2]++;

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
                if (L2ok) m_nProbesPassed[0]++;

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
                if (EFok) m_nProbesPassed[1]++;
            }
        }
        clearProbeList();
    }



    return StatusCode::SUCCESS;

}

StatusCode TrigEgammaNavZeeTPPerf::childFinalize()
{

    ATH_MSG_DEBUG(m_nProbesPassed[1] << " " <<  m_nProbes[1] );;

    ATH_MSG_DEBUG("EC : " << m_eventCounter << "; Prob : "
            << 100.0*((float)m_nProbesPassed[0]/(float)m_nProbes[0]) );
    ATH_MSG_DEBUG("EC : " << m_eventCounter << "; Prob : "
            << 100.0*((float)m_nProbesPassed[1]/(float)m_nProbes[1]) );
    ATH_MSG_DEBUG("EC : " << m_eventCounter << "; Prob : "
            << 100.0*((float)m_nProbesPassed[2]/(float)m_nProbes[2]) );
    ATH_MSG_INFO("Final Number of Events executed : \t\t" << m_eventCounter);

    return StatusCode::SUCCESS;
}

/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#include "TrigEgammaAnalysisTools/TrigEgammaNavAnalysisTool.h"

using namespace std;

TrigEgammaNavAnalysisTool::TrigEgammaNavAnalysisTool( const std::string& myname ): TrigEgammaNavBaseTool(myname)
{
}


StatusCode TrigEgammaNavAnalysisTool::childInitialize()
{
    ATH_MSG_INFO("TrigEgammaNavAnalysisTool: child Initialize");
    TrigEgammaNavBaseTool::childInitialize();
   
    ATH_MSG_DEBUG("Now configuring chains for analysis");

    for(const auto trigName:m_trigInputList){
        m_trigList.push_back(trigName);
        setTrigInfo(trigName);
    }

    return StatusCode::SUCCESS;
}







StatusCode TrigEgammaNavAnalysisTool::childExecute( const EventContext& ctx ) const 
{

    ATH_MSG_DEBUG("TrigEgammaNavAnalysisTool: child execute");
    
    //auto monGroup = findGroup(m_dir+"/Expert/Event");

    // Check HLTResult
    if(tdt()->ExperimentalAndExpertMethods()->isHLTTruncated()){
        ATH_MSG_WARNING("HLTResult truncated, skip trigger analysis");
        return StatusCode::SUCCESS;
    }



    ATH_MSG_DEBUG("Chains for Analysis " << m_trigList);

    int ilist=0;
    for(const auto trigger : m_trigList){
        ATH_MSG_DEBUG("Start Chain Analysis ============================= " << trigger << " " << getTrigInfo(trigger).trigName);
        
        // Trigger counts
        //if(tdt()->isPassed(trigger)) 
        //   fill( monGroup, m_anatype+"_trigger_counts", m_trigList.at(ilist) );
        

        const TrigInfo info = getTrigInfo(trigger);
          
        std::vector< std::pair<const xAOD::Egamma*, const TrigEgammaMatchingUtils::Element>> pairObjs;
        
        if ( TrigEgammaNavBaseTool::executeNavigation(ctx, info, pairObjs).isFailure() ){
            ATH_MSG_WARNING("executeNavigation Fails");
            return StatusCode::SUCCESS;
        }

        // Fill distributions / trigger
        for( auto& tool : m_tools) {
            // Set detail level from analysis tool each time
            ATH_MSG_DEBUG("TE Tool... name="<<tool<<name());
            if(tool->toolExecute(ctx, m_dir+"/Expert",info, pairObjs).isFailure()) {
                ATH_MSG_DEBUG("TE Tool Fails");// Requires offline match
            }
            ATH_MSG_DEBUG("TE Tool executed successfully name="<<tool<<name());
        }
        ilist++;
        ATH_MSG_DEBUG("End Chain Analysis ============================= " << trigger);
    } // End loop over trigger list
    return StatusCode::SUCCESS;
}


StatusCode TrigEgammaNavAnalysisTool::childFinalize(){

    ATH_MSG_DEBUG("TrigEgammaNavAnalysisTool: child Finalize");
    TrigEgammaNavBaseTool::childFinalize();
    return StatusCode::SUCCESS;
}




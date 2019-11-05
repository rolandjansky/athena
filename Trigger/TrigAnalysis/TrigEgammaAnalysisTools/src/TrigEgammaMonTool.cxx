/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**********************************************************************
 * IHLTMonTool: TrigEgammaMonTool
 * Author:
 *      Ryan Mackenzie White <ryan.white@cern.ch>
 * Date: May 2015
 * Description:
 *      Generic HLTMonBase tool class for trigger egamma analysis. 
 *      Inherits from IHLTMonTool and provides the wrapper 
 *      for the base analysis class to run at T0.
 *      All analysis classes derive the original base class
 **********************************************************************/
#include "TrigEgammaMonTool.h"
#include "AthenaMonitoring/AthenaMonManager.h"
#include "AthenaMonitoring/ManagedMonitorToolTest.h"
#include "TrigEgammaAnalysisTools/ITrigEgammaAnalysisBaseTool.h"
#include "AsgTools/AsgTool.h"

TrigEgammaMonTool::
TrigEgammaMonTool( const std::string & type, const std::string & name, const IInterface* parent)
    :IHLTMonTool(type, name, parent)
{
    declareProperty( "Tools", m_asgToolNames);
}

TrigEgammaMonTool::~TrigEgammaMonTool(){}


StatusCode TrigEgammaMonTool::init(){   
  return StatusCode::SUCCESS; 
}

StatusCode TrigEgammaMonTool::book(){
    ATH_MSG_INFO("Initializing " << name() << "...");
    ATH_MSG_INFO("Retrieving tools..." << m_asgToolNames.size());

    for(const auto toolname : m_asgToolNames){
        ATH_MSG_INFO("Add "<<toolname<<"..." );
        m_asgtools.push_back(ToolHandle<ITrigEgammaAnalysisBaseTool>(toolname));
    }
    for ( ToolHandleArray<ITrigEgammaAnalysisBaseTool>::const_iterator itoo=m_asgtools.begin();
            itoo!=m_asgtools.end(); ++itoo ) {
        ATH_MSG_INFO("Initializing..." );
        ToolHandle<ITrigEgammaAnalysisBaseTool> htool = *itoo;
        htool->setParent(this);
        StatusCode sc = htool->initialize();
        if ( ! sc.isSuccess() ) {
            ATH_MSG_ERROR("Tool retrieval failed:" << htool);
            return StatusCode::FAILURE;
        }
        sc = htool->book();
        if ( ! sc.isSuccess() ) {
            ATH_MSG_ERROR("Tool book failed:" << htool);
            return StatusCode::FAILURE;
        }
    }
    
    ATH_MSG_INFO("Tool retrieval completed.");
    return StatusCode::SUCCESS;
}

StatusCode TrigEgammaMonTool::fill(){
    ATH_MSG_DEBUG ("Executing " << name() << "...");
    // Loop over hello tools.
    ATH_MSG_DEBUG("Looping over ASG tools.");
    std::string line = "---------------------------------------------------";
    ATH_MSG_DEBUG(line);
    for ( ToolHandleArray<ITrigEgammaAnalysisBaseTool>::const_iterator itoo=m_asgtools.begin();
            itoo!=m_asgtools.end(); ++itoo ) {
        ToolHandle<ITrigEgammaAnalysisBaseTool> htool = *itoo;
        StatusCode sc = htool->execute();
        if ( ! sc.isSuccess() ) {
            ATH_MSG_ERROR("Tool execute failed.");
            return StatusCode::FAILURE;
        }
        ATH_MSG_DEBUG(line);
    }
    return StatusCode::SUCCESS;
}

StatusCode TrigEgammaMonTool::proc(){
    // Finalize the efficiency for end of run  
    if(!endOfRunFlag()){ return(StatusCode::SUCCESS); }
    ATH_MSG_INFO ("Finalizing " << name() << "...");
    for ( ToolHandleArray<ITrigEgammaAnalysisBaseTool>::const_iterator itoo=m_asgtools.begin();
            itoo!=m_asgtools.end(); ++itoo ) {
        ToolHandle<ITrigEgammaAnalysisBaseTool> htool = *itoo;
        StatusCode sc = htool->finalize();
        if ( ! sc.isSuccess() ) {
            ATH_MSG_ERROR("Tool finalize failed.");
            return StatusCode::FAILURE;
        }
    }
    return StatusCode::SUCCESS;
}

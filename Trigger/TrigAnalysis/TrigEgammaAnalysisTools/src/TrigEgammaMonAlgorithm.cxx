/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#include "TrigEgammaMonAlgorithm.h"
#include "TrigEgammaAnalysisTools/ITrigEgammaAnalysisBaseTool.h"



TrigEgammaMonAlgorithm::TrigEgammaMonAlgorithm( const std::string& name, ISvcLocator* pSvcLocator )
  : AthMonitorAlgorithm(name,pSvcLocator)
{
  declareProperty( "Tools", m_asgToolNames);
}


TrigEgammaMonAlgorithm::~TrigEgammaMonAlgorithm() {}


StatusCode TrigEgammaMonAlgorithm::initialize() {
    
    if(AthMonitorAlgorithm::initialize().isFailure()){
      return StatusCode::FAILURE;
    }

    ATH_MSG_INFO( "GM tools size is "<< m_tools.size() );

    for(const auto toolname : m_asgToolNames){
      ATH_MSG_INFO("Add "<<toolname<<"..." );
      m_asgtools.push_back(ToolHandle<ITrigEgammaAnalysisBaseTool>(toolname));
    }



    m_asgtools.retrieve();


    for ( ToolHandleArray<ITrigEgammaAnalysisBaseTool>::const_iterator itoo=m_asgtools.begin(); itoo!=m_asgtools.end(); ++itoo ) 
    {
        ATH_MSG_INFO("Initializing..." );
        ToolHandle<ITrigEgammaAnalysisBaseTool> htool = *itoo;
        // Link all generic monitoring tools holded by the ath monitoring to
        // the analysis tool used to fill all histograms
        ATH_MSG_INFO("Setting GM tools..." );
        htool->setGMTools( m_tools );        
        
        ATH_MSG_INFO("Initializing..." );
        // Initialize all tools and subtools inside og the analysis
        StatusCode sc = htool->initialize();
        if ( ! sc.isSuccess() ) {
            ATH_MSG_ERROR("Tool retrieval failed:" << htool);
            return StatusCode::FAILURE;
        }

    }
    
    ATH_MSG_INFO("Tool retrieval completed.");
    return StatusCode::SUCCESS;


}


StatusCode TrigEgammaMonAlgorithm::fillHistograms( const EventContext& ctx ) const {

    ATH_MSG_INFO("Executing " << name() << "...");
    ATH_MSG_INFO("Looping over ASG tools.");
    std::string line = "---------------------------------------------------";
    ATH_MSG_INFO(line);
    for ( ToolHandleArray<ITrigEgammaAnalysisBaseTool>::const_iterator itoo=m_asgtools.begin(); itoo!=m_asgtools.end(); ++itoo ) {
        ToolHandle<ITrigEgammaAnalysisBaseTool> htool = *itoo;

        // Run the analysis tool
        StatusCode sc = htool->execute( ctx );
        if ( ! sc.isSuccess() ) {
            ATH_MSG_ERROR("Tool execute failed.");
            return StatusCode::FAILURE;
        }
        ATH_MSG_DEBUG(line);
    }


    return StatusCode::SUCCESS;
}


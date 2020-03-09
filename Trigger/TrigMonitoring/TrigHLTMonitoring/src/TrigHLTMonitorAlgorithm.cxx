/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#include "TrigHLTMonitorAlgorithm.h"

TrigHLTMonitorAlgorithm::TrigHLTMonitorAlgorithm( const std::string& name, ISvcLocator* pSvcLocator )
  : AthMonitorAlgorithm(name,pSvcLocator)
{
}


TrigHLTMonitorAlgorithm::~TrigHLTMonitorAlgorithm() {}


StatusCode TrigHLTMonitorAlgorithm::initialize() {

  ATH_CHECK( m_trigDecTool.retrieve() );
  ATH_CHECK( m_hltResultReadKey.initialize() );

  return AthMonitorAlgorithm::initialize();
}


StatusCode TrigHLTMonitorAlgorithm::fillHistograms( const EventContext& ctx ) const {
    using namespace Monitored;

    // Declare the quantities which should be monitored
    //NB! The variables and histograms defined here must match the ones in the py file exactly!
    auto placeholder = Monitored::Scalar<int>("placeholder",0);
    auto HLTResultHLT = Monitored::Scalar<int>("HLTResultHLT",0);

    SG::ReadHandle<HLT::HLTResultMT> resultHandle = SG::makeHandle( m_hltResultReadKey, ctx );

    //// Set the values of the monitored variables for the event		
    placeholder = GetEventInfo(ctx)->runNumber(); //DUMMY; to be updated.
    
    HLTResultHLT = 1; //DUMMY; to be updated.

    // Fill. First argument is the tool (GMT) name as defined in the py file, 
    // all others are the variables to be saved.
    // Alternative fill method. Get the group yourself, and pass it to the fill function.
    auto tool = getGroup("TrigHLTMonitor");
    fill(tool,placeholder);
    fill(tool,HLTResultHLT);

    return StatusCode::SUCCESS;
}

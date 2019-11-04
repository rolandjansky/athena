/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#include "TrigEgammaMonitorAlgorithm.h"
#include "TRandom3.h"

TrigEgammaMonitorAlgorithm::TrigEgammaMonitorAlgorithm( const std::string& name, ISvcLocator* pSvcLocator )
  : AthMonitorAlgorithm(name,pSvcLocator)
{}


TrigEgammaMonitorAlgorithm::~TrigEgammaMonitorAlgorithm() {}


StatusCode TrigEgammaMonitorAlgorithm::initialize() {
    return AthMonitorAlgorithm::initialize();
}


StatusCode TrigEgammaMonitorAlgorithm::fillHistograms( const EventContext& ctx ) const {
    using namespace Monitored;

    // Declare the quantities which should be monitored
    //NB! The variables and histograms defined here must match the ones in the py file exactly!
    //auto lumiPerBCID = Monitored::Scalar<float>("lumiPerBCID",0.0);
    auto lb = Monitored::Scalar<int>("lb",0);
    auto run = Monitored::Scalar<int>("run",0);

    // Two variables (value and passed) needed for TEfficiency
    //auto pT = Monitored::Scalar<float>("pT",0.0);
    //auto pT_passed = Monitored::Scalar<float>("pT_passed",false);

    auto res_etInEta0 = Monitored::Scalar<float>("res_etInEta0",0.0);

    auto phi = Monitored::Scalar<float>("phi",0.0);

    //Access a hard-coded trigger chain
    //(this should probably be moved elsewhere and done in a more automatic way in the future)
    //m_trigItems.push_back("HLT_e24_lhtight_nod0");
    //m_trigItems.push_back("HLT_g140_tight");

    //ATH_MSG_DEBUG("Checking configured HLT Egamma chains");
    //for(unsigned int i=0; i<m_trigItems.size(); i++){
    //  std::string chain = "HLT_"+m_trigItems.at(i);
    //  std::vector<std::string> configured_chain = getTrigDecisionTool()->getListOfTriggers(chain);
    //  if(configured_chain.size()==0){
    //  ATH_MSG_INFO("TrigEgamma  Chain " << chain << " not configured, skipping");
    //  m_trigItems.erase(m_trigItems.begin()+i);
    //  i--;
    //}
    //if(configured_chain.size()){
    //  ATH_MSG_DEBUG("TrigEgamma  Chain " << m_trigItems.at(i) << " configured!");
    //}
    //    }
    //for(unsigned int i=0;i<m_trigItems.size(); ++i)
    //addMonGroup(new MonGroup(this,"HLT/TauMon/Expert/"+m_trigItems[i],run));

    //// Set the values of the monitored variables for the event
    //lumiPerBCID = lbAverageInteractionsPerCrossing(ctx);
    lb = GetEventInfo(ctx)->lumiBlock();
    run = GetEventInfo(ctx)->runNumber();

    TRandom3 r(ctx.eventID().event_number());

    // Fake efficiency calculator
    //pT = r.Landau(15);
    //pT_passed = pT>r.Poisson(15);

    res_etInEta0 = (r.Rndm()-0.5)/5;

    phi = (r.Rndm()-0.5)*2*3.1415;

    // Fill. First argument is the tool (GMT) name as defined in the py file,
    // all others are the variables to be saved.
    //fill("TrigEgammaMonitor",lumiPerBCID,lb);
    fill("TrigEgammaMonitor",lb);

    // Alternative fill method. Get the group yourself, and pass it to the fill function.
    auto tool = getGroup("TrigEgammaMonitor");
    fill(tool,run);

    //fill("TrigEgammaMonitor_AbsResolutions_HLT", res_etInEta0);

    fill("TrigEgammaMonitor_Distributions_L1Calo", phi);

    return StatusCode::SUCCESS;
}

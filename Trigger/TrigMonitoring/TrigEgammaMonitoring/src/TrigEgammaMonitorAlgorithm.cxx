/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#include "TrigEgammaMonitorAlgorithm.h"
#include "TRandom3.h"

#include "TrigEgammaAnalysisTools/ITrigEgammaAnalysisBaseTool.h"

TrigEgammaMonitorAlgorithm::TrigEgammaMonitorAlgorithm( const std::string& name, ISvcLocator* pSvcLocator )
  : base_class(name, pSvcLocator)
{
  declareProperty("IsChainSpecific", m_isChainSpecific=true);
}


TrigEgammaMonitorAlgorithm::~TrigEgammaMonitorAlgorithm() {}


StatusCode TrigEgammaMonitorAlgorithm::initialize() {
    ATH_MSG_INFO("Initializing " << name() << "...");

    StatusCode sc = AthMonitorAlgorithm::initialize();
    if ( ! sc.isSuccess() ) {
        ATH_MSG_ERROR("Error initialize parent");
        return StatusCode::FAILURE;
    }

    ATH_MSG_INFO("Retrieving tools... " << m_tools.size());

    for ( const auto htool : m_tools ) {
        ATH_MSG_INFO("Retrieving "<< htool <<"..." );
        StatusCode sc = htool.retrieve();
        if ( ! sc.isSuccess() ) {
            ATH_MSG_ERROR("Tool retrieval failed: " << htool);
            return StatusCode::FAILURE;
        }
        ATH_MSG_INFO("...//" );
    }

    for ( auto htool : m_tools ) {
        ATH_MSG_INFO("Initializing "<< htool <<"..." );
        htool->setMGLookup(this);
        ATH_MSG_INFO("...//" );
        ATH_MSG_INFO("Booking "<< htool <<"..." );
        sc = htool->book();
        if ( ! sc.isSuccess() ) {
            ATH_MSG_ERROR("Tool book failed:" << htool);
            return StatusCode::FAILURE;
        }
        ATH_MSG_INFO("...//" );
    }

    ATH_MSG_INFO("Tools retrieval completed.");
    return StatusCode::SUCCESS;
}


StatusCode TrigEgammaMonitorAlgorithm::fillHistograms( const EventContext& ctx ) const {
    //using namespace Monitored;

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

    auto r_x = Monitored::Scalar<float>("r#x",0.0);
    auto r_y = Monitored::Scalar<float>("r#y",0.0);
    auto t_x = Monitored::Scalar<float>("t#x",0.0);
    auto t_y = Monitored::Scalar<float>("t#y",0.0);

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

    // Alternative fill method. Get the group yourself, and pass it to the fill function.
    auto tool = getGroup("TrigEgammaMonitor");
    float aphi = 0.0;
    unsigned int codepoints[] = {0x0308BD20, 0x04888520, 0x04889DE0, 0x04888520, 0x033BBD20, 0x00000000, 0x181E9324,
        0x240294A4, 0x240E94BC, 0x3C0267A4, 0x241E64A4, 0x00000000, 0xA639EEE9, 0xA950224B, 0x4950E24F, 0x4F50224D, 0x4939EEE9, 0x00000000};
    for( int i=0; i<100; i++) {
        aphi = (r.Rndm()-0.5)*2*3.1415;

        const float radius = r.Gaus(0, 0.2)+(cos(7*aphi)+3);
        const float f_x = r.Uniform(-5, 5);
        const float f_y = r.Uniform(-5, 5);

        r_x = radius*cos(aphi);
        r_y = radius*sin(aphi);
        ATH_MSG_DEBUG("Test fill r#x, r#y: " << i);

        fill(tool, r_x, r_y);

        if ((codepoints[int(1.6*(5-f_y)+0.5)] >> int((f_x+5)*3.1+0.5))&0x1) {
            t_x = f_x*0.9;
            t_y = f_y*0.9;
            fill(tool, t_x, t_y);
        }
    }

    // Fill. First argument is the tool (GMT) name as defined in the py file,
    // all others are the variables to be saved.
    //fill("TrigEgammaMonitor",lumiPerBCID,lb);
    fill("TrigEgammaMonitor", lb, run);


    if (m_isChainSpecific) {
        //fill("TrigEgammaMonitor_AbsResolutions_HLT", res_etInEta0);

        phi = aphi;

        fill("Expert_Distributions_L1Calo", phi);

        phi = aphi*aphi/3.1415;

        fill("Expert_Distributions_HLT", phi);
    }

    ATH_MSG_DEBUG ("Executing " << name() << "...");
    // Loop over monitoring tools
    ATH_MSG_DEBUG("Looping over monitoring tools.");
    const std::string line = "---------------------------------------------------";
    ATH_MSG_DEBUG(line);
    for ( auto htool : m_tools ) {
        StatusCode sc = htool->execute();
        if ( ! sc.isSuccess() ) {
            ATH_MSG_ERROR("Tool execute failed: " << htool);
            return StatusCode::FAILURE;
        }
        ATH_MSG_DEBUG(line);
    }

    return StatusCode::SUCCESS;
}

ToolHandle<GenericMonitoringTool> TrigEgammaMonitorAlgorithm::findGroup( const std::string& name ) const {
    return getGroup(name);
}

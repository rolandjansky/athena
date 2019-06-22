/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#include "TrigBjetMonitorAlgorithm.h"

TrigBjetMonitorAlgorithm::TrigBjetMonitorAlgorithm( const std::string& name, ISvcLocator* pSvcLocator )
  : AthMonitorAlgorithm(name,pSvcLocator)
  ,m_doRandom(true)
  ,m_expert{},m_shifter{}
{
  declareProperty ("expert", m_expert);
  declareProperty ("shifter", m_shifter);
}


TrigBjetMonitorAlgorithm::~TrigBjetMonitorAlgorithm() {}


StatusCode TrigBjetMonitorAlgorithm::initialize() {
  return AthMonitorAlgorithm::initialize();
}


StatusCode TrigBjetMonitorAlgorithm::fillHistograms( const EventContext& ctx ) const {
  using namespace Monitored;

  // Declare the quantities which should be monitored
  //NB! The variables and histograms defined here must match the ones in the py file exactly!
  /* Elin's example
    auto lumiPerBCID = Monitored::Scalar<float>("lumiPerBCID",0.0);
    auto lb = Monitored::Scalar<int>("lb",0);
    auto run = Monitored::Scalar<int>("run",0);

    // Two variables (value and passed) needed for TEfficiency
    auto pT = Monitored::Scalar<float>("pT",0.0);
    auto pT_passed = Monitored::Scalar<float>("pT_passed",false);

    //// Set the values of the monitored variables for the event		
    lumiPerBCID = lbAverageInteractionsPerCrossing();
    lb = GetEventInfo(ctx)->lumiBlock();
    run = GetEventInfo(ctx)->runNumber();
    
    // Fill. First argument is the tool (GMT) name as defined in the py file, 
    // all others are the variables to be saved.
    fill("TrigBjetMonitor",lumiPerBCID,lb);

    // Alternative fill method. Get the group yourself, and pass it to the fill function.
    auto tool = getGroup("TrigBjetMonitor");
    fill(tool,run);
  */

  /* print the trigger chain names */

  std::string chainName;
  int size_expert = m_expert.size();
  ATH_MSG_INFO(" Size of the expert trigger container: " << size_expert );
  for (int i =0; i<size_expert; i++){
    chainName = m_expert.at(i);
    ATH_MSG_INFO("  Chain number: " << i << " Expert Chain Name: " << chainName );
  }
  int size_shifter = m_shifter.size();
  ATH_MSG_INFO(" Size of the shifter trigger container: " << size_shifter );
  for (int i =0; i<size_shifter; i++){
    chainName = m_shifter.at(i);
    ATH_MSG_INFO("  Chain number: " << i << " Shifter Chain Name: " << chainName );
  }

  /* Fill some of the histograms with random values */ 

    auto E1d0 = Monitored::Scalar<float>("E1d0",0.0);
    auto E2d0 = Monitored::Scalar<float>("E2d0",0.0);
    auto S1d0 = Monitored::Scalar<float>("S1d0",0.0);
    auto S2d0 = Monitored::Scalar<float>("S2d0",0.0);
    auto S2IP3D_pu = Monitored::Scalar<float>("S2IP3D_pu",0.0);
    auto random = Monitored::Scalar<float>("random",0.0);

    // Set the values of the monitored variables for the event
    /*
    E1d0 = -0.02;
    E2d0 = +0.02;
    S1d0 = -0.01;
    S2d0 = +0.01;
    */
    S2IP3D_pu = 0.5;

    //    TRandom r;
    TRandom r(ctx.eventID().event_number());
    TRandom q(ctx.eventID().event_number());

    std::cout << " E1d0 before: " << E1d0 << std::endl;
    std::cout << " S2d0 before: " << S2d0 << std::endl;
    std::cout << " random before: " << random << std::endl;
    std::cout << " m_doRandom " << m_doRandom << std::endl;
    if (m_doRandom) {
      E1d0 = r.Gaus(0.,0.1);
      E2d0 = r.Gaus(0.,0.2);
      S1d0 = r.Gaus(0.,0.3);
      S2d0 = r.Gaus(0.,0.4);
      random = q.Rndm();
      std::cout << " E1d0 in: " << E1d0 << std::endl;
      std::cout << " S2d0 in: " << S2d0 << std::endl;
      std::cout << " random in: " << random << std::endl;
    }
    std::cout << " E1d0 after: " << E1d0 << std::endl;
    std::cout << " S2d0 after: " << S2d0 << std::endl;
    std::cout << " S2IP3D_pu after: " << S2IP3D_pu << std::endl;
    std::cout << " random after: " << random << std::endl;

    

    // Fill. First argument is the tool name, all others are the variables to be saved.
    fill("TrigBjetMonitor",E1d0,S1d0);

    // Alternative fill method. Get the group yourself, and pass it to the fill function.
    auto tool = getGroup("TrigBjetMonitor");
    fill(tool,E2d0);
    fill(tool,S2d0);
    fill(tool,S2IP3D_pu);


    return StatusCode::SUCCESS;
}

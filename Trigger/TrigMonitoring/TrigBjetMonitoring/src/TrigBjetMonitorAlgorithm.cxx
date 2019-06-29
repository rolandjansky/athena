/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#include "TrigBjetMonitorAlgorithm.h"

#include "xAODTracking/TrackParticle.h"

TrigBjetMonitorAlgorithm::TrigBjetMonitorAlgorithm( const std::string& name, ISvcLocator* pSvcLocator )
  : AthMonitorAlgorithm(name,pSvcLocator)
  ,m_doRandom(true)
  ,m_expert{},m_shifter{}
  ,m_MuonContainerKey("Muons")
  ,m_VertexContainerKey("PrimaryVertices")
  ,m_trigDec("Trig::TrigDecisionTool/TrigDecisionTool")
{
  declareProperty ("expert", m_expert);
  declareProperty ("shifter", m_shifter);
  declareProperty ("expertI", m_expertI);
  declareProperty ("shifterI", m_shifterI);
  declareProperty("MuonContainerName",m_MuonContainerKey);
  declareProperty("VertexContainerName",m_VertexContainerKey);
}


TrigBjetMonitorAlgorithm::~TrigBjetMonitorAlgorithm() {}


StatusCode TrigBjetMonitorAlgorithm::initialize() {
  ATH_CHECK( m_MuonContainerKey.initialize() );
  ATH_CHECK( m_VertexContainerKey.initialize() );
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

  /* Verifiy if the trigger chain was fired and if yes, fill the corresponding histogram*/

  auto tool = getGroup("TrigBjetMonitor");

  std::string trackKey  = "InDetTrigTrackingxAODCnv_Bjet_IDTrig"; // for FTK chains should be revised !

  std::string NameF = "E";
  int i = 0;
  for ( auto& trigName : m_expert ) {
    if ( m_trigDecTool->isPassed(trigName) ) {
      std::cout << " Trigger chain from expert folder: " << trigName << " has fired !!! " << std::endl;
      std::string NameH = NameF + m_expertI.at(i) + "d0";
      std::cout << " i: " << i << " NameH: " << NameH << std::endl;
      auto d0 = Monitored::Scalar<float>(NameH,0.0);
      // here should come the retrival of d0 from the Trigger Track Container
      // Read the TrigFeature contener
      Trig::FeatureContainer fc = m_trigDec->features(trigName);
      const std::vector< Trig::Combination >& bjetCombs = fc.getCombinations();
      ATH_MSG_INFO("RETRIEVED " << bjetCombs.size() << " COMBINATIONS FOR "  << trigName);
      std::cout << " Size of bjetCombs : " << bjetCombs.size() << std::endl;
      // Take all combinations for this b-jet trigger
      std::vector< Trig::Combination >::const_iterator bjetComb;
      for( bjetComb = bjetCombs.begin(); bjetComb != bjetCombs.end(); ++bjetComb ) {
	const Trig::Combination& comb = *bjetComb;
	const std::vector< Trig::Feature<xAOD::TrackParticleContainer> > onlinetracks = comb.get<xAOD::TrackParticleContainer>(trackKey);
	ATH_MSG_INFO("RETRIEVED TRACKS -   size: " << onlinetracks.size());
	if ( not onlinetracks.empty() ) { 
	  const xAOD::TrackParticleContainer*  onlinetrack = onlinetracks[0].cptr();
	  ATH_MSG_INFO("                 -   nTrack: " << onlinetrack->size());
	  for(const auto* trk : *onlinetrack) {
	    d0 = trk->d0();
	    ATH_MSG_INFO("        d0: " << d0);
	    fill(tool,d0);
	  }// onlinetrack
	}// onlinetracks
      }// bjet combinations
    } else {
      std::cout << " Trigger chain from expert folder: " << trigName << " has not fired " << std::endl;
    }
    i++;
  } // for expert chains
  NameF = "S";
  i = 0;
  for ( auto& trigName : m_shifter ) {
    if ( m_trigDecTool->isPassed(trigName) ) {
      std::cout << " Trigger chain from shifter folder: " << trigName << " has fired !!! " << std::endl;
      std::string NameH = NameF + m_shifterI.at(i) + "d0";
      std::cout << " i: " << i << " NameH: " << NameH << std::endl;
      auto d0 = Monitored::Scalar<float>(NameH,0.0);
      // here should come the retrival of d0 from the Trigger Track Container
      fill(tool,d0);
    } else {
      std::cout << " Trigger chain from shifter folder: " << trigName << " has not fired " << std::endl;
    }
    i++;
  } // for shifter chains

  /* Read muons */
  /*
  auto pT_mu = Monitored::Scalar<float>("pT_mu",0.0);
  SG::ReadHandle<xAOD::MuonContainer> muons(m_MuonContainerKey, ctx);
  if (! muons.isValid() ) {
    ATH_MSG_ERROR("evtStore() does not contain muon Collection with name "<< m_MuonContainerKey);
    return StatusCode::FAILURE;
  }
  for (const auto& muonItr : *muons) {
    pT_mu = muonItr->pt();
    std::cout << " pT_mu: " << pT_mu << std::endl;
  }
  */

  /* Read off-line PV's  and fill histograms */

  auto OffNVtx = Monitored::Scalar<int>("Off_NVtx",0);
  auto OffxVtx = Monitored::Scalar<float>("Off_xVtx",0.0);
  auto OffyVtx = Monitored::Scalar<float>("Off_yVtx",0.0);
  auto OffzVtx = Monitored::Scalar<float>("Off_zVtx",0.0);

  SG::ReadHandle<xAOD::VertexContainer> offlinepv(m_VertexContainerKey, ctx);
  if (! offlinepv.isValid() ) {
    ATH_MSG_ERROR("evtStore() does not contain VertexContainer Collection with name "<< m_VertexContainerKey);
    return StatusCode::FAILURE;
  }
  std::cout << " Size of the Off-line PV container: " << offlinepv->size()  << std::endl;
  OffNVtx = offlinepv->size() ;
  fill(tool,OffNVtx);
  for (unsigned int j = 0; j<offlinepv->size(); j++){
    OffxVtx = (*(offlinepv))[j]->x();
    OffyVtx = (*(offlinepv))[j]->y();
    OffzVtx = (*(offlinepv))[j]->z();
    fill(tool,OffxVtx);
    fill(tool,OffyVtx);
    fill(tool,OffzVtx);
  }

  /* Fill some of the histograms with random values */ 

  /*
    auto E1d0 = Monitored::Scalar<float>("E1d0",0.0);
    auto E2d0 = Monitored::Scalar<float>("E2d0",0.0);
    auto S1d0 = Monitored::Scalar<float>("S1d0",0.0);
    auto S2d0 = Monitored::Scalar<float>("S2d0",0.0);
  */
    //auto S2IP3D_pu = Monitored::Scalar<float>("S2IP3D_pu",0.0);
    //auto random = Monitored::Scalar<float>("random",0.0);

    // Set the values of the monitored variables for the event
    /*
    E1d0 = -0.02;
    E2d0 = +0.02;
    S1d0 = -0.01;
    S2d0 = +0.01;
    */
    //S2IP3D_pu = 0.5;

    //    TRandom r;
    /*
    TRandom r(ctx.eventID().event_number());
    TRandom q(ctx.eventID().event_number());
    */

    //std::cout << " E1d0 before: " << E1d0 << std::endl;
    //std::cout << " S2d0 before: " << S2d0 << std::endl;
    //std::cout << " random before: " << random << std::endl;
    //std::cout << " m_doRandom " << m_doRandom << std::endl;
    /*
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
    */
    //std::cout << " E1d0 after: " << E1d0 << std::endl;
    //std::cout << " S2d0 after: " << S2d0 << std::endl;
    //std::cout << " S2IP3D_pu after: " << S2IP3D_pu << std::endl;
    //std::cout << " random after: " << random << std::endl;

    

    // Fill. First argument is the tool name, all others are the variables to be saved.
    /*
    fill("TrigBjetMonitor",E1d0,S1d0);
    fill(tool,E2d0);
    fill(tool,S2d0);
    */
    //fill(tool,S2IP3D_pu);


    return StatusCode::SUCCESS;
}

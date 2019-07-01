/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#include "TrigBjetMonitorAlgorithm.h"

#include "xAODTracking/TrackParticle.h"

TrigBjetMonitorAlgorithm::TrigBjetMonitorAlgorithm( const std::string& name, ISvcLocator* pSvcLocator )
  : AthMonitorAlgorithm(name,pSvcLocator)
  ,m_doRandom(true)
  ,m_AllChains{}
  ,m_MuonContainerKey("Muons")
  ,m_VertexContainerKey("PrimaryVertices")
  ,m_trigDec("Trig::TrigDecisionTool/TrigDecisionTool")
{
  declareProperty ("AllChains", m_AllChains);
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


  auto tool = getGroup("TrigBjetMonitor");



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


  /* print the trigger chain names */

  std::string chainName;

  int size_AllChains = m_AllChains.size();
  ATH_MSG_INFO(" Size of the AllChains trigger container: " << size_AllChains );
  for (int i =0; i<size_AllChains; i++){
    chainName = m_AllChains.at(i);
    ATH_MSG_INFO("  Chain number: " << i << " AllChains Chain Name: " << chainName );
  }

  /* Verifiy if the trigger chain was fired and if yes, fill the corresponding histogram*/

  std::string trackKey  = "InDetTrigTrackingxAODCnv_Bjet_IDTrig"; // for FTK chains should be revised !

  /* using AllChains */
  for ( auto& trigName : m_AllChains ) {
    if ( m_trigDecTool->isPassed(trigName) ) {
      std::cout << " Trigger chain from AllChains list: " << trigName << " has fired !!! " << std::endl;
      std::string NameH = "d0_"+trigName;
      std::cout << " NameH: " << NameH << std::endl;
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
  } // for AllChains

    return StatusCode::SUCCESS;
}

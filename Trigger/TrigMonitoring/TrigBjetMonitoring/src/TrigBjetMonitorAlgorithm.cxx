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

  /* Define keys of retrival */

  std::string trackKey  = "";
  std::string jetKey  = "";
  std::string priVtxKey  = "EFHistoPrmVtx";
  bool FTKChain = false;
  bool mujetChain = false;
  bool bjetChain = true;
  bool splitChain = false;

  for ( auto& trigName : m_AllChains ) {

    if ( m_trigDecTool->isPassed(trigName) ) {
      std::cout << " Trigger chain from AllChains list: " << trigName << " has fired !!! " << std::endl;

      // Trigger type
      // split vs unsplit
      std::size_t found = trigName.find("split");
      if (found!=std::string::npos) {
	splitChain = true;
	jetKey = "SplitJet";
	priVtxKey = "xPrimVx";
	trackKey  = "InDetTrigTrackingxAODCnv_Bjet_IDTrig";
      }// found
      // FTK vs non FTK
      std::size_t found1 = trigName.find("FTK");
      if (found1!=std::string::npos) {
	priVtxKey = "PrimVertexFTK";
	trackKey  = "InDetTrigTrackingxAODCnv_Bjet_FTK_IDTrig";
	std::size_t found2 = trigName.find("FTKRefit");
	if (found2!=std::string::npos) {
	  trackKey  = "InDetTrigTrackingxAODCnv_Bjet_FTKRefit_IDTrig";
	}// found2
	std::size_t found3 = trigName.find("FTKVtx");
	if (found3!=std::string::npos) {
	  trackKey  = "InDetTrigTrackingxAODCnv_Bjet_IDTrig";
	}// found3
      }// found1
      // gsc vs non-gsc chain
      std::size_t found4 = trigName.find("gsc");
      if (found4!=std::string::npos) {
	jetKey = "GSCJet";
      }// found4
      // bjet vs mujet
      found = trigName.find("HLT_mu");
      if (found!=std::string::npos) {
	mujetChain = true;
	bjetChain = false;
      }// found

      std::cout << " Trigger type: bjetChain " << bjetChain << " mujetChain " << mujetChain << " FTKChain " << FTKChain << " splitChain " << splitChain << std::endl;
      std::cout << " Keys -- priVtxKey:  " << priVtxKey << " jetKey: " << jetKey << " trackKey: " << trackKey << std::endl;

      // Read the TrigFeature contener
      Trig::FeatureContainer fc = m_trigDec->features(trigName);
      const std::vector< Trig::Combination >& triggerCombs = fc.getCombinations();
      ATH_MSG_INFO("RETRIEVED " << triggerCombs.size() << " COMBINATIONS FOR "  << trigName);
      std::cout << " Size of triggerCombs : " << triggerCombs.size() << std::endl;
      // Take all combinations for this b-jet trigger
      std::vector< Trig::Combination >::const_iterator triggerComb;
      for( triggerComb = triggerCombs.begin(); triggerComb != triggerCombs.end(); ++triggerComb ) {
	const Trig::Combination& comb = *triggerComb;
	
	if (bjetChain) {
	/* Tracks */
	const std::vector< Trig::Feature<xAOD::TrackParticleContainer> > onlinetracks = comb.get<xAOD::TrackParticleContainer>(trackKey);
	ATH_MSG_INFO("RETRIEVED TRACKS -   size: " << onlinetracks.size());
	if ( not onlinetracks.empty() ) { 
	  const xAOD::TrackParticleContainer*  onlinetrack = onlinetracks[0].cptr();
	  ATH_MSG_INFO("                 -   nTrack: " << onlinetrack->size());
	  for(const auto* trk : *onlinetrack) {
	    /* d0 */
	    std::string NameH = "d0_"+trigName;
	    std::cout << " NameH: " << NameH << std::endl;
	    auto d0 = Monitored::Scalar<float>(NameH,0.0);
	    d0 = trk->d0();
	    ATH_MSG_INFO("        d0: " << d0);
	    fill(tool,d0);
	  }// onlinetrack
	}// onlinetracks

	}//bjetChain

      }// trigger combinations

    } else {
      std::cout << " Trigger chain from expert folder: " << trigName << " has not fired " << std::endl;
    }
  } // for AllChains

    return StatusCode::SUCCESS;
}

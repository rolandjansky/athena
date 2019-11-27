/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#include "tauMonitoring/tauMonitorAlgorithm.h"



tauMonitorAlgorithm::tauMonitorAlgorithm( const std::string& name, ISvcLocator* pSvcLocator )
:AthMonitorAlgorithm(name,pSvcLocator)
,m_doRandom(true)
{
  //declareProperty("TauRecContainer", m_TauContainerKey="TauJets");
  declareProperty("etaMin", m_etaMin=-1000.);
  declareProperty("etaMax", m_etaMax=1000);
  declareProperty("kinGroupName", m_kinGroupName="tauMonKinGroupBA");

}


tauMonitorAlgorithm::~tauMonitorAlgorithm() {}


StatusCode tauMonitorAlgorithm::initialize() {
    using namespace Monitored;

    ATH_CHECK( m_TauContainerKey.initialize() );

    return AthMonitorAlgorithm::initialize();
}


StatusCode tauMonitorAlgorithm::fillHistograms( const EventContext& ctx ) const {
    using namespace Monitored;


    auto tauEta = Monitored::Scalar<float>("tauEta");



    auto tauPhi = Monitored::Scalar<float>("tauPhi");



    auto tauEt = Monitored::Scalar<float>("tauEt");
    auto tauCharge = Monitored::Scalar<int>("tauCharge");
    auto NumTracks = Monitored::Scalar<int>("NumTracks");
    auto nTauCandidates = Monitored::Scalar<int>("nTauCandidates");
    auto nClusters = Monitored::Scalar<int>("nClusters");
    auto LB = Monitored::Scalar<int>("LB");

    auto EMRadius = Monitored::Scalar<float>("EMRadius");
    auto hadRadius = Monitored::Scalar<float>("hadRadius");
    auto isolFrac = Monitored::Scalar<float>("isolFrac");
    auto stripWidth2 = Monitored::Scalar<float>("stripWidth2");
    auto nStrip = Monitored::Scalar<float>("nStrip");
    auto etEMAtEMScale = Monitored::Scalar<float>("etEMAtEMScale");
    auto etHadAtEMScale = Monitored::Scalar<float>("etHadAtEMScale");
    auto centFrac = Monitored::Scalar<float>("centFrac");

    auto BDTJetScore = Monitored::Scalar<float>("BDTJetScore");
    auto BDTJetScoreSigTrans = Monitored::Scalar<float>("BDTJetScoreSigTrans");
    auto eleBDTMedium = Monitored::Scalar<float>("eleBDTMedium");
    auto eleBDTTight = Monitored::Scalar<float>("eleBDTTight");
    auto muonVeto = Monitored::Scalar<float>("muonVeto");
    auto tauBDTLoose = Monitored::Scalar<float>("tauBDTLoose");
    auto tauBDTMedium = Monitored::Scalar<float>("tauBDTMedium");
    auto tauBDTTight = Monitored::Scalar<float>("tauBDTTight");

    auto PSSFrac = Monitored::Scalar<float>("PSSFrac");

    auto EMFracTrk = Monitored::Scalar<float>("EMFracTrk");
    auto EfracL2EffCluster = Monitored::Scalar<float>("EfracL2EffCluster");
    auto EisoEffCluster = Monitored::Scalar<float>("EisoEffCluster");
    auto InvMassEffClusters = Monitored::Scalar<float>("InvMassEffClusters");
    auto nNeutPFO = Monitored::Scalar<float>("nNeutPFO");
    auto nShot = Monitored::Scalar<float>("nShot");

    //auto d0 = Monitored::Scalar<float>("d0");
    auto pi0bdt = Monitored::Scalar<float>("pi0bdt");



    SG::ReadHandle<xAOD::TauJetContainer> taus(m_TauContainerKey, ctx);
    if (! taus.isValid() ) {
      ATH_MSG_ERROR("evtStore() does not contain tau Collection with name "<< m_TauContainerKey);
      return StatusCode::FAILURE;
    }

    nTauCandidates = taus->size();



    for (const auto& tau : *taus) {



      //Global and tauB
      tauEta = tau->eta();
      tauPhi = tau->phi();
	

      tauEt = tau->pt()/1000;
      tauCharge = tau->charge();
      NumTracks = tau->nTracks();
      nClusters = tau->detail<int>(xAOD::TauJetParameters::numTopoClusters) ;
      LB = GetEventInfo(ctx)->lumiBlock();



      //calo
      EMRadius =  tau->detail<float>(xAOD::TauJetParameters::EMRadius);
      hadRadius = tau->detail<float>(xAOD::TauJetParameters::hadRadius);
      isolFrac = tau->detail<float>(xAOD::TauJetParameters::isolFrac) ;
      stripWidth2 = tau->detail<float>(xAOD::TauJetParameters::stripWidth2) ;
      nStrip = tau->detail<int>(xAOD::TauJetParameters::nStrip) ;
      etEMAtEMScale = tau->detail<float>(xAOD::TauJetParameters::etEMAtEMScale);
      etHadAtEMScale = tau->detail<float>(xAOD::TauJetParameters::etHadAtEMScale);
      centFrac = tau->detail<float>(xAOD::TauJetParameters::centFrac) ;


      //identification
      BDTJetScore = tau->discriminant(xAOD::TauJetParameters::BDTJetScore);
      BDTJetScoreSigTrans = tau->discriminant(xAOD::TauJetParameters::BDTJetScoreSigTrans);
      eleBDTMedium =       tau->isTau(xAOD::TauJetParameters::EleBDTMedium);
      eleBDTTight  =       tau->isTau(xAOD::TauJetParameters::EleBDTTight);
      muonVeto     =       tau->isTau(xAOD::TauJetParameters::MuonVeto);
      tauBDTLoose  =       tau->isTau(xAOD::TauJetParameters::JetBDTSigLoose);
      tauBDTMedium =       tau->isTau(xAOD::TauJetParameters::JetBDTSigMedium);
      tauBDTTight  =       tau->isTau(xAOD::TauJetParameters::JetBDTSigTight);


      //TauB/Identification/EleVetoBDTinputs
      PSSFrac = tau->detail<float>( xAOD::TauJetParameters::PSSFraction ) ;

	
     //TauB/SubStructure
     EMFracTrk = tau->detail<float>( xAOD::TauJetParameters::ChPiEMEOverCaloEME ) ;
     EfracL2EffCluster = tau->detail<float>( xAOD::TauJetParameters::lead2ClusterEOverAllClusterE );
     EisoEffCluster = tau->detail<float>( xAOD::TauJetParameters::caloIsoCorrected)/1000  ; //puts it in GeV
     InvMassEffClusters =  tau->detail<float>( xAOD::TauJetParameters::effTopoInvMass )/1000 ; //puts it in GeV
     nNeutPFO = tau->nProtoNeutralPFOs();
     nShot = tau->nShotPFOs() ;

	


        for ( unsigned int np = 0 ; np < nNeutPFO ; np ++ ) 
        {
          const xAOD::PFO* npfo = tau->protoNeutralPFO( np ) ;
          float bdtScore = npfo->bdtPi0Score();
          pi0bdt += bdtScore;
        }

	



	if(m_kinGroupName != "tauMonKinGroupGlobal"){
		if (m_etaMin < tauEta && tauEta < m_etaMax){
			fill(m_kinGroupName,
			     tauEta,
                             tauPhi,
                             tauEt,
                             tauCharge,
                             NumTracks,
                             nTauCandidates,
                             nClusters,
			     LB,
                             EMRadius,
                             hadRadius,
                             isolFrac,
                             stripWidth2,
                             nStrip,
			     etEMAtEMScale,
			     etHadAtEMScale,
			     centFrac,
			     BDTJetScore,
			     BDTJetScoreSigTrans,
			     eleBDTMedium,
			     eleBDTTight,
			     muonVeto,
			     tauBDTLoose,
			     tauBDTMedium,
			     tauBDTTight,
			     PSSFrac,
			     EMFracTrk,
			     EfracL2EffCluster,
			     EisoEffCluster,
			     InvMassEffClusters,
			     nNeutPFO,
                             nShot,
			     pi0bdt
			);
		}
	}else{
		fill(m_kinGroupName,
		     tauEta,
		     tauPhi,
		     tauEt,
		     tauCharge,
		     NumTracks,
		     nTauCandidates,
		     nClusters,
		     LB,
		     EMRadius,
		     hadRadius,
		     isolFrac,
		     stripWidth2,
		     nStrip,
		     etEMAtEMScale,
		     etHadAtEMScale,
		     centFrac,
		     BDTJetScore,
		     BDTJetScoreSigTrans,
		     eleBDTMedium,
		     eleBDTTight,
		     muonVeto,
		     tauBDTLoose,
		     tauBDTMedium,
		     tauBDTTight,
		     PSSFrac,
		     EMFracTrk,
		     EfracL2EffCluster,
		     EisoEffCluster,
		     InvMassEffClusters,
		     nNeutPFO,
                     nShot,
		     pi0bdt
		);

	}





    }
    

    return StatusCode::SUCCESS;
}

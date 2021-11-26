/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#include <utility>

#include "NewCorePlots.h"
#include "xAODCaloEvent/CaloVertexedTopoCluster.h"

namespace Tau{

  NewCorePlots::NewCorePlots(PlotBase* pParent, const std::string& sDir, std::string sTauJetContainerName):
    PlotBase(pParent, sDir),
    m_ipZ0SinThetaSigLeadTrk(nullptr),
    m_etOverPtLeadTrk(nullptr),
    m_ipSigLeadTrk(nullptr),
    m_massTrkSys(nullptr),
    m_trkWidth2(nullptr),
    m_trFlightPathSig(nullptr),
    m_etEflow(nullptr),
    m_nPi0(nullptr),
    m_tauDRMax(nullptr),
    m_EMRadius(nullptr),
    m_hadRadius(nullptr),
    m_isolFrac(nullptr),
    m_centFrac(nullptr),
    m_stripWidth2(nullptr),
    m_nStrip(nullptr),
    m_trkAvgDist(nullptr),
    m_lead2ClusterEOverAllClusterE(nullptr),
    m_lead3ClusterEOverAllClusterE(nullptr),
    m_caloIso(nullptr),
    m_mEflowTopo(nullptr),
    m_ptRatioEflowTopo(nullptr),
    m_nPi0Topo(nullptr),
    m_PSSFraction(nullptr),
    m_ChPiEMEOverCaloEME(nullptr),
    m_EMPOverTrkSysP(nullptr),
    m_innerTrkAvgDist(nullptr),
    m_ptRatioEflowApprox(nullptr),
    m_mEflowApprox(nullptr),
    m_sTauJetContainerName(std::move(sTauJetContainerName))
  {	
  }
	
  NewCorePlots::~NewCorePlots()
  {
  }

  void NewCorePlots::initializePlots(){

    m_ipZ0SinThetaSigLeadTrk           = Book1D("ipZ0SinThetaSigLeadTrk",m_sTauJetContainerName + "Tau ipZ0SinThetaSigLeadTrk ;ipZ0SinThetaSigLeadTrk ;# of Taus", 50, -10, 10);
    m_etOverPtLeadTrk                  = Book1D("etOverPtLeadTrk",m_sTauJetContainerName + "Tau etOverPtLeadTrk ;etOverPtLeadTrk ;# of Taus", 50, 0, 4.);
    m_ipSigLeadTrk 		       = Book1D("ipSigLeadTrk",m_sTauJetContainerName + "Tau ipSigLeadTrk ;ipSigLeadTrk ;# of Taus", 50,  -15, 15);
    m_massTrkSys 		       = Book1D("massTrkSys",m_sTauJetContainerName + "Tau massTrkSys ;massTrkSys ;# of Taus", 40, 0., 2.);
    m_trkWidth2 		       = Book1D("trkWidth2",m_sTauJetContainerName + "Tau trkWidth2 ;trkWidth2 ;# of Taus", 50, 0, 0.1);		      
    m_trFlightPathSig 	               = Book1D("trFlightPathSig",m_sTauJetContainerName + "Tau trFlightPathSig ;trFlightPathSig ;# of Taus", 100, -5, 30);
    m_EMRadius                         = Book1D("Seed_Calo_EMRadius",m_sTauJetContainerName + "Tau Seed Calo EMRadius ;EMRadius ;# of Taus",    50,  -0.2, 0.4);	  
    m_hadRadius 	               = Book1D("Seed_Calo_hadRadius",m_sTauJetContainerName + "Tau Seed Calo hadRadius ;hadRadius ;# of Taus",   50,  -0.2, 0.5); 
    m_isolFrac 	                       = Book1D("Seed_Calo_isolFrac",m_sTauJetContainerName + "Tau Seed Calo isolFrac ;isolFrac ;# of Taus",    40,  0., 1.);	  
    m_centFrac 	                       = Book1D("Seed_Calo_centFrac",m_sTauJetContainerName + "Tau Seed Calo centFrac ;centFrac ;# of Taus",    40,  0., 1.2);
    m_stripWidth2                      = Book1D("Seed_Calo_stripWidth2",m_sTauJetContainerName + "Tau Seed Calo stripWidth2 ;stripWidth2 ;# of Taus", 10, -0.5, 2.5);
    m_nStrip                           = Book1D("Seed_Calo_nStrip",m_sTauJetContainerName + "Tau Seed Calo nStrip ;nStrip ;# of Taus",  20,  0, 100);
    m_trkAvgDist                       = Book1D("Seed_Calo_trkavgdist",m_sTauJetContainerName + "Tau Seed Calo trkavgdist ;trkavgdist ;# of Taus",  50, 0., 0.4);
    m_lead2ClusterEOverAllClusterE     = Book1D("Seed_Calo_lead2ClusterEOverAllClusterE",m_sTauJetContainerName + "Tau Seed Calo lead2ClusterEOverAllClusterE ;lead2ClusterEOverAllClusterE ;# of Taus",40, 0.5, 1.);	
    m_lead3ClusterEOverAllClusterE     = Book1D("Seed_Calo_lead3ClusterEOverAllClusterE",m_sTauJetContainerName + "Tau Seed Calo lead3ClusterEOverAllClusterE ;lead3ClusterEOverAllClusterE ;# of Taus",40, 0.5, 1.);	
    m_caloIso	        	       = Book1D("Seed_Calo_caloIso",m_sTauJetContainerName + "Tau Seed Calo caloIso ;caloIso ;# of Taus", 15,  0, 200);
    m_tauDRMax                         = Book1D("DRMax",m_sTauJetContainerName + " Tau DR Max track-seed; DRMax; # Taus",20,0.,0.4);
    m_PSSFraction                      = Book1D("PSSFraction",m_sTauJetContainerName + "Tau PSSFraction; PSSFraction; # of Taus", 30, -15, 15);
    m_ChPiEMEOverCaloEME               = Book1D("ChPiEMEOverCaloEME",m_sTauJetContainerName + "Tau ChPiEMEOverCaloEME; ChPiEMEOverCaloEME; # of Taus", 40, -5, 5);
    m_EMPOverTrkSysP                   = Book1D("EMPOverTrkSysP",m_sTauJetContainerName + "Tau EMPOverTrkSysP; EMPOverTrkSysP; # of Taus", 62, -1, 30);
    m_innerTrkAvgDist                  = Book1D("innerTrkAvgDist",m_sTauJetContainerName + "Tau innerTrkAvgDist; innerTrkAvgDist; # of Taus",40,0.,0.2);
    m_ptRatioEflowApprox               = Book1D("ptRatioEflowApprox",m_sTauJetContainerName + "Tau ptRatioEflowApprox; ptRatioEflowApprox; # of Taus", 40, 0, 3.);
    m_mEflowApprox		       = Book1D("mEflowApprox",m_sTauJetContainerName + "Tau mEflowApprox; mEflowApprox; # Taus", 30, 0., 10000.);

    // track variables used for track classification and RNN tau ID
    m_logSeedJetPt = Book1D("logSeedJetPt" , "log(ptJetSeed);log(ptJetSeed);Entries",50,10.,14.5);
    m_track_logPt = Book1D("track_logPt" , "track log(p_{T});track log(p_{T});Entries",50,6.,15);
    m_track_eta = Book1D("track_eta" , "track eta;track eta;Entries",50,-2.6,2.6);
    m_track_d0TJVA = Book1D("track_d0TJVA" , "track tanh(d0TJVA);track tanh(d0TJVA);Entries",50,-0.1,0.1);
    m_track_z0SinthetaTJVA = Book1D("track_z0SinthetaTJVA" , "track z0SinthetaTJVA;track z0SinthetaTJVA;Entries",50,-100.,100);
    m_track_d0SigTJVA = Book1D("track_d0SigTJVA" , "track d0SigTJVA;track d0SigTJVA;Entries",50,-10.,10.);
    m_track_z0sinthetaSigTJVA = Book1D("track_z0sinthetaSigTJVA", "track z0sinthetaSigTJVA;track z0sinthetaSigTJVA;Entries",50,-10.,10.);
    m_track_charge = Book1D("track_charge" , "track charge;track charge;Entries",8,-4.,4.);
    m_track_qOverP = Book1D("track_qOverP" , "track qOverP;track qOverP;Entries",50,-2.,2.);
    m_track_logRConv = Book1D("track_logRConv" , "track log(rConv);track log(rConv);Entries",50,0.,7.);
    m_track_tanhRConvII = Book1D("track_tanhRConvII" , "track tanh(rConvII/500);track tanh(rConvII/500);Entries",50,-0.4,0.4);
    m_track_ptRatioSeedJet = Book1D("track_ptRatioSeedJet" , "(track p_{T})/(ptJetSeed);(track p_{T})/(ptJetSeed);Entries",50,0.,0.1);
    m_track_dRJetSeedAxis = Book1D("track_dRJetSeedAxis" , "#DeltaR(tau,track);#DeltaR(tau,track);Entries",50,0.,0.5);
    m_track_nInnermostPixHits = Book1D("track_nInnermostPixHits" , "track nInnermostPixHits;track nInnermostPixHits;Entries",5,0.,5.);
    m_track_nPixHits = Book1D("track_nPixHits" , "track nPixHits;track nPixHits;Entries",10,0.,10.);
    m_track_nPixelSharedHits = Book1D("track_nPixelSharedHits" , "track nPixelSharedHits;track nPixelSharedHits;Entries",6,0.,6.);
    m_track_nSiHits = Book1D("track_nSiHits" , "track nSiHits;track nSiHits;Entries",20,0.,20.);
    m_track_nSCTSharedHits = Book1D("track_nSCTSharedHits" , "track nSCTSharedHits;track nSCTSharedHits;Entries",10,0.,10.);
    m_track_nTRTHits = Book1D("track_nTRTHits" , "track nTRTHits;track nTRTHits;Entries",60,0.,60.);
    m_track_eProbabilityHT = Book1D("track_eProbabilityHT" , "track eProbabilityHT;track eProbabilityHT;Entries",50,0.,1.);    
    m_track_eProbabilityNN = Book1D("track_eProbabilityNN" , "track eProbabilityNN;track eProbabilityNN;Entries",50,0.,1.);    
    m_track_eProbabilityHTorNN = Book1D("track_eProbabilityHTorNN" , "track eProbabilityHTorNN;track eProbabilityHTorNN;Entries",50,0.,1.);    
    m_track_idScoreCharged = Book1D("track_idScoreCharged" , "track RNN idScoreCharged;track idScoreCharged;Entries",50,0.,1.);
    m_track_idScoreIso = Book1D("track_idScoreIso" , "track RNN idScoreIso;track idScoreIso;Entries",50,0.,1.);
    m_track_idScoreConv = Book1D("track_idScoreConv" , "track RNN idScoreConv;track idScoreConv;Entries",50,0.,1.);
    m_track_idScoreFake = Book1D("track_idScoreFake" , "track RNN idScoreFake;track idScoreFake;Entries",50,0.,1.);

    // cluster variables used for RNN tau ID
    m_cluster_logEt = Book1D("cluster_logEt" , "log(cluster E_{T});log(cluster E_{T});Entries",50,1.5,6.5);
    m_cluster_eta = Book1D("cluster_eta" , "cluster eta;cluster eta;Entries",50,-2.6,2.6);
    m_cluster_phi = Book1D("cluster_phi" , "cluster phi;cluster phi;Entries",32,-3.2,3.2);
    m_cluster_secondR = Book1D("cluster_secondR" , "cluster SECOND_R;cluster SECOND_R;Entries",50,-1.,6.);
    m_cluster_secondLambda = Book1D("cluster_secondLambda" , "cluster SECOND_LAMBDA;cluster SECOND_LAMBDA;Entries",50,2.,6.5);
    m_cluster_centerLambda = Book1D("cluster_centerLambda" , "cluster CENTER_LAMBDA;cluster CENTER_LAMBDA;Entries",50,1.,4.);
    m_cluster_firstEngDens = Book1D("cluster_firstEngDens" , "cluster FIRST_ENG_DENS;cluster FIRST_ENG_DENS;Entries",50,-6.,1.);
    m_cluster_EMproba = Book1D("cluster_EMproba" , "cluster EM_PROBABILITY;cluster EM_PROBABILITY;Entries",20,0.,1.); 

    // Et-weighted average of cluster moments used in MVA TES
    m_clustersMeanCenterLambda = Book1D("clustersMeanCenterLambda" , "clustersMeanCenterLambda;clustersMeanCenterLambda;Entries",40,0.,2500.);
    m_clustersMeanFirstEngDens = Book1D("clustersMeanFirstEngDens" , "clustersMeanFirstEngDens;clustersMeanFirstEngDens;Entries",40,-8.7,-5.5);
    m_clustersMeanEMProbability = Book1D("clustersMeanEMProbability" , "clustersMeanEMProbability;clustersMeanEMProbability;Entries",20,0.,1.);
    m_clustersMeanSecondLambda = Book1D("clustersMeanSecondLambda" , "clustersMeanSecondLambda;clustersMeanSecondLambda;Entries",20,0.,6e5);
    m_clustersMeanPresamplerFrac = Book1D("clustersMeanPresamplerFrac" , "clustersMeanPresamplerFrac;clustersMeanPresamplerFrac;Entries",20,0.,0.2);
    m_PFOEngRelDiff = Book1D("PFOEngRelDiff", "PFOEngRelDiff;PFOEngRelDiff;Entries",20,-1.,1.);

    m_TVz = Book1D("tauVertexZ", "tau vertex z:tau vertex z [mmm]",24,-120.,120.);
 }

  void NewCorePlots::fill(const xAOD::TauJet& tau, float weight) {

    float avariable = 0.;
    int bvariable = 0;
    
    if(tau.nTracks()>0 && tau.track(0)->isAvailable<float>("z0sinthetaSigTJVA")) {
      m_ipZ0SinThetaSigLeadTrk->Fill(tau.track(0)->z0sinthetaSigTJVA(), weight);
      m_ipSigLeadTrk->Fill(tau.track(0)->d0SigTJVA(), weight);
    }

    bool test = tau.detail(xAOD::TauJetParameters::etOverPtLeadTrk, avariable);
    if(test) m_etOverPtLeadTrk->Fill(avariable, weight);

    test = tau.detail(xAOD::TauJetParameters::massTrkSys, avariable);
    if(test) m_massTrkSys->Fill(avariable/1000., weight);

    test = tau.detail(xAOD::TauJetParameters::trkWidth2, avariable);
    if(test) m_trkWidth2->Fill(avariable, weight);

    test = tau.detail(xAOD::TauJetParameters::trFlightPathSig, avariable);
    if(test) m_trFlightPathSig->Fill(avariable, weight);

    test = tau.detail(xAOD::TauJetParameters::EMRadius, avariable);
    if(test) m_EMRadius->Fill(avariable, weight);

    test = tau.detail(xAOD::TauJetParameters::hadRadius, avariable);
    if(test) m_hadRadius->Fill(avariable, weight);

    test = tau.detail(xAOD::TauJetParameters::isolFrac, avariable);
    if(test) m_isolFrac->Fill(avariable, weight);

    test = tau.detail(xAOD::TauJetParameters::centFrac, avariable);
    if(test) m_centFrac->Fill(avariable, weight);

    test = tau.detail(xAOD::TauJetParameters::stripWidth2, avariable);
    if(test) m_stripWidth2->Fill(avariable, weight);
  
    test = tau.detail(xAOD::TauJetParameters::nStrip, bvariable);
    if(test) m_nStrip->Fill(bvariable, weight);
  
    test = tau.detail(xAOD::TauJetParameters::trkAvgDist, avariable);
    if(test) m_trkAvgDist->Fill(avariable, weight);

    test = tau.detail(xAOD::TauJetParameters::lead2ClusterEOverAllClusterE, avariable);
    if(test) m_lead2ClusterEOverAllClusterE->Fill(avariable, weight);

    test = tau.detail(xAOD::TauJetParameters::lead3ClusterEOverAllClusterE, avariable);
    if(test) m_lead3ClusterEOverAllClusterE->Fill(avariable, weight);

    test = tau.detail(xAOD::TauJetParameters::caloIso, avariable);
    if(test) m_caloIso->Fill(avariable, weight);

    test = tau.detail(xAOD::TauJetParameters::PSSFraction, avariable);
    if(test) m_PSSFraction->Fill(avariable, weight);
  
    test = tau.detail(xAOD::TauJetParameters::ChPiEMEOverCaloEME, avariable);
    if(test) m_ChPiEMEOverCaloEME->Fill(avariable, weight);

    test = tau.detail(xAOD::TauJetParameters::EMPOverTrkSysP, avariable);
    if(test) m_EMPOverTrkSysP->Fill(avariable, weight);

    test = tau.detail(xAOD::TauJetParameters::dRmax, avariable);
    if(test) m_tauDRMax->Fill(avariable, weight);

    test = tau.detail(xAOD::TauJetParameters::innerTrkAvgDist, avariable);
    if(test) m_innerTrkAvgDist->Fill(avariable, weight);

    test = tau.detail(xAOD::TauJetParameters::ptRatioEflowApprox, avariable);
    if(test) m_ptRatioEflowApprox->Fill(avariable, weight);

    test = tau.detail(xAOD::TauJetParameters::mEflowApprox, avariable);
    if(test) m_mEflowApprox->Fill(avariable, weight);


    double tauSeedPt = tau.ptJetSeed();
    double logTauSeedPt = std::log(tauSeedPt);
    m_logSeedJetPt->Fill(logTauSeedPt, weight);

    // tracks
    static const SG::AuxElement::ConstAccessor<float> idScoreCharged("rnn_chargedScore");
    static const SG::AuxElement::ConstAccessor<float> idScoreIso("rnn_isolationScore");
    static const SG::AuxElement::ConstAccessor<float> idScoreConv("rnn_conversionScore");
    static const SG::AuxElement::ConstAccessor<float> idScoreFake("rnn_fakeScore");

    for(const xAOD::TauTrack* track : tau.allTracks()) {

      // protection against thinned tracks
      static const SG::AuxElement::Accessor< xAOD::TauTrack::TrackParticleLinks_t > trackAcc( "trackLinks" );
      if(!trackAcc(*track)[0]) {
	continue;
      }

      const xAOD::TrackParticle* trackParticle = track->track();

      // old vs new track IPs... initialise to old
      float d0TJVA = track->track()->d0();
      float z0SinthetaTJVA = track->z0sinThetaTJVA(tau);
      float d0SigTJVA = 999.;
      float z0sinthetaSigTJVA = 999.;
      float rConv = 999.;
      float rConvII = 999.;
      if(track->isAvailable<float>("z0sinthetaTJVA")) {
	d0TJVA = track->d0TJVA();
	z0SinthetaTJVA = track->z0sinthetaTJVA();
	d0SigTJVA = track->d0SigTJVA();
	z0sinthetaSigTJVA = track->z0sinthetaSigTJVA();
	rConv = track->rConv();
	rConvII = track->rConvII();
      }

      double dRJetSeedAxis = track->p4().DeltaR(tau.p4(xAOD::TauJetParameters::JetSeed));
      double qOverP = trackParticle->qOverP();
      double trackPt = trackParticle->pt();

      uint8_t nInnermostPixelLayerHits = 0;
      trackParticle->summaryValue(nInnermostPixelLayerHits, xAOD::numberOfInnermostPixelLayerHits);
      uint8_t nPixelHits = 0;
      trackParticle->summaryValue(nPixelHits, xAOD::numberOfPixelHits);
      uint8_t nPixelSharedHits = 0;
      trackParticle->summaryValue(nPixelSharedHits, xAOD::numberOfPixelSharedHits);
      uint8_t nPixelDeadSensors = 0;
      trackParticle->summaryValue(nPixelDeadSensors, xAOD::numberOfPixelDeadSensors);
      uint8_t nSCTHits = 0;
      trackParticle->summaryValue(nSCTHits, xAOD::numberOfSCTHits);
      uint8_t nSCTSharedHits = 0;
      trackParticle->summaryValue(nSCTSharedHits, xAOD::numberOfSCTSharedHits);
      uint8_t nSCTDeadSensors = 0;
      trackParticle->summaryValue(nSCTDeadSensors, xAOD::numberOfSCTDeadSensors);
      uint8_t nTRTHighThresholdHits = 0;
      trackParticle->summaryValue(nTRTHighThresholdHits, xAOD::numberOfTRTHighThresholdHits);
      uint8_t nTRTHits = 0;
      trackParticle->summaryValue(nTRTHits, xAOD::numberOfTRTHits);
      uint8_t numberOfPixelHoles = 0;
      trackParticle->summaryValue(numberOfPixelHoles, xAOD::numberOfPixelHoles);
      uint8_t numberOfSCTHoles = 0;
      trackParticle->summaryValue(numberOfSCTHoles, xAOD::numberOfSCTHoles);
      float eProbabilityHT = 0.;
      trackParticle->summaryValue(eProbabilityHT, xAOD::eProbabilityHT);
      float eProbabilityNN = -1.;
      if(trackParticle->isAvailable<float>("eProbabilityNN")) eProbabilityNN = trackParticle->auxdata<float>("eProbabilityNN");
      // hybrid variable (eProbabilityNN is not computed for tracks with pt < 2 GeV)
      float eProbabilityHTorNN = (trackPt>2000.) ? eProbabilityNN : eProbabilityHT;

      m_track_logPt->Fill(std::log(trackPt), weight);
      m_track_eta->Fill(track->eta(), weight);
      m_track_d0TJVA->Fill(std::tanh(d0TJVA/10.), weight);
      m_track_z0SinthetaTJVA->Fill(z0SinthetaTJVA, weight);
      m_track_d0SigTJVA->Fill(d0SigTJVA, weight);
      m_track_z0sinthetaSigTJVA->Fill(z0sinthetaSigTJVA, weight);
      m_track_charge->Fill(trackParticle->charge(), weight);
      m_track_qOverP->Fill(qOverP*1000., weight);
      m_track_logRConv->Fill(std::log(rConv), weight);
      m_track_tanhRConvII->Fill( std::tanh(rConvII/500.0), weight);
      m_track_ptRatioSeedJet->Fill(trackPt/tauSeedPt, weight);
      m_track_dRJetSeedAxis->Fill(dRJetSeedAxis, weight);
      m_track_nInnermostPixHits->Fill(nInnermostPixelLayerHits, weight);
      m_track_nPixHits->Fill(nPixelHits + nPixelDeadSensors, weight);
      m_track_nPixelSharedHits->Fill(nPixelSharedHits, weight);
      m_track_nSiHits->Fill(nPixelHits + nPixelDeadSensors + nSCTHits + nSCTDeadSensors, weight);
      m_track_nSCTSharedHits->Fill(nSCTSharedHits, weight);
      m_track_nTRTHits->Fill(nTRTHits, weight);
      m_track_eProbabilityHT->Fill(eProbabilityHT, weight);
      m_track_eProbabilityNN->Fill(eProbabilityNN, weight);
      m_track_eProbabilityHTorNN->Fill(eProbabilityHTorNN);

      if(track->isAvailable<float>("rnn_chargedScore")) {
	m_track_idScoreCharged->Fill(idScoreCharged(*track), weight);
	m_track_idScoreIso->Fill(idScoreIso(*track), weight);
	m_track_idScoreConv->Fill(idScoreConv(*track), weight);
	m_track_idScoreFake->Fill(idScoreFake(*track), weight);
      }
    }


    // clusters
    std::vector<const xAOD::IParticle*> particleList = tau.clusters();  
    std::vector<xAOD::CaloVertexedTopoCluster> clusters;
    const xAOD::Vertex* vertex = nullptr;
    if(tau.vertexLink().isValid()) vertex = tau.vertex();

    for (const xAOD::IParticle* particle : particleList) {
      const xAOD::CaloCluster* cluster = static_cast<const xAOD::CaloCluster*>(particle);
      if (vertex) {
	clusters.emplace_back(*cluster, xAOD::CaloCluster::State::CALIBRATED, vertex->position());
      }
      else {
	clusters.emplace_back(*cluster, xAOD::CaloCluster::State::CALIBRATED);
      }
    }

    // sort by decreasing Et
    auto et_cmp = [](const xAOD::CaloVertexedTopoCluster& lhs,
		     const xAOD::CaloVertexedTopoCluster& rhs) {
      return lhs.p4().Et() > rhs.p4().Et();
    };
    std::sort(clusters.begin(), clusters.end(), et_cmp);

    // keep first 6 leading clusters as in RNN ID
    if (clusters.size() > 6) {
      clusters.resize(6, clusters[0]);
    }

    double moment;
    for(const auto& vertexedCluster : clusters) {
      m_cluster_logEt->Fill(std::log10(vertexedCluster.p4().Et()), weight);
      m_cluster_eta->Fill(vertexedCluster.eta(), weight);
      m_cluster_phi->Fill(vertexedCluster.phi(), weight);

      const xAOD::CaloCluster& cluster = vertexedCluster.clust();

      cluster.retrieveMoment(xAOD::CaloCluster::MomentType::SECOND_R, moment);
      m_cluster_secondR->Fill(std::log10(moment + 0.1), weight);

      cluster.retrieveMoment(xAOD::CaloCluster::MomentType::SECOND_LAMBDA, moment);
      m_cluster_secondLambda->Fill(std::log10(moment + 0.1), weight);

      cluster.retrieveMoment(xAOD::CaloCluster::MomentType::CENTER_LAMBDA, moment);
      m_cluster_centerLambda->Fill(std::log10(moment + 1e-6), weight);

      cluster.retrieveMoment(xAOD::CaloCluster::MomentType::FIRST_ENG_DENS, moment);
      if(moment!=0.) moment = std::log10(std::abs(moment));
      m_cluster_firstEngDens->Fill(moment, weight);

      cluster.retrieveMoment(xAOD::CaloCluster::MomentType::EM_PROBABILITY, moment);
      m_cluster_EMproba->Fill(moment, weight);
    }

    // Et-weighted average of cluster moments (MVA TES)
    test = tau.detail(xAOD::TauJetParameters::ClustersMeanCenterLambda, avariable);
    if(test) m_clustersMeanCenterLambda->Fill(avariable, weight);

    test = tau.detail(xAOD::TauJetParameters::ClustersMeanFirstEngDens, avariable);
    if(test) m_clustersMeanFirstEngDens->Fill(avariable, weight);

    test = tau.detail(xAOD::TauJetParameters::ClustersMeanEMProbability, avariable);
    if(test) m_clustersMeanEMProbability->Fill(avariable, weight);

    test = tau.detail(xAOD::TauJetParameters::ClustersMeanSecondLambda, avariable);
    if(test) m_clustersMeanSecondLambda->Fill(avariable, weight);

    test = tau.detail(xAOD::TauJetParameters::ClustersMeanPresamplerFrac, avariable);
    if(test) m_clustersMeanPresamplerFrac->Fill(avariable, weight);

    test = tau.detail(xAOD::TauJetParameters::PFOEngRelDiff, avariable);
    if(test) m_PFOEngRelDiff->Fill(avariable, weight);

    // tau vertex
    if(vertex) {
      m_TVz->Fill(vertex->z(), weight);
    }
  }
}

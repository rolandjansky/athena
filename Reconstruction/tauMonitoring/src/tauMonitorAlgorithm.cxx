/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#include "tauMonitoring/tauMonitorAlgorithm.h"

#include "GaudiKernel/SystemOfUnits.h"

using Gaudi::Units::GeV;

tauMonitorAlgorithm::tauMonitorAlgorithm( const std::string& name, ISvcLocator* pSvcLocator )
:AthMonitorAlgorithm(name,pSvcLocator)

,m_doRandom(true){}

tauMonitorAlgorithm::~tauMonitorAlgorithm() {}

StatusCode tauMonitorAlgorithm::initialize() {
   using namespace Monitored;

   ATH_CHECK( m_TauContainerKey.initialize() );

   return AthMonitorAlgorithm::initialize();
}

StatusCode tauMonitorAlgorithm::fillHistograms( const EventContext& ctx ) const {
    using namespace Monitored;
    SG::ReadHandle<xAOD::TauJetContainer> taus(m_TauContainerKey, ctx);
    if (! taus.isValid() ) {
      ATH_MSG_ERROR("evtStore() does not contain tau Collection with name "<< m_TauContainerKey);
      return StatusCode::FAILURE;
    }

    
    //In tauMonTool these values are chosen as Et cuts for different Histograms
    const int lowerEtThreshold = 15;
    const int higherEtThreshold = 75;


    auto tool = getGroup(m_kinGroupName);
    auto tauEta = Monitored::Scalar<float>("tauEta",0.0);
    auto tauPhi = Monitored::Scalar<float>("tauPhi",0.0);
    auto tauEt = Monitored::Scalar<float>("tauEt",0.0);
    auto tauEtEt15BDTLoose = Monitored::Scalar<float>("tauEtEt15BDTLoose",0.0);
    auto PanModeEt15BDTLoose = Monitored::Scalar<float>("PanModeEt15BDTLoose",0.0);
    auto PanModeSubstructure = Monitored::Scalar<float>("PanModeSubstructure",0.0);
    auto coreTrk = Monitored::Scalar<float>("coreTrk",0.0);
    auto PtTESMVA = Monitored::Scalar<float>("PtTESMVA",0.0);
    auto PtCombined = Monitored::Scalar<float>("PtCombined",0.0);
    auto lumiPerBCID = Monitored::Scalar<float>("lumiPerBCID",0.0);

    auto tauPhiEt15 = Monitored::Scalar<float>("tauPhiEt15",0.0);
    auto tauEtaEt15 = Monitored::Scalar<float>("tauEtaEt15",0.0);

    auto tauPhiEt15BDTLoose = Monitored::Scalar<float>("tauPhiEt15BDTLoose",0.0);
    auto tauEtaEt15BDTLoose = Monitored::Scalar<float>("tauEtaEt15BDTLoose",0.0);


    auto tauCharge = Monitored::Scalar<int>("tauCharge",0.0);
    auto NumTracks = Monitored::Scalar<int>("NumTracks",0.0);
    auto NumTracksEt15BDTLoose = Monitored::Scalar<int>("NumTracksEt15BDTLoose",0.0);

    auto nTauCandidates = Monitored::Scalar<int>("nTauCandidates",0.0);
    auto nHighPtTauCandidates = Monitored::Scalar<int>("nHighPtTauCandidates",0.0);
    auto nClusters = Monitored::Scalar<int>("nClusters",0.0);
    auto nClustersEt15BDTLoose = Monitored::Scalar<int>("nClustersEt15BDTLoose",0.0);

    auto LB = Monitored::Scalar<int>("LB",0.0);

    auto EMRadius = Monitored::Scalar<float>("EMRadius",0.0);
    auto hadRadius = Monitored::Scalar<float>("hadRadius",0.0);
    auto isolFrac = Monitored::Scalar<float>("isolFrac",0.0);
    auto stripWidth2 = Monitored::Scalar<float>("stripWidth2",0.0);
    auto nStrip = Monitored::Scalar<float>("nStrip",0.0);
    auto etEMAtEMScale = Monitored::Scalar<float>("etEMAtEMScale",0.0);
    auto etHadAtEMScale = Monitored::Scalar<float>("etHadAtEMScale",0.0);
    auto centFrac = Monitored::Scalar<float>("centFrac",0.0);
    auto jetSeedEta = Monitored::Scalar<float>("jetSeedEta",0.0);
    auto jetSeedPhi = Monitored::Scalar<float>("jetSeedPhi",0.0);
    auto jetSeedPt = Monitored::Scalar<float>("jetSeedPt",0.0);

    auto BDTEleScoreSigTrans = Monitored::Scalar<float>("BDTEleScoreSigTrans",0.0);
    auto BDTJetScore = Monitored::Scalar<float>("BDTJetScore",0.0);
    auto BDTJetScoreSigTrans = Monitored::Scalar<float>("BDTJetScoreSigTrans",0.0);
    auto JetBDTBkgMedium  = Monitored::Scalar<float>("JetBDTBkgMedium",0.0);

    auto eleBDTMedium = Monitored::Scalar<float>("eleBDTMedium",0.0);
    auto eleBDTTight = Monitored::Scalar<float>("eleBDTTight",0.0);
    auto muonVeto = Monitored::Scalar<float>("muonVeto",0.0);
    auto tauBDTLoose = Monitored::Scalar<float>("tauBDTLoose",0.0);
    auto tauBDTMedium = Monitored::Scalar<float>("tauBDTMedium",0.0);
    auto tauBDTTight = Monitored::Scalar<float>("tauBDTTight",0.0);

    auto hadLeakFracFixed = Monitored::Scalar<float>("hadLeakFracFixed",0.0);
    auto PSSFrac = Monitored::Scalar<float>("PSSFrac",0.0);
    auto EMFrac = Monitored::Scalar<float>("EMFrac",0.0);
    auto etHotShotWinOverPtLeadTrk = Monitored::Scalar<float>("etHotShotWinOverPtLeadTrk",0.0);

    auto EMFracTrk = Monitored::Scalar<float>("EMFracTrk",0.0);
    auto EfracL2EffCluster = Monitored::Scalar<float>("EfracL2EffCluster",0.0);
    auto EisoEffCluster = Monitored::Scalar<float>("EisoEffCluster",0.0);
    auto InvMassEffClusters = Monitored::Scalar<float>("InvMassEffClusters",0.0);
    auto nNeutPFO = Monitored::Scalar<float>("nNeutPFO",0.0);
    auto nShot = Monitored::Scalar<float>("nShot",0.0);
    auto pt3 = Monitored::Scalar<float>("pt3",-9.0);

    auto BDTScoreAsP0 = Monitored::Scalar<float>("BDTScoreAsP0",0.0);
    auto dRmax = Monitored::Scalar<float>("dRmax",0.0);
    auto EMPOverTrkSysP = Monitored::Scalar<float>("EMPOverTrkSysP",0.0);
    auto SumPtTrkFracCorrected = Monitored::Scalar<float>("SumPtTrkFracCorrected",0.0);
    auto mEflowApprox = Monitored::Scalar<float>("mEflowApprox",0.0);
    auto ptIntermediateAxis = Monitored::Scalar<float>("ptIntermediateAxis",0.0);

    auto absipSigLeadTrk = Monitored::Scalar<float>("absipSigLeadTrk",0.0);
    auto etOverPtLeadTrk = Monitored::Scalar<float>("etOverPtLeadTrk",0.0);
    auto massTrkSys = Monitored::Scalar<float>("massTrkSys",0.0);
    auto ptRatioEflowApprox = Monitored::Scalar<float>("ptRatioEflowApprox",0.0);
    auto trFlightPathSig = Monitored::Scalar<float>("trFlightPathSig",0.0);
    auto trkAvgDist  = Monitored::Scalar<float>("trkAvgDist",0.0);

    auto panEta  = Monitored::Scalar<float>("panEta",0.0);
    auto panPhi  = Monitored::Scalar<float>("panPhi",0.0);
    auto panPt  = Monitored::Scalar<float>("panPt",0.0);
    auto d0  = Monitored::Scalar<float>("d0",0.0);
    auto eProbabilityHT = Monitored::Scalar<float>("eProbabilityHT",0.0);
    auto leadTrackDeltaEta = Monitored::Scalar<float>("leadTrackDeltaEta",0.0);
    auto leadTrackDeltaPhi = Monitored::Scalar<float>("leadTrackDeltaPhi",0.0);
    auto dRJetSeedAxis = Monitored::Scalar<float>("dRJetSeedAxis",0.0);
    auto z0TJVA  = Monitored::Scalar<float>("z0TJVA",0.0);
    auto z0PriVtx  = Monitored::Scalar<float>("z0PriVtx",0.0);

    auto eta_track  = Monitored::Scalar<float>("eta_track",0.0);
    auto pT_track  = Monitored::Scalar<float>("pT_track",0.0);
    auto phi_track  = Monitored::Scalar<float>("phi_track",0.0);
    auto leadTrkPt  = Monitored::Scalar<float>("leadTrkPt",0.0);
    auto nHighPtTaus = Monitored::Scalar<float>("nHighPtTaus",0.0);
    auto numberOfTRTHighThresholdHits  = Monitored::Scalar<float>("numberOfTRTHighThresholdHits",0.0);
    auto numberOfTRTHighThresholdOutliers  = Monitored::Scalar<float>("numberOfTRTHighThresholdOutliers",0.0);
    auto numberOfTRTHits  = Monitored::Scalar<float>("numberOfTRTHits",0.0);
    auto numberOfTRTOutliers  = Monitored::Scalar<float>("numberOfTRTOutliers",0.0);
    auto trkWidth2  = Monitored::Scalar<float>("trkWidth2",0.0);
    auto ipZ0SinThetaSigLeadTrk  = Monitored::Scalar<float>("ipZ0SinThetaSigLeadTrk",0.0);
    auto numberOfPixelHits  = Monitored::Scalar<float>("numberOfPixelHits",0.0);
    auto numberOfInnermostPixelLayerHits  = Monitored::Scalar<float>("numberOfInnermostPixelLayerHits",0.0);
    auto numberOfPixelSharedHits  = Monitored::Scalar<float>("numberOfPixelSharedHits",0.0);
    auto numberOfSCTHits  = Monitored::Scalar<float>("numberOfSCTHits",0.0);
    auto numberOfSCTSharedHits  = Monitored::Scalar<float>("numberOfSCTSharedHits",0.0);
    auto rConv  = Monitored::Scalar<float>("rConv",0.0);
    auto rConvII  = Monitored::Scalar<float>("rConvII",0.0);

    nTauCandidates = 0;
    
    for (const auto& tau : *taus) {
      //Global and tauB/CR/EC
      tauEta = tau->eta();
      tauPhi = tau->phi();
      tauEt = tau->pt()/GeV;
      tauCharge = tau->charge();
      NumTracks = tau->nTracks();
      nClusters = tau->detail<int>(xAOD::TauJetParameters::numTopoClusters) ;
      LB = GetEventInfo(ctx)->lumiBlock();
      lumiPerBCID = lbAverageInteractionsPerCrossing(ctx);
      coreTrk = tau->nTracks( xAOD::TauJetParameters::coreTrack ) ;
      PtTESMVA = tau->ptFinalCalib() /GeV; 
      PtCombined = tau->auxdata<float>( "pt_combined" )/GeV; 

      //calo
      EMRadius =  tau->detail<float>(xAOD::TauJetParameters::EMRadius);
      hadRadius = tau->detail<float>(xAOD::TauJetParameters::hadRadius);
      isolFrac = tau->detail<float>(xAOD::TauJetParameters::isolFrac) ;
      stripWidth2 = tau->detail<float>(xAOD::TauJetParameters::stripWidth2) ;
      nStrip = tau->detail<int>(xAOD::TauJetParameters::nStrip) ;
      etEMAtEMScale = tau->detail<float>(xAOD::TauJetParameters::etEMAtEMScale);
      etHadAtEMScale = tau->detail<float>(xAOD::TauJetParameters::etHadAtEMScale);
      centFrac = tau->detail<float>(xAOD::TauJetParameters::centFrac) ;
      jetSeedEta = tau->etaJetSeed(); 
      jetSeedPhi = tau->phiJetSeed(); 
      jetSeedPt = tau->ptJetSeed()/GeV; 
      ptIntermediateAxis     =    tau->ptIntermediateAxis()/GeV;

      //identification
      BDTJetScore = tau->discriminant(xAOD::TauJetParameters::BDTJetScore);
      BDTJetScoreSigTrans = tau->discriminant(xAOD::TauJetParameters::BDTJetScoreSigTrans);
      JetBDTBkgMedium = tau->isTau(xAOD::TauJetParameters::JetBDTBkgMedium);

      BDTEleScoreSigTrans = tau->auxdata<float>("BDTEleScoreSigTrans"); 

      eleBDTMedium =       tau->isTau(xAOD::TauJetParameters::EleBDTMedium);
      eleBDTTight  =       tau->isTau(xAOD::TauJetParameters::EleBDTTight);
      muonVeto     =       tau->isTau(xAOD::TauJetParameters::MuonVeto);
      tauBDTLoose  =       tau->isTau(xAOD::TauJetParameters::JetBDTSigLoose);
      tauBDTMedium =       tau->isTau(xAOD::TauJetParameters::JetBDTSigMedium);
      tauBDTTight  =       tau->isTau(xAOD::TauJetParameters::JetBDTSigTight);

	  dRmax           =    tau->detail<float>(xAOD::TauJetParameters::dRmax);
	  EMPOverTrkSysP  =    tau->detail<float>(xAOD::TauJetParameters::EMPOverTrkSysP);
	  SumPtTrkFracCorrected =    tau->detail<float>(xAOD::TauJetParameters::SumPtTrkFracCorrected);
	  mEflowApprox           =    tau->detail<float>(xAOD::TauJetParameters::mEflowApprox)/GeV;

      float ipSigLeadTrk    =    tau->detail<float>(xAOD::TauJetParameters::ipSigLeadTrk);
      absipSigLeadTrk =    fabs(ipSigLeadTrk);
      etOverPtLeadTrk = tau->detail<float>(xAOD::TauJetParameters::etOverPtLeadTrk);
      massTrkSys = tau->detail<float>(xAOD::TauJetParameters::massTrkSys) / GeV; //GeV
      ptRatioEflowApprox = tau->detail<float>( xAOD::TauJetParameters::ptRatioEflowApprox );
      trFlightPathSig = tau->detail<float>(xAOD::TauJetParameters::trFlightPathSig);
      trkAvgDist = tau->detail<float>(xAOD::TauJetParameters::trkAvgDist);

      panEta = tau->etaPanTauCellBased();
      panPhi = tau->phiPanTauCellBased() ;
      panPt = tau->ptPanTauCellBased()/GeV; //GeV ;

      //TauB/Identification/EleVetoBDTinputs
      PSSFrac = tau->detail<float>(xAOD::TauJetParameters::PSSFraction ) ;
      EMFrac = tau->auxdata<float>( "EMFracFixed" );

     if ( tau->isAvailable<float>("hadLeakFracFixed"))
        hadLeakFracFixed = tau->auxdata<float>( "hadLeakFracFixed" ); 


      if ( tau->isAvailable<float>( "etHotShotWinOverPtLeadTrk" ) ){
        etHotShotWinOverPtLeadTrk = tau->auxdata<float>( "etHotShotWinOverPtLeadTrk" );
      }

      //TauB/SubStructure
      EMFracTrk = tau->detail<float>( xAOD::TauJetParameters::ChPiEMEOverCaloEME ) ;
      EfracL2EffCluster = tau->detail<float>( xAOD::TauJetParameters::lead2ClusterEOverAllClusterE );
      EisoEffCluster = tau->detail<float>( xAOD::TauJetParameters::caloIsoCorrected)/GeV;
      InvMassEffClusters =  tau->detail<float>( xAOD::TauJetParameters::effTopoInvMass )/GeV ; //puts it in GeV
      nNeutPFO = tau->nProtoNeutralPFOs();
      nShot = tau->nShotPFOs() ;

      int panmode = -1 ;
      int panmodeSubstructure = -1 ;

      const auto& trigDecTool = getTrigDecisionTool();

      if (m_etaMin < fabs(tauEta) && fabs(tauEta) < m_etaMax){
         nTauCandidates +=1;

         if(tauEt > higherEtThreshold){
            nHighPtTauCandidates +=1;
            nHighPtTaus +=1;

         }

         if (m_kinGroupName != "tauMonKinGroupHighPt"&& m_kinGroupName!="tauMonKinGroupHighPtBDTLoose"){

             if (
                     (m_kinGroupName != "tauMonKinGroupTauTrig"  && m_kinGroupName != "tauMonKinGroupEleTrig" && m_kinGroupName != "tauMonKinGroupJetTrig") || 
                     (m_kinGroupName == "tauMonKinGroupTauTrig" && trigDecTool !=0 && trigDecTool->isPassed("HLT_tau[2-9][0-9]_.*")) ||
                     (m_kinGroupName == "tauMonKinGroupEleTrig" && trigDecTool !=0 && trigDecTool->isPassed("HLT_e[2-9][0-9]_.*")) ||
                     (m_kinGroupName == "tauMonKinGroupJetTrig" && trigDecTool !=0 && trigDecTool->isPassed("HLT_j[2-9][0-9]_.*"))
                ){



                 if(m_kinGroupName != "tauMonKinGroupGlobal" && tauEt > lowerEtThreshold && tauBDTLoose){
                     tauPhiEt15BDTLoose = tau->phi();
                     tauEtaEt15BDTLoose = tau->eta();
                     tauEtEt15BDTLoose = tau->pt()/GeV;
                     nClustersEt15BDTLoose = tau->detail<int>(xAOD::TauJetParameters::numTopoClusters) ;
                     NumTracksEt15BDTLoose = tau->nTracks();

                     tau->panTauDetail(xAOD::TauJetParameters::PanTau_DecayMode, panmode); 
                     PanModeEt15BDTLoose = panmode;

                     fill(tool,tauPhiEt15BDTLoose);
                     fill(tool,tauEtaEt15BDTLoose);
                     fill(tool,nClustersEt15BDTLoose);
                     fill(tool,NumTracksEt15BDTLoose);
                     fill(tool,tauEtEt15BDTLoose);
                     fill(tool,PanModeEt15BDTLoose);
                 }


                float maxpt = -9.9;
                for( ElementLink< xAOD::TauTrackContainer > link : tau->allTauTrackLinks() ){
                    if ( ! link.isValid() ) continue ;  

                    if ( (*link)->pt() > maxpt ){
                        leadTrackDeltaEta = (*link)->eta() - tau->etaJetSeed() ;
                        leadTrackDeltaPhi  = (*link)->phi() - tau->phiJetSeed() ;
                    }

                    const xAOD::TrackParticle* track = (*link)->track() ;

                    dRJetSeedAxis = (*link)->dRJetSeedAxis( *tau );
                    fill(tool,dRJetSeedAxis);


                    z0TJVA = (*link)->z0sinThetaTJVA( *tau ) ;
                    fill(tool,z0TJVA);


                    float ePht = 0. ;
                    if (track->summaryValue( ePht, xAOD::eProbabilityHT )){
                        eProbabilityHT = ePht;
                        fill(tool,eProbabilityHT);
                    }

                 if ( tau->nTracks() != 0){
                    const xAOD::TrackParticle* track = tau->track(0)->track();
                    const Trk::Perigee perigee = track->perigeeParameters();
                    d0 = perigee.parameters()[Trk::d0];

                    eta_track= perigee.eta();
                    leadTrkPt =tau->detail<float>(xAOD::TauJetParameters::leadTrkPt)/GeV;
                    pT_track = perigee.pT()/GeV;
                    phi_track = perigee.parameters()[Trk::phi];
                    trkWidth2  = tau->detail<float>(xAOD::TauJetParameters::trkWidth2);
                    ipZ0SinThetaSigLeadTrk = tau->detail<float>(xAOD::TauJetParameters::ipZ0SinThetaSigLeadTrk);

                    rConv = sqrt(fabs(d0)*tau->pt()/(0.15*2.));
                    double auxprod= d0 * perigee.parameters()[Trk::qOverP];
                    rConvII= auxprod > 0 ? rConv : -rConv;
                    
	                uint8_t dummy(0);
                    
                    if (track->summaryValue(dummy, xAOD::numberOfSCTSharedHits)){
                        numberOfSCTSharedHits = dummy;
                        fill(tool,numberOfSCTSharedHits);
                    }

                    if (track->summaryValue(dummy, xAOD::numberOfSCTHits)){
                        numberOfSCTHits = dummy;
                        fill(tool,numberOfSCTHits);
                    }

                    if (track->summaryValue(dummy, xAOD::numberOfPixelSharedHits)){
                        numberOfPixelSharedHits = dummy;
                        fill(tool,numberOfPixelSharedHits);
                    }
                    
                    if (track->summaryValue(dummy, xAOD::numberOfInnermostPixelLayerHits)){
                        numberOfInnermostPixelLayerHits = dummy;
                        fill(tool,numberOfInnermostPixelLayerHits);
                    }
                   
                    if (track->summaryValue(dummy, xAOD::numberOfPixelHits)){
                        numberOfPixelHits = dummy;
                        fill(tool,numberOfPixelHits);
                    }
                    if (track->summaryValue(dummy, xAOD::numberOfTRTHighThresholdHits)){
                        numberOfTRTHighThresholdHits = dummy;
                        fill(tool,numberOfTRTHighThresholdHits);
                    }
                    if (track->summaryValue(dummy, xAOD::numberOfTRTHighThresholdOutliers)){
                        numberOfTRTHighThresholdOutliers = dummy;
                        fill(tool,numberOfTRTHighThresholdOutliers);
                    }
                    if (track->summaryValue(dummy, xAOD::numberOfTRTHits)){
                        numberOfTRTHits = dummy;
                        fill(tool,numberOfTRTHits);
                    }
                    if (track->summaryValue(dummy, xAOD::numberOfTRTOutliers)){
                        numberOfTRTOutliers = dummy;
                        fill(tool,numberOfTRTOutliers);
                    }
                    
                    fill(tool,d0);
                    fill(tool,leadTrackDeltaEta);
                    fill(tool,leadTrackDeltaPhi);
                    fill(tool,eta_track);
                    fill(tool,leadTrkPt);
                    fill(tool,nHighPtTaus);
                    fill(tool,pT_track);
                    fill(tool,phi_track);
                    fill(tool,trkWidth2);
                    fill(tool,ipZ0SinThetaSigLeadTrk);
                    fill(tool,rConv);
                    fill(tool,rConvII);
                    fill(tool,numberOfSCTHits);
                 }
                
                }
                
                for ( int s = 0 ; s < nShot ; s++ ) 
                {
                  const xAOD::PFO* shot = tau->shotPFO( s ) ;
                  if ( shot != NULL ) 
                  {
                    float pt3Temp = -9.0 ;
                    shot->attribute(xAOD::PFODetails::PFOAttributes::tauShots_pt3, pt3Temp ) ;
                    pt3 = pt3Temp /GeV; //GeV
                    fill(tool,pt3);
                  }
                }

                for ( unsigned int np = 0 ; np < nNeutPFO ; np ++ ) 
                {
                  const xAOD::PFO* npfo = tau->protoNeutralPFO( np ) ;
                  BDTScoreAsP0 = npfo->bdtPi0Score();
                  fill(tool,BDTScoreAsP0);
                }

                 fill(tool,tauPhi,tauEta,LB,tauEt,centFrac, isolFrac,coreTrk); //for all 2d Histograms
                 fill(tool,PtTESMVA);
                 fill(tool,PtCombined);
                 fill(tool,EMRadius);
                 fill(tool,hadRadius);
                 fill(tool,stripWidth2);
                 fill(tool,nStrip);
                 fill(tool,etEMAtEMScale);
                 fill(tool,etHadAtEMScale);
                 fill(tool,tauCharge);
                 fill(tool,BDTEleScoreSigTrans);
                 fill(tool,BDTJetScore);
                 fill(tool,BDTJetScoreSigTrans);
                 fill(tool,JetBDTBkgMedium);
                 fill(tool,BDTEleScoreSigTrans);
                 fill(tool,eleBDTMedium);
                 fill(tool,eleBDTTight);
                 fill(tool,muonVeto);
                 fill(tool,tauBDTLoose);
                 fill(tool,tauBDTMedium);
                 fill(tool,tauBDTTight);
                 fill(tool,BDTJetScore);
                 fill(tool,PSSFrac);
                 fill(tool,hadLeakFracFixed);
                 fill(tool,etHotShotWinOverPtLeadTrk);
                 fill(tool,EMFrac);
                 fill(tool,EMFracTrk);
                 fill(tool,EfracL2EffCluster);
                 fill(tool,EisoEffCluster);
                 fill(tool,InvMassEffClusters);
                 fill(tool,nNeutPFO);
                 fill(tool,nShot);
                 fill(tool,NumTracks);
                 fill(tool,nClusters);
                 fill(tool,jetSeedEta);
                 fill(tool,jetSeedPhi);
                 fill(tool,jetSeedPt);
                 fill(tool,dRmax);
                 fill(tool,EMPOverTrkSysP);
                 fill(tool,SumPtTrkFracCorrected);
                 fill(tool,mEflowApprox);
                 fill(tool,ptIntermediateAxis);
                 fill(tool,absipSigLeadTrk);
                 fill(tool,etOverPtLeadTrk);
                 fill(tool,massTrkSys);
                 fill(tool,ptRatioEflowApprox);
                 fill(tool,trFlightPathSig);
                 fill(tool,trkAvgDist);
                 fill(tool,lumiPerBCID);

                 tau->panTauDetail(xAOD::TauJetParameters::PanTau_DecayMode, panmodeSubstructure); 
                 PanModeSubstructure = panmodeSubstructure;
                 fill(tool,PanModeSubstructure);

                
                 if ( panPhi > -100 ){
                    fill(tool,panEta);
                    fill(tool,panPhi);
                    fill(tool,panPt);
                 }
            }
        }

         if ((m_kinGroupName == "tauMonKinGroupHighPt") && tauEt > lowerEtThreshold){
             tauPhiEt15 = tau->phi();
             tauEtaEt15 = tau->eta();
             fill(tool,LB,tauPhiEt15,tauEtaEt15);
         }

         if ((m_kinGroupName == "tauMonKinGroupHighPtBDTLoose") && tauEt > lowerEtThreshold &&tauBDTLoose){
             tauPhiEt15BDTLoose = tau->phi();
             tauEtaEt15BDTLoose = tau->eta();
             tauEtEt15BDTLoose = tau->pt()/GeV; //GeV

             nClustersEt15BDTLoose = tau->detail<int>(xAOD::TauJetParameters::numTopoClusters) ;
             NumTracksEt15BDTLoose = tau->nTracks();
             fill(tool,LB,tauPhiEt15BDTLoose,tauEtaEt15BDTLoose);
             fill(tool,nClustersEt15BDTLoose);
             fill(tool,NumTracksEt15BDTLoose);
             fill(tool,tauEtEt15BDTLoose);
         }

      }
   }
   fill(tool,nHighPtTauCandidates);
   fill(tool,nTauCandidates);
   return StatusCode::SUCCESS;
}

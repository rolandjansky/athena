/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

  Author : B. Laforge (laforge@lpnhe.in2p3.fr)
  4 May 2020
*/

#include "MonitorElectronAlgorithm.h"

MonitorElectronAlgorithm::MonitorElectronAlgorithm( const std::string& name, ISvcLocator* pSvcLocator )
:AthMonitorAlgorithm(name,pSvcLocator)
{
}

MonitorElectronAlgorithm::~MonitorElectronAlgorithm() {}


StatusCode MonitorElectronAlgorithm::initialize() {
    using namespace Monitored;
    ATH_CHECK( AthMonitorAlgorithm::initialize() );
    ATH_CHECK( m_ParticleContainerKey.initialize() );
    return StatusCode::SUCCESS;
}

StatusCode MonitorElectronAlgorithm::fillHistograms( const EventContext& ctx ) const {
    using namespace Monitored;

    // Only monitor good LAr Events :

    xAOD::EventInfo::EventFlagErrorState error_state = GetEventInfo(ctx)->errorState(xAOD::EventInfo::LAr);
    if (error_state==xAOD::EventInfo::Error) {
      ATH_MSG_DEBUG("LAr event data integrity error");
      return StatusCode::SUCCESS;
    }

    //
    // now, fill the specific Electron information
    //
    // get the Electron container

    SG::ReadHandle<xAOD::ElectronContainer> electrons(m_ParticleContainerKey, ctx);
    if (! electrons.isValid() ) {
      ATH_MSG_ERROR("evtStore() does not contain electron Collection with name "<< m_ParticleContainerKey);
      return StatusCode::FAILURE;
    }

    // Event variables to be monitored
    auto lbNCandidates = Monitored::Scalar<u_int16_t>("LBEvoN",0);

    // Particle variables to be monitored
    auto np = Monitored::Scalar<int>("N",0.0);
    auto et = Monitored::Scalar<Float_t>("Et",0.0);
    auto eta = Monitored::Scalar<Float_t>("Eta",0.0);
    auto phi = Monitored::Scalar<Float_t>("Phi",0.0);
    auto is_pt_gt_2_5gev = Monitored::Scalar<bool>("is_pt_gt_2_5gev",false);
    auto is_pt_gt_4gev = Monitored::Scalar<bool>("is_pt_gt_4gev",false);
    auto is_pt_gt_20gev = Monitored::Scalar<bool>("is_pt_gt_20gev",false);
    auto time = Monitored::Scalar<Float_t>("Time",0.0);
    auto topoetcone40 = Monitored::Scalar<Float_t>("TopoEtCone40",0.0);
    auto ptcone20 = Monitored::Scalar<Float_t>("PtCone20",0.0);

    // Particle variables per Region

    // BARREL
    auto np_barrel = Monitored::Scalar<int>("NinBARREL",0.0);
    auto et_barrel = Monitored::Scalar<Float_t>("EtinBARREL",0.0);
    auto eta_barrel = Monitored::Scalar<Float_t>("EtainBARREL",0.0);
    auto phi_barrel = Monitored::Scalar<Float_t>("PhiinBARREL",0.0);
    auto time_barrel = Monitored::Scalar<Float_t>("TimeinBARREL",0.0);
    auto ehad1_barrel = Monitored::Scalar<Float_t>("Ehad1inBARREL",0.0);
    auto eoverp_barrel = Monitored::Scalar<Float_t>("EoverPinBARREL",0.0);
    auto coreem_barrel = Monitored::Scalar<Float_t>("CoreEMinBARREL",0.0);
    auto f0_barrel = Monitored::Scalar<Float_t>("F0inBARREL",0.0);
    auto f1_barrel = Monitored::Scalar<Float_t>("F1inBARREL",0.0);
    auto f2_barrel = Monitored::Scalar<Float_t>("F2inBARREL",0.0);
    auto f3_barrel = Monitored::Scalar<Float_t>("F3inBARREL",0.0);
    auto re233e237_barrel = Monitored::Scalar<Float_t>("Re233e337inBARREL",0.0);
    auto re237e277_barrel = Monitored::Scalar<Float_t>("Re237e277inBARREL",0.0);
    auto nofblayerhits_barrel = Monitored::Scalar<u_int8_t>("NOfBLayerHitsinBARREL",0);
    auto nofpixelhits_barrel = Monitored::Scalar<u_int8_t>("NOfPixelHitsinBARREL",0);
    auto nofscthits_barrel = Monitored::Scalar<u_int8_t>("NOfSCTHitsinBARREL",0);
    auto noftrthits_barrel = Monitored::Scalar<u_int8_t>("NOfTRTHitsinBARREL",0);
    auto noftrthighthresholdhits_barrel = Monitored::Scalar<u_int8_t>("NOfTRTHighThresholdHitsinBARREL",0);
    auto deltaeta1_barrel = Monitored::Scalar<Float_t>("DeltaEta1inBARREL",0.0);
    auto deltaphi2_barrel = Monitored::Scalar<Float_t>("DeltaPhi2inBARREL",0.0);
    auto trackd0_barrel = Monitored::Scalar<Float_t>("Trackd0inBARREL",0.0);

    // ENDCAP
    auto np_endcap = Monitored::Scalar<int>("NinENDCAP",0.0);
    auto et_endcap = Monitored::Scalar<Float_t>("EtinENDCAP",0.0);
    auto eta_endcap = Monitored::Scalar<Float_t>("EtainENDCAP",0.0);
    auto phi_endcap = Monitored::Scalar<Float_t>("PhiinENDCAP",0.0);
    auto time_endcap = Monitored::Scalar<Float_t>("TimeinENDCAP",0.0);
    auto ehad1_endcap = Monitored::Scalar<Float_t>("Ehad1inENDCAP",0.0);
    auto eoverp_endcap = Monitored::Scalar<Float_t>("EoverPinENDCAP",0.0);
    auto coreem_endcap = Monitored::Scalar<Float_t>("CoreEMinENDCAP",0.0);
    auto f0_endcap = Monitored::Scalar<Float_t>("F0_endcapinENDCAP",0.0);
    auto f1_endcap = Monitored::Scalar<Float_t>("F1_endcapinENDCAP",0.0);
    auto f2_endcap = Monitored::Scalar<Float_t>("F2_endcapinENDCAP",0.0);
    auto f3_endcap = Monitored::Scalar<Float_t>("F3_endcapinENDCAP",0.0);
    auto re233e237_endcap = Monitored::Scalar<Float_t>("Re233e337_endcapinENDCAP",0.0);
    auto re237e277_endcap = Monitored::Scalar<Float_t>("Re237e277_endcapinENDCAP",0.0);
    auto nofblayerhits_endcap = Monitored::Scalar<u_int8_t>("NOfBLayerHitsinENDCAP",0);
    auto nofpixelhits_endcap = Monitored::Scalar<u_int8_t>("NOfPixelHitsinENDCAP",0);
    auto nofscthits_endcap = Monitored::Scalar<u_int8_t>("NOfSCTHitsinENDCAP",0);
    auto noftrthits_endcap = Monitored::Scalar<u_int8_t>("NOfTRTHitsinENDCAP",0);
    auto noftrthighthresholdhits_endcap = Monitored::Scalar<u_int8_t>("NOfTRTHighThresholdHitsinENDCAP",0);
    auto deltaeta1_endcap = Monitored::Scalar<Float_t>("DeltaEta1inENDCAP",0.0);
    auto deltaphi2_endcap = Monitored::Scalar<Float_t>("DeltaPhi2inENDCAP",0.0);
    auto trackd0_endcap = Monitored::Scalar<Float_t>("Trackd0inENDCAP",0.0);

    // CRACK
    auto np_crack = Monitored::Scalar<int>("NinCRACK",0.0);
    auto et_crack = Monitored::Scalar<Float_t>("EtinCRACK",0.0);
    auto eta_crack = Monitored::Scalar<Float_t>("EtainCRACK",0.0);
    auto phi_crack = Monitored::Scalar<Float_t>("PhiinCRACK",0.0);
    auto time_crack = Monitored::Scalar<Float_t>("TimeinCRACK",0.0);
    auto ehad1_crack = Monitored::Scalar<Float_t>("Ehad1inCRACK",0.0);
    auto eoverp_crack = Monitored::Scalar<Float_t>("EoverPinCRACK",0.0);
    auto coreem_crack = Monitored::Scalar<Float_t>("CoreEMinCRACK",0.0);
    auto f0_crack = Monitored::Scalar<Float_t>("F0inCRACK",0.0);
    auto f1_crack = Monitored::Scalar<Float_t>("F1inCRACK",0.0);
    auto f2_crack = Monitored::Scalar<Float_t>("F2inCRACK",0.0);
    auto f3_crack = Monitored::Scalar<Float_t>("F3inCRACK",0.0);
    auto re233e237_crack = Monitored::Scalar<Float_t>("Re233e337inCRACK",0.0);
    auto re237e277_crack = Monitored::Scalar<Float_t>("Re237e277inCRACK",0.0);
    auto nofblayerhits_crack = Monitored::Scalar<u_int8_t>("NOfBLayerHitsinCRACK",0);
    auto nofpixelhits_crack = Monitored::Scalar<u_int8_t>("NOfPixelHitsinCRACK",0);
    auto nofscthits_crack = Monitored::Scalar<u_int8_t>("NOfSCTHitsinCRACK",0);
    auto noftrthits_crack = Monitored::Scalar<u_int8_t>("NOfTRTHitsinCRACK",0);
    auto noftrthighthresholdhits_crack = Monitored::Scalar<u_int8_t>("NOfTRTHighThresholdHitsinCRACK",0);
    auto deltaeta1_crack = Monitored::Scalar<Float_t>("DeltaEta1inCRACK",0.0);
    auto deltaphi2_crack = Monitored::Scalar<Float_t>("DeltaPhi2inCRACK",0.0);
    auto trackd0_crack = Monitored::Scalar<Float_t>("Trackd0inCRACK",0.0);

    // Set the values of the monitored variables for the event

    u_int16_t mylb = GetEventInfo(ctx)->lumiBlock();
    lbNCandidates = mylb;

    u_int16_t mynp = 0;
    for (const auto& e_iter : *electrons) {
      // Check that the electron meets our requirements
      bool isGood;
      if (! e_iter->passSelection(isGood,m_RecoName)) {
        ATH_MSG_WARNING("Misconfiguration: " << m_RecoName << " is not a valid working point for electrons");
        break; // no point in continuing
      }
      if(isGood) {mynp++;}
      else continue;

      // do specific stuff with electrons

      Float_t myet = e_iter->pt(); // in MeV (/Gaudi::Units::GeV; // in GeV)
      Float_t myeta = e_iter->eta();
      Float_t myphi = e_iter->phi();

      is_pt_gt_2_5gev = myet > 2500. ;
      is_pt_gt_4gev = myet > 4000. ;
      is_pt_gt_20gev = myet > 20000. ;

      // Isolation Energy
      Float_t mytopoetcone40 = -999.;
      e_iter->isolationValue(mytopoetcone40,xAOD::Iso::topoetcone40);
      topoetcone40 = mytopoetcone40;

      Float_t myptcone20 = -999.;
      e_iter->isolationValue(myptcone20,xAOD::Iso::ptcone20);
      ptcone20 = myptcone20;

      Float_t mytime=0.0;

      // Shower shape variable details
      Float_t myehad1 = 0.0;
      Float_t myecore = 0.0;
      Float_t myf0    = 0.0;
      Float_t myf1    = 0.0;
      Float_t myf2    = 0.0;
      Float_t myf3    = 0.0;
      Float_t e233  = 0.0;
      Float_t e237  = 0.0;
      Float_t e277  = 0.0;
      Float_t myre233e237 = 0.0;
      Float_t myre237e277 = 0.0;

      e_iter->showerShapeValue(myehad1, xAOD::EgammaParameters::ehad1);
      e_iter->showerShapeValue(myecore, xAOD::EgammaParameters::ecore);

      e_iter->showerShapeValue(e237, xAOD::EgammaParameters::e237);
      e_iter->showerShapeValue(e233, xAOD::EgammaParameters::e233);
      e_iter->showerShapeValue(e277, xAOD::EgammaParameters::e277);

      if (e237!=0) myre233e237 = e233 / e237;
      if (e277!=0) myre237e277 = e237 / e277;


      // Cluster track match details
      Float_t mydeltaeta1 = -999.0;
      e_iter->trackCaloMatchValue(mydeltaeta1, xAOD::EgammaParameters::deltaEta1);
      Float_t mydeltaphi2 = -999.0;
      e_iter->trackCaloMatchValue(mydeltaphi2, xAOD::EgammaParameters::deltaPhi2);

      Float_t myeoverp = -999.0;
      Float_t mytrackd0 = -999.0;

      // associated track details
      const xAOD::TrackParticle *t = e_iter->trackParticle();
      double trackp = 0;
      u_int8_t mynofblayerhits=-1;
      u_int8_t mynofpixelhits=-1;
      u_int8_t mynofscthits=-1;
      u_int8_t mynoftrthits=-1;
      u_int8_t mynoftrthighthresholdhits=-1;

      if(t) {
        trackp = t->pt()*cosh(t->eta());
        // retrieve track summary information
        t->summaryValue(mynofblayerhits,xAOD::numberOfInnermostPixelLayerHits);
        t->summaryValue(mynofpixelhits,xAOD::numberOfPixelHits);
        t->summaryValue(mynofscthits,xAOD::numberOfSCTHits);
        t->summaryValue(mynoftrthits,xAOD::numberOfTRTHits);
        t->summaryValue(mynoftrthighthresholdhits,xAOD::numberOfTRTHighThresholdHits);
        mytrackd0 = t->d0();
        }
      else
        ATH_MSG_DEBUG("Error attempting to retrieve associated track");


      // Associated cluster details

      const xAOD::CaloCluster *aCluster = e_iter->caloCluster();
      if (aCluster) {
        mytime = aCluster->time();
        time = mytime ;
        // Shower shape variable details
        double ec = aCluster->et()*cosh(aCluster->eta());
        if (ec!=0) myf0 = aCluster->energyBE(0)/ec;
        if (ec!=0) myf1 = aCluster->energyBE(1)/ec;
        if (ec!=0) myf2 = aCluster->energyBE(2)/ec;
        if (ec!=0) myf3 = aCluster->energyBE(3)/ec;

        if(trackp !=0) myeoverp = aCluster->e()/trackp;

        //ATH_MSG_WARNING("Test eoverp : " << myeoverp);
      }

      // Fill per region histograms
      auto region = GetRegion(myeta);
      switch(region){
        case BARREL :
          nofblayerhits_barrel = mynofblayerhits; nofpixelhits_barrel = mynofpixelhits ; nofscthits_barrel = mynofscthits ;
          noftrthits_barrel = mynoftrthits ; noftrthighthresholdhits_barrel = mynoftrthighthresholdhits;
          fill("MonitorElectron",nofblayerhits_barrel, nofpixelhits_barrel,noftrthits_barrel,noftrthighthresholdhits_barrel,is_pt_gt_4gev);
          deltaeta1_barrel = mydeltaeta1; deltaphi2_barrel = mydeltaphi2; trackd0_barrel = mytrackd0;
          eoverp_barrel = myeoverp;
          fill("MonitorElectron",deltaeta1_barrel, deltaphi2_barrel,trackd0_barrel,eoverp_barrel,is_pt_gt_4gev);
          np_barrel = mynp; et_barrel = myet ; eta_barrel = myeta ; phi_barrel = myphi ;
          fill("MonitorElectron",np_barrel,et_barrel,eta_barrel,phi_barrel,is_pt_gt_4gev);
          time_barrel = mytime; ehad1_barrel = myehad1; coreem_barrel = myecore;
          fill("MonitorElectron",time_barrel, ehad1_barrel,coreem_barrel,is_pt_gt_4gev);
          f0_barrel = myf0; f1_barrel = myf1; f2_barrel = myf2; f3_barrel = myf3; re233e237_barrel = myre233e237; re237e277_barrel = myre237e277;
          fill("MonitorElectron",f0_barrel,f1_barrel,f2_barrel, f3_barrel,re233e237_barrel,re237e277_barrel,is_pt_gt_4gev);
          break;
        case ENDCAP :
          nofblayerhits_endcap = mynofblayerhits; nofpixelhits_endcap = mynofpixelhits ; nofscthits_endcap = mynofscthits ;
          noftrthits_endcap = mynoftrthits ; noftrthighthresholdhits_endcap = mynoftrthighthresholdhits;
          fill("MonitorElectron",nofblayerhits_endcap, nofpixelhits_endcap,noftrthits_endcap,noftrthighthresholdhits_endcap,is_pt_gt_4gev);
          deltaeta1_endcap = mydeltaeta1; deltaphi2_endcap = mydeltaphi2; trackd0_endcap = mytrackd0;
          eoverp_endcap = myeoverp;
          fill("MonitorElectron",deltaeta1_endcap, deltaphi2_endcap,trackd0_endcap,eoverp_endcap,is_pt_gt_4gev);
          np_endcap = mynp ; et_endcap = myet ; eta_endcap = myeta ; phi_endcap = myphi ;
          fill("MonitorElectron",np_endcap,et_endcap,eta_endcap,phi_endcap,is_pt_gt_4gev);
          time_endcap = mytime; ehad1_endcap = myehad1; coreem_endcap = myecore;
          fill("MonitorElectron",time_endcap, ehad1_endcap,coreem_endcap,is_pt_gt_4gev);
          f0_endcap = myf0; f1_endcap = myf1; f2_endcap = myf2; f3_endcap = myf3; re233e237_endcap = myre233e237; re237e277_endcap = myre237e277;
          fill("MonitorElectron",f0_endcap,f1_endcap,f2_endcap,f3_endcap,re233e237_endcap,re237e277_endcap,is_pt_gt_4gev);
          break;
        case CRACK :
          nofblayerhits_crack = mynofblayerhits; nofpixelhits_crack = mynofpixelhits ; nofscthits_crack = mynofscthits ;
          noftrthits_crack = mynoftrthits ; noftrthighthresholdhits_crack = mynoftrthighthresholdhits;
          fill("MonitorElectron",nofblayerhits_crack, nofpixelhits_crack,noftrthits_crack,noftrthighthresholdhits_crack,is_pt_gt_4gev);
          deltaeta1_crack = mydeltaeta1; deltaphi2_crack = mydeltaphi2; trackd0_crack = mytrackd0;
          eoverp_barrel = myeoverp;
          fill("MonitorElectron",deltaeta1_crack, deltaphi2_crack,trackd0_crack,eoverp_crack,is_pt_gt_4gev);
          np_crack = mynp ; et_crack = myet ; eta_crack = myeta ; phi_crack = myphi ;
          fill("MonitorElectron",np_crack,et_crack,eta_crack,phi_crack,is_pt_gt_4gev);
          time_crack = mytime; ehad1_crack = myehad1; coreem_crack = myecore;
          fill("MonitorElectron",time_crack, ehad1_crack,coreem_crack,is_pt_gt_4gev);
          f0_crack = myf0; f1_crack = myf1; f2_crack = myf2; f3_crack = myf3; re233e237_crack = myre233e237; re237e277_crack = myre237e277;
          fill("MonitorElectron",f0_crack,f1_crack,f2_crack,f3_crack,re233e237_crack,re237e277_crack,is_pt_gt_4gev);
          break;
        default :
          //ATH_MSG_WARNING("found an electron outside the |eta| > 2.47 acceptance");
          break;
      }

      et = myet ; eta = myeta ; phi = myphi ;
      fill("MonitorElectron",et,eta,phi,time,ptcone20,topoetcone40,lbNCandidates,
      is_pt_gt_2_5gev, is_pt_gt_4gev, is_pt_gt_20gev);

      // Fill. First argument is the tool name, all others are the variables to be histogramed
    }

    np = mynp;
    fill("MonitorElectron",np);

    return StatusCode::SUCCESS;
}

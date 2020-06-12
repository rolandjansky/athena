/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

  Author : B. Laforge (laforge@lpnhe.in2p3.fr)
  4 May 2020
*/

#include "MonitorPhotonAlgorithm.h"

MonitorPhotonAlgorithm::MonitorPhotonAlgorithm( const std::string& name, ISvcLocator* pSvcLocator )
:AthMonitorAlgorithm(name,pSvcLocator)
{
}

MonitorPhotonAlgorithm::~MonitorPhotonAlgorithm() {}


StatusCode MonitorPhotonAlgorithm::initialize() {
    using namespace Monitored;
    ATH_CHECK(AthMonitorAlgorithm::initialize() );
    ATH_CHECK( m_ParticleContainerKey.initialize() );
    return StatusCode::SUCCESS;
}

StatusCode MonitorPhotonAlgorithm::fillHistograms( const EventContext& ctx ) const {
    using namespace Monitored;

    // Only monitor good LAr Events :

    xAOD::EventInfo::EventFlagErrorState error_state = GetEventInfo(ctx)->errorState(xAOD::EventInfo::LAr);
    if (error_state==xAOD::EventInfo::Error) {
      ATH_MSG_DEBUG("LAr event data integrity error");
      return StatusCode::SUCCESS;
    }
    //
    // now, fill the Photon information
    // get the Photon container

    SG::ReadHandle<xAOD::PhotonContainer> photons(m_ParticleContainerKey, ctx);
    if (! photons.isValid() ) {
      ATH_MSG_ERROR("evtStore() does not contain photon Collection with name "<< m_ParticleContainerKey);
      return StatusCode::FAILURE;
    }

    // Specific Photon variables to be monitored

    u_int16_t mylb = GetEventInfo(ctx)->lumiBlock();

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

    // ENDCAP
    auto np_endcap = Monitored::Scalar<int>("NinENDCAP",0.0);
    auto et_endcap = Monitored::Scalar<Float_t>("EtinENDCAP",0.0);
    auto eta_endcap = Monitored::Scalar<Float_t>("EtainENDCAP",0.0);
    auto phi_endcap = Monitored::Scalar<Float_t>("PhiinENDCAP",0.0);
    auto time_endcap = Monitored::Scalar<Float_t>("TimeinENDCAP",0.0);
    auto ehad1_endcap = Monitored::Scalar<Float_t>("Ehad1inENDCAP",0.0);
    auto eoverp_endcap = Monitored::Scalar<Float_t>("EoverPinENDCAP",0.0);
    auto coreem_endcap = Monitored::Scalar<Float_t>("CoreEMinENDCAP",0.0);
    auto f0_endcap = Monitored::Scalar<Float_t>("F0inENDCAP",0.0);
    auto f1_endcap = Monitored::Scalar<Float_t>("F1inENDCAP",0.0);
    auto f2_endcap = Monitored::Scalar<Float_t>("F2inENDCAP",0.0);
    auto f3_endcap = Monitored::Scalar<Float_t>("F3inENDCAP",0.0);
    auto re233e237_endcap = Monitored::Scalar<Float_t>("Re233e337inENDCAP",0.0);
    auto re237e277_endcap = Monitored::Scalar<Float_t>("Re237e277inENDCAP",0.0);

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

    // Specific Photon variables

    auto lb = Monitored::Scalar<u_int16_t>("LB",0);

    auto is_pt_gt_2_5gevandconv = Monitored::Scalar<bool>("is_pt_gt_2_5gevandconv",false);
    auto is_pt_gt_2_5gevandunconv = Monitored::Scalar<bool>("is_pt_gt_2_5gevandunconv",false);

    auto is_pt_gt_4gevandconv = Monitored::Scalar<bool>("is_pt_gt_4gevandconv",false);
    auto is_pt_gt_4gevandunconv = Monitored::Scalar<bool>("is_pt_gt_4gevandunconv",false);

    auto is_pt_gt_20gevandconv = Monitored::Scalar<bool>("is_pt_gt_20gevandconv",false);
    auto is_pt_gt_20gevandunconv = Monitored::Scalar<bool>("is_pt_gt_20gevandunconv",false);

    auto lbevonphotonsunconv = Monitored::Scalar<u_int16_t>("LBEvoNPhotonsUnconv",0);
    auto lbevonphotonsconv = Monitored::Scalar<u_int16_t>("LBEvoNPhotonsConv",0);

    // Specific Photon variables per Region

    // BARREL

    auto rconv_barrel = Monitored::Scalar<Float_t>("RConvinBARREL",0.0);
    auto convtype_barrel = Monitored::Scalar<xAOD::EgammaParameters::ConversionType>("ConvTypeinBARREL",nullptr);
    auto contrkmatch1_barrel = Monitored::Scalar<u_int8_t>("ConvTrkmatch1inBARREL",0);
    auto contrkmatch2_barrel = Monitored::Scalar<u_int8_t>("ConvTrkmatch2inBARREL",0);

    // ENDCAP
    auto rconv_endcap = Monitored::Scalar<Float_t>("RConvinENDCAP",0.0);
    auto convtype_endcap = Monitored::Scalar<xAOD::EgammaParameters::ConversionType>("ConvTypeinENDCAP",nullptr);
    auto contrkmatch1_endcap = Monitored::Scalar<u_int8_t>("ConvTrkmatch1inENDCAP",0);
    auto contrkmatch2_endcap = Monitored::Scalar<u_int8_t>("ConvTrkmatch2inENDCAP",0);

    // CRACK
    auto rconv_crack = Monitored::Scalar<Float_t>("RConvinCRACK",0.0);
    auto convtype_crack = Monitored::Scalar<xAOD::EgammaParameters::ConversionType>("ConvTypeinCRACK",nullptr);
    auto contrkmatch1_crack = Monitored::Scalar<u_int8_t>("ConvTrkmatch1inCRACK",0);
    auto contrkmatch2_crack = Monitored::Scalar<u_int8_t>("ConvTrkmatch2inCRACK",0);

    // Set the values of the monitored variables for the event

    u_int16_t mynp=0;
    for (const auto& p_iter : *photons) {
      // Check that the electron meets our requirements
      bool isGood;
      if (! p_iter->passSelection(isGood,m_RecoName)) {
        ATH_MSG_WARNING("Misconfiguration: " << m_RecoName << " is not a valid working point for photons");
        break; // no point in continuing
      }
      if(isGood) {mynp++;}
      else continue;

      Float_t myet = p_iter->pt(); // in MeV (/Gaudi::Units::GeV; // in GeV)
      Float_t myeta = p_iter->eta();
      Float_t myphi = p_iter->phi();
      et = myet ; eta = myeta ; phi = myphi ;

      bool myis_pt_gt_2_5gev = myet > 2500. ;
      bool myis_pt_gt_4gev = myet > 4000. ;
      bool myis_pt_gt_20gev = myet > 20000. ;

      is_pt_gt_2_5gev = myis_pt_gt_2_5gev ;
      is_pt_gt_4gev = myis_pt_gt_4gev ;
      is_pt_gt_20gev = myis_pt_gt_20gev ;

      // Isolation Energy
      Float_t mytopoetcone40 = -999.;
      p_iter->isolationValue(mytopoetcone40,xAOD::Iso::topoetcone40);
      topoetcone40 = mytopoetcone40;

      Float_t myptcone20 = -999.;
      p_iter->isolationValue(myptcone20,xAOD::Iso::ptcone20);
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

      p_iter->showerShapeValue(myehad1, xAOD::EgammaParameters::ehad1);
      p_iter->showerShapeValue(myecore, xAOD::EgammaParameters::ecore);

      p_iter->showerShapeValue(e237, xAOD::EgammaParameters::e237);
      p_iter->showerShapeValue(e233, xAOD::EgammaParameters::e233);
      p_iter->showerShapeValue(e277, xAOD::EgammaParameters::e277);

      if (e237!=0) myre233e237 = e233 / e237;
      if (e277!=0) myre237e277 = e237 / e277;

      // Associated cluster details
      const xAOD::CaloCluster *aCluster = p_iter->caloCluster();
      if (aCluster) {
        mytime = aCluster->time();
        time = mytime ;
        // Shower shape variable details
        double ec = aCluster->et()*cosh(aCluster->eta());
      	if (ec!=0) myf0 = aCluster->energyBE(0)/ec;
      	if (ec!=0) myf1 = aCluster->energyBE(1)/ec;
      	if (ec!=0) myf2 = aCluster->energyBE(2)/ec;
      	if (ec!=0) myf3 = aCluster->energyBE(3)/ec;

      }

      // do specific stuff with photons

      // Conversion details
      xAOD::EgammaParameters::ConversionType myconvtype = xAOD::EgammaHelpers::conversionType(p_iter);
      bool isUnconverted = (myconvtype==xAOD::EgammaParameters::ConversionType::unconverted) ;

      is_pt_gt_2_5gevandconv = myis_pt_gt_2_5gev && !isUnconverted ;
      is_pt_gt_2_5gevandunconv = myis_pt_gt_2_5gev && isUnconverted ;

      is_pt_gt_4gevandconv = myis_pt_gt_4gev && !isUnconverted ;
      is_pt_gt_4gevandunconv = myis_pt_gt_4gev && isUnconverted ;

      is_pt_gt_20gevandconv = myis_pt_gt_20gev && !isUnconverted ;
      is_pt_gt_20gevandunconv = myis_pt_gt_20gev && isUnconverted ;

      lb = mylb;
      lbevonphotonsconv = mylb;
      lbevonphotonsunconv = mylb;

      fill("MonitorPhoton",lbevonphotonsconv,lbevonphotonsunconv,
          is_pt_gt_4gevandconv,is_pt_gt_4gevandunconv,
          is_pt_gt_2_5gevandconv,is_pt_gt_2_5gevandunconv,
          is_pt_gt_20gevandconv, is_pt_gt_20gevandunconv);

      Float_t myrconv = 0.0;
      myrconv = xAOD::EgammaHelpers::conversionRadius(p_iter);

      Float_t mycontrkmatch1 = 0.0;
      (p_iter)->vertexCaloMatchValue(mycontrkmatch1, xAOD::EgammaParameters::convMatchDeltaPhi1);
      Float_t mycontrkmatch2 = 0.0;
      (p_iter)->vertexCaloMatchValue(mycontrkmatch2, xAOD::EgammaParameters::convMatchDeltaPhi2);

      // Fill per region histograms
      auto region = GetRegion(myeta);
      switch(region){
        case BARREL :
          et_barrel = myet ; eta_barrel = myeta ; phi_barrel = myphi ;
          fill("MonitorPhoton",np_barrel,et_barrel,eta_barrel,phi_barrel,is_pt_gt_4gev);
          time_barrel = mytime; ehad1_barrel = myehad1; coreem_barrel = myecore;
          fill("MonitorPhoton",time_barrel, ehad1_barrel,coreem_barrel,is_pt_gt_4gev);
          f0_barrel = myf0; f1_barrel = myf1; f2_barrel = myf2; f3_barrel = myf3; re233e237_barrel = myre233e237; re237e277_barrel = myre237e277;
          fill("MonitorPhoton",f0_barrel,f1_barrel,f2_barrel, f3_barrel,re233e237_barrel,re237e277_barrel,is_pt_gt_4gev);
          rconv_barrel = myrconv ; convtype_barrel = myconvtype ; contrkmatch1_barrel = mycontrkmatch1 ; contrkmatch2_barrel = mycontrkmatch2 ;
          fill("MonitorPhoton",rconv_barrel,convtype_barrel,contrkmatch1_barrel,contrkmatch2_barrel,is_pt_gt_4gev);
          break;

        case ENDCAP :
          et_endcap = myet ; eta_endcap = myeta ; phi_endcap = myphi ;
          fill("MonitorPhoton",np_endcap,et_endcap,eta_endcap,phi_endcap,is_pt_gt_4gev);
          time_endcap = mytime; ehad1_endcap = myehad1; coreem_endcap = myecore;
          fill("MonitorPhoton",time_endcap, ehad1_endcap,coreem_endcap,is_pt_gt_4gev);
          f0_endcap = myf0; f1_endcap = myf1; f2_endcap = myf2; f3_endcap = myf3; re233e237_endcap = myre233e237; re237e277_endcap = myre237e277;
          fill("MonitorPhoton",f0_endcap,f1_endcap,f2_endcap,f3_endcap,re233e237_endcap,re237e277_endcap,is_pt_gt_4gev);
          rconv_endcap = myrconv ; convtype_endcap = myconvtype ; contrkmatch1_endcap = mycontrkmatch1 ; contrkmatch2_endcap = mycontrkmatch2 ;
          fill("MonitorPhoton",rconv_endcap,convtype_endcap,contrkmatch1_endcap,contrkmatch2_endcap,is_pt_gt_4gev);
          break;

        case CRACK :
          et_crack = myet ; eta_crack = myeta ; phi_crack = myphi ;
          fill("MonitorPhoton",np_crack,et_crack,eta_crack,phi_crack,is_pt_gt_4gev);
          time_crack = mytime; ehad1_crack = myehad1; coreem_crack = myecore;
          fill("MonitorPhoton",time_crack, ehad1_crack,coreem_crack,is_pt_gt_4gev);
          f0_crack = myf0; f1_crack = myf1; f2_crack = myf2; f3_crack = myf3; re233e237_crack = myre233e237; re237e277_crack = myre237e277;
          fill("MonitorPhoton",f0_crack,f1_crack,f2_crack,f3_crack,re233e237_crack,re237e277_crack,is_pt_gt_4gev);
          rconv_crack = myrconv ; convtype_crack = myconvtype ; contrkmatch1_crack = mycontrkmatch1 ; contrkmatch2_crack = mycontrkmatch2 ;
          fill("MonitorPhoton",rconv_crack,convtype_crack,contrkmatch1_crack,contrkmatch2_crack,is_pt_gt_4gev);
          break;

        default :
          ATH_MSG_DEBUG("found an electron outside the |eta| > 2.47 acceptance");
          break;
      }
      // Fill. First argument is the tool name, all others are the variables to be histogramed


      fill("MonitorPhoton",et,eta,phi,time,ptcone20,topoetcone40,is_pt_gt_4gev,is_pt_gt_20gev,
        is_pt_gt_2_5gevandconv,is_pt_gt_2_5gevandunconv,
        is_pt_gt_4gevandconv,is_pt_gt_4gevandunconv,
        is_pt_gt_20gevandconv,is_pt_gt_20gevandunconv);

      lb = mylb; lbevonphotonsunconv = mylb ; lbevonphotonsconv = mylb; lbNCandidates = mylb;
      fill("MonitorPhoton",lb, lbNCandidates,
          is_pt_gt_2_5gev,is_pt_gt_2_5gevandconv,is_pt_gt_2_5gevandunconv,
          is_pt_gt_4gev,is_pt_gt_4gevandconv,is_pt_gt_4gevandunconv,
          is_pt_gt_20gev,is_pt_gt_20gevandconv,is_pt_gt_20gevandunconv);

    }

    np = mynp;
    fill("MonitorPhoton",np);

    return StatusCode::SUCCESS;
}

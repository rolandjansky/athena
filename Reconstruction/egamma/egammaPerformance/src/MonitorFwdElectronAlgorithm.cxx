/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

  Author : B. Laforge (laforge@lpnhe.in2p3.fr)
  4 May 2020
*/

#include "MonitorFwdElectronAlgorithm.h"

MonitorFwdElectronAlgorithm::MonitorFwdElectronAlgorithm( const std::string& name, ISvcLocator* pSvcLocator )
:AthMonitorAlgorithm(name,pSvcLocator)
{
}

MonitorFwdElectronAlgorithm::~MonitorFwdElectronAlgorithm() {}


StatusCode MonitorFwdElectronAlgorithm::initialize() {
    using namespace Monitored;
    ATH_CHECK( AthMonitorAlgorithm::initialize() );
    ATH_CHECK( m_ParticleContainerKey.initialize() );
    return StatusCode::SUCCESS;
}

StatusCode MonitorFwdElectronAlgorithm::fillHistograms( const EventContext& ctx ) const {
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
      ATH_MSG_ERROR("evtStore() does not contain Forward Electron Collection with name "<< m_ParticleContainerKey);
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
    auto is_pt_gt_10gev = Monitored::Scalar<bool>("is_pt_gt_10gev",false);
    auto time = Monitored::Scalar<Float_t>("Time",0.0);

    auto firstENGdens = Monitored::Scalar<Float_t>("EnergyDensity",0.0);
    auto fracMax = Monitored::Scalar<Float_t>("FracMax",0.0);
    auto lateral = Monitored::Scalar<Float_t>("Lateral",0.0);
    auto longitudinal = Monitored::Scalar<Float_t>("Longitudinal",0.0);
    auto secondLambda = Monitored::Scalar<Float_t>("SecondLambda",0.0);
    auto secondR = Monitored::Scalar<Float_t>("SecondR",0.0);
    auto centerLambda = Monitored::Scalar<Float_t>("CenterLambda",0.0);


    // Particle variables per Region

    // variables per Region

    // ENDCAP
    auto np_endcap = Monitored::Scalar<int>("NinENDCAP",0.0);
    auto et_endcap = Monitored::Scalar<Float_t>("EtinENDCAP",0.0);
    auto eta_endcap = Monitored::Scalar<Float_t>("EtainENDCAP",0.0);
    auto phi_endcap = Monitored::Scalar<Float_t>("PhiinENDCAP",0.0);
    auto time_endcap = Monitored::Scalar<Float_t>("TimeinENDCAP",0.0);

    // FORWARD
    auto np_forward = Monitored::Scalar<int>("NinFORWARD",0.0);
    auto et_forward = Monitored::Scalar<Float_t>("EtinFORWARD",0.0);
    auto eta_forward = Monitored::Scalar<Float_t>("EtainFORWARD",0.0);
    auto phi_forward = Monitored::Scalar<Float_t>("PhiinFORWARD",0.0);
    auto time_forward = Monitored::Scalar<Float_t>("TimeinFORWARD",0.0);

    // Set the values of the monitored variables for the event

    u_int16_t mylb = GetEventInfo(ctx)->lumiBlock();
    lbNCandidates = mylb;

    u_int16_t mynp = 0;
    u_int16_t mynp_endcap = 0;
    u_int16_t mynp_forward = 0;


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
      is_pt_gt_10gev = myet > 10000. ;

      // Associated cluster details
      // Shower shape variable details

      double myfirstENGdens=-999.;
      double myfracMax=-999.;
      double mylateral=-999.;
      double mylongitudinal=-999.;
      double mysecondLambda=-999.;
      double mysecondR=-999.;
      double mycenterLambda=-999.;
      double mytime= -999.;

      const xAOD::CaloCluster *aCluster = e_iter->caloCluster();
      if (aCluster) {
        aCluster->retrieveMoment(xAOD::CaloCluster::FIRST_ENG_DENS,myfirstENGdens);
        aCluster->retrieveMoment(xAOD::CaloCluster::ENG_FRAC_MAX, myfracMax);
        aCluster->retrieveMoment(xAOD::CaloCluster::LONGITUDINAL, mylongitudinal);
        aCluster->retrieveMoment(xAOD::CaloCluster::SECOND_LAMBDA,mysecondLambda);
        aCluster->retrieveMoment(xAOD::CaloCluster::LATERAL, mylateral);
        aCluster->retrieveMoment(xAOD::CaloCluster::SECOND_R, mysecondR);
        aCluster->retrieveMoment(xAOD::CaloCluster::CENTER_LAMBDA, mycenterLambda);
        mytime = aCluster->time();
      }

      firstENGdens = (Float_t) myfirstENGdens;
      fracMax = (Float_t) myfracMax;
      lateral = (Float_t) mylateral;
      longitudinal = (Float_t) mylongitudinal;
      secondLambda = (Float_t) mysecondLambda;
      secondR = (Float_t) mysecondR;
      centerLambda = (Float_t) mycenterLambda;
      time = mytime;

      // Fill per region histograms

      auto region = GetRegion(myeta);
      switch(region){
        case ENDCAP :
          mynp_endcap++;
          et_endcap = myet ; eta_endcap = myeta ;
          phi_endcap = myphi ; time_endcap = mytime;
          fill("MonitorFwdElectron",et_endcap,eta_endcap,phi_endcap,time_endcap,is_pt_gt_10gev);
          break;
        case FORWARD :
          mynp_forward++;
          mynp_forward++;
          et_forward = myet ; eta_forward = myeta ;
          phi_forward = myphi ; time_forward = mytime;
          fill("MonitorFwdElectron",et_forward,eta_forward,phi_forward,time_forward,is_pt_gt_10gev);
          break;
        default :
          //ATH_MSG_WARNING("found an electron outside the |eta| > 2.47 acceptance");
          break;
      }

      // Fill global histograms

      et = myet ; eta = myeta ; phi = myphi ;
      fill("MonitorFwdElectron",et,eta,phi,time,lbNCandidates,
      firstENGdens, fracMax, lateral, longitudinal, secondLambda,
      secondR, centerLambda, time,
      is_pt_gt_2_5gev, is_pt_gt_10gev);

      // Fill. First argument is the tool name, all others are the variables to be histogramed
    }

    np = mynp; np_endcap = mynp_endcap ; np_forward = mynp_forward ;
    fill("MonitorFwdElectron",np,np_endcap,np_forward,is_pt_gt_10gev);

    return StatusCode::SUCCESS;
}

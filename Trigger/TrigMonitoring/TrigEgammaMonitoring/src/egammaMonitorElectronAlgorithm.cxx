/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#include "egammaMonitorElectronAlgorithm.h"


egammaMonitorElectronAlgorithm::egammaMonitorElectronAlgorithm( const std::string& name, ISvcLocator* pSvcLocator ):
  egammaMonitorBaseAlgorithm( name, pSvcLocator )

{}

StatusCode egammaMonitorElectronAlgorithm::initialize() 
{
  
  ATH_CHECK(m_electronsKey.initialize());
  ATH_CHECK(egammaMonitorBaseAlgorithm::initialize());
  if (!m_monTool.empty()) CHECK(m_monTool.retrieve());
  
  return StatusCode::SUCCESS;
}


StatusCode egammaMonitorElectronAlgorithm::execute(const EventContext& ctx) const
{
  
  filltopoElectronTrackCaloMatch(ctx);
  filltopoElectronShowerShapes(ctx);
  filltopoElectronIsolation(ctx);
  return StatusCode::SUCCESS;
}

void egammaMonitorElectronAlgorithm::filltopoElectronTrackCaloMatch( const EventContext& ctx) const{

    SG::ReadHandle<xAOD::ElectronContainer> electrons(m_electronsKey, ctx);

    std::vector<float> deltaEta0_vec, deltaEta1_vec, deltaEta2_vec, deltaEta3_vec, deltaPhi0_vec, deltaPhi1_vec, deltaPhi2_vec, deltaPhi3_vec, deltaPhiRescaled0_vec, deltaPhiRescaled1_vec, deltaPhiRescaled2_vec, deltaPhiRescaled3_vec, deltaPhiLast_vec, d0_vec;

    auto deltaEta0_col = Monitored::Collection("deltaEta0",deltaEta0_vec);
    auto deltaEta1_col = Monitored::Collection("deltaEta1",deltaEta1_vec);
    auto deltaEta2_col = Monitored::Collection("deltaEta2",deltaEta2_vec);
    auto deltaEta3_col = Monitored::Collection("deltaEta3",deltaEta3_vec);
    auto deltaPhi0_col = Monitored::Collection("deltaPhi0",deltaPhi0_vec);
    auto deltaPhi1_col = Monitored::Collection("deltaPhi1",deltaPhi1_vec);
    auto deltaPhi2_col = Monitored::Collection("deltaPhi2",deltaPhi2_vec);
    auto deltaPhi3_col = Monitored::Collection("deltaPhi3",deltaPhi3_vec);
    auto deltaPhiRescaled0_col = Monitored::Collection("deltaPhiRescaled0",deltaPhiRescaled0_vec);
    auto deltaPhiRescaled1_col = Monitored::Collection("deltaPhiRescaled1",deltaPhiRescaled1_vec);
    auto deltaPhiRescaled2_col = Monitored::Collection("deltaPhiRescaled2",deltaPhiRescaled2_vec);
    auto deltaPhiRescaled3_col = Monitored::Collection("deltaPhiRescaled3",deltaPhiRescaled3_vec);
    auto deltaPhiLast_col = Monitored::Collection("deltaPhiLast",deltaPhiLast_vec );
    auto d0_col = Monitored::Collection("d0",d0_vec);

    for (const auto electron : *electrons){

        deltaEta0_vec.push_back(getCaloTrackMatch_deltaEta0(electron));
        deltaEta1_vec.push_back(getCaloTrackMatch_deltaEta1(electron));
        deltaEta2_vec.push_back(getCaloTrackMatch_deltaEta2(electron));
        deltaEta3_vec.push_back(getCaloTrackMatch_deltaEta3(electron));
        deltaPhi0_vec.push_back(getCaloTrackMatch_deltaPhi0(electron));
        deltaPhi1_vec.push_back(getCaloTrackMatch_deltaPhi1(electron));
        deltaPhi2_vec.push_back(getCaloTrackMatch_deltaPhi2(electron));
        deltaPhi3_vec.push_back(getCaloTrackMatch_deltaPhi3(electron));
        deltaPhiRescaled0_vec.push_back(getCaloTrackMatch_deltaPhiRescaled0(electron));
        deltaPhiRescaled1_vec.push_back(getCaloTrackMatch_deltaPhiRescaled1(electron));
        deltaPhiRescaled2_vec.push_back(getCaloTrackMatch_deltaPhiRescaled2(electron));
        deltaPhiRescaled3_vec.push_back(getCaloTrackMatch_deltaPhiRescaled3(electron));
        deltaPhiLast_vec.push_back(getCaloTrackMatch_deltaPhiFromLastMeasurement(electron));
        d0_vec.push_back(getTrack_d0(electron));
                    
    }
  
    auto mon = Monitored::Group(m_monTool, deltaEta0_col,deltaEta1_col,deltaEta2_col,deltaEta3_col,
                                deltaPhi0_col,deltaPhi1_col,deltaPhi2_col,deltaPhi3_col,deltaPhiRescaled0_col,
                                deltaPhiRescaled1_col,deltaPhiRescaled2_col,deltaPhiRescaled3_col,deltaPhiLast_col,d0_col);

    ATH_MSG_DEBUG("Electron - Track Online Monitoring in Reconstruction ..."); 
    
    
  }

void egammaMonitorElectronAlgorithm::filltopoElectronShowerShapes(const EventContext& ctx) const{
    
    SG::ReadHandle<xAOD::ElectronContainer> electrons(m_electronsKey, ctx);
    ATH_MSG_DEBUG("Fill SS Reco Electron distributions: ");

    std::vector<float> ethad_vec, ethad1_vec, Rhad_vec, Rhad1_vec, Reta_vec, Rphi_vec,e237_vec,e277_vec, weta1_vec, weta2_vec, wtots1_vec,
      f1_vec, f3_vec, eratio_vec, et_vec, highet_vec , eta_vec, phi_vec, topoetcone20_vec, topoetcone40_shift_vec, 
      topoetcone20_rel_vec, topoetcone40_shift_rel_vec;
 

    auto ethad_col              = Monitored::Collection("ethad"    , ethad_vec );
    auto ethad1_col             = Monitored::Collection("ethad1"   , ethad1_vec );
    auto Rhad_col               = Monitored::Collection("Rhad"     , Rhad_vec    );
    auto Rhad1_col              = Monitored::Collection("Rhad1"    , Rhad1_vec   );
    auto Reta_col               = Monitored::Collection("Reta"     , Reta_vec    );
    auto Rphi_col               = Monitored::Collection("Rphi"     , Rphi_vec    );
    auto e237_col               = Monitored::Collection("e237"     , e237_vec    );
    auto e277_col               = Monitored::Collection("e277"     , e277_vec    );
    auto weta1_col              = Monitored::Collection("weta1"    , weta1_vec   );
    auto weta2_col              = Monitored::Collection("weta2"    , weta2_vec   );
    auto wtots1_col             = Monitored::Collection("wtots1"   , wtots1_vec   );
    auto f1_col                 = Monitored::Collection("f1"       , f1_vec      );
    auto f3_col                 = Monitored::Collection("f3"       , f3_vec      );
    auto eratio_col             = Monitored::Collection("eratio"   , eratio_vec  );
    auto et_col                 = Monitored::Collection("et"       , et_vec      );
    auto highet_col             = Monitored::Collection("highet"   , highet_vec  );
    auto eta_col                = Monitored::Collection("eta"      , eta_vec     );
    auto phi_col                = Monitored::Collection("phi"      , phi_vec     );
    auto topoetcone20_col       = Monitored::Collection("topoetcone20", topoetcone20_vec);
    auto topoetcone40_shift_col = Monitored::Collection("topoetcone40_shift",  topoetcone40_shift_vec );
  
    
    for ( const auto electron : *electrons ){

        if(!electron) continue;
        
        Reta_vec.push_back( getShowerShape_Reta(electron));
        Rphi_vec.push_back( getShowerShape_Rphi(electron));
        e237_vec.push_back( getShowerShape_e237(electron));
        e277_vec.push_back( getShowerShape_e277(electron));
        ethad_vec.push_back( getShowerShape_ethad(electron)/Gaudi::Units::GeV);
        ethad1_vec.push_back( getShowerShape_ethad1(electron)/Gaudi::Units::GeV);
        Rhad_vec.push_back( getShowerShape_Rhad(electron));
        Rhad1_vec.push_back( getShowerShape_Rhad(electron));       
        weta1_vec.push_back( getShowerShape_weta1(electron));
        weta2_vec.push_back( getShowerShape_weta2(electron));
        wtots1_vec.push_back( getShowerShape_wtots1(electron));
        f1_vec.push_back( getShowerShape_f1(electron));
        f3_vec.push_back( getShowerShape_f3(electron));
        eratio_vec.push_back( getShowerShape_Eratio(electron));
        et_vec.push_back( electron->pt()/Gaudi::Units::GeV);
        highet_vec.push_back( electron->pt()/Gaudi::Units::GeV);
        eta_vec.push_back( electron->eta());
        phi_vec.push_back( electron->phi());
        topoetcone20_vec.push_back( getIsolation_topoetcone20(electron)/Gaudi::Units::GeV);
        topoetcone40_shift_vec.push_back( (getIsolation_topoetcone40(electron)-2450)/Gaudi::Units::GeV );
        

    }

    auto mon = Monitored::Group(m_monTool,ethad_col, ethad1_col, Rhad_col, Rhad1_col, Reta_col, Rphi_col,e237_col,e277_col, weta1_col, weta2_col, wtots1_col, 
          f1_col, f3_col, eratio_col, et_col, highet_col , eta_col, phi_col, topoetcone20_col, topoetcone40_shift_col);

}

void egammaMonitorElectronAlgorithm::filltopoElectronIsolation( const EventContext& ctx) const{


    SG::ReadHandle<xAOD::ElectronContainer> electrons(m_electronsKey, ctx);
    std::vector<float> ptcone20_vec, ptcone30_vec, ptcone40_vec, ptvarcone20_vec, ptvarcone30_vec, ptvarcone40_vec;

    auto ptcone20_col = Monitored::Collection("ptcone20", ptcone20_vec);
    auto ptcone30_col = Monitored::Collection("ptcone30", ptcone30_vec);
    auto ptcone40_col = Monitored::Collection("ptcone40", ptcone40_vec);
    auto ptvarcone20_col = Monitored::Collection("ptvarcone20", ptvarcone20_vec);
    auto ptvarcone30_col = Monitored::Collection("ptvarcone30", ptvarcone30_vec);
    auto ptvarcone40_col = Monitored::Collection("ptvarcone40", ptvarcone40_vec);

    for (const xAOD::Electron* electron : *electrons){

      ptcone20_vec.push_back( getIsolation_ptcone20(electron)/Gaudi::Units::GeV);
      ptcone30_vec.push_back( getIsolation_ptcone30(electron)/Gaudi::Units::GeV);
      ptcone40_vec.push_back( getIsolation_ptcone40(electron)/Gaudi::Units::GeV);
      ptvarcone20_vec.push_back( getIsolation_ptvarcone20(electron)/Gaudi::Units::GeV);
      ptvarcone30_vec.push_back( getIsolation_ptvarcone30(electron)/Gaudi::Units::GeV);
      ptvarcone40_vec.push_back( getIsolation_ptvarcone40(electron)/Gaudi::Units::GeV);

    }
    
    auto mon = Monitored::Group(m_monTool,ptcone20_col, ptcone30_col, ptcone40_col,ptvarcone20_col, ptvarcone30_col, ptvarcone40_col );
    
    ATH_MSG_DEBUG("Electron - Isolation Online Monitoring in Reconstruction ..."); 
   
  }

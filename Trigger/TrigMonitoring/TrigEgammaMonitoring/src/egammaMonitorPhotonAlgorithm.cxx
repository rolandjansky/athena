/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#include "egammaMonitorPhotonAlgorithm.h"


egammaMonitorPhotonAlgorithm::egammaMonitorPhotonAlgorithm( const std::string& name, ISvcLocator* pSvcLocator ):
  egammaMonitorBaseAlgorithm( name, pSvcLocator )

{}

StatusCode egammaMonitorPhotonAlgorithm::initialize() 
{
  
  ATH_CHECK(m_photonsKey.initialize());
  ATH_CHECK(egammaMonitorBaseAlgorithm::initialize());
  if (!m_monTool.empty()) CHECK(m_monTool.retrieve());

  return StatusCode::SUCCESS;
}

StatusCode egammaMonitorPhotonAlgorithm::execute(const EventContext& ctx) const
{
  
  filltopoPhotonShowerShapes(ctx);
  filltopoPhotonIsolation(ctx);
  return StatusCode::SUCCESS;
}


void egammaMonitorPhotonAlgorithm::filltopoPhotonShowerShapes( const EventContext& ctx) const{

    SG::ReadHandle<xAOD::PhotonContainer> photons(m_photonsKey, ctx);
    ATH_MSG_DEBUG("Fill SS Reco Photon distributions: ");
    
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
  
    
    for ( const auto photon : *photons ){

        if(!photon) continue;
        
        Reta_vec.push_back( getShowerShape_Reta(photon));
        Rphi_vec.push_back( getShowerShape_Rphi(photon));
        e237_vec.push_back( getShowerShape_e237(photon));
        e277_vec.push_back( getShowerShape_e277(photon));
        ethad_vec.push_back( getShowerShape_ethad(photon)/Gaudi::Units::GeV);
        ethad1_vec.push_back( getShowerShape_ethad1(photon)/Gaudi::Units::GeV);
        Rhad_vec.push_back( getShowerShape_Rhad(photon));
        Rhad1_vec.push_back( getShowerShape_Rhad(photon));       
        weta1_vec.push_back( getShowerShape_weta1(photon));
        weta2_vec.push_back( getShowerShape_weta2(photon));
        wtots1_vec.push_back( getShowerShape_wtots1(photon));
        f1_vec.push_back( getShowerShape_f1(photon));
        f3_vec.push_back( getShowerShape_f3(photon));
        eratio_vec.push_back( getShowerShape_Eratio(photon));
        et_vec.push_back( photon->pt()/Gaudi::Units::GeV);
        highet_vec.push_back( photon->pt()/Gaudi::Units::GeV);
        eta_vec.push_back( photon->eta());
        phi_vec.push_back( photon->phi());
        topoetcone20_vec.push_back( getIsolation_topoetcone20(photon)/Gaudi::Units::GeV);
        topoetcone40_shift_vec.push_back( (getIsolation_topoetcone40(photon)-2450)/Gaudi::Units::GeV );
        

    }

    auto mon = Monitored::Group(m_monTool,ethad_col, ethad1_col, Rhad_col, Rhad1_col, Reta_col, Rphi_col,e237_col,e277_col, weta1_col, weta2_col, wtots1_col, 
          f1_col, f3_col, eratio_col, et_col, highet_col , eta_col, phi_col, topoetcone20_col, topoetcone40_shift_col);
    
}



void egammaMonitorPhotonAlgorithm::filltopoPhotonIsolation( const EventContext& ctx) const{


    SG::ReadHandle<xAOD::PhotonContainer> photons(m_photonsKey, ctx);
    std::vector<float> ptcone20_vec, ptcone30_vec, ptcone40_vec, ptvarcone20_vec, ptvarcone30_vec, ptvarcone40_vec;

    auto ptcone20_col = Monitored::Collection("ptcone20", ptcone20_vec);
    auto ptcone30_col = Monitored::Collection("ptcone30", ptcone30_vec);
    auto ptcone40_col = Monitored::Collection("ptcone40", ptcone40_vec);
    auto ptvarcone20_col = Monitored::Collection("ptvarcone20", ptvarcone20_vec);
    auto ptvarcone30_col = Monitored::Collection("ptvarcone30", ptvarcone30_vec);
    auto ptvarcone40_col = Monitored::Collection("ptvarcone40", ptvarcone40_vec);


    
    for (const xAOD::Photon* photon : *photons){

      ptcone20_vec.push_back( getIsolation_ptcone20(photon)/Gaudi::Units::GeV);
      ptcone30_vec.push_back( getIsolation_ptcone30(photon)/Gaudi::Units::GeV);
      ptcone40_vec.push_back( getIsolation_ptcone40(photon)/Gaudi::Units::GeV);
      ptvarcone20_vec.push_back( getIsolation_ptvarcone20(photon)/Gaudi::Units::GeV);
      ptvarcone30_vec.push_back( getIsolation_ptvarcone30(photon)/Gaudi::Units::GeV);
      ptvarcone40_vec.push_back( getIsolation_ptvarcone40(photon)/Gaudi::Units::GeV);

    }
    
    auto mon = Monitored::Group(m_monTool,ptcone20_col, ptcone30_col, ptcone40_col,ptvarcone20_col, ptvarcone30_col, ptvarcone40_col );
    
    ATH_MSG_DEBUG("Photon - Isolation Online Monitoring in Reconstruction ..."); 
  
  
}

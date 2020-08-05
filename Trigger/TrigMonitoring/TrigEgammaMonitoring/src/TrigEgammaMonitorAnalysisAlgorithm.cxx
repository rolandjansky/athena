

#include "TrigEgammaMonitorAnalysisAlgorithm.h"


TrigEgammaMonitorAnalysisAlgorithm::TrigEgammaMonitorAnalysisAlgorithm( const std::string& name, ISvcLocator* pSvcLocator ):
  TrigEgammaMonitorBaseAlgorithm( name, pSvcLocator )

{}

TrigEgammaMonitorAnalysisAlgorithm::~TrigEgammaMonitorAnalysisAlgorithm()
{}


StatusCode TrigEgammaMonitorAnalysisAlgorithm::initialize() 
{
  
  ATH_CHECK(TrigEgammaMonitorBaseAlgorithm::initialize());
 
  return StatusCode::SUCCESS;
}




// *********************************************************************************




void TrigEgammaMonitorAnalysisAlgorithm::fillLabel( const ToolHandle<GenericMonitoringTool>& groupHandle, 
                                                    const std::string &histname, 
                                                    const std::string &label ) const
{
  auto mon = Monitored::Scalar<std::string>( histname, label );
  fill( groupHandle, mon );
}




// *********************************************************************************

void TrigEgammaMonitorAnalysisAlgorithm::fillEfficiencies( std::vector< std::pair< const xAOD::Egamma*, const TrigCompositeUtils::Decision * >> pairObjs,
                                                           const TrigInfo info ) const
{

  std::vector< std::pair< const xAOD::Egamma*, const TrigCompositeUtils::Decision * >> pair_vec;
  std::vector< std::pair< const xAOD::Egamma*, const TrigCompositeUtils::Decision * >> pair_iso_vec;


  for( auto pairObj : pairObjs ){

    if(pairObj.first->type()==xAOD::Type::Electron){
      const xAOD::Electron* el = static_cast<const xAOD::Electron *> (pairObj.first);
      float et = getEt(el)/Gaudi::Units::GeV;
      if(et < info.trigThrHLT-5.0) continue; 
     
    }else if(pairObj.first->type()==xAOD::Type::Photon){
      float et = getCluster_et(pairObj.first)/Gaudi::Units::GeV;
      if(et < info.trigThrHLT-5.0) continue; 
      
       if(boost::contains(info.trigName,"icalovloose")) {
          if (getIsolation_topoetcone20(pairObj.first)/getCluster_et(pairObj.first) >= 0.065) continue; // pass FixedCutLoose offline isolation
      }
      else {
          if ((getIsolation_topoetcone40(pairObj.first)-2450.0)/getCluster_et(pairObj.first) >= 0.022) continue; // pass FixedCutTightCaloOnly offline isolation
      }
    } // Offline photon
  
    // Good pair to be measure
    pair_vec.push_back(pairObj);

    if( pairObj.first->auxdecor<bool>("Isolated") ){
      pair_iso_vec.push_back(pairObj);
    }
  }


  if (info.trigL1){
    fillEfficiency( "L1Calo", "L1Calo" ,  info.trigPidDecorator, info, pair_vec );
  }else{
    fillEfficiency( "L1Calo"  , "L1Calo", info.trigPidDecorator, info, pair_vec );
    fillEfficiency( "L2Calo"  , "L2Calo", info.trigPidDecorator, info, pair_vec );
    fillEfficiency( "L2"      , "L2"    , info.trigPidDecorator, info, pair_vec );
    fillEfficiency( "EFCalo"  , "EFCalo", info.trigPidDecorator, info, pair_vec );
    fillEfficiency( "HLT"     , "HLT"   , info.trigPidDecorator, info, pair_vec );

    if( m_detailedHists ){
      
      for( const auto& pid : m_isemname ){
        fillEfficiency( "HLT_" + pid, "HLT", "is"+pid, info, pair_vec );
        fillEfficiency( "HLT_" + pid + "Iso", "HLT", "is"+pid, info, pair_iso_vec );
      }

      for( const auto& pid : m_lhname ){
        fillEfficiency( "HLT_" + pid, "HLT", "is"+pid, info, pair_vec );
        fillEfficiency( "HLT_" + pid + "Iso", "HLT", "is"+pid, info, pair_iso_vec );
      }
    } 
  }
}




void TrigEgammaMonitorAnalysisAlgorithm::fillEfficiency( const std::string &subgroup, 
                                                         const std::string &level,
                                                         const std::string &pidword,
                                                         const TrigInfo info,
                                                         std::vector< std::pair< const xAOD::Egamma *, const TrigCompositeUtils::Decision* >> pairObjs) const
{


    
    const float etthr = info.trigThrHLT;
    const std::string trigger = info.trigName;


    auto monGroup = getGroup( trigger + "_Efficiency_" + subgroup );

    
    std::vector<float> et_vec, highet_vec, pt_vec, eta_vec, phi_vec, avgmu_vec, npvtx_vec;
    std::vector<float> match_et_vec, match_highet_vec, match_pt_vec, match_eta_vec, match_phi_vec, match_avgmu_vec, match_npvtx_vec;
    std::vector<bool> et_passed_vec, highet_passed_vec, pt_passed_vec, eta_passed_vec, phi_passed_vec, avgmu_passed_vec, npvtx_passed_vec;

    auto et_col     = Monitored::Collection( "et"     , et_vec );
    auto highet_col = Monitored::Collection( "highet" , highet_vec );
    auto pt_col     = Monitored::Collection( "pt"     , pt_vec );
    auto eta_col    = Monitored::Collection( "eta"    , eta_vec );
    auto phi_col    = Monitored::Collection( "phi"    , phi_vec );
    auto avgmu_col  = Monitored::Collection( "avgmu"  , avgmu_vec );
    auto npvtx_col  = Monitored::Collection( "npvtx"  , npvtx_vec );
    
    auto match_et_col     = Monitored::Collection( "match_et"     , match_et_vec );
    auto match_highet_col = Monitored::Collection( "match_ethigh" , match_highet_vec );
    auto match_pt_col     = Monitored::Collection( "match_pt"     , match_pt_vec );
    auto match_eta_col    = Monitored::Collection( "match_eta"    , match_eta_vec );
    auto match_phi_col    = Monitored::Collection( "match_phi"    , match_phi_vec );
    auto match_avgmu_col  = Monitored::Collection( "match_avgmu"  , match_avgmu_vec );
    auto match_npvtx_col  = Monitored::Collection( "match_npvtx"  , match_npvtx_vec );

    auto et_passed_col     = Monitored::Collection( "et_passed"     , et_passed_vec );
    auto highet_passed_col = Monitored::Collection( "highet_passed" , highet_passed_vec );
    auto pt_passed_col     = Monitored::Collection( "pt_passed"     , pt_passed_vec );
    auto eta_passed_col    = Monitored::Collection( "eta_passed"    , eta_passed_vec );
    auto phi_passed_col    = Monitored::Collection( "phi_passed"    , phi_passed_vec );
    auto avgmu_passed_col  = Monitored::Collection( "avgmu_passed"  , avgmu_passed_vec );
    auto npvtx_passed_col  = Monitored::Collection( "npvtx_passed"  , npvtx_passed_vec );
    

    for( auto pairObj : pairObjs ){
       
        auto acceptData = setAccept( pairObj.second, info );
        bool pid=true;
        bool isPassed = acceptData.getCutResult( level );
        float et=0.;
        const auto *eg = pairObj.first;
        ATH_MSG_DEBUG("Default pid " << pid << " te " << isPassed);
        if(xAOD::EgammaHelpers::isElectron(eg)){
            ATH_MSG_DEBUG("Offline Electron with pidword " << pidword);
            const xAOD::Electron* el =static_cast<const xAOD::Electron*> (eg);
            pid=el->auxdecor<bool>(pidword);
            ATH_MSG_DEBUG("Electron pid " << pid);
            et = getEt(el)/Gaudi::Units::GeV;
        }
        else  et=eg->caloCluster()->et()/Gaudi::Units::GeV;

        float eta = eg->caloCluster()->etaBE(2);
        float phi = eg->phi();
        float pt = eg->pt()/Gaudi::Units::GeV;
        float avgmu=lbAverageInteractionsPerCrossing( Gaudi::Hive::currentContext() );
        float npvtx=0.0;
        
        ATH_MSG_DEBUG("PID decision efficiency " << eg->auxdecor<bool>(pidword));
        
        if(pid){  
            et_vec.push_back( et );
            pt_vec.push_back( pt );
            highet_vec.push_back( et );

            if(et > etthr+1.0){
                eta_vec.push_back(eta);
                phi_vec.push_back(phi);
                avgmu_vec.push_back(avgmu);
                npvtx_vec.push_back(npvtx);
            }

            if(isPassed) {
                match_et_vec.push_back( et );
                match_pt_vec.push_back( pt );
                match_highet_vec.push_back( et );

                if(et > etthr+1.0){
                    match_eta_vec.push_back(eta);
                    match_phi_vec.push_back(phi);
                    match_avgmu_vec.push_back(avgmu);
                    match_npvtx_vec.push_back(npvtx);
                }

                et_passed_vec.push_back( true ); 
                pt_passed_vec.push_back( true ); 
                highet_passed_vec.push_back( true ); 

                if(et > etthr+1.0){
                    eta_passed_vec.push_back( true ); 
                    phi_passed_vec.push_back( true ); 
                    avgmu_passed_vec.push_back( true ); 
                    npvtx_passed_vec.push_back( true ); 
                }
            } // Passes Trigger selection
            else {

                et_passed_vec.push_back( false ); 
                pt_passed_vec.push_back( false ); 
                highet_passed_vec.push_back( false ); 

                if(et > etthr+1.0){
                    eta_passed_vec.push_back( false ); 
                    phi_passed_vec.push_back( false ); 
                    avgmu_passed_vec.push_back( false ); 
                    npvtx_passed_vec.push_back( false ); 
                }
            } // Fails Trigger selection
        } // Passes offline pid, fill histograms
    }

 
    fill( monGroup, et_col, highet_col, pt_col, eta_col, phi_col, avgmu_col, npvtx_col,
          match_et_col, match_highet_col, match_pt_col, match_eta_col, match_phi_col, match_avgmu_col, match_npvtx_col,
          et_passed_col, highet_passed_col, pt_passed_col, eta_passed_col, phi_passed_col, avgmu_passed_col, npvtx_passed_col);

}



// *********************************************************************************


void TrigEgammaMonitorAnalysisAlgorithm::fillDistributions( std::vector< std::pair< const xAOD::Egamma*, const TrigCompositeUtils::Decision * >> pairObjs,
                                                           const TrigInfo info ) const
{

  const std::string trigger = info.trigName;

  if (info.trigL1){
  
      //  Fill L1 features
      std::vector<const xAOD::EmTauRoI*> l1_vec;
      auto initRois =  tdt()->features<TrigRoiDescriptorCollection>(trigger,TrigDefs::includeFailedDecisions,"",
                                                                    TrigDefs::allFeaturesOfType,"initialRoI");       
      for( auto &initRoi: initRois ){               
        if( !initRoi.link.isValid() ) continue;      
        auto feat = match()->getL1Feature( initRoi.source );
        if(feat)
          l1_vec.push_back(feat);
      }

      fillL1Calo( trigger, l1_vec );

  
  }else{
 

    // Offline
    std::vector<const xAOD::Egamma*> eg_vec;
    std::vector<const xAOD::Electron*> el_vec;
    for( auto pairObj: pairObjs )
    {
        eg_vec.push_back(pairObj.first);
        if( xAOD::EgammaHelpers::isElectron(pairObj.first)){
            const xAOD::Electron* elOff = static_cast<const xAOD::Electron*> (pairObj.first);
            el_vec.push_back(elOff);
        }
    }
    
    // Offline
    fillShowerShapes( trigger, eg_vec, false );
    fillTracking( trigger, el_vec, false );
   


    if( m_tp ){
        
        if( info.trigType == "electron" ){
            // HLT Electron
            {
                std::vector<const xAOD::Electron*> el_vec;
                std::vector<const xAOD::Egamma*> eg_vec;
                auto vec =  tdt()->features<xAOD::ElectronContainer>(trigger,TrigDefs::includeFailedDecisions ,match()->key("Electron") );      
                for( auto &featLinkInfo : vec ){
                    const auto *feat = *(featLinkInfo.link);
                    if(!feat) continue;
                    // If not pass, continue
                    el_vec.push_back(feat);
                    eg_vec.push_back(feat);
                }
                fillShowerShapes( trigger, eg_vec, true );
                fillTracking( trigger, el_vec, true );
            }

        }else{
          ATH_MSG_WARNING( "Chain type not Electron for TP trigger" );
        }


    }else{

        // L1Calo
        {
          //  Fill L1 features
          std::vector<const xAOD::EmTauRoI*> l1_vec;
          auto initRois =  tdt()->features<TrigRoiDescriptorCollection>(trigger,TrigDefs::includeFailedDecisions,"",
                                                                        TrigDefs::allFeaturesOfType,"initialRoI");       
          for( auto &initRoi: initRois ){               
            if( !initRoi.link.isValid() ) continue;      
            auto feat = match()->getL1Feature( initRoi.source );
            if(feat)
              l1_vec.push_back(feat);
          }

          fillL1Calo( trigger, l1_vec );
        }
 

        // L2Calo
        {
          std::vector<const xAOD::TrigEMCluster*> emCluster_vec;
          auto vec =  tdt()->features<xAOD::TrigEMClusterContainer>(trigger,TrigDefs::includeFailedDecisions ,match()->key("L2Calo") );      
          for(auto &featLinkInfo : vec ){                                             
            if(! featLinkInfo.isValid() ) continue;
            const auto *feat = *(featLinkInfo.link);                   
            if(!feat) continue;
            emCluster_vec.push_back(feat);
          }
          fillL2Calo( trigger, emCluster_vec );
        }


        // EFCalo
        {
          std::vector<const xAOD::CaloCluster* > clus_vec;
          auto vec =  tdt()->features<xAOD::CaloClusterContainer>(trigger,TrigDefs::Physics ,match()->key("EFCalo") );      
          for(auto &featLinkInfo : vec ){                                             
            if(! featLinkInfo.isValid() ) continue;
            const auto *feat = *(featLinkInfo.link);                   
            if(!feat) continue;
            clus_vec.push_back(feat);
          } 
          fillEFCalo( trigger,  clus_vec );
        }


        if ( info.trigType == "electron" ){
            
            // L2 Electron
            {
                std::vector<const xAOD::TrigElectron*> el_vec;
                // Get only passed objects
                auto vec =  tdt()->features<xAOD::TrigElectronContainer>(trigger,TrigDefs::Physics ,match()->key("L2Electron") );      
                for( auto &featLinkInfo : vec ){
                    if(! featLinkInfo.isValid() ) continue;
                    const auto *feat = *(featLinkInfo.link);
                    if(!feat) continue;
                    el_vec.push_back(feat);
                }
                fillL2Electron( trigger, el_vec );
            }
 

            // HLT Electron
            {
                std::vector<const xAOD::Electron*> el_vec;
                std::vector<const xAOD::Egamma*> eg_vec;
                auto vec =  tdt()->features<xAOD::ElectronContainer>(trigger, TrigDefs::Physics ,match()->key("Electron") );      
                for( auto &featLinkInfo : vec ){
                    if(! featLinkInfo.isValid() ) continue;
                    const auto *feat = *(featLinkInfo.link);
                    if(!feat) continue;
                    el_vec.push_back(feat);
                    eg_vec.push_back(feat);
                }
                fillShowerShapes( trigger, eg_vec, true );
                fillTracking( trigger, el_vec, true );
            }


        }else if ( info.trigType == "photon"){
            // HLT Photon
            {
                std::vector<const xAOD::Egamma*> ph_vec;
                auto vec =  tdt()->features<xAOD::PhotonContainer>(trigger,TrigDefs::Physics ,match()->key("Photon") );      
                for( auto &featLinkInfo : vec ){
                    if(! featLinkInfo.isValid() ) continue;
                    const auto *feat = *(featLinkInfo.link);
                    if(!feat) continue;
                    ph_vec.push_back(feat);
                }
                fillShowerShapes( trigger, ph_vec, true );
            }
        }else{
            ATH_MSG_INFO( "Chain type not specified" );
        }

    }
  
  }
}




void TrigEgammaMonitorAnalysisAlgorithm::fillL1Calo( const std::string &trigger, std::vector< const xAOD::EmTauRoI* > l1_vec ) const 
{
    auto monGroup = getGroup(trigger+"_Distributions_L1Calo");

    std::vector<float> eta_vec, phi_vec, energy_vec, roi_et_vec, emIso_vec, hadCore_vec;

    auto eta_col      = Monitored::Collection( "eta"     , eta_vec       );
    auto phi_col      = Monitored::Collection( "phi"     , phi_vec       );
    auto energy_col   = Monitored::Collection( "energy"  , energy_vec    );
    auto roi_et_col   = Monitored::Collection( "roi_et"  , roi_et_vec    );
    auto emIso_col    = Monitored::Collection( "emIso"   , emIso_vec     );
    auto hadCore_col  = Monitored::Collection( "hadCore" , hadCore_vec  );

    for( auto l1 : l1_vec )
    {
      if(!l1)  continue;
      eta_vec.push_back( l1->eta() );
      phi_vec.push_back( l1->phi() );
      energy_vec.push_back( l1->emClus()/Gaudi::Units::GeV );
      roi_et_vec.push_back( l1->eT()/Gaudi::Units::GeV );
      emIso_vec.push_back( l1->emIsol()/Gaudi::Units::GeV );
      hadCore_vec.push_back( l1->hadCore()/Gaudi::Units::GeV );
    }

    fill( monGroup, eta_col, phi_col, energy_col, roi_et_col, emIso_col, hadCore_col );

}





void TrigEgammaMonitorAnalysisAlgorithm::fillL2Calo(const std::string &trigger, std::vector< const xAOD::TrigEMCluster *> emCluster_vec) const
{
    auto monGroup = getGroup(trigger+"_Distributions_L2Calo");
    
    std::vector<float> et_vec, eta_vec, phi_vec;
    
    auto et_col   = Monitored::Collection("et" , et_vec  );    
    auto eta_col  = Monitored::Collection("eta", eta_vec );    
    auto phi_col  = Monitored::Collection("phi", phi_vec );    
    
    for ( auto emCluster : emCluster_vec )
    {
      if(!emCluster)  continue;
      et_vec.push_back(  emCluster->et()/Gaudi::Units::GeV );
      eta_vec.push_back( emCluster->eta() );
      phi_vec.push_back( emCluster->phi() );
    }

    fill( monGroup, et_col, eta_col, phi_col );


}



void TrigEgammaMonitorAnalysisAlgorithm::fillL2Electron(const std::string &trigger, std::vector< const xAOD::TrigElectron* > el_vec) const
{
 
    auto monGroup = getGroup(trigger+"_Distributions_L2Electron");
    
    std::vector<float> et_vec, eta_vec, phi_vec;
    
    auto et_col   = Monitored::Collection("et" , et_vec  );    
    auto eta_col  = Monitored::Collection("eta", eta_vec );    
    auto phi_col  = Monitored::Collection("phi", phi_vec );    
    
    for ( auto el : el_vec )
    {
      if(!el)  continue;
      et_vec.push_back( el->pt()/Gaudi::Units::GeV );
      eta_vec.push_back( el->eta() );
      phi_vec.push_back( el->phi() );
    }

    fill( monGroup, et_col, eta_col, phi_col );
}

void TrigEgammaMonitorAnalysisAlgorithm::fillEFCalo(const std::string &trigger, std::vector< const xAOD::CaloCluster*> clus_vec) const
{
    
    auto monGroup = getGroup( trigger + "_Distributions_EFCalo" );
    
   
    std::vector<float> energyBE0_vec, energyBE1_vec, energyBE2_vec, energyBE3_vec, 
      energy_vec, et_vec, eta_vec, phi_vec, eta_calo_vec, phi_calo_vec;
   


    auto energyBE0_col  = Monitored::Collection("energyBE0", energyBE0_vec);
    auto energyBE1_col  = Monitored::Collection("energyBE1", energyBE1_vec);
    auto energyBE2_col  = Monitored::Collection("energyBE2", energyBE2_vec);
    auto energyBE3_col  = Monitored::Collection("energyBE3", energyBE3_vec);
    auto energy_col     = Monitored::Collection("energy"   , energy_vec );
    auto et_col         = Monitored::Collection("et"       , et_vec );
    auto eta_col        = Monitored::Collection("eta"      , eta_vec );
    auto phi_col        = Monitored::Collection("phi"      , phi_vec );
    auto eta_calo_col   = Monitored::Collection("eta_calo" , eta_calo_vec );
    auto phi_calo_col   = Monitored::Collection("phi_calo" , phi_calo_vec );

    for ( auto clus : clus_vec )
    {
        double tmpeta = -999.;
        if(!clus->retrieveMoment(xAOD::CaloCluster::ETACALOFRAME,tmpeta))
            tmpeta=-999.;
        double tmpphi = -999.;
        if(!clus->retrieveMoment(xAOD::CaloCluster::PHICALOFRAME,tmpphi))
            tmpphi=-999.;
     
        energyBE0_vec.push_back( clus->energyBE(0)/Gaudi::Units::GeV ); 
        energyBE1_vec.push_back( clus->energyBE(1)/Gaudi::Units::GeV ); 
        energyBE2_vec.push_back( clus->energyBE(2)/Gaudi::Units::GeV ); 
        energyBE3_vec.push_back( clus->energyBE(3)/Gaudi::Units::GeV ); 
        energy_vec.push_back( clus->e()/Gaudi::Units::GeV ); 
        et_vec.push_back( clus->et()/Gaudi::Units::GeV ); 
        eta_vec.push_back( clus->eta() );
        phi_vec.push_back( clus->phi() );
        eta_calo_vec.push_back( tmpeta );
        phi_calo_vec.push_back( tmpphi );

    }

 
    fill( monGroup,  energyBE0_col, energyBE1_col, energyBE2_col, energyBE3_col, 
      energy_col, et_col, eta_col, phi_col, eta_calo_col, phi_calo_col);
}





void TrigEgammaMonitorAnalysisAlgorithm::fillShowerShapes(const std::string &trigger, std::vector<const xAOD::Egamma*> eg_vec , bool online) const 
{
    
    ATH_MSG_DEBUG("Fill SS distributions: " << trigger);
    auto monGroup = getGroup( trigger + ( online ? "_Distributions_HLT" : "_Distributions_Offline") );
    
    std::vector<float> ethad_vec, ethad1_vec, Rhad_vec, Rhad1_vec, Reta_vec, Rphi_vec, weta1_vec, weta2_vec, 
      f1_vec, f3_vec, eratio_vec, et_vec, highet_vec , eta_vec, phi_vec, topoetcone20_vec, topoetcone40_shift_vec, 
      topoetcone20_rel_vec, topoetcone40_shift_rel_vec;
 

    auto ethad_col              = Monitored::Collection("ethad"    , ethad_vec );
    auto ethad1_col             = Monitored::Collection("ethad1"   , ethad1_vec );
    auto Rhad_col               = Monitored::Collection("Rhad"     , Rhad_vec    );
    auto Rhad1_col              = Monitored::Collection("Rhad1"    , Rhad1_vec   );
    auto Reta_col               = Monitored::Collection("Reta"     , Reta_vec    );
    auto Rphi_col               = Monitored::Collection("Rphi"     , Rphi_vec    );
    auto weta1_col              = Monitored::Collection("weta1"    , weta1_vec   );
    auto weta2_col              = Monitored::Collection("weta2"    , weta2_vec   );
    auto f1_col                 = Monitored::Collection("f1"       , f1_vec      );
    auto f3_col                 = Monitored::Collection("f3"       , f3_vec      );
    auto eratio_col             = Monitored::Collection("eratio"   , eratio_vec  );
    auto et_col                 = Monitored::Collection("et"       , et_vec      );
    auto highet_col             = Monitored::Collection("highet"   , highet_vec  );
    auto eta_col                = Monitored::Collection("eta"      , eta_vec     );
    auto phi_col                = Monitored::Collection("phi"      , phi_vec     );
    auto topoetcone20_col       = Monitored::Collection("topoetcone20", topoetcone20_vec);
    auto topoetcone40_shift_col = Monitored::Collection("topoetcone40_shift",  topoetcone40_shift_vec );
    auto topoetcone20_rel_col   = Monitored::Collection("topoetcone20_rel", topoetcone20_rel_vec);
    auto topoetcone40_shift_rel_col   = Monitored::Collection("topoetcone40_shift_rel",  topoetcone40_shift_rel_vec );
     
    for ( auto eg : eg_vec ){

        if(!eg) continue;

        ethad_vec.push_back( getShowerShape_ethad(eg)/Gaudi::Units::GeV);
        ethad1_vec.push_back( getShowerShape_ethad1(eg)/Gaudi::Units::GeV);
        Rhad_vec.push_back( getShowerShape_Rhad(eg));
        Rhad1_vec.push_back( getShowerShape_Rhad(eg));
        Reta_vec.push_back( getShowerShape_Reta(eg));
        Rphi_vec.push_back( getShowerShape_Rphi(eg));
        weta1_vec.push_back( getShowerShape_weta1(eg));
        weta2_vec.push_back( getShowerShape_weta2(eg));
        f1_vec.push_back( getShowerShape_f1(eg));
        f3_vec.push_back( getShowerShape_f3(eg));
        eratio_vec.push_back( getShowerShape_Eratio(eg));
        et_vec.push_back( eg->pt()/Gaudi::Units::GeV);
        highet_vec.push_back( eg->pt()/Gaudi::Units::GeV);
        eta_vec.push_back( eg->eta());
        phi_vec.push_back( eg->phi());
        topoetcone20_vec.push_back( getIsolation_topoetcone20(eg)/Gaudi::Units::GeV);
        topoetcone40_shift_vec.push_back( (getIsolation_topoetcone40(eg)-2450)/Gaudi::Units::GeV );
        
        if (eg->pt() > 0) {
            topoetcone20_rel_vec.push_back( getIsolation_topoetcone20(eg)/eg->pt());
            topoetcone40_shift_rel_vec.push_back( (getIsolation_topoetcone40(eg)-2450)/eg->pt() );       
        }

    }// Loop over egamma objects

    fill( monGroup, ethad_col, ethad1_col, Rhad_col, Rhad1_col, Reta_col, Rphi_col, weta1_col, weta2_col, 
          f1_col, f3_col, eratio_col, et_col, highet_col , eta_col, phi_col, topoetcone20_col, topoetcone40_shift_col, 
          topoetcone20_rel_col, topoetcone40_shift_rel_col );

}


void TrigEgammaMonitorAnalysisAlgorithm::fillTracking(const std::string &trigger, std::vector< const xAOD::Electron *> eg_vec, bool online ) const
{

    ATH_MSG_DEBUG("Fill tracking");
    
    auto monGroup = getGroup( trigger + ( online ? "_Distributions_HLT" : "_Distributions_Offline") );
    
    std::vector<float> deta1_vec, deta1_EMECA_vec, deta1_EMECC_vec, deta1_EMEBA_vec, deta1_EMEBC_vec, deta2_vec, dphi2_vec,
      dphiresc_vec, eprobht_vec, npixhits_vec, nscthits_vec, charge_vec, ptcone20_vec, ptvarcone20_vec, d0_vec, d0sig_vec,
      pt_vec, ptcone20_rel_vec, ptvarcone20_rel_vec;

    auto deta1_col            = Monitored::Collection( "deta1"       , deta1_vec           );
    auto deta1_EMECA_col      = Monitored::Collection( "deta1_EMECA" , deta1_EMECA_vec     );
    auto deta1_EMECC_col      = Monitored::Collection( "deta1_EMECC" , deta1_EMECC_vec     );
    auto deta1_EMEBA_col      = Monitored::Collection( "deta1_EMEBA" , deta1_EMEBA_vec     );
    auto deta1_EMEBC_col      = Monitored::Collection( "deta1_EMEBC" , deta1_EMEBC_vec     );
    auto deta2_col            = Monitored::Collection( "deta2"       , deta2_vec           );
    auto dphi2_col            = Monitored::Collection( "dphi2"       , dphi2_vec           );
    auto dphiresc_col         = Monitored::Collection( "dphiresc"    , dphiresc_vec        );
    auto eprobht_col          = Monitored::Collection( "eprobht"     , eprobht_vec         );
    auto npixhits_col         = Monitored::Collection( "npixhits"    , npixhits_vec        );
    auto nscthits_col         = Monitored::Collection( "nscthits"    , nscthits_vec        );
    auto charge_col           = Monitored::Collection( "charge"      , charge_vec          );
    auto ptcone20_col         = Monitored::Collection( "ptcone20"    , ptcone20_vec        );
    auto ptvarcone20_col      = Monitored::Collection( "ptvarcone20" , ptvarcone20_vec     );
    auto d0_col               = Monitored::Collection( "d0"          , d0_vec              );
    auto d0sig_col            = Monitored::Collection( "d0sig"       , d0sig_vec           );
    auto pt_col               = Monitored::Collection( "pt"          , pt_vec              );
    auto ptcone20_rel_col     = Monitored::Collection( "ptcone20_rel", ptcone20_rel_vec    );
    auto ptvarcone20_rel_col  = Monitored::Collection( "ptvarcone20" , ptvarcone20_rel_vec );


    for ( auto eg : eg_vec ){
    
      if(!eg)  continue;

      float cleta = 0.;
      if(eg->caloCluster()) cleta=eg->caloCluster()->eta();
      else cleta=eg->eta();
      
      deta1_vec.push_back( getCaloTrackMatch_deltaEta1(eg));

      if(cleta > 1.375 && cleta < 3.2)
          deta1_EMECA_vec.push_back( getCaloTrackMatch_deltaEta1(eg));
      if(cleta < -1.375 && cleta > -3.2)
          deta1_EMECC_vec.push_back( getCaloTrackMatch_deltaEta1(eg));
      if(cleta > 0 && cleta < 1.375)
          deta1_EMEBA_vec.push_back( getCaloTrackMatch_deltaEta1(eg));
      if(cleta < 0 && cleta > -1.375)
          deta1_EMEBC_vec.push_back( getCaloTrackMatch_deltaEta1(eg));
      
      deta2_vec.push_back( getCaloTrackMatch_deltaEta2(eg));
      dphi2_vec.push_back(  getCaloTrackMatch_deltaPhi2(eg));
      dphiresc_vec.push_back( getCaloTrackMatch_deltaPhiRescaled2(eg));
      eprobht_vec.push_back( getTrackSummaryFloat_eProbabilityHT(eg));
      npixhits_vec.push_back( getTrackSummary_numberOfPixelHits(eg));
      nscthits_vec.push_back( getTrackSummary_numberOfSCTHits(eg));
      charge_vec.push_back( eg->charge());
      ptcone20_vec.push_back( getIsolation_ptcone20(eg)/Gaudi::Units::GeV);
      ptvarcone20_vec.push_back( getIsolation_ptvarcone20(eg)/Gaudi::Units::GeV);
      
      // Quantities directly from tracks
      ATH_MSG_DEBUG("Get track Quantities");
      d0_vec.push_back( getTrack_d0(eg));
      d0sig_vec.push_back(getD0sig(eg));
      pt_vec.push_back( getTrack_pt(eg)/Gaudi::Units::GeV);
      
      if (eg->pt() > 0) {
          ptcone20_rel_vec.push_back( getIsolation_ptcone20(eg)/eg->pt());
          ptvarcone20_rel_vec.push_back(  getIsolation_ptvarcone20(eg)/eg->pt());
      }

    }
    
    
    fill( monGroup, deta1_col, deta1_EMECA_col, deta1_EMECC_col, deta1_EMEBA_col, deta1_EMEBC_col, deta2_col, dphi2_col,
      dphiresc_col, eprobht_col, npixhits_col, nscthits_col, charge_col, ptcone20_col, ptvarcone20_col, d0_col, d0sig_col,
      pt_col, ptcone20_rel_col, ptvarcone20_rel_col);
}



//!***********************************************************************************************************************8
//
//


void TrigEgammaMonitorAnalysisAlgorithm::fillResolutions( std::vector< std::pair< const xAOD::Egamma*, const TrigCompositeUtils::Decision * >> pairObjs,
                                                          const TrigInfo info ) const
{

  std::vector< std::pair< const xAOD::Egamma*, const xAOD::EmTauRoI * >> pair_l1_vec;
  std::vector< std::pair< const xAOD::Egamma*, const TrigCompositeUtils::Decision * >> pair_eg_vec;
  const std::string trigger = info.trigName;
  bool filliso=false;
  if(boost::contains(info.trigName,"iloose") || boost::contains(info.trigName,"ivarloose")) filliso=true;
  if(boost::contains(info.trigName,"icaloloose") || boost::contains(info.trigName,"icalovloose") || boost::contains(info.trigName,"icalotight")) filliso=true;


  for( auto pairObj : pairObjs ){

    const xAOD::Egamma* eg = pairObj.first;
    const auto *feat = pairObj.second;

    if (feat){
      if (info.trigL1){
        // Get l1 for all level one objects found for each off object
        auto l1 = match()->getL1Feature( feat );
        if(l1){
          pair_l1_vec.push_back( std::make_pair(eg,l1) );
        }
      }else{
        //
        // Get only off and l1 where the offline object passed by the offline pid selector
        //
        auto l1 = match()->getL1Feature( feat  );
        if(eg->type()==xAOD::Type::Electron){
          const xAOD::Electron* el = static_cast<const xAOD::Electron*>(eg);
          float et = getEt(el)/Gaudi::Units::GeV;
          if( et < info.trigThrHLT-5.0) continue;
          if(!eg->auxdecor<bool>(info.trigPidDecorator)) continue;
          pair_eg_vec.push_back(std::make_pair(el,feat));
          if(l1)  pair_l1_vec.push_back( std::make_pair(eg,l1) );
        }
        else if(eg->type()==xAOD::Type::Photon){
          float et = getCluster_et(eg)/Gaudi::Units::GeV;
          if( et < info.trigThrHLT-5.0) continue;
          pair_eg_vec.push_back(std::make_pair(eg,feat));
          if(l1)  pair_l1_vec.push_back( std::make_pair(eg,l1) );
        }
      }

    }
  }


  if (info.trigL1){
    fillL1CaloResolution( trigger, pair_l1_vec );
    fillL1CaloAbsResolution( trigger, pair_l1_vec );
  }else{
   
    // Fill L1Calo for all level 1 objects found
    fillL1CaloResolution( trigger, pair_l1_vec );
    fillL1CaloAbsResolution( trigger, pair_l1_vec );
    fillL2CaloResolution( trigger, pair_eg_vec ); 
    
    // Fill HLT electron for all onl objects found
    if ( info.trigType=="electron"){
      fillHLTElectronResolution( trigger, pair_eg_vec, filliso ); 
    }

    else if ( info.trigType=="photon"){
      fillHLTPhotonResolution( trigger, pair_eg_vec, filliso );
    }


  }

}




void TrigEgammaMonitorAnalysisAlgorithm::fillL1CaloResolution(const std::string &trigger,
                                                              std::vector< std::pair< const xAOD::Egamma*, const xAOD::EmTauRoI * >> pairObjs ) const
{
    auto monGroup = getGroup( trigger + "_Resolutions_L1Calo" );
    
    std::vector<float> eta_vec, res_et_vec;

    auto eta_col    = Monitored::Collection( "eta"      , eta_vec        );
    auto res_et_col = Monitored::Collection( "res_et"   , res_et_vec     );
 

    for (auto& pairObj : pairObjs){
      auto off = pairObj.first;
      auto l1 = pairObj.second;
      ATH_MSG_DEBUG("Fill L1CaloResolution");
      if(off->type()==xAOD::Type::Electron){
        const xAOD::Electron* eloff =static_cast<const xAOD::Electron*> (off);
        eta_vec.push_back( l1->eta() );
        res_et_vec.push_back( (l1->emClus()-getEt(eloff))/getEt(eloff) ) ;

      }
    }

    fill( monGroup, eta_col, res_et_col );
}





void TrigEgammaMonitorAnalysisAlgorithm::fillL1CaloAbsResolution(const std::string &trigger,
                                                                 std::vector< std::pair< const xAOD::Egamma*, const xAOD::EmTauRoI * >> pairObjs ) const
{
    auto monGroup = getGroup( trigger + "_AbsResolutions_L1Calo" );
    
    std::vector<float> eta_vec, res_et_vec;

    auto eta_col    = Monitored::Collection( "eta"      , eta_vec        );
    auto res_et_col = Monitored::Collection( "res_et"   , res_et_vec     );
 

    for (auto& pairObj : pairObjs){
      auto off = pairObj.first;
      auto l1 = pairObj.second;
      ATH_MSG_DEBUG("Fill L1CaloAbsResolution");
      if(off->type()==xAOD::Type::Electron){
        const xAOD::Electron* eloff =static_cast<const xAOD::Electron*> (off);
        eta_vec.push_back( l1->eta() );
        res_et_vec.push_back( (l1->emClus()-getEt(eloff))/Gaudi::Units::GeV ) ;
      }
    }

    fill( monGroup, eta_col, res_et_col );
}




void TrigEgammaMonitorAnalysisAlgorithm::fillHLTElectronResolution(const std::string &trigger,
                                                        std::vector< std::pair< const xAOD::Egamma*, const TrigCompositeUtils::Decision * >> pairObjs,
                                                        bool filliso) const
{

    auto monGroup = getGroup( trigger + "_Resolutions_HLT" );

    std::vector<float> res_pt_vec, res_et_vec, res_phi_vec, res_eta_vec, res_deta1_vec, res_deta2_vec, res_dphi2_vec, res_dphiresc_vec,
    res_d0_vec, res_d0sig_vec, res_eprobht_vec, res_npixhits_vec, res_nscthits_vec, res_Rhad_vec, res_Rhad1_vec, res_Reta_vec,
    res_Rphi_vec, res_weta1_vec, res_weta2_vec, res_wtots1_vec, res_f1_vec, res_f3_vec, res_eratio_vec, res_ethad_vec, res_ethad1_vec,
    et_vec, eta_vec, mu_vec;
    std::vector<float> res_ptcone20_vec, res_ptcone20_rel_vec, res_ptvarcone20_vec, res_ptvarcone20_rel_vec;
    std::vector<float> res_etInEta0_vec, res_etInEta1_vec, res_etInEta2_vec, res_etInEta3_vec;


    auto et_col           = Monitored::Collection( "et"             , et_vec                );
    auto eta_col          = Monitored::Collection( "eta"            , eta_vec               );
    auto mu_col           = Monitored::Collection( "mu"             , mu_vec                );

    // For calo
    auto res_et_col       = Monitored::Collection( "res_et"         , res_et_vec            );
    auto res_eta_col      = Monitored::Collection( "res_eta"        , res_eta_vec           );
    auto res_phi_col      = Monitored::Collection( "res_phi"        , res_phi_vec           );
    auto res_ethad_col    = Monitored::Collection( "res_ethad"      , res_ethad_vec         );
    auto res_ethad1_col   = Monitored::Collection( "res_ethad1"     , res_ethad1_vec        );
    auto res_Rhad_col     = Monitored::Collection( "res_Rhad"       , res_Rhad_vec          );
    auto res_Rhad1_col    = Monitored::Collection( "res_Rhad1"      , res_Rhad1_vec         );
    auto res_Reta_col     = Monitored::Collection( "res_Reta"       , res_Reta_vec          );
    auto res_Rphi_col     = Monitored::Collection( "res_Rphi"       , res_Rphi_vec          );
    auto res_weta1_col    = Monitored::Collection( "res_weta1"      , res_weta1_vec         );
    auto res_weta2_col    = Monitored::Collection( "res_weta2"      , res_weta2_vec         );
    auto res_wtots1_col   = Monitored::Collection( "res_wtots1"     , res_wtots1_vec        );
    auto res_f1_col       = Monitored::Collection( "res_f1"         , res_f1_vec            );
    auto res_f3_col       = Monitored::Collection( "res_f3"         , res_f3_vec            );
    auto res_eratio_col   = Monitored::Collection( "res_eratio"     , res_eratio_vec        );

    auto res_etInEta0_col       = Monitored::Collection( "res_etInEta0"         , res_etInEta0_vec      );
    auto res_etInEta1_col       = Monitored::Collection( "res_etInEta1"         , res_etInEta1_vec      );
    auto res_etInEta2_col       = Monitored::Collection( "res_etInEta2"         , res_etInEta2_vec      );
    auto res_etInEta3_col       = Monitored::Collection( "res_etInEta3"         , res_etInEta3_vec      );
 
    // For electron 
    auto res_pt_col               = Monitored::Collection( "res_pt"                   , res_pt_vec                  );
    auto res_deta1_col            = Monitored::Collection( "res_deta1"                , res_deta1_vec               );
    auto res_deta2_col            = Monitored::Collection( "res_deta2"                , res_deta2_vec               );
    auto res_dphi2_col            = Monitored::Collection( "res_dphi2"                , res_dphi2_vec               );
    auto res_dphiresc_col         = Monitored::Collection( "res_dphiresc"             , res_dphiresc_vec            );
    auto res_d0_col               = Monitored::Collection( "res_d0"                   , res_d0_vec                  );
    auto res_d0sig_col            = Monitored::Collection( "res_d0sig"                , res_d0sig_vec               );
    auto res_eprobht_col          = Monitored::Collection( "res_eprobht"              , res_eprobht_vec             );
    auto res_npixhits_col         = Monitored::Collection( "res_npixhits"             , res_npixhits_vec            );
    auto res_nscthits_col         = Monitored::Collection( "res_nscthits"             , res_nscthits_vec            );
    auto res_ptcone20_col         = Monitored::Collection( "res_ptcone20"             , res_ptcone20_vec            );
    auto res_ptcone20_rel_col     = Monitored::Collection( "res_ptcone20_rel"         , res_ptcone20_rel_vec        );
    auto res_ptvarcone20_col      = Monitored::Collection( "res_ptvarcone20"          , res_ptvarcone20_vec         );
    auto res_ptvarcone20_rel_col  = Monitored::Collection( "res_ptvarcone20_rel"      , res_ptvarcone20_rel_vec     );




    // Check for zero before filling
    ATH_MSG_DEBUG("Fill Resolution");



    for ( auto& pairObj : pairObjs ){

      const xAOD::Electron *off = static_cast<const xAOD::Electron*>(pairObj.first);
      const xAOD::Electron *onl=nullptr;


      { // Get the closest electron object from the trigger starting with deltaR = 0.15
        float maxDeltaR=0.05;
        auto vec =  tdt()->features<xAOD::ElectronContainer>(trigger,TrigDefs::Physics ,match()->key("Electron") );      
        for(auto &featLinkInfo : vec ){                                             
          if(! featLinkInfo.isValid() ) continue;
          const auto *feat = *(featLinkInfo.link);                   
          if(!feat) continue;
          float deltaR = dR( off->eta(), off->phi(), feat->eta(), feat->phi() );
          if( deltaR < maxDeltaR){
            maxDeltaR=deltaR;
            onl=feat;
          }
        } 
      }
      
      if(!onl)  continue;

      float val_off=0.;
      const float onl_eta=onl->eta();
      const float feta = abs(onl_eta);
      const float onl_et = getEt(onl)/Gaudi::Units::GeV;
      const float avgmu=lbAverageInteractionsPerCrossing( Gaudi::Hive::currentContext() );
      const float dummy=dummy;

      eta_vec.push_back( onl_eta );
      et_vec.push_back( onl_et );
      mu_vec.push_back( avgmu );

      val_off=getTrack_pt(off);
      if(val_off!=0.){
          res_pt_vec.push_back( (getTrack_pt(off)-val_off)/val_off );
      }else{
      }


      val_off=getEt(off);
      if(val_off!=0.){
          res_et_vec.push_back( (getEt(onl)-val_off)/val_off  );
          if( feta < 1.37 )
              res_etInEta0_vec.push_back((getEt(onl)-val_off)/val_off);
          else if( feta >=1.37 && feta <= 1.52 )
              res_etInEta1_vec.push_back((getEt(onl)-val_off)/val_off);
          else if( feta >= 1.55 && feta < 1.8 )
              res_etInEta2_vec.push_back((getEt(onl)-val_off)/val_off);
          else if( feta >= 1.8 && feta < 2.45 )
              res_etInEta3_vec.push_back((getEt(onl)-val_off)/val_off);
      }

      val_off=off->eta();
      if(val_off!=0.){
          res_eta_vec.push_back( (onl_eta-val_off)/val_off );
      }else{
          res_eta_vec.push_back( dummy );
      }

      val_off=off->phi();
      if(val_off!=0.){ 
          res_phi_vec.push_back(  (onl->phi()-val_off)/val_off );
      }else{
          res_phi_vec.push_back(dummy );
      }

      val_off=getShowerShape_ethad(off);
      if(val_off!=0.) {
          res_ethad_vec.push_back((getShowerShape_ethad(onl)-val_off)/val_off);
      }else{
          res_ethad_vec.push_back( dummy );
      }

      val_off=getShowerShape_ethad1(off);
      if(val_off!=0){
          res_ethad1_vec.push_back((getShowerShape_ethad1(onl)-val_off)/val_off);
      }else{
          res_ethad1_vec.push_back( dummy);
      }

      val_off=getShowerShape_Rhad(off);
      if(val_off!=0.){
          res_Rhad_vec.push_back(  (getShowerShape_Rhad(onl)-val_off)/val_off );
      }else{
          res_Rhad_vec.push_back( dummy );
      }

      val_off=getShowerShape_Rhad1(off);
      if(val_off!=0.){
          res_Rhad1_vec.push_back(  (getShowerShape_Rhad1(onl)-val_off)/val_off );
      }else{
          res_Rhad1_vec.push_back( dummy );
      }

      val_off=getShowerShape_Reta(off);
      if(val_off!=0.){
          res_Reta_vec.push_back(  (getShowerShape_Reta(onl)-val_off)/val_off );
      }else{
          res_Reta_vec.push_back( dummy );
      }

      val_off=getShowerShape_Rphi(off);
      if(val_off!=0.){
          res_Rphi_vec.push_back(  (getShowerShape_Rphi(onl)-val_off)/val_off );
      }else{
          res_Rphi_vec.push_back(  (getShowerShape_Rphi(onl)-val_off)/val_off );
      }

      val_off=getShowerShape_weta1(off);
      if(val_off!=0.){
          res_weta1_vec.push_back( (getShowerShape_weta1(onl)-val_off)/val_off );
      }else{
          res_weta1_vec.push_back( dummy );
      }

      val_off=getShowerShape_weta2(off);
      if(val_off!=0.){
          res_weta2_vec.push_back( (getShowerShape_weta2(onl)-val_off)/val_off );
      }else{
          res_weta2_vec.push_back( dummy );
      }

      val_off=getShowerShape_wtots1(off);
      if(val_off!=0.){
          res_wtots1_vec.push_back( (getShowerShape_wtots1(onl)-val_off)/val_off );
      }else{
          res_wtots1_vec.push_back( dummy );
      }

      val_off=getShowerShape_f1(off);
      if(val_off!=0.){
          res_f1_vec.push_back(  (getShowerShape_f1(onl)-val_off)/val_off );
      }else{
          res_f1_vec.push_back(dummy );
      }

      val_off=getShowerShape_f3(off);
      if(val_off!=0.){
          res_f3_vec.push_back( (getShowerShape_f3(onl)-val_off)/val_off );
      }else{
          res_f3_vec.push_back( dummy );
      }

      val_off=getShowerShape_Eratio(off);
      if(val_off!=0.){
          res_eratio_vec.push_back(  (getShowerShape_Eratio(onl)-val_off)/val_off );
      }else{
          res_eratio_vec.push_back( dummy );
      }


      //
      // Track variables
      //

      val_off=getTrack_pt(off);
      if(val_off!=0.){
        res_pt_vec.push_back(  (getTrack_pt(onl)-val_off)/val_off );
      }else{
        res_pt_vec.push_back( dummy );
      }
        
      val_off=getEt(off);
      if(val_off!=0.) {
        res_et_vec.push_back(  (getEt(onl)-val_off)/val_off );
      }else{
        res_et_vec.push_back(  dummy );
      }
      
      val_off=getCaloTrackMatch_deltaEta1(off);
      if(val_off!=0.) {
        res_deta1_vec.push_back( (getCaloTrackMatch_deltaEta1(onl)-val_off)/val_off );
      }else{
        res_deta1_vec.push_back( dummy );
      }
      
      val_off=getCaloTrackMatch_deltaEta2(off);
      res_deta2_vec.push_back( (getCaloTrackMatch_deltaEta2(onl)-val_off)/val_off );
      val_off=getCaloTrackMatch_deltaPhi2(off);
      if(val_off!=0.) {
        res_dphi2_vec.push_back(  (getCaloTrackMatch_deltaPhi2(onl)-val_off)/val_off );
      }else{
        res_dphi2_vec.push_back(dummy );
      }
      
      val_off=getCaloTrackMatch_deltaPhiRescaled2(off);
      res_dphiresc_vec.push_back( (getCaloTrackMatch_deltaPhiRescaled2(onl)-val_off)/val_off );
      // Absolute resolution for impact parameter
      val_off=getTrack_d0(off);
      if(val_off!=0.) {
        res_d0_vec.push_back(  getTrack_d0(onl)-val_off );
      }else{
        res_d0_vec.push_back(  dummy );
      }
      
      val_off=getD0sig(off);
      if(val_off!=0.) {
        res_d0sig_vec.push_back(  getD0sig(onl)-val_off );
      }else{
        res_d0sig_vec.push_back(  dummy );
      }
      
      // Absolute resolution on track summary ints/floats 
      val_off=getTrackSummaryFloat_eProbabilityHT(off);
      res_eprobht_vec.push_back( (getTrackSummaryFloat_eProbabilityHT(onl)-val_off) );
      res_npixhits_vec.push_back( getTrackSummary_numberOfPixelHits(onl)-getTrackSummary_numberOfPixelHits(onl) );
      res_nscthits_vec.push_back( getTrackSummary_numberOfSCTHits(onl)-getTrackSummary_numberOfSCTHits(onl) );



      
      if(filliso){

        float val_off=getIsolation_ptcone20(off);
        if (val_off > 0.) {
            res_ptcone20_vec.push_back((getIsolation_ptcone20(onl)-val_off)/val_off);
            if (getEt(onl) > 0. && getEt(off) > 0.) {
                const float reliso_onl=getIsolation_ptcone20(onl)/getEt(onl);
                const float reliso_off=getIsolation_ptcone20(off)/getEt(off);
                res_ptcone20_rel_vec.push_back((reliso_onl-reliso_off)/reliso_off);
            }else{
                res_ptcone20_rel_vec.push_back(dummy);
            }
        }else{
            res_ptcone20_vec.push_back(dummy);
            res_ptcone20_rel_vec.push_back(dummy);
        }

        // ptvarcone20 isolation
        val_off=getIsolation_ptvarcone20(off);
        if (val_off > 0.) {
            if (getEt(onl) > 0. && getEt(off) > 0.) {
                res_ptvarcone20_vec.push_back((getIsolation_ptvarcone20(onl)-val_off)/val_off);
                const float reliso_onl=getIsolation_ptvarcone20(onl)/getEt(onl);
                const float reliso_off=getIsolation_ptvarcone20(off)/getEt(off);
                res_ptvarcone20_rel_vec.push_back((reliso_onl-reliso_off)/reliso_off);
            }else{
                res_ptvarcone20_rel_vec.push_back(dummy);
            }
        }else{
            res_ptvarcone20_vec.push_back(dummy);
            res_ptvarcone20_rel_vec.push_back(dummy);
        }

      }





    } // Loop over all offline objects

    // Fill everything
    fill( monGroup        ,
          et_col          ,
          eta_col         ,
          mu_col          ,
          res_pt_col      , 
          res_et_col      , 
          res_eta_col     , 
          res_phi_col     , 
          res_deta1_col   , 
          res_deta2_col   , 
          res_dphi2_col   , 
          res_dphiresc_col, 
          res_d0_col      , 
          res_d0sig_col   , 
          res_eprobht_col , 
          res_npixhits_col, 
          res_nscthits_col, 
          res_ethad_col   , 
          res_ethad1_col  , 
          res_Rhad_col    , 
          res_Rhad1_col   , 
          res_Reta_col    , 
          res_Rphi_col    , 
          res_weta1_col   , 
          res_weta2_col   , 
          res_wtots1_col  , 
          res_f1_col      , 
          res_f3_col      , 
          res_eratio_col  , 
          res_ptcone20_col        , 
          res_ptcone20_rel_col    , 
          res_ptvarcone20_col     , 
          res_ptvarcone20_rel_col ,
          res_etInEta0_col,
          res_etInEta1_col,
          res_etInEta2_col,
          res_etInEta3_col ); 



}





void TrigEgammaMonitorAnalysisAlgorithm::fillHLTPhotonResolution(const std::string &trigger,
                                                        std::vector< std::pair< const xAOD::Egamma*, const TrigCompositeUtils::Decision * >> pairObjs, 
                                                        bool filliso) const
{

    auto monGroup = getGroup( trigger + "_Resolutions_HLT" );

    std::vector<float> res_phi_vec, res_eta_vec, res_Rhad_vec, res_Rhad1_vec, res_Reta_vec, res_ethad_vec, res_ethad1_vec,
    res_Rphi_vec, res_weta1_vec, res_weta2_vec, res_wtots1_vec, res_f1_vec, res_f3_vec, res_eratio_vec, et_vec, eta_vec, mu_vec;
    
    std::vector<float> res_et_vec, res_et_cnv_vec, res_et_uncnv_vec;
    std::vector<float> res_etInEta0_vec, res_etInEta1_vec, res_etInEta2_vec, res_etInEta3_vec;
    std::vector<float> res_cnv_etInEta0_vec, res_cnv_etInEta1_vec, res_cnv_etInEta2_vec, res_cnv_etInEta3_vec;
    std::vector<float> res_uncnv_etInEta0_vec, res_uncnv_etInEta1_vec, res_uncnv_etInEta2_vec, res_uncnv_etInEta3_vec;



    std::vector<float> res_topoetcone20_vec, res_topoetcone20_rel_vec;

    auto et_col           = Monitored::Collection( "et"             , et_vec                );
    auto eta_col          = Monitored::Collection( "eta"            , eta_vec               );
    auto mu_col           = Monitored::Collection( "mu"             , mu_vec                );
    
    // For calo
    auto res_eta_col      = Monitored::Collection( "res_eta"        , res_eta_vec           );
    auto res_phi_col      = Monitored::Collection( "res_phi"        , res_phi_vec           );
    auto res_ethad_col    = Monitored::Collection( "res_ethad"      , res_ethad_vec         );
    auto res_ethad1_col   = Monitored::Collection( "res_ethad1"     , res_ethad1_vec        );
    auto res_Rhad_col     = Monitored::Collection( "res_Rhad"       , res_Rhad_vec          );
    auto res_Rhad1_col    = Monitored::Collection( "res_Rhad1"      , res_Rhad1_vec         );
    auto res_Reta_col     = Monitored::Collection( "res_Reta"       , res_Reta_vec          );
    auto res_Rphi_col     = Monitored::Collection( "res_Rphi"       , res_Rphi_vec          );
    auto res_weta1_col    = Monitored::Collection( "res_weta1"      , res_weta1_vec         );
    auto res_weta2_col    = Monitored::Collection( "res_weta2"      , res_weta2_vec         );
    auto res_wtots1_col   = Monitored::Collection( "res_wtots1"     , res_wtots1_vec        );
    auto res_f1_col       = Monitored::Collection( "res_f1"         , res_f1_vec            );
    auto res_f3_col       = Monitored::Collection( "res_f3"         , res_f3_vec            );
    auto res_eratio_col   = Monitored::Collection( "res_eratio"     , res_eratio_vec        );


    auto res_et_col             = Monitored::Collection( "res_et"               , res_et_vec            );
    auto res_et_cnv_col         = Monitored::Collection( "res_et_cnv"           , res_et_cnv_vec        );
    auto res_et_uncnv_col       = Monitored::Collection( "res_et_uncnv"         , res_et_uncnv_vec      );
    auto res_etInEta0_col       = Monitored::Collection( "res_etInEta0"         , res_etInEta0_vec      );
    auto res_etInEta1_col       = Monitored::Collection( "res_etInEta1"         , res_etInEta1_vec      );
    auto res_etInEta2_col       = Monitored::Collection( "res_etInEta2"         , res_etInEta2_vec      );
    auto res_etInEta3_col       = Monitored::Collection( "res_etInEta3"         , res_etInEta3_vec      );
    auto res_cnv_etInEta0_col   = Monitored::Collection( "res_cnv_etInEta0"     , res_cnv_etInEta0_vec  );
    auto res_cnv_etInEta1_col   = Monitored::Collection( "res_cnv_etInEta1"     , res_cnv_etInEta1_vec  );
    auto res_cnv_etInEta2_col   = Monitored::Collection( "res_cnv_etInEta2"     , res_cnv_etInEta2_vec  );
    auto res_cnv_etInEta3_col   = Monitored::Collection( "res_cnv_etInEta3"     , res_cnv_etInEta3_vec  );
    auto res_uncnv_etInEta0_col = Monitored::Collection( "res_uncnv_etInEta0"   , res_uncnv_etInEta0_vec);
    auto res_uncnv_etInEta1_col = Monitored::Collection( "res_uncnv_etInEta1"   , res_uncnv_etInEta1_vec);
    auto res_uncnv_etInEta2_col = Monitored::Collection( "res_uncnv_etInEta2"   , res_uncnv_etInEta2_vec);
    auto res_uncnv_etInEta3_col = Monitored::Collection( "res_uncnv_etInEta3"   , res_uncnv_etInEta3_vec);
 
    // For photon
    auto res_topoetcone20_col         = Monitored::Collection( "res_topoetcone20"            , res_topoetcone20_vec             );
    auto res_topoetcone20_rel_col     = Monitored::Collection( "res_topoetcone20_rel"        , res_topoetcone20_rel_vec         );




    // Check for zero before filling
    ATH_MSG_DEBUG("Fill Resolution");



    for ( auto& pairObj : pairObjs ){

      const xAOD::Photon *off = static_cast<const xAOD::Photon*>(pairObj.first);
      const xAOD::Photon *onl=nullptr;


      { // Get the closest electron object from the trigger starting with deltaR = 0.15
        float maxDeltaR=0.05;
        auto vec =  tdt()->features<xAOD::PhotonContainer>(trigger,TrigDefs::Physics ,match()->key("Photon") );      
        for(auto &featLinkInfo : vec ){                                             
          if(! featLinkInfo.isValid() ) continue;
          const auto *feat = *(featLinkInfo.link);                   
          if(!feat) continue;
          float deltaR = dR( off->eta(), off->phi(), feat->eta(), feat->phi() );
          if( deltaR < maxDeltaR){
            maxDeltaR=deltaR;
            onl=feat;
          }
        } 
      }
    
      // If not found, skip this off object!
      if(!onl)  continue;

      float val_off=0.;
      const float onl_eta=onl->eta();
      const float feta = abs(onl_eta);
      const float onl_et = getCluster_et(onl)/Gaudi::Units::GeV;
      const float dummy=dummy;

      const float avgmu=lbAverageInteractionsPerCrossing( Gaudi::Hive::currentContext() );
      et_vec.push_back( onl_et );
      eta_vec.push_back( onl_eta );
      mu_vec.push_back( avgmu );


      val_off=getCluster_et(off);
      if(val_off!=0.){
          res_et_vec.push_back( (getCluster_et(onl)-val_off)/val_off  );
          if( feta < 1.37 )
              res_etInEta0_vec.push_back((getCluster_et(onl)-val_off)/val_off);
          else if( feta >=1.37 && feta <= 1.52 )
              res_etInEta1_vec.push_back((getCluster_et(onl)-val_off)/val_off);
          else if( feta >= 1.55 && feta < 1.8 )
              res_etInEta2_vec.push_back((getCluster_et(onl)-val_off)/val_off);
          else if( feta >= 1.8 && feta < 2.45 )
              res_etInEta3_vec.push_back((getCluster_et(onl)-val_off)/val_off);

          if(xAOD::EgammaHelpers::isConvertedPhoton(off)){
              res_et_cnv_vec.push_back((getCluster_et(onl)-val_off)/val_off);
              if( feta < 1.37 )
                  res_cnv_etInEta0_vec.push_back((getCluster_et(onl)-val_off)/val_off);
              else if( feta >=1.37 && feta <= 1.52 )
                  res_cnv_etInEta1_vec.push_back((getCluster_et(onl)-val_off)/val_off);
              else if( feta >= 1.55 && feta < 1.8 )
                  res_cnv_etInEta2_vec.push_back((getCluster_et(onl)-val_off)/val_off);
              else if( feta >= 1.8 && feta < 2.45 )
                  res_cnv_etInEta3_vec.push_back((getCluster_et(onl)-val_off)/val_off);
          }else{
              res_et_uncnv_vec.push_back((getCluster_et(onl)-val_off)/val_off);
              if( feta < 1.37 )
                  res_uncnv_etInEta0_vec.push_back((getCluster_et(onl)-val_off)/val_off);
              else if( feta >=1.37 && feta <= 1.52 )
                  res_uncnv_etInEta1_vec.push_back((getCluster_et(onl)-val_off)/val_off);
              else if( feta >= 1.55 && feta < 1.8 )
                  res_uncnv_etInEta2_vec.push_back((getCluster_et(onl)-val_off)/val_off);
              else if( feta >= 1.8 && feta < 2.45 )
                  res_uncnv_etInEta3_vec.push_back((getCluster_et(onl)-val_off)/val_off);
          }
      }

      val_off=off->eta();
      if(val_off!=0.){
          res_eta_vec.push_back( (onl_eta-val_off)/val_off ) ;
      }else{
          res_eta_vec.push_back( dummy ) ;
      }

      val_off=off->phi();
      if(val_off!=0.){ 
          res_phi_vec.push_back(  (onl->phi()-val_off)/val_off );
      }else{
          res_phi_vec.push_back(dummy );
      }

      val_off=getShowerShape_ethad(off);
      if(val_off!=0.) {
          res_ethad_vec.push_back((getShowerShape_ethad(onl)-val_off)/val_off);
      }else{
          res_ethad_vec.push_back(dummy);
      }

      val_off=getShowerShape_ethad1(off);
      if(val_off!=0){
          res_ethad1_vec.push_back((getShowerShape_ethad1(onl)-val_off)/val_off);
      }else{
          res_ethad1_vec.push_back(dummy);
      }

      val_off=getShowerShape_Rhad(off);
      if(val_off!=0.){
          res_Rhad_vec.push_back(  (getShowerShape_Rhad(onl)-val_off)/val_off );
      }else{
          res_Rhad_vec.push_back(dummy );
      }

      val_off=getShowerShape_Rhad1(off);
      if(val_off!=0.){
          res_Rhad1_vec.push_back(  (getShowerShape_Rhad1(onl)-val_off)/val_off );
      }else{
          res_Rhad1_vec.push_back(dummy );
      }

      val_off=getShowerShape_Reta(off);
      if(val_off!=0.){
          res_Reta_vec.push_back(  (getShowerShape_Reta(onl)-val_off)/val_off );
      }else{
          res_Reta_vec.push_back( dummy );
      }

      val_off=getShowerShape_Rphi(off);
      if(val_off!=0.){
          res_Rphi_vec.push_back(  (getShowerShape_Rphi(onl)-val_off)/val_off );
      }else{
          res_Rphi_vec.push_back(dummy );
      }

      val_off=getShowerShape_weta1(off);
      if(val_off!=0.){
          res_weta1_vec.push_back( (getShowerShape_weta1(onl)-val_off)/val_off );
      }else{
          res_weta1_vec.push_back( dummy );
      }

      val_off=getShowerShape_weta2(off);
      if(val_off!=0.){
          res_weta2_vec.push_back( (getShowerShape_weta2(onl)-val_off)/val_off );
      }else{
          res_weta2_vec.push_back(dummy);
      }

      val_off=getShowerShape_wtots1(off);
      if(val_off!=0.){
          res_wtots1_vec.push_back( (getShowerShape_wtots1(onl)-val_off)/val_off );
      }else{
          res_wtots1_vec.push_back( dummy );
      }

      val_off=getShowerShape_f1(off);
      if(val_off!=0.){
          res_f1_vec.push_back(  (getShowerShape_f1(onl)-val_off)/val_off );
      }else{
          res_f1_vec.push_back( dummy );
      }

      val_off=getShowerShape_f3(off);
      if(val_off!=0.){
          res_f3_vec.push_back( (getShowerShape_f3(onl)-val_off)/val_off );
      }else{
          res_f3_vec.push_back( dummy );
      }

      val_off=getShowerShape_Eratio(off);
      if(val_off!=0.){
          res_eratio_vec.push_back(  (getShowerShape_Eratio(onl)-val_off)/val_off);
      }else{
          res_eratio_vec.push_back( dummy);
      }
   

      if( filliso ){
        // topoetcone20 isolation
        float val_off=getIsolation_topoetcone20(off);
        float etonl=onl->pt();
        float etoff=off->pt();
        if (val_off > 0.) {
            res_topoetcone20_vec.push_back((getIsolation_topoetcone20(onl)-val_off)/val_off);
            if (etonl > 0. && etoff > 0.) {
                const float reliso_onl=getIsolation_topoetcone20(onl)/etonl;
                const float reliso_off=getIsolation_topoetcone20(off)/etoff;
                res_topoetcone20_rel_vec.push_back((reliso_onl-reliso_off)/reliso_off);
            }else{
                res_topoetcone20_rel_vec.push_back(dummy);
            }
        }else{
            res_topoetcone20_vec.push_back(dummy);
            res_topoetcone20_rel_vec.push_back(dummy);
        }
      }


    
    } // Loop over all offline objects

    // Fill everything
    fill( monGroup        ,
          et_col          ,
          eta_col         ,
          mu_col          ,
          res_et_col      , 
          res_eta_col     , 
          res_phi_col     , 
          res_ethad_col   , 
          res_ethad1_col  , 
          res_Rhad_col    , 
          res_Rhad1_col   , 
          res_Reta_col    , 
          res_Rphi_col    , 
          res_weta1_col   , 
          res_weta2_col   , 
          res_wtots1_col  , 
          res_f1_col      , 
          res_f3_col      , 
          res_eratio_col  ,
          res_topoetcone20_col    , 
          res_topoetcone20_rel_col ,
          res_etInEta0_col,
          res_etInEta1_col,
          res_etInEta2_col,
          res_etInEta3_col,
          res_et_uncnv_col,
          res_cnv_etInEta0_col,
          res_cnv_etInEta1_col,
          res_cnv_etInEta2_col,
          res_cnv_etInEta3_col,
          res_et_cnv_col,
          res_uncnv_etInEta0_col,
          res_uncnv_etInEta1_col,
          res_uncnv_etInEta2_col,
          res_uncnv_etInEta3_col
          );
     


}


void TrigEgammaMonitorAnalysisAlgorithm::fillL2CaloResolution(const std::string &trigger,
                                                        std::vector< std::pair< const xAOD::Egamma*, const TrigCompositeUtils::Decision * >> pairObjs ) const

{
    ATH_MSG_DEBUG("Fill L2Calo Resolution");

    auto monGroup = getGroup( trigger + "_Resolutions_L2Calo" );

    std::vector<float> res_et_vec, res_phi_vec, res_eta_vec, res_Rhad_vec, res_Rhad1_vec, res_Reta_vec, res_ethad_vec, res_ethad1_vec,
    res_Rphi_vec, res_weta2_vec, res_f1_vec, res_f3_vec, res_eratio_vec, et_vec, eta_vec;
    

    auto et_col           = Monitored::Collection( "et"             , et_vec                );
    auto eta_col          = Monitored::Collection( "eta"            , eta_vec               );
    auto res_et_col       = Monitored::Collection( "res_et"         , res_et_vec            );
    auto res_eta_col      = Monitored::Collection( "res_eta"        , res_eta_vec           );
    auto res_phi_col      = Monitored::Collection( "res_phi"        , res_phi_vec           );
    auto res_ethad_col    = Monitored::Collection( "res_ethad"      , res_ethad_vec         );
    auto res_ethad1_col   = Monitored::Collection( "res_ethad1"     , res_ethad1_vec        );
    auto res_Rhad_col     = Monitored::Collection( "res_Rhad"       , res_Rhad_vec          );
    auto res_Rhad1_col    = Monitored::Collection( "res_Rhad1"      , res_Rhad1_vec         );
    auto res_Reta_col     = Monitored::Collection( "res_Reta"       , res_Reta_vec          );
    auto res_Rphi_col     = Monitored::Collection( "res_Rphi"       , res_Rphi_vec          );
    auto res_weta2_col    = Monitored::Collection( "res_weta2"      , res_weta2_vec         );
    auto res_f1_col       = Monitored::Collection( "res_f1"         , res_f1_vec            );
    auto res_f3_col       = Monitored::Collection( "res_f3"         , res_f3_vec            );
    auto res_eratio_col   = Monitored::Collection( "res_eratio"     , res_eratio_vec        );


    for ( auto& pairObj : pairObjs ){


        const xAOD::Egamma *off = pairObj.first;
        const xAOD::TrigEMCluster *onl=nullptr;

        { // Get the closest electron object from the trigger starting with deltaR = 0.15
          float maxDeltaR=0.05;
          auto vec =  tdt()->features<xAOD::TrigEMClusterContainer>(trigger,TrigDefs::Physics ,match()->key("L2Calo") );      
          for(auto &featLinkInfo : vec ){                                             
            if(! featLinkInfo.isValid() ) continue;
            const auto *feat = *(featLinkInfo.link);                   
            if(!feat) continue;
            float deltaR = dR( off->eta(), off->phi(), feat->eta(), feat->phi() );
            if( deltaR < maxDeltaR){
              maxDeltaR=deltaR;
              onl=feat;
            }
          } 
        }

        if(!onl)  continue;
        
        et_vec.push_back(onl->et()*1e-3);
        eta_vec.push_back(onl->eta());
        const float dummy=-999;
       
        float val_off=0.;

        val_off=off->caloCluster()->et();
        if(val_off!=0.){
            res_et_vec.push_back(((onl->et())-val_off)/val_off);
        }else{
            res_et_vec.push_back(dummy);
        }

        val_off=off->caloCluster()->eta();
        if(val_off!=0.){
            res_eta_vec.push_back((onl->eta()-val_off)/val_off);
        }else{
            res_eta_vec.push_back(dummy);
        }

        val_off=off->caloCluster()->phi();
        if(val_off!=0.){
            res_phi_vec.push_back((onl->phi()-val_off)/val_off);
        }else{
            res_phi_vec.push_back((onl->phi()-val_off)/val_off);
        }
        
        float elonl_ethad = onl->energy( CaloSampling::HEC0 ); elonl_ethad += onl->energy( CaloSampling::HEC1 );
        elonl_ethad += onl->energy( CaloSampling::HEC2 ); elonl_ethad += onl->energy( CaloSampling::HEC3 );
        elonl_ethad += onl->energy( CaloSampling::TileBar0 ); elonl_ethad += onl->energy( CaloSampling::TileExt0 ); 
        elonl_ethad += onl->energy( CaloSampling::TileBar1 ); elonl_ethad += onl->energy( CaloSampling::TileExt1 ); 
        elonl_ethad += onl->energy( CaloSampling::TileBar2 ); elonl_ethad += onl->energy( CaloSampling::TileExt2 ); 
        elonl_ethad /= TMath::CosH(onl->eta() );
        val_off=getShowerShape_ethad(off);
        if(val_off!=0.){
            res_ethad_vec.push_back((elonl_ethad-val_off)/val_off);
        }else{
            res_ethad_vec.push_back(dummy);
        }

        val_off=getShowerShape_ethad1(off);
        if(val_off!=0.){
            res_ethad1_vec.push_back(( (onl->ehad1()/TMath::Abs(onl->eta()) )-val_off)/val_off);
        }else{
            res_ethad1_vec.push_back(dummy);
        }

        float elonl_Rhad = elonl_ethad / onl->energy() ;
        val_off=getShowerShape_Rhad(off);
        if(val_off!=0.){
            res_Rhad_vec.push_back(( elonl_Rhad-val_off)/val_off);
        }else{
            res_Rhad_vec.push_back(dummy);
        }

        float elonl_Rhad1 = onl->ehad1() / onl->energy() ;
        val_off=getShowerShape_Rhad1(off);
        if(val_off!=0.){
            res_Rhad1_vec.push_back(( elonl_Rhad1-val_off)/val_off);
        }else{
            res_Rhad1_vec.push_back(dummy);
        }

        float onl_reta= 999.0;
        if ( fabsf ( onl->e277() ) > 0.01 ) onl_reta = onl->e237() / onl->e277();
        val_off=getShowerShape_Reta(off);
        if(val_off!=0.){
            res_Reta_vec.push_back( (onl_reta -val_off)/val_off);
        }else{
            res_Reta_vec.push_back(dummy);
        }

        val_off=getShowerShape_weta2(off);
        if(val_off!=0.){
            res_weta2_vec.push_back(( (onl->weta2())-val_off)/val_off);
        }else{
            res_weta2_vec.push_back(dummy);
        }

        float onl_f1 = onl->energy(CaloSampling::EMB1)+onl->energy(CaloSampling::EME1);
        onl_f1 /= onl->energy();
        val_off=getShowerShape_f1(off);
        if(val_off!=0.){
            res_f1_vec.push_back(( (onl_f1)-val_off)/val_off);
        }else{
            res_f1_vec.push_back(dummy);
        }

        float onl_f3 = onl->energy(CaloSampling::EMB3)+onl->energy(CaloSampling::EME3);
        onl_f3 /= onl->energy();
        val_off=getShowerShape_f3(off);
        if(val_off!=0.){
            res_f3_vec.push_back(( (onl_f3)-val_off)/val_off);
        }else{
            res_f3_vec.push_back(dummy);
        }

        float onl_eratio = 999.0;
        if ( fabsf(onl->emaxs1() + onl->e2tsts1()) > 0.01 ) 
            onl_eratio = (onl->emaxs1() - onl->e2tsts1()) / (onl->emaxs1() + onl->e2tsts1());
        val_off=getShowerShape_Eratio(off);
        if(val_off!=0.){
            res_eratio_vec.push_back(( (onl_eratio)-val_off)/val_off);
        }else{
            res_eratio_vec.push_back(dummy);
        }

    }// Loop over all pair objects


    // Fill everything
    fill( monGroup        ,
          et_col          ,
          eta_col         ,
          res_et_col      , 
          res_eta_col     , 
          res_phi_col     , 
          res_ethad_col   , 
          res_ethad1_col  , 
          res_Rhad_col    , 
          res_Rhad1_col   , 
          res_Reta_col    , 
          res_Rphi_col    , 
          res_weta2_col   , 
          res_f1_col      , 
          res_f3_col      , 
          res_eratio_col  
          );

}


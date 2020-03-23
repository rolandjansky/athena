

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


void TrigEgammaMonitorAnalysisAlgorithm::fillEfficiency( const std::string &trigger, 
                                                         const std::string &level, 
                                                         bool isPassed,
                                                         const float etthr, 
                                                         const std::string pidword, 
                                                         const xAOD::Egamma *eg) const
{

    auto monGroup = getGroup( trigger + "_Efficiencies_" + level );
    std::vector< Monitored::Scalar<float> >  variables;
    std::vector< Monitored::Scalar<bool> >  variables_passed;
    
    float et=0.;
    bool pid=true;

    ATH_MSG_DEBUG("Default pid " << pid << " te " << isPassed);
    if(xAOD::EgammaHelpers::isElectron(eg)){
        ATH_MSG_DEBUG("Offline Electron with pidword " << pidword);
        const xAOD::Electron* el =static_cast<const xAOD::Electron*> (eg);
        pid=el->auxdecor<bool>(pidword);
        ATH_MSG_DEBUG("Electron pid " << pid);
        et = getEt(el)/1e3;
    }
    else  et=eg->caloCluster()->et()/1e3;

    float eta = eg->caloCluster()->etaBE(2);
    float phi = eg->phi();
    float pt = eg->pt()/1e3;
    float avgmu=lbAverageInteractionsPerCrossing();
    float npvtx=0.0;
    
    
    ATH_MSG_DEBUG("PID decision efficiency " << eg->auxdecor<bool>(pidword));
    if(pid){

        variables.push_back( Monitored::Scalar<float>( "et", et ) );
        variables.push_back( Monitored::Scalar<float>( "pt", pt ) );
        variables.push_back( Monitored::Scalar<float>( "highet", et ) );


        if(et > etthr+1.0){

            variables.push_back( Monitored::Scalar<float>( "eta", eta ) );
            variables.push_back( Monitored::Scalar<float>( "phi", phi ) );
            variables.push_back( Monitored::Scalar<float>( "avgmu", avgmu ) );
            variables.push_back( Monitored::Scalar<float>("npvtx",npvtx));
        }

        if(isPassed) {
            
            variables.push_back( Monitored::Scalar<float>( "match_et", et ) );
            variables.push_back( Monitored::Scalar<float>( "match_pt", pt ) );
            variables.push_back( Monitored::Scalar<float>( "match_highet", et ) );


            if(et > etthr+1.0){
                variables.push_back( Monitored::Scalar<float>( "match_eta", eta ) );
                variables.push_back( Monitored::Scalar<float>( "match_phi", phi ) );
                variables.push_back( Monitored::Scalar<float>( "match_mu", avgmu ) );
                variables.push_back( Monitored::Scalar<float>( "match_npvtx", npvtx ) );
            }

            
            variables_passed.push_back( Monitored::Scalar<bool>( "et_passed", true ) );
            variables_passed.push_back( Monitored::Scalar<bool>( "pd_passed", true ) );
            variables_passed.push_back( Monitored::Scalar<bool>( "highet_passed", true ) );


            if(et > etthr+1.0){
                variables_passed.push_back( Monitored::Scalar<bool>( "eta_passed", true ) );
                variables_passed.push_back( Monitored::Scalar<bool>( "phi_passed", true ) );
                variables_passed.push_back( Monitored::Scalar<bool>( "mu_passed", true ) );
                variables_passed.push_back( Monitored::Scalar<bool>( "npvtx_passed", true ) );
            }
        } // Passes Trigger selection
        else {
            variables_passed.push_back( Monitored::Scalar<bool>( "et_passed", false ) );
            variables_passed.push_back( Monitored::Scalar<bool>( "pd_passed", false ) );
            variables_passed.push_back( Monitored::Scalar<bool>( "highet_passed", false ) );

            
            if(et > etthr+1.0){
                variables_passed.push_back( Monitored::Scalar<bool>( "eta_passed", false ) );
                variables_passed.push_back( Monitored::Scalar<bool>( "phi_passed", false ) );
                variables_passed.push_back( Monitored::Scalar<bool>( "mu_passed", false ) );
                variables_passed.push_back( Monitored::Scalar<bool>( "npvtx_passed", false ) );
            }

        } // Fails Trigger selection

    } // Passes offline pid, fill histograms


    std::vector<std::reference_wrapper<Monitored::IMonitoredVariable>>  mon_variables;
    for( auto&v : variables)  mon_variables.push_back( v );
    for( auto&v : variables_passed)  mon_variables.push_back( v );

    fill( monGroup, mon_variables );
}







// *********************************************************************************



// DONE
void TrigEgammaMonitorAnalysisAlgorithm::fillL2Calo(const std::string &trigger, const xAOD::TrigEMCluster *emCluster) const
{
    
    if(!emCluster) ATH_MSG_DEBUG("Online pointer fails"); 
    else{
        auto monGroup = getGroup(trigger+"_Distributions_L2Calo");
        auto et = Monitored::Scalar<float>("et", emCluster->et()/1.e3);
        auto eta = Monitored::Scalar<float>("eta", emCluster->eta());
        auto phi = Monitored::Scalar<float>("phi", emCluster->phi());
        
        fill( monGroup, et, eta, phi );
    }
}



// DONE
void TrigEgammaMonitorAnalysisAlgorithm::fillL2Electron(const std::string &trigger, const xAOD::TrigElectron *el) const
{
    
    if(!el) ATH_MSG_DEBUG("TrigElectron nullptr");
    else {
        auto monGroup = getGroup(trigger+"_Distributions_L2Electron");
        auto et = Monitored::Scalar<float>("et", el->pt()/1.e3);
        auto eta = Monitored::Scalar<float>("eta", el->eta());
        auto phi = Monitored::Scalar<float>("phi", el->phi());
        
        fill( monGroup, et, eta, phi );
    }

}

// DONE
void TrigEgammaMonitorAnalysisAlgorithm::fillEFCalo(const std::string &trigger, const xAOD::CaloCluster *clus) const
{
    
    ATH_MSG_DEBUG("Fill EFCalo distributions:" << trigger);
    ATH_MSG_DEBUG("Energy " << clus->e()/1.e3);
    ATH_MSG_DEBUG("eta " << clus->eta());
    ATH_MSG_DEBUG("phi " << clus->phi());
    
    double tmpeta = -999.;
    if(!clus->retrieveMoment(xAOD::CaloCluster::ETACALOFRAME,tmpeta))
        tmpeta=-999.;
    double tmpphi = -999.;
    if(!clus->retrieveMoment(xAOD::CaloCluster::PHICALOFRAME,tmpphi))
        tmpphi=-999.;
    ATH_MSG_DEBUG("etacalo " << tmpeta);
    ATH_MSG_DEBUG("phicalo " << tmpphi);
    
    auto monGroup = getGroup( trigger + "_Distributions_EFCalo" );

    auto energyBE0 = Monitored::Scalar<float>("energyBE0", clus->energyBE(0)/1.e3);
    auto energyBE1 = Monitored::Scalar<float>("energyBE1", clus->energyBE(1)/1.e3);
    auto energyBE2 = Monitored::Scalar<float>("energyBE2", clus->energyBE(2)/1.e3);
    auto energyBE3 = Monitored::Scalar<float>("energyBE3", clus->energyBE(3)/1.e3);
    auto energy = Monitored::Scalar<float>("energy", clus->e()/1.e3);
    auto et = Monitored::Scalar<float>("et", clus->et()/1.e3);
    auto eta = Monitored::Scalar<float>("eta", clus->eta());
    auto phi = Monitored::Scalar<float>("phi", clus->phi());
    auto eta_calo = Monitored::Scalar<float>("eta_calo", tmpeta);
    auto phi_calo = Monitored::Scalar<float>("phi_calo", tmpphi);

    fill( monGroup, energyBE0, energyBE1, energyBE2, energyBE3, energy, et, eta, phi, eta_calo, phi_calo );
}





void TrigEgammaMonitorAnalysisAlgorithm::fillShowerShapes(const std::string &trigger,const xAOD::Egamma *eg, bool online ) const 
{
    ATH_MSG_DEBUG("Fill SS distributions: " << trigger);
    if(!eg) ATH_MSG_WARNING("Egamma pointer fails"); 
    else {
        ATH_MSG_DEBUG("Shower Shapes");

        auto monGroup = getGroup( trigger + ( online ? "_Distributions_HLT" : "_Distributions_Offline") );

        std::vector<Monitored::Scalar<float>>  variables;
        variables.push_back( Monitored::Scalar<float>("ethad", getShowerShape_ethad(eg)/1e3));
        variables.push_back( Monitored::Scalar<float>("ethad1", getShowerShape_ethad1(eg)/1e3));
        variables.push_back( Monitored::Scalar<float>("Rhad", getShowerShape_Rhad(eg)));
        variables.push_back( Monitored::Scalar<float>("Rhad1", getShowerShape_Rhad(eg)));
        variables.push_back( Monitored::Scalar<float>("Reta", getShowerShape_Reta(eg)));
        variables.push_back( Monitored::Scalar<float>("Rphi", getShowerShape_Rphi(eg)));
        variables.push_back( Monitored::Scalar<float>("weta1", getShowerShape_weta1(eg)));
        variables.push_back( Monitored::Scalar<float>("weta2", getShowerShape_weta2(eg)));
        variables.push_back( Monitored::Scalar<float>("f1", getShowerShape_f1(eg)));
        variables.push_back( Monitored::Scalar<float>("f3", getShowerShape_f3(eg)));
        variables.push_back( Monitored::Scalar<float>("eratio", getShowerShape_Eratio(eg)));
        variables.push_back( Monitored::Scalar<float>("et", eg->pt()/1e3));
        variables.push_back( Monitored::Scalar<float>("highet", eg->pt()/1e3));
        variables.push_back( Monitored::Scalar<float>("eta", eg->eta()));
        variables.push_back( Monitored::Scalar<float>("phi", eg->phi()));
        variables.push_back( Monitored::Scalar<float>("topoetcone20", getIsolation_topoetcone20(eg)/1e3));
        variables.push_back( Monitored::Scalar<float>("topoetcone40_shift", (getIsolation_topoetcone40(eg)-2450)/1e3));
        
        if (eg->pt() > 0) {
            variables.push_back( Monitored::Scalar<float>("topoetcone20_rel", getIsolation_topoetcone20(eg)/eg->pt()));
            variables.push_back( Monitored::Scalar<float>("topoetcone40_shift_rel", (getIsolation_topoetcone40(eg)-2450)/eg->pt()));
        
        }

        std::vector<std::reference_wrapper<Monitored::IMonitoredVariable>>  mon_variables;
        for(auto&v:variables) mon_variables.push_back(v);
        fill( monGroup, mon_variables );
    }
}


void TrigEgammaMonitorAnalysisAlgorithm::fillTracking(const std::string &trigger, const xAOD::Electron *eg, bool online ) const
{
    ATH_MSG_DEBUG("Fill tracking");
    if(!eg) ATH_MSG_WARNING("Electron pointer fails");
    else {

        auto monGroup = getGroup( trigger + ( online ? "_Distributions_HLT" : "_Distributions_Offline") );

        std::vector<Monitored::Scalar<float>>  variables;

        float cleta = 0.;
        if(eg->caloCluster()) cleta=eg->caloCluster()->eta();
        else cleta=eg->eta();

        ATH_MSG_DEBUG("Calo-track match");
        variables.push_back( Monitored::Scalar<float>("deta1", getCaloTrackMatch_deltaEta1(eg)) );
        if(cleta > 1.375 && cleta < 3.2)
            variables.push_back( Monitored::Scalar<float>("deta1_EMECA", getCaloTrackMatch_deltaEta1(eg)));
        if(cleta < -1.375 && cleta > -3.2)
            variables.push_back( Monitored::Scalar<float>("deta1_EMECC", getCaloTrackMatch_deltaEta1(eg)));
        if(cleta > 0 && cleta < 1.375)
            variables.push_back( Monitored::Scalar<float>("deta1_EMEBA", getCaloTrackMatch_deltaEta1(eg)));
        if(cleta < 0 && cleta > -1.375)
            variables.push_back( Monitored::Scalar<float>("deta1_EMEBC", getCaloTrackMatch_deltaEta1(eg)));
        
        variables.push_back( Monitored::Scalar<float>("deta2", getCaloTrackMatch_deltaEta2(eg)));
        variables.push_back( Monitored::Scalar<float>("dphi2", getCaloTrackMatch_deltaPhi2(eg)));
        variables.push_back( Monitored::Scalar<float>("dphiresc", getCaloTrackMatch_deltaPhiRescaled2(eg)));
        
        
        variables.push_back( Monitored::Scalar<float>("eprobht", getTrackSummaryFloat_eProbabilityHT(eg)));
        variables.push_back( Monitored::Scalar<float>("npixhits", getTrackSummary_numberOfPixelHits(eg)));
        variables.push_back( Monitored::Scalar<float>("nscthits", getTrackSummary_numberOfSCTHits(eg)));
        variables.push_back( Monitored::Scalar<float>("charge", eg->charge()));
        variables.push_back( Monitored::Scalar<float>("ptcone20", getIsolation_ptcone20(eg)/1e3));
        
        variables.push_back( Monitored::Scalar<float>("ptvarcone20", getIsolation_ptvarcone20(eg)/1e3));
        // Quantities directly from tracks
        ATH_MSG_DEBUG("Get track Quantities");
        variables.push_back( Monitored::Scalar<float>("d0", getTrack_d0(eg)));
        variables.push_back( Monitored::Scalar<float>("d0sig", getD0sig(eg)));
        variables.push_back( Monitored::Scalar<float>("pt", getTrack_pt(eg)/1e3));
        
        if (eg->pt() > 0) {
            variables.push_back( Monitored::Scalar<float>("ptcone20_rel", getIsolation_ptcone20(eg)/eg->pt()));
            variables.push_back( Monitored::Scalar<float>("ptvarcone20_rel", getIsolation_ptvarcone20(eg)/eg->pt()));
        }

        std::vector<std::reference_wrapper<Monitored::IMonitoredVariable>>  mon_variables;
        for(auto&v:variables) mon_variables.push_back(v);
        fill( monGroup, mon_variables );
 
    }
}


// *********************************************************************************

void TrigEgammaMonitorAnalysisAlgorithm::fillL1CaloResolution(const std::string &trigger,const xAOD::EmTauRoI *l1, const xAOD::Egamma *off) const 
{
    ATH_MSG_DEBUG("Fill L1CaloResolution");
    if(off->type()==xAOD::Type::Electron){

      const xAOD::Electron* eloff =static_cast<const xAOD::Electron*> (off);
      auto monGroup = getGroup( trigger + "_Resolutions_L1Calo" );
      auto eta = Monitored::Scalar<float>( "eta" , l1->eta() );
      auto res_et = Monitored::Scalar<float>("res_et", (l1->emClus()-getEt(eloff))/getEt(eloff));
      fill( monGroup, eta, res_et );
    }
}

void TrigEgammaMonitorAnalysisAlgorithm::fillL1CaloAbsResolution(const std::string &trigger,const xAOD::EmTauRoI *l1, const xAOD::Egamma *off) const
{
    ATH_MSG_DEBUG("Fill L1CaloAbsResolution");
    if(off->type()==xAOD::Type::Electron){

      const xAOD::Electron* eloff =static_cast<const xAOD::Electron*> (off);
      auto monGroup = getGroup( trigger + "_AbsResolutions_L1Calo" );
      auto eta = Monitored::Scalar<float>( "eta" , l1->eta() );
      auto res_et = Monitored::Scalar<float>("res_et", 0.001* (l1->emClus()-getEt(eloff)));
      fill( monGroup, eta, res_et );
    }
}


void TrigEgammaMonitorAnalysisAlgorithm::fillL2CaloResolution(const std::string &trigger,const xAOD::TrigEMCluster *onl, const xAOD::Egamma *off) const
{
        
    ATH_MSG_DEBUG("Fill Resolution");
    float val_off=0.;
    if(xAOD::EgammaHelpers::isElectron(off)){

        auto monGroup = getGroup( trigger + "_Resolutions_L2Calo" );
        std::vector<Monitored::Scalar<float>>  variables;

        const xAOD::TrigEMCluster* elonl =onl;
        const xAOD::Electron* eloff =static_cast<const xAOD::Electron*> (off);

        val_off=getEt(eloff);

        variables.push_back( Monitored::Scalar<float>("et", elonl->et()/1e3 ));
        variables.push_back( Monitored::Scalar<float>("eta", elonl->eta() ));
        variables.push_back( Monitored::Scalar<float>("phi", elonl->phi() ));


        if(val_off!=0.)
            variables.push_back( Monitored::Scalar<float>("res_et", (elonl->et()-val_off)/val_off) );


        val_off=eloff->caloCluster()->eta();
        if(val_off!=0.) 
            variables.push_back( Monitored::Scalar<float>("res_eta", (elonl->eta()-val_off)/val_off));
        

        val_off=eloff->caloCluster()->phi();
        if(val_off!=0.) 
            variables.push_back( Monitored::Scalar<float>("res_phi", (elonl->phi()-val_off)/val_off));


        float elonl_ethad = elonl->energy( CaloSampling::HEC0 ); elonl_ethad += elonl->energy( CaloSampling::HEC1 );
        elonl_ethad += elonl->energy( CaloSampling::HEC2 ); elonl_ethad += elonl->energy( CaloSampling::HEC3 );
        elonl_ethad += elonl->energy( CaloSampling::TileBar0 ); elonl_ethad += elonl->energy( CaloSampling::TileExt0 ); 
        elonl_ethad += elonl->energy( CaloSampling::TileBar1 ); elonl_ethad += elonl->energy( CaloSampling::TileExt1 ); 
        elonl_ethad += elonl->energy( CaloSampling::TileBar2 ); elonl_ethad += elonl->energy( CaloSampling::TileExt2 ); 
        elonl_ethad /= TMath::CosH(elonl->eta() );
        val_off=getShowerShape_ethad(off);
        
        if(val_off!=0.)
            variables.push_back( Monitored::Scalar<float>("res_ethad", (elonl_ethad-val_off)/val_off));

        val_off=getShowerShape_ethad1(off);
        if(val_off!=0.)
            variables.push_back( Monitored::Scalar<float>("res_ethad1", ( (onl->ehad1()/TMath::Abs(onl->eta()) )-val_off)/val_off));

        float elonl_Rhad = elonl_ethad / onl->energy() ;
        val_off=getShowerShape_Rhad(off);
        if(val_off!=0.)
            variables.push_back( Monitored::Scalar<float>("res_Rhad", ( elonl_Rhad-val_off)/val_off));

        float elonl_Rhad1 = onl->ehad1() / onl->energy() ;
        val_off=getShowerShape_Rhad1(off);
        if(val_off!=0.)
            variables.push_back( Monitored::Scalar<float>("res_Rhad1", ( elonl_Rhad1-val_off)/val_off));


        float onl_reta= 999.0;
        if ( fabsf ( onl->e277() ) > 0.01 ) onl_reta = onl->e237() / onl->e277();
        val_off=getShowerShape_Reta(off);
        if(val_off!=0.){
            variables.push_back( Monitored::Scalar<float>("res_Reta",  (onl_reta -val_off)/val_off));
        }
        
        val_off=getShowerShape_weta2(off);
        if(val_off!=0.){
            variables.push_back( Monitored::Scalar<float>("res_weta2",  (onl->weta2()-val_off)/val_off) );
        }

        float onl_f1 = onl->energy(CaloSampling::EMB1)+onl->energy(CaloSampling::EME1);
        onl_f1 /= onl->energy();
        val_off=getShowerShape_f1(off);
        if(val_off!=0.){
            variables.push_back( Monitored::Scalar<float>("res_f1", ( (onl_f1)-val_off)/val_off));
        }
        float onl_f3 = onl->energy(CaloSampling::EMB3)+onl->energy(CaloSampling::EME3);
        onl_f3 /= onl->energy();
        val_off=getShowerShape_f3(off);
        if(val_off!=0.){
            variables.push_back( Monitored::Scalar<float>("res_f3",  (onl_f3-val_off)/val_off));
        }
        float onl_eratio = 999.0;
        if ( fabsf(onl->emaxs1() + onl->e2tsts1()) > 0.01 ) 
            onl_eratio = (onl->emaxs1() - onl->e2tsts1()) / (onl->emaxs1() + onl->e2tsts1());
        val_off=getShowerShape_Eratio(off);
        if(val_off!=0.){
            variables.push_back( Monitored::Scalar<float>("res_eratio",  (onl_eratio-val_off)/val_off));
        }

        
        std::vector<std::reference_wrapper<Monitored::IMonitoredVariable>>  mon_variables;
        for(auto&v:variables) mon_variables.push_back(v);
        fill( monGroup, mon_variables );
 

    } // Electron
}





void TrigEgammaMonitorAnalysisAlgorithm::fillHLTAbsResolution(const std::string &trigger,const xAOD::Egamma *onl, const xAOD::Egamma *off) const 
{
   
    ATH_MSG_DEBUG("Fill Abs Resolution");    
    auto monGroup = getGroup( trigger + "_AbsResolutions_HLT" );
    std::vector<Monitored::Scalar<float>>  variables;


    if(xAOD::EgammaHelpers::isElectron(onl)){

        const xAOD::Electron* elonl =static_cast<const xAOD::Electron*> (onl);
        const xAOD::Electron* eloff =static_cast<const xAOD::Electron*> (off);
        variables.push_back( Monitored::Scalar<float>("res_pt", (getTrack_pt(elonl)-getTrack_pt(eloff))) );
        variables.push_back( Monitored::Scalar<float>("res_et", (getEt(elonl)-getEt(eloff))/getEt(eloff)) );
        
        const float onl_eta=onl->eta();
        const float feta = fabs(onl_eta);
        const float off_eta=off->eta();
        const float avgmu = lbAverageInteractionsPerCrossing();        

        variables.push_back( Monitored::Scalar<float>("et", getEt(elonl)/1e3 ));
        variables.push_back( Monitored::Scalar<float>("eta", elonl->eta() ));
        variables.push_back( Monitored::Scalar<float>("phi", elonl->phi() ));
        variables.push_back( Monitored::Scalar<float>("avgmu", avgmu ));




        variables.push_back( Monitored::Scalar<float>("res_eta", onl_eta-off_eta));
        variables.push_back( Monitored::Scalar<float>("res_phi", (elonl->phi()-eloff->phi())));
        variables.push_back( Monitored::Scalar<float>("res_ptcone20", getIsolation_ptcone20(elonl)-getIsolation_ptcone20(eloff)));

        //ptcone20/pt
        if (getEt(elonl) > 0 && getEt(eloff) > 0) {
            variables.push_back( Monitored::Scalar<float>("res_ptcone20_rel", getIsolation_ptcone20(elonl)/getEt(elonl)-getIsolation_ptcone20(eloff)/getEt(eloff)));
        }

        //ptcone20
        variables.push_back( Monitored::Scalar<float>("res_ptcone20", getIsolation_ptcone20(elonl)-getIsolation_ptcone20(eloff)));


        if( feta < 1.37 )
            variables.push_back( Monitored::Scalar<float>("res_etInEta0", (getEt(elonl)-getEt(eloff))));
        else if( feta >=1.37 && feta <= 1.52 )
            variables.push_back( Monitored::Scalar<float>("res_etInEta1", (getEt(elonl)-getEt(eloff)) ) );
        else if( feta >= 1.55 && feta < 1.8 )
            variables.push_back( Monitored::Scalar<float>("res_etInEta2", (getEt(elonl)-getEt(eloff))));
        else if( feta >= 1.8 && feta < 2.45 )
            variables.push_back( Monitored::Scalar<float>("res_etInEta3", (getEt(elonl)-getEt(eloff))));
        
        variables.push_back( Monitored::Scalar<float>("res_deta1", (getCaloTrackMatch_deltaEta1(elonl)-getCaloTrackMatch_deltaEta1(eloff))));
        variables.push_back( Monitored::Scalar<float>("res_deta2", (getCaloTrackMatch_deltaEta2(elonl)-getCaloTrackMatch_deltaEta2(eloff))));
        variables.push_back( Monitored::Scalar<float>("res_dphi2", (getCaloTrackMatch_deltaPhi2(elonl)-getCaloTrackMatch_deltaPhi2(eloff))));
        variables.push_back( Monitored::Scalar<float>("res_dphiresc", (getCaloTrackMatch_deltaPhiRescaled2(elonl)-getCaloTrackMatch_deltaPhiRescaled2(eloff))));
        variables.push_back( Monitored::Scalar<float>("res_d0", (getTrack_d0(elonl)-getTrack_d0(eloff))));
        variables.push_back( Monitored::Scalar<float>("res_d0sig", (getD0sig(elonl)-getD0sig(eloff))));
        variables.push_back( Monitored::Scalar<float>("res_eprobht",  (getTrackSummaryFloat_eProbabilityHT(elonl) - getTrackSummaryFloat_eProbabilityHT(eloff))));
        variables.push_back( Monitored::Scalar<float>("res_npixhits", (getTrackSummary_numberOfPixelHits(elonl)-getTrackSummary_numberOfPixelHits(elonl))));
        variables.push_back( Monitored::Scalar<float>("res_nscthits", (getTrackSummary_numberOfSCTHits(elonl)-getTrackSummary_numberOfSCTHits(elonl))));
    }
    else{ 
      variables.push_back( Monitored::Scalar<float>("res_et", (getCluster_et(onl)-getCluster_et(off))));
      variables.push_back( Monitored::Scalar<float>("res_eta", (onl->eta()-off->eta())));
      variables.push_back( Monitored::Scalar<float>("res_phi", (onl->phi()-off->phi())));

      float feta = fabs(onl->eta());
      if( feta < 1.37 )
          variables.push_back( Monitored::Scalar<float>("res_etInEta0", (getCluster_et(onl)-getCluster_et(off))));
      else if( feta >=1.37 && feta <= 1.52 )
          variables.push_back( Monitored::Scalar<float>("res_etInEta1", (getCluster_et(onl)-getCluster_et(off))));
      else if( feta >= 1.55 && feta < 1.8 )
          variables.push_back( Monitored::Scalar<float>("res_etInEta2", (getCluster_et(onl)-getCluster_et(off))));
      else if( feta >= 1.8 && feta < 2.45 )
          variables.push_back( Monitored::Scalar<float>("res_etInEta3",(getCluster_et(onl)-getCluster_et(off))));
    }
    
    variables.push_back( Monitored::Scalar<float>("res_e132", getShowerShape_e132(onl)-getShowerShape_e132(off)));
    variables.push_back( Monitored::Scalar<float>("res_e237", getShowerShape_e237(onl)-getShowerShape_e237(off)));
    variables.push_back( Monitored::Scalar<float>("res_e277", getShowerShape_e277(onl)-getShowerShape_e277(off)));
    variables.push_back( Monitored::Scalar<float>("res_ethad", getShowerShape_ethad(onl)-getShowerShape_ethad(off)));
    variables.push_back( Monitored::Scalar<float>("res_ethad1", getShowerShape_ethad1(onl)-getShowerShape_ethad1(off)));
    variables.push_back( Monitored::Scalar<float>("res_Rhad", getShowerShape_Rhad(onl)-getShowerShape_Rhad(off)));
    variables.push_back( Monitored::Scalar<float>("res_Rhad1", getShowerShape_Rhad1(onl)-getShowerShape_Rhad1(off)));
    variables.push_back( Monitored::Scalar<float>("res_Reta", getShowerShape_Reta(onl)-getShowerShape_Reta(off)));
    variables.push_back( Monitored::Scalar<float>("res_Rphi", getShowerShape_Rphi(onl)-getShowerShape_Rphi(off)));
    variables.push_back( Monitored::Scalar<float>("res_weta1", getShowerShape_weta1(onl)-getShowerShape_weta1(off)));
    variables.push_back( Monitored::Scalar<float>("res_weta2", getShowerShape_weta2(onl)-getShowerShape_weta2(off)));
    variables.push_back( Monitored::Scalar<float>("res_wtots1", getShowerShape_wtots1(onl)-getShowerShape_wtots1(off)));
    variables.push_back( Monitored::Scalar<float>("res_f1", getShowerShape_f1(onl)-getShowerShape_f1(off)));
    variables.push_back( Monitored::Scalar<float>("res_f3", getShowerShape_f3(onl)-getShowerShape_f3(off)));
    variables.push_back( Monitored::Scalar<float>("res_eratio",getShowerShape_Eratio(onl)-getShowerShape_Eratio(off)));
    std::vector<std::reference_wrapper<Monitored::IMonitoredVariable>>  mon_variables;
    for(auto&v:variables) mon_variables.push_back(v);
    fill( monGroup, mon_variables );
 

}









void TrigEgammaMonitorAnalysisAlgorithm::fillHLTResolution(const std::string &trigger,const xAOD::Egamma *onl, const xAOD::Egamma *off) const 
{
    auto monGroup = getGroup( trigger + "_Resolutions_HLT" );
    std::vector<Monitored::Scalar<float>>  variables;



    // Check for zero before filling
    ATH_MSG_DEBUG("Fill Resolution");
    //float getOnlEt=0;
    float val_off=0.;
    const float onl_eta=onl->eta();
    //const float feta = fabs(onl_eta);
    val_off=off->eta();
    if(val_off!=0.) 
        variables.push_back( Monitored::Scalar<float>("res_eta", (onl_eta-val_off)/val_off));
    val_off=off->phi();
    if(val_off!=0.) 
        variables.push_back( Monitored::Scalar<float>("res_phi", (onl->phi()-val_off)/val_off));


    if(xAOD::EgammaHelpers::isElectron(onl)){
        const xAOD::Electron* elonl =static_cast<const xAOD::Electron*> (onl);
        const xAOD::Electron* eloff =static_cast<const xAOD::Electron*> (off);
        val_off=getTrack_pt(eloff);
        if(val_off!=0.) variables.push_back( Monitored::Scalar<float>("res_pt", (getTrack_pt(elonl)-val_off)/val_off));
        val_off=getEt(eloff);
        if(val_off!=0.) variables.push_back( Monitored::Scalar<float>("res_et", (getEt(elonl)-val_off)/val_off));
        //getOnlEt = getEt(elonl);


        val_off=getCaloTrackMatch_deltaEta1(eloff);
        if(val_off!=0.) variables.push_back( Monitored::Scalar<float>("res_deta1", (getCaloTrackMatch_deltaEta1(elonl)-val_off)/val_off));
        val_off=getCaloTrackMatch_deltaEta2(eloff);
        variables.push_back( Monitored::Scalar<float>("res_deta2", (getCaloTrackMatch_deltaEta2(elonl)-val_off)/val_off));
        val_off=getCaloTrackMatch_deltaPhi2(eloff);
        if(val_off!=0.) variables.push_back( Monitored::Scalar<float>("res_dphi2", (getCaloTrackMatch_deltaPhi2(elonl)-val_off)/val_off));
        val_off=getCaloTrackMatch_deltaPhiRescaled2(eloff);
        variables.push_back( Monitored::Scalar<float>("res_dphiresc", (getCaloTrackMatch_deltaPhiRescaled2(elonl)-val_off)/val_off));

        // Absolute resolution for impact parameter
        val_off=getTrack_d0(eloff);
        if(val_off!=0.) variables.push_back( Monitored::Scalar<float>("res_d0", getTrack_d0(elonl)-val_off));
        val_off=getD0sig(eloff);
        if(val_off!=0.) variables.push_back( Monitored::Scalar<float>("res_d0sig", getD0sig(elonl)-val_off));
        
        // Absolute resolution on track summary ints/floats 
        val_off=getTrackSummaryFloat_eProbabilityHT(eloff);
        variables.push_back( Monitored::Scalar<float>("res_eprobht",  (getTrackSummaryFloat_eProbabilityHT(elonl)-val_off)));
        variables.push_back( Monitored::Scalar<float>("res_npixhits", getTrackSummary_numberOfPixelHits(elonl)-getTrackSummary_numberOfPixelHits(elonl)));
        variables.push_back( Monitored::Scalar<float>("res_nscthits", getTrackSummary_numberOfSCTHits(elonl)-getTrackSummary_numberOfSCTHits(elonl)));

    } else { 
        val_off=getCluster_et(off);
        if(val_off!=0.){
            variables.push_back( Monitored::Scalar<float>("res_et", (getCluster_et(onl)-val_off)/val_off));

            const xAOD::Photon* phoff =static_cast<const xAOD::Photon*> (off);
            if(xAOD::EgammaHelpers::isConvertedPhoton(phoff)) {
                variables.push_back( Monitored::Scalar<float>("res_et_cnv", (getCluster_et(onl)-val_off)/val_off));
    
            }
        }
    }
    if(val_off!=0.){
        variables.push_back( Monitored::Scalar<float>("res_Rhad", (getShowerShape_Rhad(onl)-val_off)/val_off));
    }
    val_off=getShowerShape_Rhad1(off);
    if(val_off!=0.){
        variables.push_back( Monitored::Scalar<float>("res_Rhad1", (getShowerShape_Rhad1(onl)-val_off)/val_off));
    }
    val_off=getShowerShape_Reta(off);
    if(val_off!=0.){
        variables.push_back( Monitored::Scalar<float>("res_Reta", (getShowerShape_Reta(onl)-val_off)/val_off));
    }
    val_off=getShowerShape_Rphi(off);
    if(val_off!=0.){
        variables.push_back( Monitored::Scalar<float>("res_Rphi", (getShowerShape_Rphi(onl)-val_off)/val_off));
    }
    val_off=getShowerShape_weta1(off);
    if(val_off!=0.){
        variables.push_back( Monitored::Scalar<float>("res_weta1", (getShowerShape_weta1(onl)-val_off)/val_off));
    }
    val_off=getShowerShape_weta2(off);
    if(val_off!=0.){
        variables.push_back( Monitored::Scalar<float>("res_weta2", (getShowerShape_weta2(onl)-val_off)/val_off));
    }
    val_off=getShowerShape_wtots1(off);
    if(val_off!=0.){
        variables.push_back( Monitored::Scalar<float>("res_wtots1", (getShowerShape_wtots1(onl)-val_off)/val_off));
    }
    val_off=getShowerShape_f1(off);
    if(val_off!=0.){
        variables.push_back( Monitored::Scalar<float>("res_f1", (getShowerShape_f1(onl)-val_off)/val_off));
    }
    val_off=getShowerShape_f3(off);
    if(val_off!=0.){
        variables.push_back( Monitored::Scalar<float>("res_f3", (getShowerShape_f3(onl)-val_off)/val_off));
    }
    val_off=getShowerShape_Eratio(off);
    if(val_off!=0.){
        variables.push_back( Monitored::Scalar<float>("res_eratio", (getShowerShape_Eratio(onl)-val_off)/val_off));
    }

    std::vector<std::reference_wrapper<Monitored::IMonitoredVariable>>  mon_variables;
    for(auto&v:variables) mon_variables.push_back(v);
    fill( monGroup, mon_variables );
 

}

void TrigEgammaMonitorAnalysisAlgorithm::fillIsolationResolution(const std::string &trigger, const xAOD::Egamma * /*onl*/, const xAOD::Egamma * /*off*/) const
{
    auto monGroup = getGroup( trigger + "_Resolutions_Iso" );
    std::vector<std::reference_wrapper<Monitored::IMonitoredVariable>>  variables;

    fill( monGroup, variables );
}


/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include <sstream>

#include "GaudiKernel/IJobOptionsSvc.h"
#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/StatusCode.h"

#include "AthenaMonitoring/AthenaMonManager.h"
#include "HIMonitoring/HIMonitoringElectronsTool.h"

#include "LWHists/TH1D_LW.h"
#include "LWHists/TH2D_LW.h"
#include "LWHists/TProfile_LW.h"

HIMonitoringElectronsTool::
HIMonitoringElectronsTool( const std::string & type, const std::string & name,
const IInterface* parent ): ManagedMonitorToolBase( type, name, parent )   	     	  
{
	m_FCalEt=0;  
	m_FCalEt_nbins=70; 
	m_low_FCalEt=-0.5; 
	m_high_FCalEt=6.5; 

	m_eta_nbins = 60;
	m_low_eta = -3.0;
	m_high_eta = 3.0; 

	m_phi_nbins = 64;
	m_low_phi = -3.14159;
	m_high_phi = 3.14159;

	m_z0sintheta_nbins = 50;
	m_low_z0sintheta = -10.0;
	m_high_z0sintheta = 10.0;

	m_iso_nbins = 60;
	m_low_iso = -30.0;
	m_high_iso = 30.0;

	m_reta_nbins = 40;
	m_low_reta = 0.7;
	m_high_reta = 1.1;

	m_rphi_nbins = 50;
	m_low_rphi = 0.6;
	m_high_rphi = 1.1;

	m_weta2_nbins = 50;
	m_low_weta2 = 0.005;
	m_high_weta2 = 0.015;

	m_rhad_nbins = 60;
	m_low_rhad = -0.3;
	m_high_rhad = 0.3;

	m_wtots1_nbins = 60;
	m_low_wtots1 = 0.0;
	m_high_wtots1 = 6.0;

	m_eratio_nbins = 60;
	m_low_eratio = -0.1;
	m_high_eratio = 1.1;

	m_f1_nbins = 65;
	m_low_f1 = -0.05;
	m_high_f1 = 0.6;

	m_f3_nbins = 80;
	m_low_f3 = -0.05;
	m_high_f3 = 0.15;

	m_deltaeta1_nbins = 50;
	m_low_deltaeta1 = -0.05;
	m_high_deltaeta1 = 0.05;

	m_deltaphires_nbins = 50;
	m_low_deltaphires = -0.05;
	m_high_deltaphires = 0.05;

	m_eprobabilityht_nbins = 60;
	m_low_eprobabilityht = -0.1;
	m_high_eprobabilityht = 1.1;

	//cut-based selection tools
	m_electronLooseIsEMSelector = new AsgElectronIsEMSelector ( "ElectronLooseIsEMSelector" );
	m_electronLooseIsEMSelector->setProperty("isEMMask",egammaPID::ElectronLoose);
	m_electronLooseIsEMSelector->setProperty("ConfigFile","ElectronPhotonSelectorTools/offline/mc15_20150712/ElectronIsEMLooseSelectorCutDefs.conf");
	if (!m_electronLooseIsEMSelector->initialize().isSuccess()) {
	  Fatal("MyFunction", "Failed to initialize ElectronLooseIsEMSelector ");
	}

	m_electronMediumIsEMSelector = new AsgElectronIsEMSelector ( "ElectronMediumIsEMSelector" );
	m_electronMediumIsEMSelector->setProperty("isEMMask",egammaPID::ElectronMedium);
	m_electronMediumIsEMSelector->setProperty("ConfigFile","ElectronPhotonSelectorTools/offline/mc15_20150712/ElectronIsEMMediumSelectorCutDefs.conf");
	if (!m_electronMediumIsEMSelector->initialize().isSuccess()) {
	  Fatal("MyFunction", "Failed to initialize ElectronMediumIsEMSelector ");
	}

	//LH-based selection tools
	m_electronLooseLHTool = new AsgElectronLikelihoodTool ( "ElectronLooseLHTool" );
	m_electronLooseLHTool->setProperty("ConfigFile","ElectronPhotonSelectorTools/offline/mc15_20150712/ElectronLikelihoodLooseOfflineConfig2015.conf");
	if (!m_electronLooseLHTool->initialize().isSuccess()) {
	  Fatal("MyFunction", "Failed to initialize ElectronLooseLHTool ");
	}

	m_electronMediumLHTool = new AsgElectronLikelihoodTool ( "ElectronMediumLHTool" );
	m_electronMediumLHTool->setProperty("ConfigFile","ElectronPhotonSelectorTools/offline/mc15_20150712/ElectronLikelihoodMediumOfflineConfig2015.conf");
	if (!m_electronMediumLHTool->initialize().isSuccess()) {
	  Fatal("MyFunction", "Failed to initialize ElectronMediumLHTool ");
	}
}


HIMonitoringElectronsTool::~HIMonitoringElectronsTool()
{
}

// Description: Used for rebooking unmanaged histograms       
StatusCode HIMonitoringElectronsTool::bookHistogramsRecurrent( )
{
	return StatusCode::SUCCESS;
}


// Description: Used for re-booking managed histograms       
StatusCode HIMonitoringElectronsTool::bookHistograms( )
{
	if( m_environment == AthenaMonManager::online ) {
		// book histograms that are only made in the online environment...
	}

	if( m_dataType == AthenaMonManager::cosmics ) {
		// book histograms that are only relevant for cosmics data...
	}
   
	book_hist(); 

 
	return StatusCode::SUCCESS;
}


StatusCode HIMonitoringElectronsTool::fillHistograms()
{
	StatusCode sc; 
	
	/// HI event shape
	const xAOD::HIEventShapeContainer* evtShape;
	sc = evtStore()->retrieve(evtShape, "HIEventShape");
	if (sc.isFailure()) 
	{
		ATH_MSG_ERROR("Could not find HIEventShape");
		return StatusCode::FAILURE;
	} 
	else 
	{
		ATH_MSG_INFO("HIEventShape retrieved from StoreGate");
	}
	
	int size=evtShape->size();
	for(int i=0;i<size;i++)
	{       
		const xAOD::HIEventShape *sh=evtShape->at(i);
		if(sh->layer()==21 || sh->layer()==22 || sh->layer()==23)
		{
			m_FCalEt += sh->et();
		} 
	}
	m_FCalEt = m_FCalEt/1000000.;
	  	
 	  
	const xAOD::ElectronContainer* electrons = 0;
	sc = evtStore()->retrieve( electrons, "Electrons" );
	if (sc.isFailure()) {
	  ATH_MSG_ERROR("Could not find Electrons");
	  return StatusCode::FAILURE;
	}
	else {
	  ATH_MSG_INFO("Electrons retrieved from StoreGate");
	}

	xAOD::ElectronContainer::const_iterator electrons_itr = electrons->begin();
	xAOD::ElectronContainer::const_iterator electrons_end = electrons->end();

	int n_loose_CB = 0;
	int n_loose_LH = 0;

	int n_medium_CB = 0;
	int n_medium_LH = 0;

	for( ; electrons_itr != electrons_end; ++electrons_itr ) {	    
	    float pt = (*electrons_itr)->pt() * 1e-3;
	    float eta = (*electrons_itr)->caloCluster()->etaBE(2);

	    if (pt < 15 || fabs(eta) > 2.47) continue;

	    float phi = (*electrons_itr)->phi();
	    h_electron_eta_phi->Fill(eta,phi);

	    float z0 = (*electrons_itr)->trackParticle()->z0();
	    h_electron_z0sintheta->Fill(z0*sin(2*TMath::ATan(exp(-eta))));

	    bool loose_IsEM = m_electronLooseIsEMSelector->accept(*electrons_itr);
	    bool medium_IsEM = m_electronMediumIsEMSelector->accept(*electrons_itr);

	    bool loose_LH = m_electronLooseLHTool->accept(*electrons_itr);
	    bool medium_LH = m_electronMediumLHTool->accept(*electrons_itr);

	    float etcone20 = (*electrons_itr)->auxdata<float>("etcone20") * 1e-3;

	    float reta = (*electrons_itr)->auxdata<float>("Reta");
	    float rphi = (*electrons_itr)->auxdata<float>("Rphi");
	    float weta2 = (*electrons_itr)->auxdata< float >("weta2");

	    float rhad = (*electrons_itr)->auxdata<float>("Rhad");
	    float wtots1 = (*electrons_itr)->auxdata<float>("wtots1");

	    float eratio = (*electrons_itr)->auxdata<float>("Eratio");
	    float f1 = (*electrons_itr)->auxdata<float>("f1");
	    float f3 = (*electrons_itr)->auxdata<float>("f3");

	    float deltaeta1;
	    (*electrons_itr)->trackCaloMatchValue(deltaeta1, xAOD::EgammaParameters::deltaEta1);
	    float deltaphires;
	    (*electrons_itr)->trackCaloMatchValue(deltaphires, xAOD::EgammaParameters::deltaPhiRescaled2);
	    float TRT_PID;
	    (*electrons_itr)->trackParticle()->summaryValue(TRT_PID, xAOD::eProbabilityHT);

	    if (loose_IsEM){
	        n_loose_CB++;

	        h_electron_fcal_etcone20_looseCB->Fill(m_FCalEt, etcone20);

	        h_electron_fcal_reta_looseCB->Fill(m_FCalEt, reta);
	        h_electron_fcal_rphi_looseCB->Fill(m_FCalEt, rphi);
	        h_electron_fcal_weta2_looseCB->Fill(m_FCalEt, weta2);

	        h_electron_fcal_rhad_looseCB->Fill(m_FCalEt, rhad);
	        h_electron_fcal_wtots1_looseCB->Fill(m_FCalEt, wtots1);

	        h_electron_fcal_eratio_looseCB->Fill(m_FCalEt, eratio);
	        h_electron_fcal_f1_looseCB->Fill(m_FCalEt, f1);
	        h_electron_fcal_f3_looseCB->Fill(m_FCalEt, f3);

	        h_electron_fcal_deltaeta1_looseCB->Fill(m_FCalEt, deltaeta1);
	        h_electron_fcal_deltaphires_looseCB->Fill(m_FCalEt, deltaphires);
	        if (fabs(eta) < 2.01) h_electron_fcal_eprobabilityht_looseCB->Fill(m_FCalEt, TRT_PID);
	    }

	    if (loose_LH){
	        n_loose_LH++;

	        h_electron_fcal_etcone20_looseLH->Fill(m_FCalEt, etcone20);

	        h_electron_fcal_reta_looseLH->Fill(m_FCalEt, reta);
	        h_electron_fcal_rphi_looseLH->Fill(m_FCalEt, rphi);
	        h_electron_fcal_weta2_looseLH->Fill(m_FCalEt, weta2);

	        h_electron_fcal_rhad_looseLH->Fill(m_FCalEt, rhad);
	        h_electron_fcal_wtots1_looseLH->Fill(m_FCalEt, wtots1);

	        h_electron_fcal_eratio_looseLH->Fill(m_FCalEt, eratio);
	        h_electron_fcal_f1_looseLH->Fill(m_FCalEt, f1);
	        h_electron_fcal_f3_looseLH->Fill(m_FCalEt, f3);

	        h_electron_fcal_deltaeta1_looseLH->Fill(m_FCalEt, deltaeta1);
	        h_electron_fcal_deltaphires_looseLH->Fill(m_FCalEt, deltaphires);
	        if (fabs(eta) < 2.01) h_electron_fcal_eprobabilityht_looseLH->Fill(m_FCalEt, TRT_PID);
	    }

	    if (medium_IsEM){
	        n_medium_CB++;

	        h_electron_fcal_etcone20_mediumCB->Fill(m_FCalEt, etcone20);

	        h_electron_fcal_reta_mediumCB->Fill(m_FCalEt, reta);
	        h_electron_fcal_rphi_mediumCB->Fill(m_FCalEt, rphi);
	        h_electron_fcal_weta2_mediumCB->Fill(m_FCalEt, weta2);

	        h_electron_fcal_rhad_mediumCB->Fill(m_FCalEt, rhad);
	        h_electron_fcal_wtots1_mediumCB->Fill(m_FCalEt, wtots1);

	        h_electron_fcal_eratio_mediumCB->Fill(m_FCalEt, eratio);
	        h_electron_fcal_f1_mediumCB->Fill(m_FCalEt, f1);
	        h_electron_fcal_f3_mediumCB->Fill(m_FCalEt, f3);

	        h_electron_fcal_deltaeta1_mediumCB->Fill(m_FCalEt, deltaeta1);
	        h_electron_fcal_deltaphires_mediumCB->Fill(m_FCalEt, deltaphires);
	        if (fabs(eta) < 2.01) h_electron_fcal_eprobabilityht_mediumCB->Fill(m_FCalEt, TRT_PID);
	    }

	    if (medium_LH){
	        n_medium_LH++;

	        h_electron_fcal_etcone20_mediumLH->Fill(m_FCalEt, etcone20);

	        h_electron_fcal_reta_mediumLH->Fill(m_FCalEt, reta);
	        h_electron_fcal_rphi_mediumLH->Fill(m_FCalEt, rphi);
	        h_electron_fcal_weta2_mediumLH->Fill(m_FCalEt, weta2);

	        h_electron_fcal_rhad_mediumLH->Fill(m_FCalEt, rhad);
	        h_electron_fcal_wtots1_mediumLH->Fill(m_FCalEt, wtots1);

	        h_electron_fcal_eratio_mediumLH->Fill(m_FCalEt, eratio);
	        h_electron_fcal_f1_mediumLH->Fill(m_FCalEt, f1);
	        h_electron_fcal_f3_mediumLH->Fill(m_FCalEt, f3);

	        h_electron_fcal_deltaeta1_mediumLH->Fill(m_FCalEt, deltaeta1);
	        h_electron_fcal_deltaphires_mediumLH->Fill(m_FCalEt, deltaphires);
	        if (fabs(eta) < 2.01) h_electron_fcal_eprobabilityht_mediumLH->Fill(m_FCalEt, TRT_PID);
	    }
	}

	if (n_loose_CB) h_FCalEt_looseCB->Fill(m_FCalEt);
	if (n_loose_LH) h_FCalEt_looseLH->Fill(m_FCalEt);

	if (n_medium_CB) h_FCalEt_mediumCB->Fill(m_FCalEt);
	if (n_medium_LH) h_FCalEt_mediumLH->Fill(m_FCalEt);
	
	return StatusCode::SUCCESS;
}


StatusCode HIMonitoringElectronsTool::procHistograms( )
{
   
	if( endOfLowStat || endOfLumiBlock ) 
	{
	   
	}


	if( endOfRun ) 
	{
   
	}

	return StatusCode::SUCCESS;
}


void HIMonitoringElectronsTool::book_hist()
{
	std::string path = "HeavyIon/Electrons"; 
	
	//FCal sum Et
	h_FCalEt_looseCB = TH1D_LW::create( "h_FCalEt_looseCB", "; FCal #Sigma E_{T} [TeV]; entries", m_FCalEt_nbins, m_low_FCalEt, m_high_FCalEt); 
	regHist(h_FCalEt_looseCB, path, run).ignore();
	h_FCalEt_looseLH= TH1D_LW::create( "h_FCalEt_looseLH", "; FCal #Sigma E_{T} [TeV]; entries", m_FCalEt_nbins, m_low_FCalEt, m_high_FCalEt); 
	regHist(h_FCalEt_looseLH, path, run).ignore();
	h_FCalEt_mediumCB = TH1D_LW::create( "h_FCalEt_mediumCB", "; FCal #Sigma E_{T} [TeV]; entries", m_FCalEt_nbins, m_low_FCalEt, m_high_FCalEt); 
	regHist(h_FCalEt_mediumCB, path, run).ignore();
	h_FCalEt_mediumLH= TH1D_LW::create( "h_FCalEt_mediumLH", "; FCal #Sigma E_{T} [TeV]; entries", m_FCalEt_nbins, m_low_FCalEt, m_high_FCalEt); 
	regHist(h_FCalEt_mediumLH, path, run).ignore();

	//eta vs phi
	h_electron_eta_phi = TH2D_LW::create( "h_electron_eta_phi", "; #eta; #phi", m_eta_nbins, m_low_eta, m_high_eta, m_phi_nbins, m_low_phi, m_high_phi); 
	regHist(h_electron_eta_phi, path, run).ignore();

	//eta vs phi
	h_electron_z0sintheta = TH1D_LW::create( "h_electron_z0sintheta", "; z0 sin#theta [mm]; entries", m_z0sintheta_nbins, m_low_z0sintheta, m_high_z0sintheta); 
	regHist(h_electron_z0sintheta, path, run).ignore();

	//isolation vs centrality
	h_electron_fcal_etcone20_looseCB = TH2D_LW::create( "h_electron_fcal_etcone20_looseCB", "; FCal #Sigma E_{T} [TeV]; electron etcone20 [MeV]", m_FCalEt_nbins, m_low_FCalEt, m_high_FCalEt, m_iso_nbins, m_low_iso, m_high_iso );
	regHist(h_electron_fcal_etcone20_looseCB, path, run).ignore();
	h_electron_fcal_etcone20_looseLH = TH2D_LW::create( "h_electron_fcal_etcone20_looseLH", "; FCal #Sigma E_{T} [TeV]; electron etcone20 [MeV]", m_FCalEt_nbins, m_low_FCalEt, m_high_FCalEt, m_iso_nbins, m_low_iso, m_high_iso );
	regHist(h_electron_fcal_etcone20_looseLH, path, run).ignore();
	h_electron_fcal_etcone20_mediumCB = TH2D_LW::create( "h_electron_fcal_etcone20_mediumCB", "; FCal #Sigma E_{T} [TeV]; electron etcone20 [MeV]", m_FCalEt_nbins, m_low_FCalEt, m_high_FCalEt, m_iso_nbins, m_low_iso, m_high_iso );
	regHist(h_electron_fcal_etcone20_mediumCB, path, run).ignore();
	h_electron_fcal_etcone20_mediumLH = TH2D_LW::create( "h_electron_fcal_etcone20_mediumLH", "; FCal #Sigma E_{T} [TeV]; electron etcone20 [MeV]", m_FCalEt_nbins, m_low_FCalEt, m_high_FCalEt, m_iso_nbins, m_low_iso, m_high_iso );
	regHist(h_electron_fcal_etcone20_mediumLH, path, run).ignore();

	//R_eta
	h_electron_fcal_reta_looseCB = TH2D_LW::create( "h_electron_fcal_reta_looseCB", "; FCal #Sigma E_{T} [TeV]; R_{#eta}", m_FCalEt_nbins, m_low_FCalEt, m_high_FCalEt, m_reta_nbins, m_low_reta, m_high_reta );
	regHist(h_electron_fcal_reta_looseCB, path, run).ignore();
	h_electron_fcal_reta_looseLH = TH2D_LW::create( "h_electron_fcal_reta_looseLH", "; FCal #Sigma E_{T} [TeV]; R_{#eta}", m_FCalEt_nbins, m_low_FCalEt, m_high_FCalEt, m_reta_nbins, m_low_reta, m_high_reta );
	regHist(h_electron_fcal_reta_looseLH, path, run).ignore();
	h_electron_fcal_reta_mediumCB = TH2D_LW::create( "h_electron_fcal_reta_mediumCB", "; FCal #Sigma E_{T} [TeV]; R_{#eta}", m_FCalEt_nbins, m_low_FCalEt, m_high_FCalEt, m_reta_nbins, m_low_reta, m_high_reta );
	regHist(h_electron_fcal_reta_mediumCB, path, run).ignore();
	h_electron_fcal_reta_mediumLH = TH2D_LW::create( "h_electron_fcal_reta_mediumLH", "; FCal #Sigma E_{T} [TeV]; R_{#eta}", m_FCalEt_nbins, m_low_FCalEt, m_high_FCalEt, m_reta_nbins, m_low_reta, m_high_reta );
	regHist(h_electron_fcal_reta_mediumLH, path, run).ignore();

	//R_phi
	h_electron_fcal_rphi_looseCB = TH2D_LW::create( "h_electron_fcal_rphi_looseCB", "; FCal #Sigma E_{T} [TeV]; R_{#phi}", m_FCalEt_nbins, m_low_FCalEt, m_high_FCalEt, m_rphi_nbins, m_low_rphi, m_high_rphi );
	regHist(h_electron_fcal_rphi_looseCB, path, run).ignore();
	h_electron_fcal_rphi_looseLH = TH2D_LW::create( "h_electron_fcal_rphi_looseLH", "; FCal #Sigma E_{T} [TeV]; R_{#phi}", m_FCalEt_nbins, m_low_FCalEt, m_high_FCalEt, m_rphi_nbins, m_low_rphi, m_high_rphi );
	regHist(h_electron_fcal_rphi_looseLH, path, run).ignore();
	h_electron_fcal_rphi_mediumCB = TH2D_LW::create( "h_electron_fcal_rphi_mediumCB", "; FCal #Sigma E_{T} [TeV]; R_{#phi}", m_FCalEt_nbins, m_low_FCalEt, m_high_FCalEt, m_rphi_nbins, m_low_rphi, m_high_rphi );
	regHist(h_electron_fcal_rphi_mediumCB, path, run).ignore();
	h_electron_fcal_rphi_mediumLH = TH2D_LW::create( "h_electron_fcal_rphi_mediumLH", "; FCal #Sigma E_{T} [TeV]; R_{#phi}", m_FCalEt_nbins, m_low_FCalEt, m_high_FCalEt, m_rphi_nbins, m_low_rphi, m_high_rphi );
	regHist(h_electron_fcal_rphi_mediumLH, path, run).ignore();

	//W_eta2
	h_electron_fcal_weta2_looseCB = TH2D_LW::create( "h_electron_fcal_weta2_looseCB", "; FCal #Sigma E_{T} [TeV]; W_{#eta^{2}}", m_FCalEt_nbins, m_low_FCalEt, m_high_FCalEt, m_weta2_nbins, m_low_weta2, m_high_weta2 );
	regHist(h_electron_fcal_weta2_looseCB, path, run).ignore();
	h_electron_fcal_weta2_looseLH = TH2D_LW::create( "h_electron_fcal_weta2_looseLH", "; FCal #Sigma E_{T} [TeV]; W_{#eta^{2}}", m_FCalEt_nbins, m_low_FCalEt, m_high_FCalEt, m_weta2_nbins, m_low_weta2, m_high_weta2 );
	regHist(h_electron_fcal_weta2_looseLH, path, run).ignore();
	h_electron_fcal_weta2_mediumCB = TH2D_LW::create( "h_electron_fcal_weta2_mediumCB", "; FCal #Sigma E_{T} [TeV]; W_{#eta^{2}}", m_FCalEt_nbins, m_low_FCalEt, m_high_FCalEt, m_weta2_nbins, m_low_weta2, m_high_weta2 );
	regHist(h_electron_fcal_weta2_mediumCB, path, run).ignore();
	h_electron_fcal_weta2_mediumLH = TH2D_LW::create( "h_electron_fcal_weta2_mediumLH", "; FCal #Sigma E_{T} [TeV]; W_{#eta^{2}}", m_FCalEt_nbins, m_low_FCalEt, m_high_FCalEt, m_weta2_nbins, m_low_weta2, m_high_weta2 );
	regHist(h_electron_fcal_weta2_mediumLH, path, run).ignore();

	//R_had
	h_electron_fcal_rhad_looseCB = TH2D_LW::create( "h_electron_fcal_rhad_looseCB", "; FCal #Sigma E_{T} [TeV]; R_{had}", m_FCalEt_nbins, m_low_FCalEt, m_high_FCalEt, m_rhad_nbins, m_low_rhad, m_high_rhad );
	regHist(h_electron_fcal_rhad_looseCB, path, run).ignore();
	h_electron_fcal_rhad_looseLH = TH2D_LW::create( "h_electron_fcal_rhad_looseLH", "; FCal #Sigma E_{T} [TeV]; R_{had}", m_FCalEt_nbins, m_low_FCalEt, m_high_FCalEt, m_rhad_nbins, m_low_rhad, m_high_rhad );
	regHist(h_electron_fcal_rhad_looseLH, path, run).ignore();
	h_electron_fcal_rhad_mediumCB = TH2D_LW::create( "h_electron_fcal_rhad_mediumCB", "; FCal #Sigma E_{T} [TeV]; R_{had}", m_FCalEt_nbins, m_low_FCalEt, m_high_FCalEt, m_rhad_nbins, m_low_rhad, m_high_rhad );
	regHist(h_electron_fcal_rhad_mediumCB, path, run).ignore();
	h_electron_fcal_rhad_mediumLH = TH2D_LW::create( "h_electron_fcal_rhad_mediumLH", "; FCal #Sigma E_{T} [TeV]; R_{had}", m_FCalEt_nbins, m_low_FCalEt, m_high_FCalEt, m_rhad_nbins, m_low_rhad, m_high_rhad );
	regHist(h_electron_fcal_rhad_mediumLH, path, run).ignore();

	//W_tots1
	h_electron_fcal_wtots1_looseCB = TH2D_LW::create( "h_electron_fcal_wtots1_looseCB", "; FCal #Sigma E_{T} [TeV]; W_{tots1}", m_FCalEt_nbins, m_low_FCalEt, m_high_FCalEt, m_wtots1_nbins, m_low_wtots1, m_high_wtots1 );
	regHist(h_electron_fcal_wtots1_looseCB, path, run).ignore();
	h_electron_fcal_wtots1_looseLH = TH2D_LW::create( "h_electron_fcal_wtots1_looseLH", "; FCal #Sigma E_{T} [TeV]; W_{tots1}", m_FCalEt_nbins, m_low_FCalEt, m_high_FCalEt, m_wtots1_nbins, m_low_wtots1, m_high_wtots1 );
	regHist(h_electron_fcal_wtots1_looseLH, path, run).ignore();
	h_electron_fcal_wtots1_mediumCB = TH2D_LW::create( "h_electron_fcal_wtots1_mediumCB", "; FCal #Sigma E_{T} [TeV]; W_{tots1}", m_FCalEt_nbins, m_low_FCalEt, m_high_FCalEt, m_wtots1_nbins, m_low_wtots1, m_high_wtots1 );
	regHist(h_electron_fcal_wtots1_mediumCB, path, run).ignore();
	h_electron_fcal_wtots1_mediumLH = TH2D_LW::create( "h_electron_fcal_wtots1_mediumLH", "; FCal #Sigma E_{T} [TeV]; W_{tots1}", m_FCalEt_nbins, m_low_FCalEt, m_high_FCalEt, m_wtots1_nbins, m_low_wtots1, m_high_wtots1 );
	regHist(h_electron_fcal_wtots1_mediumLH, path, run).ignore();

	//E_ratio
	h_electron_fcal_eratio_looseCB = TH2D_LW::create( "h_electron_fcal_eratio_looseCB", "; FCal #Sigma E_{T} [TeV]; E_{ratio}", m_FCalEt_nbins, m_low_FCalEt, m_high_FCalEt, m_eratio_nbins, m_low_eratio, m_high_eratio );
	regHist(h_electron_fcal_eratio_looseCB, path, run).ignore();
	h_electron_fcal_eratio_looseLH = TH2D_LW::create( "h_electron_fcal_eratio_looseLH", "; FCal #Sigma E_{T} [TeV]; E_{ratio}", m_FCalEt_nbins, m_low_FCalEt, m_high_FCalEt, m_eratio_nbins, m_low_eratio, m_high_eratio );
	regHist(h_electron_fcal_eratio_looseLH, path, run).ignore();
	h_electron_fcal_eratio_mediumCB = TH2D_LW::create( "h_electron_fcal_eratio_mediumCB", "; FCal #Sigma E_{T} [TeV]; E_{ratio}", m_FCalEt_nbins, m_low_FCalEt, m_high_FCalEt, m_eratio_nbins, m_low_eratio, m_high_eratio );
	regHist(h_electron_fcal_eratio_mediumCB, path, run).ignore();
	h_electron_fcal_eratio_mediumLH = TH2D_LW::create( "h_electron_fcal_eratio_mediumLH", "; FCal #Sigma E_{T} [TeV]; E_{ratio}", m_FCalEt_nbins, m_low_FCalEt, m_high_FCalEt, m_eratio_nbins, m_low_eratio, m_high_eratio );
	regHist(h_electron_fcal_eratio_mediumLH, path, run).ignore();

	//f_1
	h_electron_fcal_f1_looseCB = TH2D_LW::create( "h_electron_fcal_f1_looseCB", "; FCal #Sigma E_{T} [TeV]; f_{1}", m_FCalEt_nbins, m_low_FCalEt, m_high_FCalEt, m_f1_nbins, m_low_f1, m_high_f1 );
	regHist(h_electron_fcal_f1_looseCB, path, run).ignore();
	h_electron_fcal_f1_looseLH = TH2D_LW::create( "h_electron_fcal_f1_looseLH", "; FCal #Sigma E_{T} [TeV]; f_{1}", m_FCalEt_nbins, m_low_FCalEt, m_high_FCalEt, m_f1_nbins, m_low_f1, m_high_f1 );
	regHist(h_electron_fcal_f1_looseLH, path, run).ignore();
	h_electron_fcal_f1_mediumCB = TH2D_LW::create( "h_electron_fcal_f1_mediumCB", "; FCal #Sigma E_{T} [TeV]; f_{1}", m_FCalEt_nbins, m_low_FCalEt, m_high_FCalEt, m_f1_nbins, m_low_f1, m_high_f1 );
	regHist(h_electron_fcal_f1_mediumCB, path, run).ignore();
	h_electron_fcal_f1_mediumLH = TH2D_LW::create( "h_electron_fcal_f1_mediumLH", "; FCal #Sigma E_{T} [TeV]; f_{1}", m_FCalEt_nbins, m_low_FCalEt, m_high_FCalEt, m_f1_nbins, m_low_f1, m_high_f1 );
	regHist(h_electron_fcal_f1_mediumLH, path, run).ignore();

	//f_3
	h_electron_fcal_f3_looseCB = TH2D_LW::create( "h_electron_fcal_f3_looseCB", "; FCal #Sigma E_{T} [TeV]; f_{3}", m_FCalEt_nbins, m_low_FCalEt, m_high_FCalEt, m_f3_nbins, m_low_f3, m_high_f3 );
	regHist(h_electron_fcal_f3_looseCB, path, run).ignore();
	h_electron_fcal_f3_looseLH = TH2D_LW::create( "h_electron_fcal_f3_looseLH", "; FCal #Sigma E_{T} [TeV]; f_{3}", m_FCalEt_nbins, m_low_FCalEt, m_high_FCalEt, m_f3_nbins, m_low_f3, m_high_f3 );
	regHist(h_electron_fcal_f3_looseLH, path, run).ignore();
	h_electron_fcal_f3_mediumCB = TH2D_LW::create( "h_electron_fcal_f3_mediumCB", "; FCal #Sigma E_{T} [TeV]; f_{3}", m_FCalEt_nbins, m_low_FCalEt, m_high_FCalEt, m_f3_nbins, m_low_f3, m_high_f3 );
	regHist(h_electron_fcal_f3_mediumCB, path, run).ignore();
	h_electron_fcal_f3_mediumLH = TH2D_LW::create( "h_electron_fcal_f3_mediumLH", "; FCal #Sigma E_{T} [TeV]; f_{3}", m_FCalEt_nbins, m_low_FCalEt, m_high_FCalEt, m_f3_nbins, m_low_f3, m_high_f3 );
	regHist(h_electron_fcal_f3_mediumLH, path, run).ignore();

	//delta_eta1
	h_electron_fcal_deltaeta1_looseCB = TH2D_LW::create( "h_electron_fcal_deltaeta1_looseCB", "; FCal #Sigma E_{T} [TeV]; #Delta#eta", m_FCalEt_nbins, m_low_FCalEt, m_high_FCalEt, m_deltaeta1_nbins, m_low_deltaeta1, m_high_deltaeta1 );
	regHist(h_electron_fcal_deltaeta1_looseCB, path, run).ignore();
	h_electron_fcal_deltaeta1_looseLH = TH2D_LW::create( "h_electron_fcal_deltaeta1_looseLH", "; FCal #Sigma E_{T} [TeV]; #Delta#eta", m_FCalEt_nbins, m_low_FCalEt, m_high_FCalEt, m_deltaeta1_nbins, m_low_deltaeta1, m_high_deltaeta1 );
	regHist(h_electron_fcal_deltaeta1_looseLH, path, run).ignore();
	h_electron_fcal_deltaeta1_mediumCB = TH2D_LW::create( "h_electron_fcal_deltaeta1_mediumCB", "; FCal #Sigma E_{T} [TeV]; #Delta#eta", m_FCalEt_nbins, m_low_FCalEt, m_high_FCalEt, m_deltaeta1_nbins, m_low_deltaeta1, m_high_deltaeta1 );
	regHist(h_electron_fcal_deltaeta1_mediumCB, path, run).ignore();
	h_electron_fcal_deltaeta1_mediumLH = TH2D_LW::create( "h_electron_fcal_deltaeta1_mediumLH", "; FCal #Sigma E_{T} [TeV]; #Delta#eta", m_FCalEt_nbins, m_low_FCalEt, m_high_FCalEt, m_deltaeta1_nbins, m_low_deltaeta1, m_high_deltaeta1 );
	regHist(h_electron_fcal_deltaeta1_mediumLH, path, run).ignore();

	//delta_phi_rescaled
	h_electron_fcal_deltaphires_looseCB = TH2D_LW::create( "h_electron_fcal_deltaphires_looseCB", "; FCal #Sigma E_{T} [TeV]; #Delta#phi rescaled", m_FCalEt_nbins, m_low_FCalEt, m_high_FCalEt, m_deltaphires_nbins, m_low_deltaphires, m_high_deltaphires );
	regHist(h_electron_fcal_deltaphires_looseCB, path, run).ignore();
	h_electron_fcal_deltaphires_looseLH = TH2D_LW::create( "h_electron_fcal_deltaphires_looseLH", "; FCal #Sigma E_{T} [TeV]; #Delta#phi rescaled", m_FCalEt_nbins, m_low_FCalEt, m_high_FCalEt, m_deltaphires_nbins, m_low_deltaphires, m_high_deltaphires );
	regHist(h_electron_fcal_deltaphires_looseLH, path, run).ignore();
	h_electron_fcal_deltaphires_mediumCB = TH2D_LW::create( "h_electron_fcal_deltaphires_mediumCB", "; FCal #Sigma E_{T} [TeV]; #Delta#phi rescaled", m_FCalEt_nbins, m_low_FCalEt, m_high_FCalEt, m_deltaphires_nbins, m_low_deltaphires, m_high_deltaphires );
	regHist(h_electron_fcal_deltaphires_mediumCB, path, run).ignore();
	h_electron_fcal_deltaphires_mediumLH = TH2D_LW::create( "h_electron_fcal_deltaphires_mediumLH", "; FCal #Sigma E_{T} [TeV]; #Delta#phi rescaled", m_FCalEt_nbins, m_low_FCalEt, m_high_FCalEt, m_deltaphires_nbins, m_low_deltaphires, m_high_deltaphires );
	regHist(h_electron_fcal_deltaphires_mediumLH, path, run).ignore();

	//e_probability_HT
	h_electron_fcal_eprobabilityht_looseCB = TH2D_LW::create( "h_electron_fcal_eprobabilityht_looseCB", "; FCal #Sigma E_{T} [TeV]; eProbabilityHT", m_FCalEt_nbins, m_low_FCalEt, m_high_FCalEt, m_eprobabilityht_nbins, m_low_eprobabilityht, m_high_eprobabilityht );
	regHist(h_electron_fcal_eprobabilityht_looseCB, path, run).ignore();
	h_electron_fcal_eprobabilityht_looseLH = TH2D_LW::create( "h_electron_fcal_eprobabilityht_looseLH", "; FCal #Sigma E_{T} [TeV]; eProbabilityHT", m_FCalEt_nbins, m_low_FCalEt, m_high_FCalEt, m_eprobabilityht_nbins, m_low_eprobabilityht, m_high_eprobabilityht );
	regHist(h_electron_fcal_eprobabilityht_looseLH, path, run).ignore();
	h_electron_fcal_eprobabilityht_mediumCB = TH2D_LW::create( "h_electron_fcal_eprobabilityht_mediumCB", "; FCal #Sigma E_{T} [TeV]; eProbabilityHT", m_FCalEt_nbins, m_low_FCalEt, m_high_FCalEt, m_eprobabilityht_nbins, m_low_eprobabilityht, m_high_eprobabilityht );
	regHist(h_electron_fcal_eprobabilityht_mediumCB, path, run).ignore();
	h_electron_fcal_eprobabilityht_mediumLH = TH2D_LW::create( "h_electron_fcal_eprobabilityht_mediumLH", "; FCal #Sigma E_{T} [TeV]; eProbabilityHT", m_FCalEt_nbins, m_low_FCalEt, m_high_FCalEt, m_eprobabilityht_nbins, m_low_eprobabilityht, m_high_eprobabilityht );
	regHist(h_electron_fcal_eprobabilityht_mediumLH, path, run).ignore();
}

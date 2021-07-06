/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/


/**
 * NAME : 	EFexAnalysis.cxx
 * PACKAGE : 	Trigger/TrigL1Upgrade/TrigL1CaloUpgrade/EFexAnalysis
 *
 * AUTHOR : 	Denis Oliveira Damazio
 *
 *
 * **/

#include "EFexAnalysis.h"
#include <math.h>
#include <string>
#include "TFile.h"
#include "TH1F.h"
#include "TH2F.h"
#include "xAODTrigCalo/TrigEMClusterContainer.h"
#include "xAODTruth/TruthParticleContainer.h"
#include "xAODEgamma/ElectronContainer.h"
#include "xAODTracking/VertexContainer.h"
#include "xAODTrigger/EmTauRoIContainer.h"

EFexAnalysis::EFexAnalysis( const std::string& name, ISvcLocator* pSvcLocator ) : AthAlgorithm (name, pSvcLocator){
       declareProperty("EnableMonitoring", m_enableMon=false);
       declareProperty("DoTruth", m_doTruth=true);
       declareProperty("DoOffline", m_doOffline=true);
       declareProperty("InputClusterName", m_inputClusterName = "SCluster" );
       declareProperty("InputLvl1Name", m_inputLvl1Name = "LVL1EmTauRoIs" );
}

EFexAnalysis::~EFexAnalysis(){}

StatusCode EFexAnalysis::initialize(){

        MsgStream msg(msgSvc(), name());
	msg << MSG::DEBUG << "initializing TrigT1CaloEFex" << endmsg;
        if ( m_enableMon ){
		std::string histoName(name());
		histoName+="Algo.root";
		m_histFile = new TFile(histoName.c_str(),"RECREATE");
		m_EtSElectron = new TH1F("EtSElectron","Et of Super Cell based Electron",60,0,60);
		m_HadEtSElectron = new TH1F("HadEtSElectron","HadEt of Super Cell based Electron",60,0,6);
		m_EtaSElectron = new TH1F("EtaSElectron","Eta of Super Cell based Electron",50,-2.5,2.5);
		m_PhiSElectron = new TH1F("PhiSElectron","Phi of Super Cell based Electron",64,-3.2,3.2);
		m_EtSElectronEta = new TH2F("EtSElectronEta","Et of Super Cell based Electron versus Eta",50,-2.5,2.5,60,0,60);
		m_HadEtSElectronEta = new TH2F("HadEtSElectronEta","HadEt of Super Cell based Electron versus Eta",50,-2.5,2.5,60,0,6);
		m_rEta = new TH1F("rEta","ratio of the energy in a 3x2 cluster to a 7x2 one",60,0,1.2);
		m_f1 = new TH1F("f1","ratio of the cluster energy in the first layer",70,-0.2,1.2);
		m_f3 = new TH1F("f3","ratio of the cluster energy in the third layer",140,-0.2,1.2);
		m_EtSElectronA = new TH1F("EtSElectronA","Et of Super Cell based Electron",60,0,60);
		m_HadEtSElectronA = new TH1F("HadEtSElectronA","HadEt of Super Cell based Electron",60,0,6);
		m_EtaSElectronA = new TH1F("EtaSElectronA","Eta of Super Cell based Electron",50,-5.0,5.0);
		m_PhiSElectronA = new TH1F("PhiSElectronA","Phi of Super Cell based Electron",64,-3.2,3.2);
		m_EtSElectronEtaA = new TH2F("EtSElectronEtaA","Et of Super Cell based Electron versus Eta",50,-5.0,5.0,60,0,60);
		m_HadEtSElectronEtaA = new TH2F("HadEtSElectronEtaA","HadEt of Super Cell based Electron versus Eta",50,-5.0,5.0,60,0,6);
		m_rEtaA = new TH1F("rEtaA","ratio of the energy in a 3x2 cluster to a 7x2 one",60,0,1.2);
		m_f1A = new TH1F("f1A","ratio of the cluster energy in the first layer",70,-0.2,1.2);
		m_f3A = new TH1F("f3A","ratio of the cluster energy in the third layer",140,-0.2,1.2);

	        if ( m_doTruth ) {
		m_res_truth = new TH1F("res_truth","resolution with respect truth",60,-30.0,30.0);
		m_res_truth_eta = new TH2F("res_truth_eta","resolution with respect truth versus eta",50,-5.0,5.0,60,-30.0,30.0);
		m_res_truth_pt = new TH2F("res_truth_pt","resolution with respect truth versus pt",60,0.0,60.0,60,-30.0,30.0);
		m_res_truth_ptA = new TH2F("res_truth_ptA","resolution with respect truth versus pt",60,0.0,60.0,60,-30.0,30.0);

		m_eff_truth_pt_n = new TH1F("eff_truth_pt_n","efficiency with respect to truth pt",60,0,60.0);
		m_eff_truth_pt_d = new TH1F("eff_truth_pt_d","efficiency with respect to truth pt",60,0,60.0);
		m_eff_truth_ptA_n = new TH1F("eff_truth_ptA_n","efficiency with respect to truth pt",60,0,60.0);
		m_eff_truth_ptA_d = new TH1F("eff_truth_ptA_d","efficiency with respect to truth pt",60,0,60.0);
		m_eff_truth_eta_n = new TH1F("eff_truth_eta_n","efficiency with respect to truth eta",50,-5.0,5.0);
		m_eff_truth_eta_d = new TH1F("eff_truth_eta_d","efficiency with respect to truth eta",50,-5.0,5.0);
		}
		if ( m_doOffline ) {
		m_res_off = new TH1F("res_off","resolution with respect offline electron",60,-30.0,30.0);
		m_res_off_eta = new TH2F("res_off_eta","resolution with respect offline versus eta",50,-5.0,5.0,60,-30.0,30.0);
		m_res_off_pt = new TH2F("res_off_pt","resolution with respect offline versus pt",60,0.0,60.0,60,-30.0,30.0);
		m_res_off_nvtx = new TH2F("res_off_nvtx","resolution with respect offline versus nvtx",120,0.0,120.0,60,-30.0,30.0);

		m_res_phi_off = new TH1F("res_phi_off","phi resolution with respect offline electron",60,-30.0,30.0);
		m_res_phi_off_eta = new TH2F("res_phi_off_eta","phi resolution with respect offline versus eta",50,-5.0,5.0,60,-30.0,30.0);
		m_res_phi_off_pt = new TH2F("res_phi_off_pt","phi resolution with respect offline versus pt",60,0.0,60.0,60,-30.0,30.0);

		m_res_eta_off = new TH1F("res_eta_off","eta resolution with respect offline electron",60,-30.0,30.0);
		m_res_eta_off_eta = new TH2F("res_eta_off_eta","eta resolution with respect offline versus eta",50,-5.0,5.0,60,-30.0,30.0);
		m_res_eta_off_pt = new TH2F("res_eta_off_pt","eta resolution with respect offline versus pt",60,0.0,60.0,60,-30.0,30.0);

		m_res_rEta_off = new TH1F("res_rEta_off","rEta resolution with respect offline electron",60,-30.0,30.0);
		m_res_rEta_off_eta = new TH2F("res_rEta_off_eta","rEta resolution with respect offline versus eta",50,-5.0,5.0,60,-30.0,30.0);
		m_res_rEta_off_pt = new TH2F("res_rEta_off_pt","rEta resolution with respect offline versus pt",60,0.0,60.0,60,-30.0,30.0);
		m_res_rEta_off_nvtx = new TH2F("res_rEta_off_nvtx","rEta resolution with respect offline versus nvtx",120,0.0,120.0,60,-30.0,30.0);

		m_res_f1_off = new TH1F("res_f1_off","f1 resolution with respect offline electron",60,-30.0,30.0);
		m_res_f1_off_eta = new TH2F("res_f1_off_eta","f1 resolution with respect offline versus eta",50,-5.0,5.0,60,-30.0,30.0);
		m_res_f1_off_pt = new TH2F("res_f1_off_pt","f1 resolution with respect offline versus pt",60,0.0,60.0,60,-30.0,30.0);
		m_res_f1_off_nvtx = new TH2F("res_f1_off_nvtx","f1 resolution with respect offline versus nvtx",120,0.0,120.0,60,-30.0,30.0);

		m_res_f3_off = new TH1F("res_f3_off","f3 resolution with respect offline electron",60,-30.0,30.0);
		m_res_f3_off_eta = new TH2F("res_f3_off_eta","f3 resolution with respect offline versus eta",50,-5.0,5.0,60,-30.0,30.0);
		m_res_f3_off_pt = new TH2F("res_f3_off_pt","f3 resolution with respect offline versus pt",60,0.0,60.0,60,-30.0,30.0);

		m_eff_off_pt_n = new TH1F("eff_off_pt_n","efficiency with respect to offline pt",60,0,60.0);
		m_eff_off_pt_d = new TH1F("eff_off_pt_d","efficiency with respect to offline pt",60,0,60.0);
		m_eff_off_eta_n = new TH1F("eff_off_eta_n","efficiency with respect to offline eta",50,-5.0,5.0);
		m_eff_off_eta_d = new TH1F("eff_off_eta_d","efficiency with respect to offline eta",50,-5.0,5.0);
		}
	}
	return StatusCode::SUCCESS;
}

StatusCode EFexAnalysis::finalize(){
        MsgStream msg(msgSvc(), name());
	msg << MSG::DEBUG << "finalizing TrigT1CaloEFex" << endmsg;
        if ( m_enableMon ){
		m_histFile->Write();
		m_histFile->Close();
	}
	return StatusCode::SUCCESS;
}

StatusCode EFexAnalysis::execute(){

        MsgStream msg(msgSvc(), name());
	msg << MSG::DEBUG << "execute TrigT1CaloEFex" << endmsg;
        const xAOD::TrigEMClusterContainer* scluster(nullptr);
	if ( evtStore()->retrieve(scluster,m_inputClusterName).isFailure() ){
		msg << MSG::WARNING << "did not find super cluster container" << endmsg;
		return StatusCode::SUCCESS;
	}
        const xAOD::EmTauRoIContainer* lvl1(nullptr);
	if ( evtStore()->retrieve(lvl1,m_inputLvl1Name).isFailure() ){
		msg << MSG::WARNING << "did not find old l1 container" << endmsg;
		return StatusCode::SUCCESS;
	}
        if ( lvl1 ) {
		//for(auto l1 : *lvl1 )
		//std::cout << "test L1 : " << l1->eta() << " " << l1->phi() << " " << l1->eT() << std::endl;
	}

	const xAOD::VertexContainer* nvtx(NULL);
        int nvtxs=0;
        if ( evtStore()->retrieve(nvtx,"PrimaryVertices").isFailure() ) {
                msg << MSG::WARNING << "did not find Vectices container" << endmsg;
                return StatusCode::SUCCESS;
        }
        if ( nvtx != NULL) nvtxs = nvtx->size();

	for( auto cl : *scluster ){
		msg << MSG::DEBUG << " " << cl->et() << " " << cl->eta() << std::endl;
		if ( m_enableMon ){
			m_EtSElectronA->Fill( cl->et()/1e3);
			m_EtaSElectronA->Fill( cl->eta() );
			m_PhiSElectronA->Fill( cl->phi() );
			m_EtSElectronEtaA->Fill( cl->eta(), cl->et()/1e3);
			m_HadEtSElectronEtaA->Fill( cl->eta(), cl->ehad1()/1e3 );
			m_HadEtSElectronA->Fill( cl->ehad1()/1e3 );
			if ( cl->e277() > 0.0 )
				m_rEtaA->Fill ( cl->e237() / cl->e277() );
			else m_rEtaA->Fill(-1.0);
			m_f1A->Fill( (cl->energy( CaloSampling::EMB1 ) + cl->energy( CaloSampling::EME1 ) ) / cl->energy() );
			m_f3A->Fill( (cl->energy( CaloSampling::EMB3 ) + cl->energy( CaloSampling::EME3 ) ) / cl->energy() );
			if ( fabsf( cl->eta() )<2.5 ) {
			 m_EtSElectron->Fill( cl->et()/1e3);
			 m_EtaSElectron->Fill( cl->eta() );
			 m_PhiSElectron->Fill( cl->phi() );
			 m_EtSElectronEta->Fill( cl->eta(), cl->et()/1e3);
			 m_HadEtSElectronEta->Fill( cl->eta(), cl->ehad1()/1e3 );
			 m_HadEtSElectron->Fill( cl->ehad1()/1e3 );
			 if ( cl->e277() > 0.0 )
				m_rEta->Fill ( cl->e237() / cl->e277() );
			 else m_rEta->Fill(-1.0);
			 m_f1->Fill( (cl->energy( CaloSampling::EMB1 ) + cl->energy( CaloSampling::EME1 ) ) / cl->energy() );
			 m_f3->Fill( (cl->energy( CaloSampling::EMB3 ) + cl->energy( CaloSampling::EME3 ) ) / cl->energy() );
			}
		}

	}
	if ( m_doTruth ) {
	const xAOD::TruthParticleContainer* truth;
	if ( evtStore()->retrieve(truth,"TruthParticles").isFailure() ) {
		msg << MSG::WARNING << "did not find truth particle container" << endmsg;
		return StatusCode::SUCCESS;
	}

	for( auto tt : *truth ){
		if ( tt->status() != 1 ) continue;
		if ( std::abs(tt->absPdgId()) != 11 ) continue;
		if ( tt->barcode() >= 10000 ) continue;
		if ( tt->pt() < 1e3 ) continue;
		m_eff_truth_pt_d->Fill( tt->pt()/1e3 );
		if ( std::abs( tt->eta() )<2.5 ) m_eff_truth_ptA_d->Fill( tt->pt()/1e3 );
		if ( tt->pt() > 16000 ) m_eff_truth_eta_d->Fill( tt->eta() );
		for( auto cl : *scluster ){
                        if ( std::abs( tt->eta() - cl->eta() ) > 0.06 ) continue;
			float dphi = std::abs( tt->eta() - cl->eta() );
			dphi = std::abs( M_PI - dphi );
			dphi = std::abs( M_PI - dphi );
			if ( dphi > 0.09 ) continue;
			float resolution = 100.0*(tt->pt() - cl->et() ) / tt->pt();
			m_res_truth->Fill( resolution );
			m_res_truth_eta->Fill( tt->eta(), resolution );
			m_res_truth_pt->Fill( tt->pt()/1e3, resolution );
			if ( std::abs( tt->eta() )<2.5 ) m_res_truth_ptA->Fill( tt->pt()/1e3, resolution );
			if ( tt->pt() > 16000 ) m_eff_truth_pt_n->Fill( tt->pt()/1e3 );
			if ( std::abs( tt->eta() )<2.5 ) m_eff_truth_ptA_n->Fill( tt->pt()/1e3 );
			m_eff_truth_eta_n->Fill( tt->eta() );
		}

	}
	} // end of if m_doTruth

	if ( m_doOffline ){
        const xAOD::ElectronContainer* electrons;
        if ( evtStore()->retrieve(electrons,"Electrons").isFailure() ) {
                msg << MSG::WARNING << "did not find electron offline container" << endmsg;
                return StatusCode::SUCCESS;
        }

        for( auto el : *electrons ){
                if ( el->pt() < 1e3 ) continue;
                //if ( !el->passSelection( xAOD::EgammaParameters::LHMedium ) ) continue;
                if ( !el->passSelection( "LHLoose" ) ) continue;
                m_eff_off_pt_d->Fill( el->pt()/1e3 );
                if ( el->pt() > 16000 ) m_eff_off_eta_d->Fill( el->eta() );
                for( auto cl : *scluster ){
                        if ( std::abs( el->eta() - cl->eta() ) > 0.05 ) continue;
                        float dphi = std::abs( el->phi() - cl->phi() );
                        dphi = std::abs( M_PI - dphi );
                        dphi = std::abs( M_PI - dphi );
                        if ( dphi > 0.08 ) continue;
                        float resolution = 100.0*(el->pt() - cl->et() ) / el->pt();
                        m_res_off->Fill( resolution );
                        m_res_off_eta->Fill( el->eta(), resolution );
                        m_res_off_pt->Fill( el->pt()/1e3, resolution );

			if ( (el->caloCluster()->et() > 9e3) && (std::abs(el->eta())<2.47) )
                        m_res_off_nvtx->Fill( nvtxs, resolution );

                        float off_eta = el->caloCluster()->eta();
                        float SE_eta = cl->eta();
                        float resol_eta = -100.0;
                        if ( std::abs(off_eta) > 0 ) resol_eta =
                                100.0*(off_eta-SE_eta)/off_eta;
                        m_res_eta_off->Fill( resol_eta );
                        m_res_eta_off_eta->Fill( el->eta(), resol_eta );
                        m_res_eta_off_pt->Fill( el->pt()/1e3, resol_eta );

                        float off_phi = el->caloCluster()->phi();
                        float SE_phi = cl->phi();
                        float resol_phi = -100.0;
                        if ( std::abs(off_phi) > 0 ) resol_phi =
                                100.0*(off_phi-SE_phi)/off_phi;
                        m_res_phi_off->Fill( resol_phi );
                        m_res_phi_off_eta->Fill( el->eta(), resol_phi );
                        m_res_phi_off_pt->Fill( el->pt()/1e3, resol_phi );

			float off_reta = el->auxdata<float>("Reta");
			float SE_reta = 0.0;
			if ( cl->e277() > 0.0 )
				{ SE_reta = cl->e237()/cl->e277(); }
			float resol_reta = -100.0;
			if ( off_reta > 0 ) resol_reta =
				100.0*(off_reta-SE_reta)/off_reta;
			m_res_rEta_off->Fill( resol_reta );
			m_res_rEta_off_eta->Fill( el->eta(), resol_reta );
			m_res_rEta_off_pt->Fill( el->pt()/1e3, resol_reta );
			if ( (el->caloCluster()->et() > 9e3) && (std::abs(el->eta())<2.47) )
                        m_res_rEta_off_nvtx->Fill( nvtxs, resol_reta );

			float off_f1 = el->auxdata<float>("f1");
			float SE_f1 = 0.0;
			if ( cl->energy() > 0.0 )
				{ SE_f1 = (cl->energy(CaloSampling::EMB1)+cl->energy(CaloSampling::EME1))/cl->energy(); }
			float resol_f1 = -100.0;
			if ( fabsf(off_f1) > 0.01 ) resol_f1 =
				100.0*(off_f1-SE_f1)/off_f1;
			m_res_f1_off->Fill( resol_f1 );
			m_res_f1_off_eta->Fill( el->eta(), resol_f1 );
			m_res_f1_off_pt->Fill( el->pt()/1e3, resol_f1 );

			if ( (el->caloCluster()->et() > 9e3) && (std::abs(el->eta())<2.47) )
                        m_res_f1_off_nvtx->Fill( nvtxs, resol_f1 );

			float off_f3 = el->auxdata<float>("f3");
			float SE_f3 = 0.0;
			if ( cl->energy() > 0.0 )
				{ SE_f3 = (cl->energy(CaloSampling::EMB3)+cl->energy(CaloSampling::EME3))/cl->energy(); }
			float resol_f3 = -100.0;
			if ( fabsf(off_f3) > 0.001 ) resol_f3 =
				100.0*(off_f3-SE_f3)/off_f3;
			m_res_f3_off->Fill( resol_f3 );
			m_res_f3_off_eta->Fill( el->eta(), resol_f3 );
			m_res_f3_off_pt->Fill( el->pt()/1e3, resol_f3 );

                        m_eff_off_pt_n->Fill( el->pt()/1e3 );
                        if ( el->pt() > 16000 ) m_eff_off_eta_n->Fill( el->eta() );
                }
        }
	} // end of if m_doTruth

	return StatusCode::SUCCESS;
}

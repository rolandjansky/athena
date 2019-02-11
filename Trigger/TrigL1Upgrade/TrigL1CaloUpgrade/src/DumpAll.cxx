/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/


/** 
 * NAME : 	DumpAll.cxx
 * PACKAGE : 	Trigger/L1CaloUpgrade/DumpAll
 *
 * AUTHOR : 	Denis Oliveira Damazio
 *
 *
 * **/

#include "DumpAll.h"
#include "CaloEvent/CaloCellContainer.h"
#include "LArRawEvent/LArDigitContainer.h"
#include "CaloIdentifier/CaloCell_SuperCell_ID.h"
#include "CaloIdentifier/CaloIdManager.h"
#include "xAODTracking/VertexContainer.h"
#include "xAODTrigCalo/TrigEMClusterContainer.h"
#include "xAODTruth/TruthParticleContainer.h"
#include "xAODEgamma/ElectronContainer.h"
#include "xAODTrigger/EmTauRoIContainer.h"
#include "TFile.h"
#include "TH1F.h"
#include "TH2F.h"
#include "TH1I.h"
#include "TTree.h"
#include "TNtuple.h"
#include <math.h>

DumpAll::DumpAll( const std::string& name, ISvcLocator* pSvcLocator ) : AthAlgorithm (name, pSvcLocator)  {
       declareProperty("InputClusterName", m_inputClusterName = "SCluster" );
       declareProperty("InputLvl1Name", m_inputLvl1Name = "LVL1EmTauRoIs" );
}

DumpAll::~DumpAll(){}

StatusCode DumpAll::initialize(){
	
        MsgStream msg(msgSvc(), name());
	msg << MSG::DEBUG << "initializing DumpAll" << endmsg;
        m_counter = 0;
	std::string filename=name();
	filename+=".DumpAll.root";
	m_file = new TFile (filename.c_str(),"RECREATE");
	m_file->mkdir("DumpAll");
	m_file->cd("DumpAll");
	m_evt = (TTree*)new TNtuple("Evt","Evt","count:event_number:nVtx");
	//m_truth = (TTree*)new TNtuple("Truth","Truth","count:truth_pt:truth_eta:truth_phi:truth_pdg:truth_barcode");
	m_truth = (TTree*)new TTree("Truth","Truth");
	m_truth->Branch("truth_pt",    &m_truth_pt);
	m_truth->Branch("truth_eta",    &m_truth_eta);
	m_truth->Branch("truth_phi",    &m_truth_phi);
	m_truth->Branch("truth_pdg",    &m_truth_pdg);
	m_truth->Branch("truth_barcode",    &m_truth_barcode);
	m_truth->Branch("truth_charge",    &m_truth_charge);

	m_l1 = (TTree*)new TTree("L1","L1"); //,"count:l1_et:l1_eta:l1_phi:l1_emclus:l1_hadcore:l1_emisol:l1_hadisol");
	m_l1->Branch("l1_et",&m_l1_et);
	m_l1->Branch("l1_eta",&m_l1_eta);
	m_l1->Branch("l1_phi",&m_l1_phi);
	m_l1->Branch("l1_emclus",&m_l1_emclus);
	m_l1->Branch("l1_hadcore",&m_l1_hadcore);
	m_l1->Branch("l1_hadisol",&m_l1_hadisol);
	m_l1->Branch("l1_emisol",&m_l1_emisol);


	m_selectron = new TTree("SCluster","SCluster");
	m_selectron->Branch("selectron_et"    ,&m_selectron_et);
	m_selectron->Branch("selectron_eta"    ,&m_selectron_eta);
	m_selectron->Branch("selectron_phi"    ,&m_selectron_phi);
	m_selectron->Branch("selectron_energy"    ,&m_selectron_energy);
	m_selectron->Branch("selectron_e237"    ,&m_selectron_e237);
	m_selectron->Branch("selectron_e277"    ,&m_selectron_e277);
	m_selectron->Branch("selectron_ehad1"    ,&m_selectron_ehad1);
	m_selectron->Branch("selectron_e0"    ,&m_selectron_e0);
	m_selectron->Branch("selectron_e1"    ,&m_selectron_e1);
	m_selectron->Branch("selectron_e2"    ,&m_selectron_e2);
	m_selectron->Branch("selectron_e3"    ,&m_selectron_e3);
	m_selectron->Branch("selectron_wstot"    ,&m_selectron_wstot);
	m_selectron->Branch("selectron_time"    ,&m_selectron_time);

	m_offelectron = new TTree("OffEl","OffEl");
	m_offelectron->Branch("offel_et", &m_offel_et);
	m_offelectron->Branch("offel_eta", &m_offel_eta);
	m_offelectron->Branch("offel_phi", &m_offel_phi);
	m_offelectron->Branch("offel_cleta", &m_offel_cleta);
	m_offelectron->Branch("offel_clphi", &m_offel_clphi);
	m_offelectron->Branch("offel_reta", &m_offel_reta);
	m_offelectron->Branch("offel_f1", &m_offel_f1);
	m_offelectron->Branch("offel_f3", &m_offel_f3);
	m_offelectron->Branch("offel_had", &m_offel_had);
	m_offelectron->Branch("offel_istight", &m_offel_istight);
	m_offelectron->Branch("offel_ismedium", &m_offel_ismedium);
	m_offelectron->Branch("offel_isloose", &m_offel_isloose);

	m_cell_infront = new TTree("CellInFront","CellInFront");
	m_cell_infront->Branch("e",&m_cell_infront_e);
	m_cell_infront->Branch("eta",&m_cell_infront_eta);
	m_cell_infront->Branch("phi",&m_cell_infront_phi);
	m_cell_infront->Branch("layer",&m_cell_infront_layer);
	m_cell_infront->Branch("time",&m_cell_infront_time);
	m_cell_infront->Branch("quality",&m_cell_infront_quality);

	m_allcell_infront = new TTree("AllCellInFrontLayer","CellInFront");
	m_allcell_infront->Branch("idx",&m_allcell_infront_idx);
	m_allcell_infront->Branch("e",&m_allcell_infront_e);
	m_allcell_infront->Branch("eta",&m_allcell_infront_eta);
	m_allcell_infront->Branch("phi",&m_allcell_infront_phi);
	m_allcell_infront->Branch("layer",&m_allcell_infront_layer);
	m_allcell_infront->Branch("time",&m_allcell_infront_time);
	m_allcell_infront->Branch("quality",&m_allcell_infront_quality);

	m_file->cd("/");

	return StatusCode::SUCCESS;
}

StatusCode DumpAll::finalize(){
        MsgStream msg(msgSvc(), name());
	msg << MSG::DEBUG << "finalizing SimpleLArDigitsChecks" << endmsg;
	m_file->Write();
	m_file->Close();
	return StatusCode::SUCCESS;
}

StatusCode DumpAll::execute(){
	
        MsgStream msg(msgSvc(), name());
	msg << MSG::DEBUG << "execute DumpAll" << endmsg;
	std::cout << "DumpAll" << std::endl;
        const CaloCellContainer* scells;
	if ( evtStore()->retrieve(scells,"SCell").isFailure() ){
		msg << MSG::WARNING << "did not find cell container" << endmsg;
		return StatusCode::SUCCESS;
	}
/*
	if ( evtStore()->retrieve(allcalo,"LArDigitSCL1").isFailure() ){
		msg << MSG::WARNING << "did not find lardigit container for regular cells" << endmsg;
		return StatusCode::SUCCESS;
	}
*/
	const xAOD::VertexContainer* nvtx(NULL);
	if ( evtStore()->retrieve(nvtx,"PrimaryVertices").isFailure() ) {
		msg << MSG::WARNING << "did not find Vectices container" << endmsg;
		return StatusCode::SUCCESS;
	}
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
	const xAOD::TruthParticleContainer* truth;
        if ( evtStore()->retrieve(truth,"TruthParticles").isFailure() ) {
                msg << MSG::WARNING << "did not find truth particle container" << endmsg;
                return StatusCode::SUCCESS;
        }

	const xAOD::ElectronContainer* electrons;
        if ( evtStore()->retrieve(electrons,"Electrons").isFailure() ) {
                msg << MSG::WARNING << "did not find electron offline container" << endmsg;
                return StatusCode::SUCCESS;
        }

	bool caloavail=true;
	const CaloCellContainer* allcalo;
        if ( evtStore()->retrieve(allcalo,"AllCalo").isFailure() ) {
                msg << MSG::WARNING << "did not find allcalo container" << endmsg;
		caloavail=false;
        }

        // for(auto digit : *allcalo) {
        // m_selectron_et.clear();
        ResetAllBranches();
        for(auto scl : *scluster) {
	   m_selectron_et.push_back(scl->et());
	   m_selectron_eta.push_back(scl->eta());
	   m_selectron_phi.push_back(scl->phi());
	   m_selectron_energy.push_back(scl->energy());
	   m_selectron_e237.push_back(scl->e237());
	   m_selectron_e277.push_back(scl->e277());
	   m_selectron_ehad1.push_back(scl->ehad1());
	   m_selectron_e0.push_back(scl->energy(CaloSampling::PreSamplerB)+scl->energy(CaloSampling::PreSamplerE));
	   m_selectron_e1.push_back(scl->energy(CaloSampling::EMB1) + scl->energy(CaloSampling::EME1) );
	   m_selectron_e2.push_back(scl->energy(CaloSampling::EMB2) + scl->energy(CaloSampling::EME2) );
	   m_selectron_e3.push_back(scl->energy(CaloSampling::EMB3) + scl->energy(CaloSampling::EME3) );
	   m_selectron_wstot.push_back(scl->wstot() );
	   m_selectron_time.push_back(scl->energy(CaloSampling::FCAL2) );
	   if ( caloavail ) {
		float etaCl = scl->eta();
		float phiCl = scl->phi();
		std::vector< float > e;
		std::vector< float > eta;
		std::vector< float > phi;
		std::vector< float > layer;
		std::vector< float > time;
		std::vector< float > quality;
		std::vector< float > all_idx;
		std::vector< float > all_e;
		std::vector< float > all_eta;
		std::vector< float > all_phi;
		std::vector< float > all_layer;
		std::vector< float > all_time;
		std::vector< float > all_quality;
		int index = 0;
		for(auto c : *allcalo) {
			int l = c->caloDDE()->getSampling();
			if ( (l!=1) && (l!=5) ) continue;
			float seedEta=c->eta();
			float absSeedEta = TMath::Abs( seedEta );
			float seedPhi=c->phi();
			if ( c->energy() / TMath::CosH( absSeedEta ) > 125 ) { // found a seed
			   for(auto d : *allcalo) {
				int k = c->caloDDE()->getSampling();
				if ( (k!=1) && (k!=5) ) continue;
				float limit = 0.066;
				if ( absSeedEta > 1.7 ) limit = 0.088;
				if ( absSeedEta > 1.9 ) limit = 0.132;
				if ( std::abs( seedEta-d->eta() ) > limit ) continue;
				float dphi = std::abs( seedPhi-d->phi() );
				dphi = std::abs(M_PI - dphi);
				dphi = std::abs(M_PI - dphi);
				if ( dphi > 0.12 ) continue;
				all_idx.push_back( index );
				all_e.push_back ( d->energy() );
				all_eta.push_back ( d->eta() );
				all_phi.push_back ( d->phi() );
				all_layer.push_back ( d->caloDDE()->getSampling() );
				all_time.push_back ( d->time() );
				all_quality.push_back ( d->quality() );
			   }
			   index++;
			}
			if ( std::abs( etaCl-c->eta() ) > 0.2 ) continue;
			float dphi = std::abs( phiCl-c->phi() );
			dphi = std::abs(M_PI - dphi);
			dphi = std::abs(M_PI - dphi);
			if ( dphi > 0.2 ) continue;
			e.push_back ( c->energy() );
			eta.push_back ( c->eta() );
			phi.push_back ( c->phi() );
			layer.push_back ( c->caloDDE()->getSampling() );
			time.push_back ( c->time() );
			quality.push_back ( c->quality() );
		}
		m_cell_infront_e.push_back(e);
		m_cell_infront_eta.push_back(eta);
		m_cell_infront_phi.push_back(phi);
		m_cell_infront_layer.push_back(layer);
		m_cell_infront_time.push_back(time);
		m_cell_infront_quality.push_back(quality);

		m_allcell_infront_idx.push_back(all_idx);
		m_allcell_infront_e.push_back(all_e);
		m_allcell_infront_eta.push_back(all_eta);
		m_allcell_infront_phi.push_back(all_phi);
		m_allcell_infront_layer.push_back(all_layer);
		m_allcell_infront_time.push_back(all_time);
		m_allcell_infront_quality.push_back(all_quality);
	   }
	}
	if ( caloavail ) {
		m_allcell_infront->Fill();
		m_cell_infront->Fill();
	}
	m_selectron->Fill();
        int nvtxs=-1;
	if ( nvtx != NULL) nvtxs = nvtx->size();
        int event_number=0;
	((TNtuple*)m_evt)->Fill(m_counter,event_number,nvtxs);

	for( auto tt : *truth ){
		if ( tt->status() != 1 ) continue;
                if ( std::abs(tt->absPdgId()) != 11 ) continue;
                if ( tt->barcode() >= 10000 ) continue;
		m_truth_pt.push_back( tt->pt() );
		m_truth_eta.push_back( tt->eta() );
		m_truth_phi.push_back( tt->phi() );
		m_truth_pdg.push_back( tt->pdgId() );
		m_truth_barcode.push_back( tt->barcode() );
		m_truth_charge.push_back( tt->charge() );
	}
	m_truth->Fill();

	if ( lvl1 ) {
                for(auto l1 : *lvl1 ){
		//((TNtuple*)m_l1)->Fill(m_counter,l1->eT(),l1->eta(),l1->phi(), l1->emClus(), l1->hadCore(), l1->emIsol(), l1->hadIsol() );
		m_l1_et.push_back( l1->eT() );
		m_l1_eta.push_back( l1->eta() );
		m_l1_phi.push_back( l1->phi() );
		m_l1_emclus.push_back( l1->emClus() );
		m_l1_hadcore.push_back( l1->hadCore() );
		m_l1_emisol.push_back( l1->emIsol() );
		m_l1_hadisol.push_back( l1->hadIsol() );
		}
	m_l1->Fill();
        }
	for( auto el : *electrons ){
		if ( el->pt() < 1e3 ) continue; // dont care about too low energy things
		m_offel_et.push_back(el->pt() );
		m_offel_eta.push_back(el->eta() );
		m_offel_phi.push_back(el->phi() );
		m_offel_cleta.push_back(el->caloCluster()->eta() );
		m_offel_clphi.push_back(el->caloCluster()->phi() );
		m_offel_reta.push_back(el->auxdata<float>("Reta") );
		m_offel_f1.push_back(el->auxdata<float>("f1") );
		m_offel_f3.push_back(el->auxdata<float>("f3") );
		m_offel_had.push_back( 0.0 ); // not yet there
		m_offel_istight.push_back( (el->passSelection( xAOD::EgammaParameters::LHTight ) ? 1 : 0 ) );
		m_offel_ismedium.push_back( (el->passSelection( xAOD::EgammaParameters::LHMedium ) ? 1 : 0 ) );
		m_offel_isloose.push_back( (el->passSelection( xAOD::EgammaParameters::LHLoose ) ? 1 : 0 ) );
	} // end of electron
	m_offelectron->Fill();


	m_counter++;
	
	return StatusCode::SUCCESS;
}

void DumpAll::ResetAllBranches(){
        m_selectron_et.clear();
        m_selectron_eta.clear();
        m_selectron_phi.clear();
        m_selectron_energy.clear();
        m_selectron_e237.clear();
        m_selectron_e277.clear();
        m_selectron_ehad1.clear();
        m_selectron_e0.clear();
        m_selectron_e1.clear();
        m_selectron_e2.clear();
        m_selectron_e3.clear();
	m_selectron_wstot.clear();
	m_selectron_time.clear();

        m_truth_pt.clear();
        m_truth_eta.clear();
        m_truth_phi.clear();
        m_truth_pdg.clear();
        m_truth_barcode.clear();
        m_truth_charge.clear();

        m_l1_et.clear();
        m_l1_eta.clear();
        m_l1_phi.clear();
        m_l1_emclus.clear();
        m_l1_hadcore.clear();
        m_l1_hadisol.clear();
        m_l1_emisol.clear();

        m_offel_et.clear();
        m_offel_eta.clear();
        m_offel_phi.clear();
        m_offel_cleta.clear();
        m_offel_clphi.clear();
        m_offel_reta.clear();
        m_offel_f1.clear();
        m_offel_f3.clear();
        m_offel_had.clear();
        m_offel_istight.clear();
        m_offel_ismedium.clear();
        m_offel_isloose.clear();

	m_cell_infront_e.clear();
	m_cell_infront_eta.clear();
	m_cell_infront_phi.clear();
	m_cell_infront_layer.clear();
	m_cell_infront_time.clear();
	m_cell_infront_quality.clear();

	m_allcell_infront_idx.clear();
	m_allcell_infront_e.clear();
	m_allcell_infront_eta.clear();
	m_allcell_infront_phi.clear();
	m_allcell_infront_layer.clear();
	m_allcell_infront_time.clear();
	m_allcell_infront_quality.clear();

}


/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/


/** 
 * NAME : 	LArFexAnalysis.cxx
 * PACKAGE : 	Trigger/TrigL1Upgrade/TrigL1CaloUpgrade/LArFexAnalysis
 *
 * AUTHOR : 	Denis Oliveira Damazio
 *
 *
 * **/

#include "LArFexAnalysis.h"
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

LArFexAnalysis::LArFexAnalysis( const std::string& name, ISvcLocator* pSvcLocator ) : AthAlgorithm (name, pSvcLocator){
       declareProperty("EnableMonitoring", m_enableMon=true);
       declareProperty("DoTruth", m_doTruth=true);
       declareProperty("DoOffline", m_doOffline=true);
       declareProperty("InputClusterName", m_inputClusterName = "LArLayer1Vars" );
}

LArFexAnalysis::~LArFexAnalysis(){}

StatusCode LArFexAnalysis::initialize(){
	
        MsgStream msg(msgSvc(), name());
	msg << MSG::DEBUG << "initializing LArFexAnalysis" << endreq;
        if ( m_enableMon ){
		std::string histoName(name());
		histoName+="LArFexCheck.root";
		m_histFile = new TFile(histoName.c_str(),"RECREATE");
	        if ( m_doTruth ) {
		m_eff_truth_pt_n = new TH1F("eff_truth_pt_n","efficiency with respect to truth pt",60,0,60.0);
		m_eff_truth_pt_d = new TH1F("eff_truth_pt_d","efficiency with respect to truth pt",60,0,60.0);
		m_eff_truth_ptA_n = new TH1F("eff_truth_ptA_n","efficiency with respect to truth pt",60,0,60.0);
		m_eff_truth_ptA_d = new TH1F("eff_truth_ptA_d","efficiency with respect to truth pt",60,0,60.0);
		m_eff_truth_eta_n = new TH1F("eff_truth_eta_n","efficiency with respect to truth eta",50,-5.0,5.0);
		m_eff_truth_eta_d = new TH1F("eff_truth_eta_d","efficiency with respect to truth eta",50,-5.0,5.0);
		}
		if ( m_doOffline ) {

		m_res_Eratio_off = new TH1F("res_Eratio_off","Eratio resolution with respect offline electron",60,-30.0,30.0);
		m_res_Eratio_off_eta = new TH2F("res_Eratio_off_eta","Eratio resolution with respect offline versus eta",50,-5.0,5.0,60,-30.0,30.0);
		m_res_Eratio_off_pt = new TH2F("res_Eratio_off_pt","Eratio resolution with respect offline versus pt",60,0.0,60.0,60,-30.0,30.0);

		m_res_fracs1_off = new TH1F("res_fracs1_off","fracs1 resolution with respect offline electron",60,-30.0,30.0);
		m_res_fracs1_off_eta = new TH2F("res_fracs1_off_eta","fracs1 resolution with respect offline versus eta",50,-5.0,5.0,60,-30.0,30.0);
		m_res_fracs1_off_pt = new TH2F("res_fracs1_off_pt","fracs1 resolution with respect offline versus pt",60,0.0,60.0,60,-30.0,30.0);

		m_res_wtots1_off = new TH1F("res_wtots_off","wtots1 resolution with respect offline electron",60,-30.0,30.0);
		m_res_wtots1_off_eta = new TH2F("res_wtots1_off_eta","wtots1 resolution with respect offline versus eta",50,-5.0,5.0,60,-30.0,30.0);
		m_res_wtots1_off_pt = new TH2F("res_wtots1_off_pt","wtots1 resolution with respect offline versus pt",60,0.0,60.0,60,-30.0,30.0);

		m_eff_pt_n = new TH1F("eff_off_pt_n","efficiency with respect to offline pt",60,0,60.0);
		m_eff_pt_d = new TH1F("eff_off_pt_d","efficiency with respect to offline pt",60,0,60.0);
		m_eff_eta_n = new TH1F("eff_off_eta_n","efficiency with respect to offline eta",50,-5.0,5.0);
		m_eff_eta_d = new TH1F("eff_off_eta_d","efficiency with respect to offline eta",50,-5.0,5.0);
		}
	}
	return StatusCode::SUCCESS;
}

StatusCode LArFexAnalysis::finalize(){
        MsgStream msg(msgSvc(), name());
	msg << MSG::DEBUG << "finalizing LArFexAnalysis" << endreq;
        if ( m_enableMon ){
		m_histFile->Write();
		m_histFile->Close();
	}
	return StatusCode::SUCCESS;
}

StatusCode LArFexAnalysis::execute(){
	
        MsgStream msg(msgSvc(), name());
	msg << MSG::DEBUG << "execute LArFexAnalysis" << endreq;
        const xAOD::TrigEMClusterContainer* scluster(nullptr);
	if ( evtStore()->retrieve(scluster,m_inputClusterName).isFailure() ){
		msg << MSG::WARNING << "did not find super cluster container" << endreq;
		return StatusCode::SUCCESS;
	}
        const xAOD::EmTauRoIContainer* lvl1(nullptr);
	if ( evtStore()->retrieve(lvl1,m_inputLvl1Name).isFailure() ){
		msg << MSG::WARNING << "did not find old l1 container" << endreq;
		//return StatusCode::SUCCESS;
	}
#ifdef DONTDO
        if ( lvl1 ) {
		//for(auto l1 : *lvl1 )
		//std::cout << "test L1 : " << l1->eta() << " " << l1->phi() << " " << l1->eT() << std::endl;
	}
#endif

	const xAOD::VertexContainer* nvtx(NULL);
        int nvtxs=0;
        if ( evtStore()->retrieve(nvtx,"PrimaryVertices").isFailure() ) {
                msg << MSG::WARNING << "did not find Vectices container" << endreq;
                return StatusCode::SUCCESS;
        }
        if ( nvtx != NULL) nvtxs = nvtx->size();

	if ( m_doTruth ) {
	const xAOD::TruthParticleContainer* truth;
	if ( evtStore()->retrieve(truth,"TruthParticles").isFailure() ) {
		msg << MSG::WARNING << "did not find truth particle container" << endreq;
		return StatusCode::SUCCESS;
	}

	for( auto tt : *truth ){
		if ( tt->status() != 1 ) continue;
		if ( fabsf(tt->absPdgId()) != 11 ) continue;
		if ( tt->barcode() >= 10000 ) continue;
		if ( tt->pt() < 1e3 ) continue;
		bool denominator=true;
		bool numerator=false;


		for( auto cl : *scluster ){
			if ( fabsf( tt->eta() - cl->eta() ) > 0.03 ) continue;
			float dphi = fabsf( tt->phi() - cl->phi() );
			dphi = fabsf( M_PI - dphi );
			dphi = fabsf( M_PI - dphi );
			if ( dphi > 0.08 ) continue;
			float den = cl->emaxs1() + cl->e2tsts1();
			if ( den < 1e-3 ) continue;
			if ( ((cl->emaxs1() - cl->e2tsts1()) / den) < 0.7 ) continue;
			numerator=true;
		}
		if ( denominator ) {
		  m_eff_truth_pt_d->Fill( tt->pt()/1e3 );
		  if ( fabsf( tt->eta() )<2.5 ) m_eff_truth_ptA_d->Fill( tt->pt()/1e3 );
		  if ( tt->pt() > 16000 ) m_eff_truth_eta_d->Fill( tt->eta() );
		}
		if ( numerator ) {
                        m_eff_truth_pt_n->Fill( tt->pt()/1e3 );
                        if ( fabsf( tt->eta() )<2.5 ) m_eff_truth_ptA_n->Fill( tt->pt()/1e3 );
                        if ( tt->pt() > 16000 ) m_eff_truth_eta_n->Fill( tt->eta() );
		}
		
	}
	} // end of if m_doTruth

	if ( m_doOffline ){
        const xAOD::ElectronContainer* electrons;
        if ( evtStore()->retrieve(electrons,"Electrons").isFailure() ) {
                msg << MSG::WARNING << "did not find electron offline container" << endreq;
                return StatusCode::SUCCESS;
        }

        for( auto el : *electrons ){
                if ( !el->passSelection( xAOD::EgammaParameters::LHMedium ) ) continue;
		if ( (el->caloCluster()->et() < 9e3) || (fabsf(el->eta())>2.47) ) continue;
                //if ( !el->passSelection( xAOD::EgammaParameters::LHLoose ) ) continue;
                bool numerator=false;
		bool denominator=true;
                for( auto cl : *scluster ){
                        if ( fabsf( el->caloCluster()->eta() - cl->eta() ) > 0.03 ) continue;
                        float dphi = fabsf( el->caloCluster()->phi() - cl->phi() );
                        dphi = fabsf( M_PI - dphi );
                        dphi = fabsf( M_PI - dphi );
                        if ( dphi > 0.08 ) continue;
			
			float off_Eratio = el->auxdata<float>("Eratio");
			float off_fracs1 = el->auxdata<float>("fracs1");
			float off_wtots1 = el->auxdata<float>("wtots1");
			float den = cl->emaxs1() + cl->e2tsts1();
			if ( den < 1e-3 ) continue;
			if ( ((cl->emaxs1() - cl->e2tsts1()) / den) > 0.7 ) numerator=true;
			float SE_Eratio = ((cl->emaxs1() - cl->e2tsts1()) / den);
			float SE_fracs1 = cl->fracs1();
			float SE_wtots1 = cl->wstot();
			

			float resol_Eratio = -100.0;
			if ( fabsf(off_Eratio) > 0.001 ) resol_Eratio = 
				100.0*(off_Eratio-SE_Eratio)/off_Eratio;
			m_res_Eratio_off->Fill( resol_Eratio );
			m_res_Eratio_off_eta->Fill( el->eta(), resol_Eratio );
			m_res_Eratio_off_pt->Fill( el->pt()/1e3, resol_Eratio );

			float resol_fracs1 = -100.0;
			if ( fabsf(off_fracs1) > 0.001 ) resol_fracs1 = 
				100.0*(off_fracs1-SE_fracs1)/off_fracs1;
			m_res_fracs1_off->Fill( resol_fracs1 );
			m_res_fracs1_off_eta->Fill( el->eta(), resol_fracs1 );
			m_res_fracs1_off_pt->Fill( el->pt()/1e3, resol_fracs1 );

			float resol_wtots1 = -100.0;
			if ( fabsf(off_wtots1) > 0.001 ) resol_wtots1 = 
				100.0*(off_wtots1-SE_wtots1)/off_wtots1;
			m_res_wtots1_off->Fill( resol_wtots1 );
			m_res_wtots1_off_eta->Fill( el->eta(), resol_wtots1 );
			m_res_wtots1_off_pt->Fill( el->pt()/1e3, resol_wtots1 );
                }
		if ( denominator ) {
                  m_eff_pt_d->Fill( el->pt()/1e3 );
                  if ( el->pt() > 16000 ) m_eff_eta_d->Fill( el->eta() );
		}
		if ( numerator ) {
                  m_eff_pt_n->Fill( el->pt()/1e3 );
                  if ( el->pt() > 16000 ) m_eff_eta_n->Fill( el->eta() );
		}
        }
	} // end of if m_doTruth

	return StatusCode::SUCCESS;
}

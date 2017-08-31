/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/


/** 
 * NAME : 	EFexEratioAlgo.cxx
 * PACKAGE : 	Trigger/TrigL1Upgrade/TrigL1CaloUpgradeAnalysis/EFexEratioAlgo
 *
 * AUTHOR : 	Denis Oliveira Damazio
 *
 *
 * **/

#include "EFexEratioAlgo.h"
#include <math.h>
#include <string>
#include "TFile.h"
#include "TH1F.h"
#include "TH2F.h"
#include "xAODTrigCalo/TrigEMClusterContainer.h"
#include "xAODTruth/TruthParticleContainer.h"
#include "xAODEgamma/ElectronContainer.h"
#include "xAODTracking/VertexContainer.h"
#include "xAODTracking/TrackParticleContainer.h"
#include "xAODTrigger/EmTauRoIContainer.h"
#include "xAODEventInfo/EventInfo.h"
#include "TrigL1CaloUpgrade/BCID.h"

EFexEratioAlgo::EFexEratioAlgo( const std::string& name, ISvcLocator* pSvcLocator ) : AthAlgorithm (name, pSvcLocator){
       declareProperty("EnableMonitoring", m_enableMon=true);
       declareProperty("DoTruth", m_doTruth=true);
       declareProperty("DoOffline", m_doOffline=false);
       declareProperty("InputClusterName", m_inputClusterName = "SCluster" );
       declareProperty("InputEratioName", m_inputEratioName = "LArLayer1Vars" );
       declareProperty("InputLvl1Name", m_inputLvl1Name = "LVL1EmTauRoIs" );
       declareProperty("eFexEtThreshold",m_EEtThr = 20e3 );
       declareProperty("rEtaThreshold",m_rEtaThr = 0.745 );
       declareProperty("rEratioThreshold",m_EratioThr = 0.41 );
}

EFexEratioAlgo::~EFexEratioAlgo(){}

StatusCode EFexEratioAlgo::initialize(){
	
        MsgStream msg(msgSvc(), name());
	msg << MSG::DEBUG << "initializing EFexEratioAlgo" << endreq;
        if ( m_enableMon ){
		std::string histoName(name());
		histoName+="Algo.root";
		m_histFile = new TFile(histoName.c_str(),"RECREATE");
		m_EtSElectron = new TH1F("EtSElectron","Et of Super Cell based Electron",60,0,60);
		m_numpt1 = new TH1F("numpt1","numpt1",100,0,100);
		m_numpt2 = new TH1F("numpt2","numpt2",100,0,100);
		m_numpt3 = new TH1F("numpt3","numpt3",100,0,100);
		m_numpt4 = new TH1F("numpt4","numpt4",100,0,100);
		m_denpt = new TH1F("denpt","denpt",100,0,100);
		m_numeta1 = new TH1F("numeta1","numeta1",100,-2.5,2.5);
		m_numeta2 = new TH1F("numeta2","numeta2",100,-2.5,2.5);
		m_numeta3 = new TH1F("numeta3","numeta3",100,-2.5,2.5);
		m_numeta4 = new TH1F("numeta4","numeta4",100,-2.5,2.5);
		m_deneta = new TH1F("deneta","deneta"   ,100,-2.5,2.5);
		m_numphi1 = new TH1F("numphi1","numphi1",128,-M_PI,M_PI);
		m_numphi2 = new TH1F("numphi2","numphi2",128,-M_PI,M_PI);
		m_numphi3 = new TH1F("numphi3","numphi3",128,-M_PI,M_PI);
		m_numphi4 = new TH1F("numphi4","numphi4",128,-M_PI,M_PI);
		m_denphi = new TH1F("denphi","denphi"    ,128,-M_PI,M_PI);
		m_trackPt = new TH1F("trackPt","trackPt",160,0,80);
		m_trackEta = new TH1F("trackEta","trackEta",100,-2.5,2.5);

	}
	return StatusCode::SUCCESS;
}

StatusCode EFexEratioAlgo::finalize(){
        MsgStream msg(msgSvc(), name());
	msg << MSG::DEBUG << "finalizing EFexEratio" << endreq;
        if ( m_enableMon ){
		m_numpt1->Write();
		m_numpt2->Write();
		m_numpt3->Write();
		m_numpt4->Write();
		m_denpt->Write();
		m_numeta1->Write();
		m_numeta2->Write();
		m_numeta3->Write();
		m_numeta4->Write();
		m_deneta->Write();
		m_numphi1->Write();
		m_numphi2->Write();
		m_numphi3->Write();
		m_numphi4->Write();
		m_denphi->Write();
		m_trackPt->Write();
		m_trackEta->Write();
		m_histFile->Write();
		m_histFile->Close();
	}
	return StatusCode::SUCCESS;
}

StatusCode EFexEratioAlgo::execute(){
	
        MsgStream msg(msgSvc(), name());
	msg << MSG::DEBUG << "execute TrigT1CaloEFex" << endreq;
        const xAOD::EventInfo* evt(0);
        if ( evtStore()->retrieve(evt,"EventInfo").isFailure() ){
                msg << MSG::WARNING << "did not find EventInfo container" << endreq;
        }
        long bunch_crossing(-1);
        long bunch_crossingNor(-1);
        if ( evt ) {
           bunch_crossing = evt->bcid();
           bunch_crossingNor = bcids_from_start ( bunch_crossing );
        }
        const xAOD::TrigEMClusterContainer* scluster(nullptr);
	if ( evtStore()->retrieve(scluster,m_inputClusterName).isFailure() ){
		msg << MSG::WARNING << "did not find super cluster container" << endreq;
		return StatusCode::SUCCESS;
	}
        const xAOD::TrigEMClusterContainer* lASP(nullptr);
	if ( evtStore()->retrieve(lASP,m_inputEratioName).isFailure() ){
		msg << MSG::WARNING << "did not find Eratio cluster container" << endreq;
		return StatusCode::SUCCESS;
	}
        const xAOD::EmTauRoIContainer* lvl1(nullptr);
	if ( evtStore()->retrieve(lvl1,m_inputLvl1Name).isFailure() ){
		msg << MSG::WARNING << "did not find old l1 container" << endreq;
		//return StatusCode::SUCCESS;
	}

	const xAOD::VertexContainer* nvtx(NULL);
        int nvtxs=0;
        if ( evtStore()->retrieve(nvtx,"PrimaryVertices").isFailure() ) {
                msg << MSG::WARNING << "did not find Vectices container" << endreq;
                return StatusCode::SUCCESS;
        }
        if ( nvtx != NULL) nvtxs = nvtx->size();

	const xAOD::TrackParticleContainer* tracks;
	if ( evtStore()->retrieve(tracks,"InDetTrackParticles").isFailure() ) {
		msg << MSG::WARNING << "did not find track particle container" << endreq;
		return StatusCode::SUCCESS;
	}

	if ( m_doTruth ) {
	const xAOD::TruthParticleContainer* truth;
	if ( evtStore()->retrieve(truth,"TruthParticles").isFailure() ) {
		msg << MSG::WARNING << "did not find truth particle container" << endreq;
		return StatusCode::SUCCESS;
	}

	for( auto tt : *truth ){
                float pt = tt->pt()*1e-3;
		if ( pt < 15 ) continue;
		float aeta = fabsf(tt->eta());
		if ( aeta > 2.47 ) continue;
		if ( (aeta > 1.37) && (aeta < 1.54) ) continue;
		if ( fabsf(tt->absPdgId()) != 11 ) continue;
		if ( tt->barcode() >= 10000 ) continue;
		if ( tt->parent() && (tt->parent()->pdgId()!= 23) ) continue;
		std::cout << "candidate : " << pt << " " << tt->eta() << " " << tt->phi() << std::endl;
		m_denpt->Fill( pt );
		if ( pt > 20 ) {
		    m_deneta->Fill( tt->eta() );
		    m_denphi->Fill( tt->phi() );
		}
		for( auto cl : *scluster ){
			if ( cl->et() < m_EEtThr ) continue;
			if ( fabsf( tt->eta() - cl->eta() ) > 0.06 ) continue;
			if ( deltaPhi(tt->phi(), cl->phi() ) > 0.1 ) continue;
			m_numpt1->Fill( pt );
			if ( pt > 20 ) {
			    m_numeta1->Fill( tt->eta() );
			    m_numphi1->Fill( tt->phi() );
			}
			float rEta=-999.0;
			float eRatio=-999.0;
			if ( cl->e277() > 0.1 ) 
			    rEta= cl->e237() / cl->e277();
			if ( rEta < m_rEtaThr ) continue;
			m_numpt2->Fill( pt );
			if ( pt > 20 ) {
			    m_numeta2->Fill( tt->eta() );
			    m_numphi2->Fill( tt->phi() );
			}
			for( auto er : *lASP ) {
			    if ( fabsf( er->eta() - cl->eta() ) > 0.035 ) continue;
			    if ( deltaPhi( er->phi(), cl->phi() ) > 0.1 ) continue;
			    if ( er->emaxs1() + er->e2tsts1() > 0.1 ) 
				eRatio = ( er->emaxs1() - er->e2tsts1() ) / (er->emaxs1() + er->e2tsts1() );
			}
			if ( eRatio < m_EratioThr ) continue;
			m_numpt3->Fill( pt );
			if ( pt > 20 ) {
			    m_numeta3->Fill( tt->eta() );
			    m_numphi3->Fill( tt->phi() );
			}
			for(auto track : *tracks ){
			    if ( track->pt() < 2e3 ) continue;
			    if ( fabsf( track->eta() - cl->eta() ) > 0.02 ) continue;
			    if ( deltaPhi( track->phi(), cl->phi() ) > 0.03 ) continue;
			    m_trackPt->Fill( track->pt() / 1e3 );
			    m_trackEta->Fill(track->eta() );
			}
			
			//float resolution = 100.0*(tt->pt() - cl->et() ) / tt->pt();
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
                if ( el->pt() < 1e3 ) continue;
                //if ( !el->passSelection( xAOD::EgammaParameters::LHMedium ) ) continue;
                if ( !el->passSelection( xAOD::EgammaParameters::LHLoose ) ) continue;
                for( auto cl : *scluster ){
                        if ( fabsf( el->eta() - cl->eta() ) > 0.05 ) continue;
                        float dphi = fabsf( el->phi() - cl->phi() );
                        dphi = fabsf( M_PI - dphi );
                        dphi = fabsf( M_PI - dphi );
                        if ( dphi > 0.08 ) continue;
                        float resolution = 100.0*(el->pt() - cl->et() ) / el->pt();
                        float off_eta = el->caloCluster()->eta();
                        float SE_eta = cl->eta();
                        float resol_eta = -100.0;
                        if ( fabsf(off_eta) > 0 ) resol_eta =
                                100.0*(off_eta-SE_eta)/off_eta;

                        float off_phi = el->caloCluster()->phi();
                        float SE_phi = cl->phi();
                        float resol_phi = -100.0;
                        if ( fabsf(off_phi) > 0 ) resol_phi =
                                100.0*(off_phi-SE_phi)/off_phi;
			float off_reta = el->auxdata<float>("Reta");
			float SE_reta = 0.0;
			if ( cl->e277() > 0.0 )
				{ SE_reta = cl->e237()/cl->e277(); }
			float resol_reta = -100.0;
			if ( off_reta > 0 ) resol_reta = 
				100.0*(off_reta-SE_reta)/off_reta;
			if ( (el->caloCluster()->et() > 9e3) && (fabsf(el->eta())<2.47) )
				continue;
			float off_f1 = el->auxdata<float>("f1");
			float SE_f1 = 0.0;
			if ( cl->energy() > 0.0 )
				{ SE_f1 = (cl->energy(CaloSampling::EMB1)+cl->energy(CaloSampling::EME1))/cl->energy(); }
			float resol_f1 = -100.0;
			if ( fabsf(off_f1) > 0.01 ) resol_f1 = 
				100.0*(off_f1-SE_f1)/off_f1;

			float off_f3 = el->auxdata<float>("f3");
			float SE_f3 = 0.0;
			if ( cl->energy() > 0.0 )
				{ SE_f3 = (cl->energy(CaloSampling::EMB3)+cl->energy(CaloSampling::EME3))/cl->energy(); }
			float resol_f3 = -100.0;
			if ( fabsf(off_f3) > 0.001 ) resol_f3 = 
				100.0*(off_f3-SE_f3)/off_f3;

                }
        }
	} // end of if m_doTruth

	return StatusCode::SUCCESS;
}

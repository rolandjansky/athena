/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/


/** 
 * NAME : 	NoiseStudy.cxx
 * PACKAGE : 	Trigger/L1CaloUpgrade/NoiseStudy
 *
 * AUTHOR : 	Denis Oliveira Damazio
 *
 *
 * **/

#include "NoiseStudy.h"
#include "CaloEvent/CaloCellContainer.h"
#include "CaloIdentifier/CaloCell_SuperCell_ID.h"
#include "CaloIdentifier/CaloIdManager.h"
#include "CaloDetDescr/CaloDetDescrManager.h"
#include "xAODTruth/TruthParticleContainer.h"
#include "xAODTracking/VertexContainer.h"
#include "xAODEventInfo/EventInfo.h"
#include "TFile.h"
#include "TH1F.h"
#include "TH2F.h"
#include "TNtuple.h"
#include "TH1I.h"
#include <math.h>
#include "TrigL1CaloUpgrade/BCID.h"
#include "CaloInterface/ICaloNoiseTool.h"
#include <random>

NoiseStudy::NoiseStudy( const std::string& name, ISvcLocator* pSvcLocator ) : AthAlgorithm (name, pSvcLocator), m_scidtool("CaloSuperCellIDTool"), m_noisetool("CaloNoiseToolDefault"), m_caloLumiBCIDTool(""),m_schelper(0)  {
	
	declareProperty("CaloNoiseTool",          m_noisetool, "Tool Handle for noise tool");
	declareProperty("LumiBCIDTool",m_caloLumiBCIDTool,"Tool for BCID pileup offset average correction");
	declareProperty("CompensateForNoise", m_compNoise=true,"Compensate for the noise with low noise");
	declareProperty("addBCID", m_addBCID=true,"Compensate for the BCID");
	declareProperty("InputSCellName", m_inputSCellName = "SimpleSCellBCID" );
	declareProperty("InputBaseSCellName", m_inputBaseSCellName = "SCell" );
}

NoiseStudy::~NoiseStudy(){}

StatusCode NoiseStudy::initialize(){
	
	ATH_MSG_DEBUG("initializing NoiseStudy");
	std::string filename=name();
	filename+=".BasicCheck.root";
        counter=0;
	m_file = new TFile (filename.c_str(),"RECREATE");
	m_inconePt = new TH1F("HinconePt","HinconePt",100,0,100);
	m_inconeEtaPhi = new TH2F("HinconeEtaPhi","HinconeEtaPhi",100,-5,5,64,-M_PI,M_PI);
	m_outconePt = new TH1F("HoutconePt","HoutconePt",100,0,100);
	m_outconeEtaPhi = new TH2F("HoutconeEtaPhi","HoutconeEtaPhi",100,-5,5,64,-M_PI,M_PI);
	m_ntuple = new TNtuple("Hcompare","Hcompare","ev:ID:bcidNor:en1:en2:en3:eta:phi:incone:samp:t1:t3:prov1:prov3");
	m_noisetuple = new TNtuple("Hnoise","Hnoise","ID:n1:n2:n3:pu:t1:t2:t3:eta:phi:ncells:ngains:samp");

	// for cell <-> SCell comparison
	if ( m_scidtool.retrieve().isFailure() ){
		ATH_MSG_ERROR("cannot perform comparisons between cell and SuperCells");
	}
	m_first = true;

        const CaloIdManager* caloMgr;
	const CaloSuperCellDetDescrManager* sem_mgr;

	ATH_CHECK( detStore()->retrieve (sem_mgr, "CaloSuperCellMgr") );
      
        CHECK( detStore()->retrieve(caloMgr) );
      
        //
        //..... need of course the LVL1 helper
        //
        m_schelper = caloMgr->getCaloCell_SuperCell_ID();
        if (!m_schelper) {
          ATH_MSG_ERROR( "Could not access CaloCell_SuperCell_ID helper");
          return StatusCode::FAILURE;
        } else {
          ATH_MSG_DEBUG( "Successfully accessed CaloCell_SuperCell_ID helper");
        }
        size_t lenresimplescells = m_schelper->calo_cell_hash_max();
	m_resimplescells.resize(lenresimplescells );
	m_resimplescellsID.resize(lenresimplescells );
        m_ncells.resize(lenresimplescells );
	m_ngains.resize(lenresimplescells );
	m_noise.resize(lenresimplescells );
	for (size_t i=0;i<lenresimplescells; i++){
	   Identifier id = m_schelper->cell_id( (IdentifierHash)i );
	   CaloDetDescrElement* cdde = sem_mgr->get_element(id);
	   m_resimplescells[i] = new CaloCell(cdde,id,0.0,0.0,0.0,(CaloGain::CaloGain)0);
	   m_ncells[i]=0;
	   m_ngains[i]=0;
	   m_resimplescellsID[i] = id.get_identifier32().get_compact();
	   m_noise[i].id = id.get_identifier32().get_compact();
	   m_noise[i].elec_hnoise = 0.0;
	   m_noise[i].elec_mnoise = 0.0;
	   m_noise[i].elec_lnoise = 0.0;
	   m_noise[i].pileup = 0.0;
	   m_noise[i].total_hnoise = 0.0;
	   m_noise[i].total_mnoise = 0.0;
	   m_noise[i].total_lnoise = 0.0;
	}

	if ( m_compNoise ) ATH_CHECK( m_noisetool.retrieve() );
	if ( m_addBCID ) ATH_CHECK( m_caloLumiBCIDTool.retrieve() );

	return StatusCode::SUCCESS;
}

StatusCode NoiseStudy::finalize(){
	ATH_MSG_DEBUG("finalizing NoiseStudy");
	m_file->Write();
	m_file->Close();
	return StatusCode::SUCCESS;
}

StatusCode NoiseStudy::execute(){
	

	std::default_random_engine generator;
	

	ATH_MSG_DEBUG ("execute NoiseStudy" );
	const xAOD::EventInfo* evt = nullptr;
	if ( evtStore()->retrieve(evt,"EventInfo").isFailure() ){
		ATH_MSG_WARNING("did not find EventInfo container");
	}
	long bunch_crossing(-1);
	long bunch_crossingNor(-1);
	if ( evt ) {
	   bunch_crossing = evt->bcid();
	   bunch_crossingNor = bcids_from_start ( bunch_crossing );
	}
        const CaloCellContainer* scells;
        const CaloCellContainer* scellsimple;
        const CaloCellContainer* allcalo(NULL);
	if ( evtStore()->retrieve(scells,m_inputBaseSCellName).isFailure() ){
		ATH_MSG_WARNING ("did not find cell container" );
		return StatusCode::SUCCESS;
	}
	if ( evtStore()->retrieve(scellsimple,m_inputSCellName).isFailure() ){
		ATH_MSG_WARNING("did not find cell container");
		return StatusCode::SUCCESS;
	}
	if ( evtStore()->retrieve(allcalo,"AllCalo").isFailure() ){
		ATH_MSG_WARNING("did not find cell container for regular cells, no resolution test possible" );
	}

	const xAOD::TruthParticleContainer* truth;
        if ( evtStore()->retrieve(truth,"TruthParticles").isFailure() ) {
                ATH_MSG_WARNING ("did not find truth particle container");
                return StatusCode::SUCCESS;
        }
	std::vector<float> el_etas, el_phis;
	for( auto tt : *truth ){
		if ( tt->parent() == nullptr ) continue;
                if ( tt->parent()->pdgId() != 23 ) continue;
                if ( fabsf(tt->absPdgId()) != 11 ) continue;
                if ( tt->barcode() >= 10000 ) continue;
                if ( tt->pt() < 1e3 ) continue;
		el_etas.push_back ( tt->eta() );
		el_phis.push_back ( tt->phi() );
	}
        uint32_t lenscellsimple = scellsimple->size();
	std::vector<uint32_t> scellsimpleID(lenscellsimple);
	for(size_t j=0; j<lenscellsimple;j++ ){
	       scellsimpleID.at(j) = scellsimple->at(j)->ID().get_identifier32().get_compact() ;
	}

        size_t lenresimplescells = m_schelper->calo_cell_hash_max();
	for (size_t i=0;i<lenresimplescells; i++){
	   m_resimplescells[i]->setEnergy(0);;
	   m_ncells[i]=0;
	   m_ngains[i]=0;
	}

	std::vector<float> ele_noise_sigma(lenresimplescells,0.0);
	std::vector<float> ele_noise_norm(lenresimplescells,0.0);
        
	
        for(auto cell : *allcalo) {
		if ( cell->caloDDE()->is_tile() ) continue;
		float pedestalshift = 0.0;
		if ( m_addBCID ) pedestalshift = m_caloLumiBCIDTool->average(cell,bunch_crossing);
		size_t idx = m_schelper->calo_cell_hash( m_scidtool->offlineToSuperCellID ( cell->ID() ) ).value();
		if ( idx > lenresimplescells ) { ATH_MSG_DEBUG ("Problems with index : " << cell->ID().get_identifier32().get_compact() << " " << m_scidtool->offlineToSuperCellID ( cell->ID() ) << " " << idx) ; continue; }
		m_resimplescells[idx]->setEnergy( m_resimplescells[idx]->energy() + cell->energy() + pedestalshift );
		m_ncells[idx]++;
		if ( cell->gain() != 0 ) m_ngains[idx]++;
		const CaloDetDescrElement* cdde = cell->caloDDE();
		if ( !m_compNoise ) continue;
		std::vector<float> nn = m_noisetool->elecNoiseRMS3gains(cdde);
                float pu=m_noisetool->pileupNoiseRMS(cdde);
                float nn1 = m_noisetool->totalNoiseRMS(cdde,CaloGain::LARHIGHGAIN);
                float nn2 = m_noisetool->totalNoiseRMS(cdde,CaloGain::LARMEDIUMGAIN);
                float nn3 = m_noisetool->totalNoiseRMS(cdde,CaloGain::LARLOWGAIN);
                m_noise[idx].elec_hnoise = nn[0];
                m_noise[idx].elec_mnoise = nn[1];
                m_noise[idx].elec_lnoise = nn[2];
                m_noise[idx].pileup = pu;
                m_noise[idx].total_hnoise = nn1;
                m_noise[idx].total_mnoise = nn2;
                m_noise[idx].total_lnoise = nn3;
		if ( cell->gain() == CaloGain::LARHIGHGAIN ){
		   // assume that if the gain is not high, we have to
		   // apply a correction
		   ele_noise_sigma[idx]+= (nn[1]*nn[1]-nn[0]*nn[0]);
		   ele_noise_norm[idx]++;
		}
	}
	for(size_t idx=0;idx<lenresimplescells;idx++){
		if ( ele_noise_norm[idx] != 0 ) {
	          float sigma = std::sqrt( ele_noise_sigma[idx] ) ;
	          std::normal_distribution<double> distribution(0.0,sigma);
	          float noise = distribution(generator);
	          m_resimplescells[idx]->setEnergy( m_resimplescells[idx]->energy() + noise );
		}
	}
	if ( m_first ){
	for(size_t i=0;i<m_noise.size();i++){
	   if ( m_ncells[i] == 0 ) continue;
	   float inv = 1.0/m_ncells[i];
	   m_noisetuple->Fill(m_noise[i].id,m_noise[i].elec_hnoise*inv,m_noise[i].elec_mnoise*inv,m_noise[i].elec_lnoise*inv,m_noise[i].pileup*inv,m_noise[i].total_hnoise*inv,m_noise[i].total_mnoise*inv,m_noise[i].total_lnoise*inv,m_resimplescells[i]->eta(), m_resimplescells[i]->phi(), m_ncells[i], m_ngains[i], m_resimplescells[i]->caloDDE()->getSampling() );
	}
	m_first=false;
	}
	


        int c = 0;
        size_t el_etas_size = el_etas.size();
        for(auto scell : *scells) {
		c++;
		float c_eta ( scell->eta() );
		float c_phi ( scell->phi() );
		bool inconeFinal=false;
		for( unsigned int ie = 0 ; ie < el_etas_size ; ++ie ){
		   bool incone=true;
		   if ( fabsf( c_eta - el_etas[ie] ) > 0.2 ) { incone=false; continue;}
		   float dphi = fabsf( c_phi - el_phis[ie] );
                   dphi = fabsf( M_PI - dphi );
                   dphi = fabsf( M_PI - dphi );
                   if ( dphi > 0.2 ) {incone=false; continue; }
		   inconeFinal |= incone;
		}
		if ( inconeFinal ) {
			m_inconePt->Fill( scell->pt()*1e-3 );
			m_inconeEtaPhi->Fill( scell->eta(), scell->phi() );
		} else {
			m_outconePt->Fill( scell->pt()*1e-3 );
			m_outconeEtaPhi->Fill( scell->eta(), scell->phi() );
		}
		uint32_t ii = scell->ID().get_identifier32().get_compact();
		const CaloCell* foundOne(0);
		for(size_t j=0; j<scellsimple->size();j++ ){
			if ( ii != scellsimpleID.at(j) ) continue;
			foundOne = scellsimple->at(j);
			break;
		}
		float energy=0.0;
		if ( foundOne ) energy = foundOne->energy();
		float time=0.0;
		if ( foundOne ) time = foundOne->time();
		float prov=0.0;
		if ( foundOne ) prov = foundOne->provenance();
		for (size_t i=0;i<lenresimplescells;i++){
			if ( ii != m_resimplescellsID[i] ) continue;
			m_ntuple->Fill(evt->eventNumber(),i,(float)bunch_crossingNor, scell->energy(), m_resimplescells[i]->energy(), energy, m_resimplescells[i]->eta(), m_resimplescells[i]->phi(), (int)inconeFinal, m_resimplescells[i]->caloDDE()->getSampling(), scell->time(), time, scell->provenance(), prov );

			break;
		   
		} // end of the loop to find resimplescells
		
	} // loop over scell
	
	return StatusCode::SUCCESS;
}


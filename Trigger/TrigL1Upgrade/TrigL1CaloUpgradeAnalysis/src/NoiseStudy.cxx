/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
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
#include "CaloIdentifier/CaloCell_SuperCell_ID.h"
#include "CaloIdentifier/CaloIdManager.h"
#include "CaloDetDescr/CaloDetDescrManager.h"
#include "TFile.h"
#include "TH1F.h"
#include "TH2F.h"
#include "TNtuple.h"
#include "TH1I.h"
#include <math.h>
#include "BCID.h"
#include <random>

NoiseStudy::NoiseStudy( const std::string& name, ISvcLocator* pSvcLocator ) : AthReentrantAlgorithm (name, pSvcLocator), m_scidtool("CaloSuperCellIDTool"), m_schelper(0)  {
	
	declareProperty("CompensateForNoise", m_compNoise=true,"Compensate for the noise with low noise");
	declareProperty("addBCID", m_addBCID=true,"Compensate for the BCID");
}

NoiseStudy::~NoiseStudy(){}

StatusCode NoiseStudy::initialize(){
	
	ATH_MSG_DEBUG("initializing NoiseStudy");
	std::string filename=name();
	filename+=".BasicCheck.root";
	m_file = new TFile (filename.c_str(),"RECREATE");
	m_ntuple = new TNtuple("Hcompare","Hcompare","ev:ID:bcidNor:en1:en2:en3:eta:phi:incone:samp:t1:t3:prov1:prov3");

	// for cell <-> SCell comparison
	if ( m_scidtool.retrieve().isFailure() ){
		ATH_MSG_ERROR("cannot perform comparisons between cell and SuperCells");
	}
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

	if ( m_compNoise ) ATH_CHECK( m_noiseCDOKey.initialize() );
	if ( m_addBCID ) ATH_CHECK( m_bcidAvgKey.initialize() );

	ATH_CHECK( m_sCellContainerInBaseKey.initialize() );
	ATH_CHECK( m_sCellContainerInKey.initialize() );
	ATH_CHECK( m_cellContainerInKey.initialize() );
	ATH_CHECK( m_evtInKey.initialize() );
	ATH_CHECK( m_truthInKey.initialize() );

	return StatusCode::SUCCESS;
}

StatusCode NoiseStudy::finalize(){
	ATH_MSG_DEBUG("finalizing NoiseStudy");
	m_file->Write();
	m_file->Close();
	return StatusCode::SUCCESS;
}

StatusCode NoiseStudy::execute(const EventContext& context) const{

	ATH_MSG_DEBUG ("execute NoiseStudy" );
	const CaloSuperCellDetDescrManager* sem_mgr;
	ATH_CHECK( detStore()->retrieve (sem_mgr, "CaloSuperCellMgr") );
	std::default_random_engine generator;
	// Not really a condition, but almost
	const CaloBCIDAverage* caloLumiBCID = nullptr;
	if (m_addBCID ) {
	  SG::ReadHandle<CaloBCIDAverage> caloLumiBCIDHandle(m_bcidAvgKey, context);
	  if ( not caloLumiBCIDHandle.isValid() ) {
	      ATH_MSG_DEBUG("Found invalid read handle for CaloBCIDAverage");
	      return StatusCode::FAILURE;
	  }
	  caloLumiBCID = caloLumiBCIDHandle.cptr();
	} // end of if m_addBCID


	// conditions
	const CaloNoise* noise = nullptr;
	if ( m_compNoise ){
	  SG::ReadCondHandle<CaloNoise> noiseHandle(m_noiseCDOKey,context);
	  if ( not noiseHandle.isValid() ) {
	      ATH_MSG_DEBUG("Found invalid read handle for CaloNoiseSigmaDiff");
	      return StatusCode::FAILURE;
	  }
	  noise = noiseHandle.cptr();
	} // end of if n_compNoise

	const xAOD::EventInfo* evt = nullptr;
	SG::ReadHandle<xAOD::EventInfo> evtHandle(m_evtInKey,context);
	if ( not evtHandle.isValid() ) {
	    ATH_MSG_DEBUG("Found invalid read handle for EventInfo");
	    return StatusCode::FAILURE;
	}
	evt = evtHandle.cptr();

	long bunch_crossing(-1);
	long bunch_crossingNor(-1);
	if ( evt ) {
	   bunch_crossing = evt->bcid();
	   bunch_crossingNor = bcids_from_start ( bunch_crossing );
	}

        auto cellsBaseHandle = SG::makeHandle( m_sCellContainerInBaseKey, context );
        if ( not cellsBaseHandle.isValid() ) {
          ATH_MSG_ERROR("Did not get CaloCellContainer input");
          return StatusCode::FAILURE;
        }
        const CaloCellContainer* scells = cellsBaseHandle.cptr();

        auto cellsHandle = SG::makeHandle( m_sCellContainerInKey, context );
        if ( not cellsHandle.isValid() ) {
          ATH_MSG_ERROR("Did not get CaloCellContainer input");
          return StatusCode::FAILURE;
        }
        const CaloCellContainer* scellsimple = cellsHandle.cptr();

        auto cellsOffHandle = SG::makeHandle( m_cellContainerInKey, context );
        if ( not cellsOffHandle.isValid() ) {
          ATH_MSG_ERROR("Did not get CaloCellContainer input");
          return StatusCode::FAILURE;
        }
        const CaloCellContainer* allcalo = cellsOffHandle.cptr();

	const xAOD::TruthParticleContainer* truth = nullptr;
	SG::ReadHandle<xAOD::TruthParticleContainer> truthHandle(m_truthInKey,context);
	if ( not truthHandle.isValid() ) {
	    ATH_MSG_DEBUG("Found invalid read handle for Truth container");
	    return StatusCode::FAILURE;
	}
	truth = truthHandle.cptr();
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

        size_t lenresimplescells = m_schelper->calo_cell_hash_max();
        std::vector<CaloCell*> resimplescells;
        std::vector<uint32_t> resimplescellsID;
        std::vector<int> ncells;
        std::vector<int> ngains;
        resimplescells.resize(lenresimplescells );
        resimplescellsID.resize(lenresimplescells );
        ncells.resize(lenresimplescells );
        ngains.resize(lenresimplescells );
        for (size_t i=0;i<lenresimplescells; i++){
           Identifier id = m_schelper->cell_id( (IdentifierHash)i );
           const CaloDetDescrElement* cdde = sem_mgr->get_element(id);
           resimplescells[i] = new CaloCell(cdde,id,0.0,0.0,0.0,(CaloGain::CaloGain)0);
           ncells[i]=0;
           ngains[i]=0;
           resimplescellsID[i] = id.get_identifier32().get_compact();
        }

        uint32_t lenscellsimple = scellsimple->size();
	std::vector<uint32_t> scellsimpleID(lenscellsimple);
	for(size_t j=0; j<lenscellsimple;j++ ){
	       scellsimpleID.at(j) = scellsimple->at(j)->ID().get_identifier32().get_compact() ;
	}

	for (size_t i=0;i<lenresimplescells; i++){
	   resimplescells[i]->setEnergy(0);;
	   ncells[i]=0;
	   ngains[i]=0;
	}

	std::vector<float> ele_noise_sigma(lenresimplescells,0.0);
	std::vector<float> ele_noise_norm(lenresimplescells,0.0);

        for(auto cell : *allcalo) {
		if ( cell->caloDDE()->is_tile() ) continue;
		float pedestalshift = 0.0;
		if ( m_addBCID ) pedestalshift = caloLumiBCID->average(cell->ID());
		size_t idx = m_schelper->calo_cell_hash( m_scidtool->offlineToSuperCellID ( cell->ID() ) ).value();
		if ( idx > lenresimplescells ) { ATH_MSG_DEBUG ("Problems with index : " << cell->ID().get_identifier32().get_compact() << " " << m_scidtool->offlineToSuperCellID ( cell->ID() ) << " " << idx) ; continue; }
		resimplescells[idx]->setEnergy( resimplescells[idx]->energy() + cell->energy() + pedestalshift );
		ncells[idx]++;
		if ( cell->gain() != 0 ) ngains[idx]++;
		if ( !m_compNoise ) continue;
		if ( cell->gain() == CaloGain::LARHIGHGAIN ){
		   IdentifierHash h(cell->caloDDE()->calo_hash() );
		   float nn1=noise->getNoise(h,1); // medium gain
		   float nn0=noise->getNoise(h,0); // high gain
		   float sigma_diff = nn1*nn1-nn0*nn0;
		   if ( sigma_diff < 0.0 ) {  continue;}
		   ele_noise_sigma[idx]+=std::sqrt(sigma_diff);
		   ele_noise_norm[idx]++;
		}
	}
	for(size_t idx=0;idx<lenresimplescells;idx++){
		if ( ele_noise_norm[idx] != 0 ) {
	          float sigma = std::sqrt( ele_noise_sigma[idx] ) ;
	          std::normal_distribution<double> distribution(0.0,sigma);
	          float noise = distribution(generator);
	          resimplescells[idx]->setEnergy( resimplescells[idx]->energy() + noise );
		}
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
			if ( ii != resimplescellsID[i] ) continue;
			m_ntuple->Fill(evt->eventNumber(),i,(float)bunch_crossingNor, scell->energy(), resimplescells[i]->energy(), energy, resimplescells[i]->eta(), resimplescells[i]->phi(), (int)inconeFinal, resimplescells[i]->caloDDE()->getSampling(), scell->time(), time, scell->provenance(), prov );

			break;
		   
		} // end of the loop to find resimplescells
		
	} // loop over scell
	
	return StatusCode::SUCCESS;
}


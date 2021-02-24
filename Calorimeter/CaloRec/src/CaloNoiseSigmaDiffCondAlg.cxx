
/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/
/**
 * NAME : CaloNoiseSigmaDiffCondAlg.cxx
 * PACKAGE : Calorimeter/CaloRec
 *
 * AUTHOR : Denis Oliveira DamaZio
 * PURPOSE : Very simple class to record simple computation on CaloNoise
 *
 * */


#include "CaloNoiseSigmaDiffCondAlg.h"
#include "StoreGate/ReadCondHandle.h"
#include "StoreGate/WriteCondHandle.h"
#include "CaloIdentifier/CaloIdManager.h"

CaloNoiseSigmaDiffCondAlg::CaloNoiseSigmaDiffCondAlg(const std::string& name, ISvcLocator* pSvcLocator) :
AthReentrantAlgorithm(name,pSvcLocator)
{
}

StatusCode CaloNoiseSigmaDiffCondAlg::initialize() {
	ATH_CHECK( m_noiseCDOKey.initialize() );
	ATH_CHECK( m_noiseSigmaDiff.initialize() );
	return StatusCode::SUCCESS;
}

StatusCode CaloNoiseSigmaDiffCondAlg::execute(const EventContext& context) const {

	// Write Conditions
	SG::WriteCondHandle<CaloNoiseSigmaDiff> output(m_noiseSigmaDiff, context);
        if ( output.isValid() ) {
		ATH_MSG_DEBUG("Already available handle");
		return StatusCode::SUCCESS;
	}
	
	const CaloIdManager* calo_id_manager(0);
	CHECK( detStore()->retrieve (calo_id_manager, "CaloIdManager") );
	const CaloCell_ID * calo_cell_id =  calo_id_manager->getCaloCell_ID();
	int hash_max = calo_cell_id->calo_cell_hash_max();

	// prepare real output object (not handle)
	auto out = std::make_unique<CaloNoiseSigmaDiff>(hash_max,0.0);

	// get Noise Table
	SG::ReadCondHandle<CaloNoise> noiseCDO (m_noiseCDOKey, context);
	const CaloNoise* nn = noiseCDO.cptr();
	for(int i=0;i<hash_max;++i){
		IdentifierHash h(i);
		if (calo_cell_id->is_tile(h) ) continue;
		float nn1=nn->getNoise(h,1); // medium gain
		float nn0=nn->getNoise(h,0); // high gain
		float sigma_diff = nn1*nn1-nn0*nn0;
		if ( sigma_diff < 0.0 ) { out->at(i)=0; continue;}
		out->at(i)=std::sqrt(sigma_diff);
	}

	output.addDependency(noiseCDO);
	ATH_CHECK(output.record( std::move(out) ) );
	ATH_MSG_INFO( "recorded new " << output.key() << " with range " << output.getRange() );
	return StatusCode::SUCCESS;
}

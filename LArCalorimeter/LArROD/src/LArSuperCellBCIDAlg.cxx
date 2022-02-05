/*
 *   Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
 *
 * Name :       LArSuperCellBCIDAlg.cxx
 * PACKAGE :    LArCalorimeter/LArROD/LArSuperCellBCIDAlg
 *
 * AUTHOR :     Denis Oliveira Damazio
 *
 * PURPOSE :    prepares SuperCellContainer to have the offset BCID-dependent correction. Special version for emulated cells
 *
 * **/

#include "LArSuperCellBCIDAlg.h"
#include "CaloEvent/CaloCellContainer.h"
#include "LArIdentifier/LArOnline_SuperCellID.h"
#include "LArCabling/LArOnOffIdMapping.h"
#include "xAODEventInfo/EventInfo.h"

LArSuperCellBCIDAlg::LArSuperCellBCIDAlg( const std::string& name, ISvcLocator* pSvcLocator)
  : AthReentrantAlgorithm( name, pSvcLocator)
{
}

StatusCode
LArSuperCellBCIDAlg::initialize()
{

        ATH_CHECK( m_sCellContainerInKey.initialize() );
        ATH_CHECK( m_sCellContainerOutKey.initialize() );
	ATH_CHECK( m_bcidAvgKey.initialize() );
	ATH_CHECK(m_cablingKey.initialize() );
	ATH_CHECK(detStore()->retrieve(m_laronline_id,"LArOnline_SuperCellID"));
        return StatusCode::SUCCESS;
}

StatusCode
LArSuperCellBCIDAlg::execute(const EventContext& context) const
{

	SG::ReadCondHandle<LArOnOffIdMapping> cabling(m_cablingKey,context);

	// Not really a condition, but almost
        SG::ReadHandle<CaloBCIDAverage> caloLumiBCIDHandle(m_bcidAvgKey, context);
        if ( not caloLumiBCIDHandle.isValid() ) {
            ATH_MSG_DEBUG("Found invalid read handle for CaloBCIDAverage");
            return StatusCode::FAILURE;
        }
        const CaloBCIDAverage* caloLumiBCID = caloLumiBCIDHandle.cptr();

        auto cellsHandle = SG::makeHandle( m_sCellContainerInKey, context );
        if ( not cellsHandle.isValid() ) {
          ATH_MSG_ERROR("Did not get CaloCellContainer input");
          return StatusCode::FAILURE;
        }

        
        const CaloCellContainer* scells_from_sg = cellsHandle.cptr();
        ATH_MSG_DEBUG("Got a CaloCellContainer input with size : "<<scells_from_sg->size());

        SG::WriteHandle<CaloCellContainer> scellContainerHandle( m_sCellContainerOutKey, context);
        auto new_scell_cont = std::make_unique<CaloCellContainer> ();
        
        new_scell_cont->reserve(scells_from_sg->size());

        for(auto sc : *scells_from_sg){
                if ( !sc ) continue;
                std::unique_ptr<CaloCell> cell = sc->clone();
                // calculate the BCID for all the cells associated to the SC
                // since this is emulation
                float correction(0.0);
		HWIdentifier hwid =  cabling->createSignalChannelID(sc->ID());
                IdentifierHash hash_id = m_laronline_id->channel_Hash(hwid); 
                correction=caloLumiBCID->average((int)hash_id); ;
                cell->setEnergy( sc->energy() - correction );
                new_scell_cont->push_back( std::move(cell) );
        }
	ATH_CHECK( scellContainerHandle.record( std::move(new_scell_cont) ) );

        return StatusCode::SUCCESS;
}

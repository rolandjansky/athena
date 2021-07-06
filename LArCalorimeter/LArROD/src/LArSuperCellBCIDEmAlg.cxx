/*
 *   Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
 *
 * Name :       LArSuperCellBCIDEmAlg.cxx
 * PACKAGE :    LArCalorimeter/LArROD/LArSuperCellBCIDEmAlg
 *
 * AUTHOR :     Denis Oliveira Damazio
 *
 * PURPOSE :    prepares SuperCellContainer to have the offset BCID-dependent correction. Special version for emulated cells
 *
 * **/

#include "LArSuperCellBCIDEmAlg.h"
#include "CaloEvent/CaloCellContainer.h"
#include "CaloDetDescr/ICaloSuperCellIDTool.h"
#include "xAODEventInfo/EventInfo.h"

LArSuperCellBCIDEmAlg::LArSuperCellBCIDEmAlg( const std::string& name, ISvcLocator* pSvcLocator)
  : AthReentrantAlgorithm( name, pSvcLocator)
{
}

StatusCode
LArSuperCellBCIDEmAlg::initialize()
{

        ATH_CHECK( m_sCellContainerInKey.initialize() );
        ATH_CHECK( m_sCellContainerOutKey.initialize() );
	ATH_CHECK( m_bcidAvgKey.initialize() );
	ATH_CHECK( m_scidtool.retrieve() );
        return StatusCode::SUCCESS;
}

StatusCode
LArSuperCellBCIDEmAlg::execute(const EventContext& context) const
{

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
        auto new_scell_cont = std::make_unique<CaloCellContainer> (SG::VIEW_ELEMENTS);
        
        new_scell_cont->reserve(scells_from_sg->size());

        for(auto sc : *scells_from_sg){
                if ( !sc ) continue;
                CaloCell* cell = sc->clone();
                // calculate the BCID for all the cells associated to the SC
                // since this is emulation
                float correction(0.0);
                std::vector<Identifier> ids = m_scidtool->superCellToOfflineID( sc->ID() );
                for ( auto id : ids ) correction+=caloLumiBCID->average(id); ;
                cell->setEnergy( sc->energy() - correction );
                new_scell_cont->push_back( cell );
        }
	ATH_CHECK( scellContainerHandle.record( std::move(new_scell_cont) ) );

        return StatusCode::SUCCESS;
}

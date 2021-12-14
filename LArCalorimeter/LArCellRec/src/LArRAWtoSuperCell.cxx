/*
 *   Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
 *
 * Name :       LArRAWtoSuperCell.cxx
 * PACKAGE :    LArCalorimeter/LArCell/LArRAWtoSuperCell
 *
 * AUTHOR :     Denis Oliveira Damazio
 *
 * PURPOSE :    prepares SuperCellContainer in CaloCellContainer formar from LArRawSCContainer
 *
 * **/

#include "LArRAWtoSuperCell.h"
#include "LArIdentifier/LArOnline_SuperCellID.h"
#include "CaloDetDescr/CaloDetDescrManager.h"
#include "LArCabling/LArOnOffIdMapping.h"
#include "xAODEventInfo/EventInfo.h"

LArRAWtoSuperCell::LArRAWtoSuperCell( const std::string& name, ISvcLocator* pSvcLocator)
  : AthReentrantAlgorithm( name, pSvcLocator)//, m_sem_mgr(0)
{
}

StatusCode
LArRAWtoSuperCell::initialize()
{

        ATH_CHECK( m_sCellContainerInKey.initialize() );
        ATH_CHECK( m_sCellContainerOutKey.initialize() );
	ATH_CHECK(detStore()->retrieve(m_laronline_id,"LArOnline_SuperCellID"));
	ATH_CHECK( m_caloMgrKey.initialize());
        return StatusCode::SUCCESS;
}

StatusCode
LArRAWtoSuperCell::execute(const EventContext& context) const
{

	SG::ReadCondHandle<CaloSuperCellDetDescrManager> caloMgrHandle{m_caloMgrKey, context};
        const CaloSuperCellDetDescrManager* sem_mgr = *caloMgrHandle;;
	
        auto cellsHandle = SG::makeHandle( m_sCellContainerInKey, context );
        if ( not cellsHandle.isValid() ) {
          ATH_MSG_ERROR("Did not get CaloCellContainer input");
          return StatusCode::FAILURE;
        }
        
        const LArRawSCContainer* scells_from_sg = cellsHandle.cptr();
        ATH_MSG_DEBUG("Got a CaloCellContainer input with size : "<<scells_from_sg->size());

        SG::WriteHandle<CaloCellContainer> scellContainerHandle( m_sCellContainerOutKey, context);
        auto new_scell_cont = std::make_unique<CaloCellContainer> ();

	const EventIDBase& EIHandle = context.eventID();
        const unsigned int bcid = EIHandle.bunch_crossing_id();
        
        new_scell_cont->reserve(scells_from_sg->size());

        for(auto sc : *scells_from_sg){
                if ( !sc ) continue;
                // calculate the BCID for all the cells associated to the SC
                // since this is emulation
                IdentifierHash hash_id = m_laronline_id->channel_Hash(sc->hardwareID()); 
                const CaloDetDescrElement* dde = sem_mgr ->get_element(hash_id);
		CaloCell* cell = new CaloCell();
		cell->setCaloDDE(dde);
		const std::vector< unsigned short >& bcids = sc->bcids();
		const std::vector< int >& energies = sc->energies();
		const std::vector< bool>& satur = sc->satur();
		float energy(0.);
		bool saturation(false);
		for(unsigned int i=0;i<bcids.size();i++) if ( bcids[i]==bcid ) {energy=energies[i]; saturation = satur[i];}
                cell->setEnergy( energy );
		// we probably should soon associate some quality information to the saturation, maybe the bcid to provenance
		cell->setQuality((unsigned short)saturation);
                new_scell_cont->push_back( std::move(cell) );
        }
	ATH_CHECK( scellContainerHandle.record( std::move(new_scell_cont) ) );

        return StatusCode::SUCCESS;
}

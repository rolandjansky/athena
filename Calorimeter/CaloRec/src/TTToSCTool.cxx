/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/


#include "CaloRec/TTToSCTool.h"
#include "CaloIdentifier/CaloIdManager.h"
#include "Identifier/Identifier.h"

#include "LArCabling/LArSuperCellCablingTool.h"
#include "CaloDetDescr/CaloDetDescrManager.h"
#include "CaloEvent/CaloCellContainer.h"
#include "CaloEvent/CaloTowerSeg.h"
#include "CaloIdentifier/CaloCell_SuperCell_ID.h"
#include "xAODTrigL1Calo/TriggerTowerContainer.h"
#include "CLHEP/Units/SystemOfUnits.h"

TTToSCTool::TTToSCTool(
                       const std::string& type,
                       const std::string& name,
                       const IInterface* parent):
	AthAlgTool(type, name, parent),
	m_ttTool("LVL1::L1TriggerTowerTool/L1TriggerTowerTool"),
	m_scidtool ("CaloSuperCellIDTool")
{
	declareInterface<ITTToSCTool>( this ); 
	declareProperty("L1TriggerTowerTool", m_ttTool);

}

TTToSCTool::~TTToSCTool()
{ }

StatusCode TTToSCTool::initialize()
{

	const CaloIdManager *caloMgr = nullptr;
	CHECK(detStore()->retrieve(caloMgr));
	m_lvl1Helper = caloMgr->getLVL1_ID();
	if(!m_lvl1Helper) {
		ATH_MSG_FATAL( "Cannot access CaloLVL1_ID helper." );
		return StatusCode::FAILURE;
	}
 
	CHECK(m_ttTool.retrieve());
	CHECK( m_scidtool.retrieve() );

	CHECK( detStore()->retrieve (m_sem_mgr) );
	CHECK( detStore()->retrieve (m_em_mgr) );

	// Make the mapping of SC to Tile
	const CaloCell_SuperCell_ID* m_scid = m_sem_mgr->getCaloCell_ID();
	unsigned hashMax = m_scid->calo_cell_hash_max();
	const Tile_SuperCell_ID *tileSCID = m_scid->tile_idHelper();
	unsigned hashMin = hashMax-tileSCID->cell_hash_max();

	const CaloCell_ID* m_cid = m_em_mgr->getCaloCell_ID();
	const TileID *tileID = m_cid->tile_idHelper();
	std::vector<Identifier> cells;
	m_tileCablingService = TileCablingService::getInstance();
	for (unsigned hs=hashMin;hs<hashMax;++hs) {
		const Identifier scid=m_scid->cell_id(hs);
		if (m_scid->sample(scid)==2)
			continue;
		cells =  m_scidtool->superCellToOfflineID (scid);
		Identifier pmt_id = tileID->pmt_id(cells.at(0), 0);
		Identifier tt_id = m_tileCablingService->pmt2tt_id(pmt_id);
		m_ttToSC[tt_id] = scid;
		IdentifierHash idhash = m_sem_mgr->getCaloCell_ID()->calo_cell_hash(scid);
		const CaloDetDescrElement* dde = m_sem_mgr->get_element (idhash);
	}
	return StatusCode::SUCCESS;
}

StatusCode TTToSCTool::buildTileSCs(CaloCellContainer* caloCell)
{
	const xAOD::TriggerTowerContainer* tts;
	ATH_CHECK( evtStore()->retrieve(tts) );

	//int count = 0;
	for(auto* tt: *tts) {
		auto id = m_ttTool->identifier(tt->eta(), tt->phi(), tt->sampling());
		bool isTile = m_lvl1Helper->is_tile(id);
		if(!isTile)
			continue;
	 
		Identifier scid = m_ttToSC[id];
		IdentifierHash idhash = m_sem_mgr->getCaloCell_ID()->calo_cell_hash(scid);
		const CaloDetDescrElement* dde = m_sem_mgr->get_element (idhash);
		if (dde==NULL)
			ATH_MSG_ERROR( "DDE is null"  );
		CaloCell* cell = new CaloCell( dde, (float)tt->e(), (float)0, (uint16_t)0, (uint16_t)0x240, (CaloGain::CaloGain)0 );
		caloCell->push_back( cell );
	}
 
	return StatusCode::SUCCESS;
}



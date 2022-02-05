/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#include "TrigT1CaloCalibUtils/L1CaloHVDummyContainers.h"

#include "GaudiKernel/MsgStream.h"

#include "TrigT1CaloEvent/TriggerTowerCollection.h"
#include "Identifier/Identifier.h"
#include "Identifier/IdentifierHash.h"
#include "CaloEvent/CaloCellContainer.h"
#include "CaloDetDescr/CaloDetDescriptor.h"
#include "CaloDetDescr/CaloDetDescrElement.h"
#include "CaloIdentifier/CaloCell_ID.h"
#include "CaloIdentifier/CaloIdManager.h"

L1CaloHVDummyContainers::L1CaloHVDummyContainers(const std::string& name, ISvcLocator *pSvcLocator)
  : AthAlgorithm(name, pSvcLocator),
    m_triggerTowerCollectionName("TriggerTowers"),
    m_caloCellContainerName("AllCalo"),
    m_caloMgr(nullptr),
    m_caloCellHelper(nullptr),
    m_firstEvent(true)
{
    declareProperty("TriggerTowerCollectionName", m_triggerTowerCollectionName);
    declareProperty("CaloCellContainerName", m_caloCellContainerName);
}

L1CaloHVDummyContainers::~L1CaloHVDummyContainers()
{
}

StatusCode L1CaloHVDummyContainers::initialize()
{
  ATH_CHECK(detStore()->retrieve(m_caloMgr));
  m_caloCellHelper = m_caloMgr->getCaloCell_ID();
  if ( !m_caloCellHelper ) {
    ATH_MSG_ERROR("Cannot retrieve CaloCell_ID");
    return StatusCode::FAILURE;
  }
  ATH_CHECK(m_caloMgrKey.initialize());

  return StatusCode::SUCCESS;
}

StatusCode L1CaloHVDummyContainers::execute()
{
    if (m_firstEvent) {

      SG::ReadCondHandle<CaloDetDescrManager> caloMgrHandle{m_caloMgrKey};
      ATH_CHECK(caloMgrHandle.isValid());
      const CaloDetDescrManager* caloDetDescrMgr = *caloMgrHandle;

        // Create dummy TriggerTower collection
	TriggerTowerCollection* ttCol = new TriggerTowerCollection();

	// Fill with dummy TriggerTowers
	const int etaBins[4] = { 25, 3, 1, 4 };
	const int phiBins[4] = { 64, 32, 32, 16 };
	const double etaOffsets[4] = { 0., 2.5, 3.1, 3.2 };
	const double etaGrans[4]   = { 0.1, 0.2, 0.1, 0.425 };
	const double phiGrans[4]   = { M_PI/32., M_PI/16., M_PI/16., M_PI/8. };
	for (int side = -1; side <= 1; side+=2) {
	    for (int region = 0; region < 4; ++region) {
	        for (int ieta = 0; ieta < etaBins[region]; ++ieta) {
	            double eta = side*(etaOffsets[region] + (ieta+0.5)*etaGrans[region]);
		    for (int iphi = 0; iphi < phiBins[region]; ++iphi) {
	                double phi = (iphi+0.5)*phiGrans[region];
                        LVL1::TriggerTower* tt = new LVL1::TriggerTower(phi, eta, 0);
			ttCol->push_back(tt);
                    }
                }
            }
        }
	ATH_MSG_INFO("Size of TriggerTowerCollection is " << ttCol->size());
	
	// Create dummy CaloCellCollection
	CaloCellContainer* ccCol = new CaloCellContainer();
	
	// Fill with dummy CaloCells
	std::vector<Identifier>::const_iterator cellItr(m_caloCellHelper->cell_begin(CaloCell_ID::LAREM));
	std::vector<Identifier>::const_iterator cellEnd(m_caloCellHelper->cell_end(CaloCell_ID::LAREM));
	for (; cellItr != cellEnd; ++cellItr) {
	    CaloCell* cell = new CaloCell();
	    cell->set(caloDetDescrMgr->get_element(*cellItr), *cellItr);
	    ccCol->push_back(cell);
        }
        cellItr = m_caloCellHelper->cell_begin(CaloCell_ID::LARHEC);
	cellEnd = m_caloCellHelper->cell_end(CaloCell_ID::LARHEC);
	for (; cellItr != cellEnd; ++cellItr) {
	    CaloCell* cell = new CaloCell();
	    cell->set(caloDetDescrMgr->get_element(*cellItr), *cellItr);
	    ccCol->push_back(cell);
        }
	cellItr = m_caloCellHelper->cell_begin(CaloCell_ID::LARFCAL);
	cellEnd = m_caloCellHelper->cell_end(CaloCell_ID::LARFCAL);
	for (; cellItr != cellEnd; ++cellItr) {
	    CaloCell* cell = new CaloCell();
	    cell->set(caloDetDescrMgr->get_element(*cellItr), *cellItr);
	    ccCol->push_back(cell);
        }
	ATH_MSG_INFO("Size of CaloCellContainer is " << ccCol->size());

	// Save in StoreGate
	StatusCode sc = evtStore()->record(ttCol, m_triggerTowerCollectionName);
	if (sc.isFailure()) {
	  ATH_MSG_ERROR("Failed to store TriggerTowerCollection in StoreGate");
	  return sc;
        }
	sc = evtStore()->record(ccCol, m_caloCellContainerName);
	if (sc.isFailure()) {
	  ATH_MSG_ERROR("Failed to store CaloCellContainer in StoreGate");
	  return sc;
        }

	m_firstEvent = false;

    }
      
    return StatusCode::SUCCESS;
}

StatusCode L1CaloHVDummyContainers::finalize()
{
    return StatusCode::SUCCESS;
}

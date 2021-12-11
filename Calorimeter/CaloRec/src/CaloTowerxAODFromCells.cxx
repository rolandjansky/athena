/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/


#include "CaloTowerxAODFromCells.h"

#include "CaloEvent/CaloCell.h"
#include "CaloEvent/CaloCellContainer.h"


#include "xAODCaloEvent/CaloTowerContainer.h"
#include "xAODCaloEvent/CaloTowerAuxContainer.h"
#include "CaloGeoHelpers/CaloPhiRange.h"

#include <limits>

CaloTowerxAODFromCells::CaloTowerxAODFromCells(const std::string& name,ISvcLocator* pSvcLocator)
  : CaloTowerxAODAlgoBase(name,pSvcLocator)
  , m_inputCellContainerKey("AllCalo")
  , m_cellThresholdE(std::numeric_limits<float>::min())
  , m_filterCells(false)
{
  declareProperty("InputCellContainer", m_inputCellContainerKey);
  declareProperty("CellEnergyThreshold", m_cellThresholdE);
}

CaloTowerxAODFromCells::~CaloTowerxAODFromCells()
{ }

StatusCode CaloTowerxAODFromCells::initialize() {

  m_filterCells=(m_cellThresholdE!=std::numeric_limits<float>::min());
  ATH_CHECK(m_inputCellContainerKey.initialize());

  // process modes
  if ( m_filterCells ) { 
    ATH_MSG_INFO("[Configuration::Status] FilterCell mode configured with Ecell > " << m_cellThresholdE << " MeV");
  } else { 
    ATH_MSG_INFO("[Configuration::Status] Using all cells");
  }

  return initBase();
}

StatusCode CaloTowerxAODFromCells::execute(const EventContext& ctx) const
{ 
  const CellToTowerVec& cellToTower = getIndexCache(ctx);
  if(cellToTower.empty()) {
    ATH_MSG_ERROR( "Failed to compute the index cache");
    return StatusCode::FAILURE;
  }

  SG::ReadHandle<CaloCellContainer> inputCellContainer(m_inputCellContainerKey, ctx);
  if (!inputCellContainer.isValid()) {
    ATH_MSG_ERROR( "Can't retrieve CaloCellContainer with key " << inputCellContainer.name()  );
    return StatusCode::FAILURE;
  }

  SG::WriteHandle<xAOD::CaloTowerContainer> caloTowerContainer =
    this->makeContainer(ctx);
  if (!caloTowerContainer.isValid())
    return StatusCode::FAILURE;

  const size_t nCell2Tower=cellToTower.size();
  if (nCell2Tower<inputCellContainer->size()) {
    ATH_MSG_ERROR( "Number of cells larger than size of internal cell2tower cache. nCells=" 
                   << inputCellContainer->size() <<  ", cell2tower size=" << nCell2Tower  );
    return StatusCode::FAILURE;
  }

  for (const CaloCell* cell : *inputCellContainer) {
    
    if (!m_filterCells || (cell->e()> m_cellThresholdE)) {

      const IdentifierHash cellHash=cell->caloDDE()->calo_hash();
      assert(cellHash<cellToTower.size());
      if (!(cellHash<cellToTower.size())) {
	ATH_MSG_ERROR( "Cell2Tower mapping too small " << cellToTower.size() << ", expected at least" << cellHash  );
        return StatusCode::FAILURE;
      }
      const auto& c2ts=cellToTower[cellHash];
      //Remember: A cell can contribute to more than one tower!
      for (const cellToTower_t& c2t : c2ts) {
	if (c2t.m_towerIdx > caloTowerContainer->size()) {
	  ATH_MSG_ERROR( "Tower container size too small " << caloTowerContainer->size() << ", expected at least" << c2t.m_towerIdx  );
	  return StatusCode::FAILURE;
	}
	(*caloTowerContainer)[c2t.m_towerIdx]->addEnergy(cell->e()*c2t.m_weight);
      }//end loop over towers coverd by this cell
    }// end if filter cells
  }//end loop over cells


  return StatusCode::SUCCESS;
}
 
StatusCode CaloTowerxAODFromCells::finalize()
{ return StatusCode::SUCCESS; }






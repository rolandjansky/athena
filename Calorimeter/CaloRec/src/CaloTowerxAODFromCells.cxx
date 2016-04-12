/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/


#include "CaloRec/CaloTowerxAODFromCells.h"

#include "CaloEvent/CaloCell.h"
#include "CaloEvent/CaloCellContainer.h"


#include "xAODCaloEvent/CaloTowerContainer.h"
#include "xAODCaloEvent/CaloTowerAuxContainer.h"
#include "CaloDetDescr/CaloDetDescrManager.h"
#include "CaloGeoHelpers/CaloPhiRange.h"

#include <limits>

CaloTowerxAODFromCells::CaloTowerxAODFromCells(const std::string& name,ISvcLocator* pSvcLocator)
  : CaloTowerxAODAlgoBase(name,pSvcLocator)
  , m_cellThresholdE(std::numeric_limits<float>::min())
    //, m_useThresholdE(false)
  , m_filterCells(false)
{
  declareProperty("InputCellContainer", m_inputCellContainerKey="AllCalo");
  declareProperty("CellEnergyThreshold", m_cellThresholdE);
}

CaloTowerxAODFromCells::~CaloTowerxAODFromCells()
{ }

StatusCode CaloTowerxAODFromCells::initialize() {

  m_filterCells=(m_cellThresholdE!=std::numeric_limits<float>::min());
  

  // process modes
  if ( m_filterCells ) { 
    ATH_MSG_INFO("[Configuration::Status] FilterCell mode configured with Ecell > " << m_cellThresholdE << " MeV");
  } else { 
    ATH_MSG_INFO("[Configuration::Status] Using all cells");
  }

  return initBase();
}

StatusCode CaloTowerxAODFromCells::execute() { 

  const CaloCellContainer* inputCellContainer=0;
  if (evtStore()->retrieve(inputCellContainer,m_inputCellContainerKey).isFailure()) {
    msg(MSG::ERROR) << "Can't retrieve CaloCellContainer with key " << m_inputCellContainerKey << endreq;
    return StatusCode::FAILURE;
  }

  // make xAOD::CaloTowerContainer (base-class functionality)
  xAOD::CaloTowerContainer* caloTowerContainer=this->makeContainer();

  const size_t nCell2Tower=m_cellToTower.size();
  if (nCell2Tower<inputCellContainer->size()) {
    msg(MSG::ERROR) << "Number of cells larger than size of internal cell2tower cache. nCells=" 
		    << inputCellContainer->size() <<  ", cell2tower size=" << nCell2Tower << endreq;
    return StatusCode::FAILURE;
  }

  for (const CaloCell* cell : *inputCellContainer) {
    
    if (!m_filterCells || (cell->e()> m_cellThresholdE)) {

      const IdentifierHash cellHash=cell->caloDDE()->calo_hash();
      assert(cellHash<m_cellToTower.size());
      if (!(cellHash<m_cellToTower.size())) {
	msg(MSG::ERROR) << "Cell2Tower mapping too small " << m_cellToTower.size() << ", expected at least" << cellHash << endreq;
	 return StatusCode::FAILURE;
      }
      const auto& c2ts=m_cellToTower[cellHash];
      //Remember: A cell can contribute to more than one tower!
      for (const cellToTower_t& c2t : c2ts) {
	if (c2t.m_towerIdx > caloTowerContainer->size()) {
	  msg(MSG::ERROR) << "Tower container size too small " << caloTowerContainer->size() << ", expected at least" << c2t.m_towerIdx << endreq;
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






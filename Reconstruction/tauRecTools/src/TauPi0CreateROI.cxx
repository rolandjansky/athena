/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#ifndef XAOD_ANALYSIS

#include "TauPi0CreateROI.h"
#include "tauRecTools/HelperFunctions.h"

#include "CaloUtils/CaloCellList.h"

#include <boost/scoped_ptr.hpp>



TauPi0CreateROI::TauPi0CreateROI(const std::string& name) :
     TauRecToolBase(name) {
}
   


StatusCode TauPi0CreateROI::initialize() {
    
    ATH_CHECK( m_caloCellInputContainer.initialize() );
    ATH_CHECK(m_caloMgrKey.initialize());
    return StatusCode::SUCCESS;
}



StatusCode TauPi0CreateROI::executePi0CreateROI(xAOD::TauJet& tau, CaloCellContainer& pi0CellContainer, boost::dynamic_bitset<>& addedCellsMap) const {

  // only run on 0-5 prong taus
  if (!tauRecTools::doPi0andShots(tau)) {
    return StatusCode::SUCCESS;
  }  

  SG::ReadHandle<CaloCellContainer> caloCellInHandle( m_caloCellInputContainer );
  if (!caloCellInHandle.isValid()) {
    ATH_MSG_ERROR ("Could not retrieve HiveDataObj with key " << caloCellInHandle.key());
    return StatusCode::FAILURE;
  }
  const CaloCellContainer *cellContainer = caloCellInHandle.cptr();;
  
  SG::ReadCondHandle<CaloDetDescrManager> caloMgrHandle{m_caloMgrKey};
  const CaloDetDescrManager* caloDDMgr = *caloMgrHandle;
  
  // get only EM cells within dR < 0.4
  // TODO: change hardcoded 0.4 to meaningful variable
  std::vector<CaloCell_ID::SUBCALO> emSubCaloBlocks;
  emSubCaloBlocks.push_back(CaloCell_ID::LAREM);
  boost::scoped_ptr<CaloCellList> cellList(new CaloCellList(caloDDMgr,cellContainer,emSubCaloBlocks)); 
  // FIXME: tau p4 is corrected to point at tau vertex, but the cells are not
  cellList->select(tau.eta(), tau.phi(), 0.4);

  for (const CaloCell* cell : *cellList) {
    // only keep cells that are in Ecal (PS, EM1, EM2 and EM3, both barrel and endcap).
    int sampling = cell->caloDDE()->getSampling();
    if (sampling > 7) continue;

    // Store cell in output container
    const IdentifierHash cellHash = cell->caloDDE()->calo_hash();

    if (!addedCellsMap.test(cellHash)) {
      pi0CellContainer.push_back(cell->clone());
      addedCellsMap.set(cellHash);
    }
  }

  return StatusCode::SUCCESS;
}

#endif

/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#include "TBCellContainerFillerTool.h"

#include "CaloEvent/CaloCellContainer.h"
#include "CaloIdentifier/CaloCell_ID.h"
#include "CaloDetDescr/CaloDetDescrManager.h"
#include "LArIdentifier/LArOnlineID.h"


TBCellContainerFillerTool::TBCellContainerFillerTool(
			     const std::string& type, 
			     const std::string& name, 
			     const IInterface* parent)
  :base_class (type, name, parent)
{ 
}




StatusCode TBCellContainerFillerTool::initialize()
{
  ATH_CHECK(m_caloMgrKey.initialize());
  // Cache pointers:
  ATH_CHECK(detStore()->retrieve(m_theCaloCCIDM, "CaloCell_ID"));

  m_hashMax = m_theCaloCCIDM->calo_cell_hash_max();

  ATH_CHECK( detStore()->retrieve(m_onlineHelper, "LArOnlineID") );

  return StatusCode::SUCCESS;
}

StatusCode
TBCellContainerFillerTool::process (CaloCellContainer* theCont,
                                    const EventContext& ctx) const
{
  SG::ReadCondHandle<CaloDetDescrManager> caloMgrHandle{m_caloMgrKey,ctx};
  ATH_CHECK(caloMgrHandle.isValid());
  const CaloDetDescrManager* caloMgr = *caloMgrHandle;

  StatusCode returnSc = StatusCode::SUCCESS ;

  unsigned int index = 0;
  CaloCellContainer::const_iterator itrCell;
  if (msgLvl(MSG::VERBOSE)) {
      for (itrCell=theCont->begin();itrCell!=theCont->end();++itrCell){
	const CaloDetDescrElement * theDDE=(*itrCell)->caloDDE();
	ATH_MSG_VERBOSE ( index << " " << (*itrCell) 
                          << " hash id " << theDDE->calo_hash()
                          << " eta " << theDDE->eta()
                          << " phi " << theDDE->phi()
                          << "e " << (*itrCell)->e() );
	++index;
      }
  }	


  // fill holes with 0 energy cells
  
  unsigned int added = 0;
  ATH_MSG_DEBUG ( " Now check all cells give meaningfull answer " );
  ATH_MSG_DEBUG ( " Size of original container: "<<theCont->size() );
  for (unsigned int theHash=0;theHash<m_hashMax;++theHash){
    const CaloCell * theCell = theCont->findCell(IdentifierHash(theHash)) ;
    CaloCell *nCell;
    if(!theCell) {
      Identifier cellid = m_theCaloCCIDM->cell_id(theHash);
      HWIdentifier chid = m_onlineHelper->channel_Id(m_theCaloCCIDM->calo_cell_hash(cellid));
      if(!chid.get_identifier32().get_compact()) continue;
      const CaloDetDescrElement* cDDE = caloMgr->get_element(cellid);
      if(!cDDE) continue;
      if(cDDE->is_lar_hec()) {
	nCell = new CaloCell(cDDE,0.,0.,0.,CaloGain::LARMEDIUMGAIN);
      }
      else {
	nCell = new CaloCell(cDDE,0.,0.,0.,CaloGain::LARHIGHGAIN);
      }
      ATH_MSG_VERBOSE ( "Adding 0. energy cell: " << m_theCaloCCIDM->cell_id(theHash));
      theCont->push_back(nCell);
      ++added;
    } 
  }
  
  theCont->order();
  
  return returnSc ;
}









/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "TBRec/TBCellContainerFillerTool.h"

#include "GaudiKernel/Service.h"
#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/Property.h"
#include "GaudiKernel/ListItem.h"

#include "StoreGate/StoreGateSvc.h"

#include "CaloEvent/CaloCellContainer.h"
#include "CaloIdentifier/CaloCell_ID.h"
#include "CaloDetDescr/CaloDetDescrManager.h"
#include "CaloIdentifier/CaloCell_ID.h"
#include "LArTools/LArCablingService.h"
#include "LArIdentifier/LArOnlineID.h"


TBCellContainerFillerTool::TBCellContainerFillerTool(
			     const std::string& type, 
			     const std::string& name, 
			     const IInterface* parent)
  :AlgTool(type, name, parent) ,
  m_storeGate(0),m_detStore(0), m_log(0), m_theCaloDDM(0), m_theCaloCCIDM(0), m_onlineHelper(0)
{ 
  declareInterface<ICaloCellMakerTool>(this); 
}




StatusCode TBCellContainerFillerTool::initialize()
{
  m_log = new MsgStream(msgSvc(),name());
 
  // Cache pointers:

  m_theCaloDDM = CaloDetDescrManager::instance() ;
  m_theCaloCCIDM   = m_theCaloDDM->getCaloCell_ID() ;

  if(!m_theCaloDDM || !m_theCaloCCIDM) return StatusCode::FAILURE;

  m_hashMax = m_theCaloCCIDM->calo_cell_hash_max();

  StatusCode sc = service("StoreGateSvc", m_storeGate);
  if (sc.isFailure()) {
    *m_log << MSG::ERROR
	<< "Unable to retrieve pointer to StoreGate Service"
	<< endreq;
  }

  sc= service("DetectorStore",m_detStore);
  if(sc.isFailure()) {
    *m_log << MSG::ERROR << "DetectorStore service not found" << endreq;
    return StatusCode::FAILURE;
  }
  
  sc = m_cablingService.retrieve();
  if (sc.isFailure()){
    *m_log << MSG::ERROR << "cannot allocate LArCablingService" << endreq;
    return sc;
  }

  sc = m_detStore->retrieve(m_onlineHelper, "LArOnlineID");
  if (sc.isFailure()) {
    *m_log << MSG::ERROR << "Could not get LArOnlineID helper !" << endreq;
    return StatusCode::FAILURE;
  }


  return sc;

}

StatusCode TBCellContainerFillerTool::process(CaloCellContainer * theCont )
{
	
  StatusCode returnSc = StatusCode::SUCCESS ;

  unsigned int index = 0;
  CaloCellContainer::const_iterator itrCell;
  if (m_log->level() <= MSG::VERBOSE) {
      for (itrCell=theCont->begin();itrCell!=theCont->end();++itrCell){
	const CaloDetDescrElement * theDDE=(*itrCell)->caloDDE();
	*m_log << MSG::VERBOSE << index << " " << (*itrCell) 
	    << " hash id " << theDDE->calo_hash()
	    << " eta " << theDDE->eta()
	    << " phi " << theDDE->phi()
	    << "e " << (*itrCell)->e() << endreq ;
	++index;
      }
  }	


  // fill holes with 0 energy cells
  
  unsigned int added = 0;
  *m_log << MSG::DEBUG << " Now check all cells give meaningfull answer " << endreq ;
  *m_log << MSG::DEBUG << " Size of original container: "<<theCont->size() << endreq ;
  for (unsigned int theHash=0;theHash<m_hashMax;++theHash){
    const CaloCell * theCell = theCont->findCell(IdentifierHash(theHash)) ;
    CaloCell *nCell;
    if(!theCell) {
       Identifier cellid = m_theCaloCCIDM->cell_id(theHash);
       HWIdentifier chid = m_onlineHelper->channel_Id(m_theCaloCCIDM->calo_cell_hash(cellid));
       if(!chid.get_identifier32().get_compact()) continue;
//       if(m_cablingService->isOnlineConnected(chid)) {
          const CaloDetDescrElement* cDDE = m_theCaloDDM->get_element(cellid);
          if(!cDDE) { continue; }
          if(cDDE->is_lar_hec())
             nCell = new CaloCell(cDDE,0.,0.,0.,CaloGain::LARMEDIUMGAIN);
          else
             nCell = new CaloCell(cDDE,0.,0.,0.,CaloGain::LARHIGHGAIN);
          *m_log << MSG::VERBOSE << "Adding 0. energy cell: " << m_theCaloCCIDM->cell_id(theHash)<<endreq;
          theCont->push_back(nCell);
          ++added;
//       }
    } 
  }

//  theCont->updateCaloIterators();
  theCont->order();
  
  return returnSc ;
}









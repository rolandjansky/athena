/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "CaloCondPhysAlgs/CaloFillCellPositionShift.h"
#include "GaudiKernel/Property.h"
#include "GaudiKernel/MsgStream.h"

#include "CaloIdentifier/CaloIdManager.h"
#include "CaloDetDescr/CaloDetDescrManager.h"
#include "CaloDetDescr/CaloDetDescrElement.h"
#include "CaloIdentifier/CaloCell_ID.h"
#include "CaloConditions/CaloCellPositionShift.h"
#include "CaloGeoHelpers/CaloSampling.h"

//Constructor
CaloFillCellPositionShift::CaloFillCellPositionShift(const std::string& name, ISvcLocator* pSvcLocator):
Algorithm(name,pSvcLocator),detStore(NULL),m_calo_id(NULL),m_cellPos(NULL),m_key("LArCellPositionShift")
{
  declareProperty("outputKey",m_key,"Key for output CaloCellPositionShift");
}

//__________________________________________________________________________
//Destructor
CaloFillCellPositionShift::~CaloFillCellPositionShift()
{
  MsgStream log( messageService(), name() ) ;
  log << MSG::DEBUG << "CaloFillCellPositionShift destructor called" << endreq;
}
//__________________________________________________________________________
StatusCode CaloFillCellPositionShift::initialize()
{

  MsgStream log( messageService(), name() );
  log << MSG::DEBUG <<"CaloFillCellPositionShift initialize()" << endreq;

  StatusCode sc = service ( "DetectorStore" , detStore ) ;
  //retrieve ID helpers 
  sc = detStore->retrieve( m_caloIdMgr );
  if (sc.isFailure()) {
   log << MSG::ERROR << "Unable to retrieve CaloIdMgr in CaloFillCellPositionShift " << endreq;
   return StatusCode::FAILURE;
  }
  m_calo_id      = m_caloIdMgr->getCaloCell_ID();

//  retrieve CaloDetDescrMgr
    sc = detStore->retrieve(m_calodetdescrmgr);
    if (sc.isFailure()) {
       log << MSG::ERROR << "Unable to retrieve CaloDetDescrMgr in LArHitEMap " << endreq;
       return StatusCode::FAILURE;
    }

  log << MSG::INFO << " end of CaloFillCellPositionShift::initialize " << endreq;
  return StatusCode::SUCCESS; 

}
//__________________________________________________________________________
StatusCode CaloFillCellPositionShift::execute()
{
  MsgStream log( messageService(), name() );
  log << MSG::DEBUG <<"CaloFillCellPositionShift execute()" << endreq;
  return StatusCode::SUCCESS; 
}

//__________________________________________________________________________
StatusCode CaloFillCellPositionShift::stop()
{
//.............................................

  MsgStream log( messageService(), name() );

  //int ncell=m_calo_id->calo_cell_hash_max();
  IdentifierHash emmin,emmax,fcalmin,fcalmax;
  m_calo_id->calo_cell_hash_range(CaloCell_ID::LAREM,emmin,emmax);
  m_calo_id->calo_cell_hash_range(CaloCell_ID::LARFCAL,fcalmin,fcalmax);
  int ncell=fcalmax-emmin;
  log << MSG::INFO << " Number of cells in LAr calo " << emmin << " " << fcalmax << " " << ncell << endreq;

  m_cellPos = new CaloRec::CaloCellPositionShift(ncell);

  StatusCode sc = detStore->record(m_cellPos,m_key);
  if (sc.isFailure()) {
    log << MSG::ERROR << " cannot record CaloCellPositionShift in storegate " << endreq;
    delete m_cellPos;
    return sc;
  }

  log << MSG::INFO << " start loop over Calo cells " << ncell << endreq;
  for (int i=0;i<ncell;i++) {
      IdentifierHash idHash=i+emmin;
      Identifier id=m_calo_id->cell_id(idHash);
      const CaloDetDescrElement* calodde = m_calodetdescrmgr->get_element(id);

      //float eta = calodde->eta();
      float phi = calodde->phi();
      int layer = m_calo_id->calo_sample(id);
     
      float dx=0.;
      float dy=0.;
      float dz=0.;
      if (layer==CaloSampling::EMB2) {
        dy = -1.*cos(phi)*cos(phi);
        dx =  1.*cos(phi)*sin(phi);
        dz = 0.;
      }

      m_cellPos->set_deltaX(i,dx);
      m_cellPos->set_deltaY(i,dy);
      m_cellPos->set_deltaZ(i,dz);

  }        // loop over cells

  return StatusCode::SUCCESS;
 }

StatusCode CaloFillCellPositionShift::finalize()
{
  return StatusCode::SUCCESS;
}

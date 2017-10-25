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
AthAlgorithm(name,pSvcLocator),m_calo_id(NULL),m_cellPos(NULL),m_key("LArCellPositionShift")
{
  declareProperty("outputKey",m_key,"Key for output CaloCellPositionShift");
}

//__________________________________________________________________________
//Destructor
CaloFillCellPositionShift::~CaloFillCellPositionShift()
{
  ATH_MSG_DEBUG( "CaloFillCellPositionShift destructor called" );
}
//__________________________________________________________________________
StatusCode CaloFillCellPositionShift::initialize()
{
  ATH_MSG_DEBUG ("CaloFillCellPositionShift initialize()" );

  ATH_CHECK( detStore()->retrieve( m_caloIdMgr ) );
  m_calo_id      = m_caloIdMgr->getCaloCell_ID();

  ATH_CHECK( detStore()->retrieve(m_calodetdescrmgr) );
  ATH_MSG_INFO ( " end of CaloFillCellPositionShift::initialize " );
  return StatusCode::SUCCESS; 

}
//__________________________________________________________________________
StatusCode CaloFillCellPositionShift::execute()
{
  ATH_MSG_DEBUG ("CaloFillCellPositionShift execute()" );
  return StatusCode::SUCCESS; 
}

//__________________________________________________________________________
StatusCode CaloFillCellPositionShift::stop()
{
  //int ncell=m_calo_id->calo_cell_hash_max();
  IdentifierHash emmin,emmax,fcalmin,fcalmax;
  m_calo_id->calo_cell_hash_range(CaloCell_ID::LAREM,emmin,emmax);
  m_calo_id->calo_cell_hash_range(CaloCell_ID::LARFCAL,fcalmin,fcalmax);
  int ncell=fcalmax-emmin;
  ATH_MSG_INFO ( " Number of cells in LAr calo " << emmin << " " << fcalmax << " " << ncell );

  m_cellPos = new CaloRec::CaloCellPositionShift(ncell);

  ATH_CHECK( detStore()->record(m_cellPos,m_key) );

  ATH_MSG_INFO ( " start loop over Calo cells " << ncell );
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

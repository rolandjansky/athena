/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "CaloCondPhysAlgs/CaloCellPosition2Ntuple.h"
#include "CaloIdentifier/CaloIdManager.h"
#include "CaloDetDescr/CaloDetDescrManager.h"
#include "CaloDetDescr/CaloDetDescrElement.h"
#include "CaloIdentifier/CaloCell_ID.h"
#include "CaloConditions/CaloCellPositionShift.h"
#include "Identifier/Identifier.h"


//Constructor
CaloCellPosition2Ntuple::CaloCellPosition2Ntuple(const std::string& name, ISvcLocator* pSvcLocator):
  AthAlgorithm(name,pSvcLocator),
  m_thistSvc(NULL),
  m_calo_id(NULL),
  m_key("LArCellPositionShift"),
  m_Hash(0),
  m_OffId(0),
  m_eta(0),
  m_phi(0),
  m_layer(0),
  m_dx(0),
  m_dy(0),
  m_dz(0),
  m_volume(0), 
  m_tree(NULL)
{
  declareProperty("inputKey",m_key,"Key for CaloCellPositionShift");
}

//__________________________________________________________________________
//Destructor
CaloCellPosition2Ntuple::~CaloCellPosition2Ntuple()
{
  ATH_MSG_DEBUG( "CaloCellPosition2Ntuple destructor called"  );
}
//__________________________________________________________________________
StatusCode CaloCellPosition2Ntuple::initialize()
{
  ATH_MSG_DEBUG ("CaloCellPosition2Ntuple initialize()" );
  ATH_CHECK( service("THistSvc",m_thistSvc) );

  ATH_CHECK( detStore()->retrieve( m_caloIdMgr ) );
  m_calo_id      = m_caloIdMgr->getCaloCell_ID();

  ATH_CHECK( detStore()->retrieve(m_calodetdescrmgr) );
  ATH_CHECK( detStore()->regHandle(m_cellPos,m_key) );

  m_tree = new TTree("mytree","Calo Noise ntuple");
  m_tree->Branch("iHash",&m_Hash,"iHash/I");
  m_tree->Branch("iOffId",&m_OffId,"iOffId/I");
  m_tree->Branch("eta",&m_eta,"eta/F");
  m_tree->Branch("phi",&m_phi,"phi/F");
  m_tree->Branch("layer",&m_layer,"layer/I");
  m_tree->Branch("dx",&m_dx,"DeltaX/F");
  m_tree->Branch("dy",&m_dy,"DeltaY/F");
  m_tree->Branch("dz",&m_dz,"DeltaZ/F");
  m_tree->Branch("volume",&m_volume,"volume/F");

  ATH_CHECK( m_thistSvc->regTree("/file1/cellpos/mytree",m_tree) );

  ATH_MSG_INFO ( " end of CaloCellPosition2Ntuple::initialize " );
  return StatusCode::SUCCESS; 

}

//__________________________________________________________________________
StatusCode CaloCellPosition2Ntuple::execute()
{
  ATH_MSG_DEBUG ("CaloCellPosition2Ntuple execute()" );
  return StatusCode::SUCCESS; 
}

//__________________________________________________________________________
StatusCode CaloCellPosition2Ntuple::stop()
{
  IdentifierHash emmin,emmax,fcalmin,fcalmax;
  m_calo_id->calo_cell_hash_range(CaloCell_ID::LAREM,emmin,emmax);
  m_calo_id->calo_cell_hash_range(CaloCell_ID::LARFCAL,fcalmin,fcalmax);
  int ncell=fcalmax-emmin;

  int nread = (int)(m_cellPos->size());

  if (nread > ncell) {
    ATH_MSG_WARNING ( " CaloCellPosition size different from max lar hash " << m_cellPos->size() << " " << ncell );
    return StatusCode::SUCCESS;
  }
  ATH_MSG_INFO ( " start loop over Calo cells " << ncell );
  for (int i=0;i<ncell;i++) {
       IdentifierHash idHash=i;
       Identifier id=m_calo_id->cell_id(idHash);
       const CaloDetDescrElement* calodde = m_calodetdescrmgr->get_element(id);

       m_Hash =  i;
       m_OffId = (int)(id.get_identifier32().get_compact());
 
       m_eta = calodde->eta();
       m_phi = calodde->phi();
       m_layer = m_calo_id->calo_sample(id);

       m_dx = m_cellPos->deltaX(i);
       m_dy = m_cellPos->deltaY(i);
       m_dz = m_cellPos->deltaZ(i);
       m_volume = calodde->volume();

       m_tree->Fill();

  }        // loop over cells

  return StatusCode::SUCCESS;
 }

StatusCode CaloCellPosition2Ntuple::finalize()
{
  return StatusCode::SUCCESS;
}

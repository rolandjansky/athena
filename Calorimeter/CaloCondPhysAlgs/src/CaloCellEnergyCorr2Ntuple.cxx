/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#include "CaloCellEnergyCorr2Ntuple.h"
#include "CaloIdentifier/CaloGain.h"
#include "CaloEvent/CaloCell.h"
#include "Identifier/Identifier.h"
#include "xAODEventInfo/EventInfo.h"
#include "AthenaPoolUtilities/AthenaAttributeList.h"
#include "CaloCondBlobObjs/CaloCondBlobFlt.h"

#include "CoolKernel/types.h"
#include "CoolKernel/Record.h"
#include "CoralBase/AttributeListSpecification.h"


//Constructor
CaloCellEnergyCorr2Ntuple::CaloCellEnergyCorr2Ntuple(const std::string& name, ISvcLocator* pSvcLocator):
  AthAlgorithm(name,pSvcLocator),
  m_thistSvc(nullptr),
  m_calo_id(nullptr),
  m_key("EnergyCorr"), 
  m_FolderName("/LAR/CellCorrOfl/EnergyCorr"), 
  m_Hash(0),
  m_OffId(0),
  m_eta(0),
  m_phi(0),
  m_layer(0),
  m_detector(0),
  m_corr(0),
  m_tree(nullptr),
  m_runNumber(0),
  m_lumiBlock(0)
{
   declareProperty("InputKey",m_key,"Key for EnergyCorr");
   declareProperty("FolderName",m_FolderName);
}

//__________________________________________________________________________
//Destructor
CaloCellEnergyCorr2Ntuple::~CaloCellEnergyCorr2Ntuple()
{
  ATH_MSG_DEBUG ( "CaloCellEnergyCorr2Ntuple destructor called" );
}
//__________________________________________________________________________
StatusCode CaloCellEnergyCorr2Ntuple::initialize()
{
  ATH_MSG_DEBUG ("CaloCellEnergyCorr2Ntuple initialize()" );

  ATH_CHECK( service("THistSvc",m_thistSvc) );

  const CaloIdManager* mgr = nullptr;
  ATH_CHECK( detStore()->retrieve( mgr ) );
  m_calo_id      = mgr->getCaloCell_ID();

  ATH_CHECK( detStore()->regHandle(m_AttrListColl,m_key) );

  m_tree = new TTree("mytree","Calo Noise ntuple");
  m_tree->Branch("iHash",&m_Hash,"iHash/I");
  m_tree->Branch("iOffId",&m_OffId,"iOffId/I");
  m_tree->Branch("eta",&m_eta,"eta/F");
  m_tree->Branch("phi",&m_phi,"phi/F");
  m_tree->Branch("layer",&m_layer,"layer/I");
  m_tree->Branch("detector",&m_detector,"detector/I");
  m_tree->Branch("ECorr",&m_corr,"ECorr/F");
  ATH_CHECK( m_thistSvc->regTree("/file1/mytree",m_tree) );

  ATH_MSG_INFO ( " end of CaloCellEnergyCorr2Ntuple::initialize " );
  return StatusCode::SUCCESS; 

}
//__________________________________________________________________________
StatusCode CaloCellEnergyCorr2Ntuple::execute()
{
  ATH_MSG_DEBUG ("CaloCellEnergyCorr2Ntuple execute()" );
  
  const xAOD::EventInfo* eventInfo = nullptr;
  if (evtStore()->retrieve(eventInfo).isFailure()) {
    ATH_MSG_WARNING ( " Cannot access to event info " );
    return StatusCode::SUCCESS;
  }
  m_lumiBlock = eventInfo->lumiBlock();
  m_runNumber = eventInfo->runNumber();

  return StatusCode::SUCCESS; 
}

//__________________________________________________________________________
StatusCode CaloCellEnergyCorr2Ntuple::stop()
{
  ATH_MSG_INFO ( "  Run Number, lumiblock " << m_runNumber << " " << m_lumiBlock );

  const coral::Blob& blob=(*m_AttrListColl)["CaloCondBlob16M"].data<coral::Blob>();
  std::unique_ptr<const CaloCondBlobFlt> flt (CaloCondBlobFlt::getInstance(blob));

  int nobj=flt->getNObjs();
  int nchan=flt->getNChans();
  ATH_MSG_INFO ( "NObjs: "<<nobj<<" nChans: "<<nchan<<" nGains: "<<flt->getNGains() );

  const CaloDetDescrManager* calodetdescrmgr = nullptr;
  ATH_CHECK( detStore()->retrieve(calodetdescrmgr) );

  int ncell=m_calo_id->calo_cell_hash_max();
  ATH_MSG_INFO ( " start loop over Calo cells " << ncell );
  for (int i=0;i<ncell;i++) {
       IdentifierHash idHash=i;
       Identifier id=m_calo_id->cell_id(idHash);
       const CaloDetDescrElement* calodde = calodetdescrmgr->get_element(id);

       m_Hash =  i;
       m_OffId = (int)(id.get_identifier32().get_compact());
 
       m_eta = calodde->eta_raw();
       m_phi = calodde->phi_raw();
       m_layer = m_calo_id->calo_sample(id);
       m_detector=-1;
       if (m_calo_id->is_em(id)) {
             if(m_calo_id->is_em_barrel(id)) {
                   m_detector=0;
             } else if(m_calo_id->is_em_endcap_outer(id)) {
                   m_detector=1;
             } else {
                   m_detector=2;
             }
       } else if (m_calo_id->is_hec(id)) {
             m_detector  = 3;
       } else if (m_calo_id->is_fcal(id)) {
             m_detector = 4;
       }

       if(m_Hash >= nchan) break;
       m_corr = flt->getData(m_Hash,0,0);


       m_tree->Fill();

  }        // loop over cells

  return StatusCode::SUCCESS;
 }

StatusCode CaloCellEnergyCorr2Ntuple::finalize()
{
 return StatusCode::SUCCESS;
}

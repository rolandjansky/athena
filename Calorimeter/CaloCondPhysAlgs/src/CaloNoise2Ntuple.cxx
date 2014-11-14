/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "CaloCondPhysAlgs/CaloNoise2Ntuple.h"
#include "GaudiKernel/Property.h"
#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/NTuple.h"
#include "GaudiKernel/ListItem.h"
#include "GaudiKernel/IToolSvc.h"
#include "CaloIdentifier/CaloGain.h"
#include "CaloEvent/CaloCell.h"
#include "Identifier/Identifier.h"
#include "xAODEventInfo/EventInfo.h"


//Constructor
CaloNoise2Ntuple::CaloNoise2Ntuple(const std::string& name, ISvcLocator* pSvcLocator):
  Algorithm(name,pSvcLocator),
  m_sgSvc(NULL),
  m_detStore(NULL),
  m_thistSvc(NULL),
  m_calo_id(NULL),
  m_noiseTool("CaloNoiseToolDB/calonoisetooldb"),
  m_averageTool(""),
  m_iCool(0),
  m_SubHash(0),
  m_Hash(0),
  m_OffId(0),
  m_eta(0),
  m_phi(0),
  m_layer(0),
  m_Gain(0),
  m_noise(0),
  m_elecNoise(0),
  m_pileupNoise(0),
  m_average(0),
  m_tree(NULL),
  m_runNumber(0),
  m_lumiBlock(0)
{
  declareProperty("noiseTool",m_noiseTool,"noise tool");
  declareProperty("averageTool",m_averageTool,"average tool");
}

//__________________________________________________________________________
//Destructor
CaloNoise2Ntuple::~CaloNoise2Ntuple()
{
  MsgStream log( messageService(), name() ) ;
  log << MSG::DEBUG << "CaloNoise2Ntuple destructor called" << endreq;
}
//__________________________________________________________________________
StatusCode CaloNoise2Ntuple::initialize()
{

  MsgStream log( messageService(), name() );
  log << MSG::DEBUG <<"CaloNoise2Ntuple initialize()" << endreq;

// Get the StoreGateSvc
  if (service("StoreGateSvc", m_sgSvc).isFailure()) {
    log << MSG::ERROR << "No StoreGate!!!!!!!" << endreq;
    return StatusCode::FAILURE;
  }

// get THistSvc
  if (service("THistSvc",m_thistSvc).isFailure()) {
    log << MSG::ERROR << " cannot find THistSvc " << endreq;
    return StatusCode::FAILURE;
  }


  StatusCode sc = service ( "DetectorStore" , m_detStore ) ;
  //retrieve ID helpers 
  sc = m_detStore->retrieve( m_caloIdMgr );
  if (sc.isFailure()) {
   log << MSG::ERROR << "Unable to retrieve CaloIdMgr in LArHitEMap " << endreq;
   return StatusCode::FAILURE;
  }
  m_calo_id      = m_caloIdMgr->getCaloCell_ID();

//  retrieve CaloDetDescrMgr
    sc = m_detStore->retrieve(m_calodetdescrmgr);
    if (sc.isFailure()) {
       log << MSG::ERROR << "Unable to retrieve CaloDetDescrMgr in LArHitEMap " << endreq;
       return StatusCode::FAILURE;
    }


  if (m_noiseTool.retrieve().isFailure()) {
    log << MSG::ERROR << "Unable to find tool for calonoisetool "  << endreq;
   return StatusCode::FAILURE;
  }

  if (!m_averageTool.empty()) {
    if (m_averageTool.retrieve().isFailure()) {
      log << MSG::ERROR << "Unable to find tool for average " << endreq;
      return StatusCode::FAILURE;
    }
  }


  m_tree = new TTree("mytree","Calo Noise ntuple");
  m_tree->Branch("iCool",&m_iCool,"iCool/I");
  m_tree->Branch("iSubHash",&m_SubHash,"iSubHash/I");
  m_tree->Branch("iHash",&m_Hash,"iHash/I");
  m_tree->Branch("iOffId",&m_OffId,"iOffId/I");
  m_tree->Branch("eta",&m_eta,"eta/F");
  m_tree->Branch("phi",&m_phi,"phi/F");
  m_tree->Branch("layer",&m_layer,"layer/I");
  m_tree->Branch("iGain",&m_Gain,"iGain/I");
  m_tree->Branch("Noise",&m_noise,"Noise/F");
  m_tree->Branch("ElecNoise",&m_elecNoise,"ElecNoise/F");
  m_tree->Branch("PileupNoise",&m_pileupNoise,"PileupNoise/F");
  m_tree->Branch("Average",&m_average,"Average/F");

  if( m_thistSvc->regTree("/file1/calonoise/mytree",m_tree).isFailure()) {
       log << MSG::ERROR << " cannot register ntuple " << endreq; 
       return StatusCode::FAILURE;
  }


  log << MSG::INFO << " end of CaloNoise2Ntuple::initialize " << endreq;
  return StatusCode::SUCCESS; 

}
//__________________________________________________________________________
StatusCode CaloNoise2Ntuple::execute()
{
  MsgStream log( messageService(), name() );
  log << MSG::DEBUG <<"CaloNoise2Ntuple execute()" << endreq;
  
  const xAOD::EventInfo* eventInfo;
  if (m_sgSvc->retrieve(eventInfo).isFailure()) {
    log << MSG::WARNING << " Cannot access to event info " << endreq;
    return StatusCode::SUCCESS;
  }
  m_lumiBlock = eventInfo->lumiBlock();
  m_runNumber = eventInfo->runNumber();

  return StatusCode::SUCCESS; 
}

//__________________________________________________________________________
StatusCode CaloNoise2Ntuple::stop()
{
//.............................................

  MsgStream log( messageService(), name() );

  log << MSG::INFO << "  Run Number, lumiblock " << m_runNumber << " " << m_lumiBlock << endreq;

  int ncell=m_calo_id->calo_cell_hash_max();
  log << MSG::INFO << " start loop over Calo cells " << ncell << endreq;
  for (int i=0;i<ncell;i++) {
       IdentifierHash idHash=i;
       Identifier id=m_calo_id->cell_id(idHash);
       const CaloDetDescrElement* calodde = m_calodetdescrmgr->get_element(id);
       int subCalo;
       IdentifierHash idSubHash = m_calo_id->subcalo_cell_hash (idHash, subCalo);

       int iCool=-1;
       if (m_calo_id->is_em(id)) {    // EM calo
         if (m_calo_id->is_em_barrel(id)) {
            if (m_calo_id->pos_neg(id) > 0 )
               iCool=2;
            else
               iCool=1;
         }
         if (m_calo_id->is_em_endcap(id)) {
            if (m_calo_id->pos_neg(id) > 0 )
               iCool=3;
            else
               iCool=0;
         }
   
       }
       if (m_calo_id->is_hec(id)) {   // HEC
        iCool=16;
       }
       if (m_calo_id->is_fcal(id)) {   // Fcal
        iCool=32;
       }
       if (m_calo_id->is_tile(id)) {   // Tile
        iCool=48;
       }
       int ii = (int) (idSubHash);

       m_iCool = iCool;
       m_SubHash = ii;
       m_Hash =  i;
       m_OffId = (int)(id.get_identifier32().get_compact());
 
       m_eta = calodde->eta_raw();
       m_phi = calodde->phi_raw();
       m_layer = m_calo_id->calo_sample(id);

       int ngain;
       if (subCalo<3) ngain=3;    
       else ngain=4;
      
       for (int igain=0;igain<ngain;igain++) {

          CaloGain::CaloGain gain=CaloGain::LARHIGHGAIN;
          if (subCalo<3) {
           if (igain==0) gain=CaloGain::LARHIGHGAIN;
           if (igain==1) gain=CaloGain::LARMEDIUMGAIN;
           if (igain==2) gain=CaloGain::LARLOWGAIN;
          } 
          else {
           if (igain==0) gain=CaloGain::TILELOWLOW;
           if (igain==1) gain=CaloGain::TILELOWHIGH;
           if (igain==2) gain=CaloGain::TILEHIGHLOW;
           if (igain==3) gain=CaloGain::TILEHIGHHIGH;
          }
          m_Gain = igain;

          m_noise = m_noiseTool->totalNoiseRMS(calodde,gain);
          m_elecNoise = m_noiseTool->elecNoiseRMS(calodde,gain,-1);
          m_pileupNoise = m_noiseTool->pileupNoiseRMS(calodde);

          if (!m_averageTool.empty()) {
              m_average = m_averageTool->average(calodde,gain);
          }
          else {
              m_average=0.;
          }

          m_tree->Fill();

       }   // loop over gains

  }        // loop over cells

  return StatusCode::SUCCESS;
 }

StatusCode CaloNoise2Ntuple::finalize()
{
 return StatusCode::SUCCESS;
}

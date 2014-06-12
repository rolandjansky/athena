/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "CaloCondPhysAlgs/CaloRescaleNoise.h"
#include "GaudiKernel/Property.h"
#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/NTuple.h"
#include "GaudiKernel/ListItem.h"
#include "GaudiKernel/IToolSvc.h"
#include "CaloIdentifier/CaloGain.h"
#include "CaloEvent/CaloCell.h"
#include "Identifier/Identifier.h"
#include "EventInfo/EventInfo.h"
#include "EventInfo/EventID.h"


//Constructor
CaloRescaleNoise::CaloRescaleNoise(const std::string& name, ISvcLocator* pSvcLocator):
  Algorithm(name,pSvcLocator),
  m_sgSvc(0),
  m_detStore(0),
  m_thistSvc(0),
  m_calo_id(0),
  m_noiseTool("CaloNoiseToolDB/calonoisetooldb"),
  m_hvCorrTool("LArHVCorrTool"),
  m_keyHVScaleCorr("LArHVScaleCorr"),
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
  m_pileupNoise (0),
  m_elecNoiseRescaled(0),
  m_tree(0)
{
  declareProperty("noiseTool",m_noiseTool,"noise tool");
  declareProperty("HVCorrTool",m_hvCorrTool);
  declareProperty("keyHVScaleCorr",m_keyHVScaleCorr);
}

//__________________________________________________________________________
//Destructor
CaloRescaleNoise::~CaloRescaleNoise()
{
  MsgStream log( messageService(), name() ) ;
  log << MSG::DEBUG << "CaloRescaleNoise destructor called" << endreq;
}
//__________________________________________________________________________
StatusCode CaloRescaleNoise::initialize()
{

  MsgStream log( messageService(), name() );
  log << MSG::DEBUG <<"CaloRescaleNoise initialize()" << endreq;

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

  sc = m_hvCorrTool.retrieve();
  if (sc.isFailure()) {
      log << MSG::ERROR << "Unable to find tool for LArHVCorrTool" << endreq; 
      return StatusCode::FAILURE;
  }


  sc = m_detStore->regHandle(m_dd_HVScaleCorr,m_keyHVScaleCorr);
  if (sc.isFailure()) {
      log << MSG::ERROR << "Unable to register handle to HVScaleCorr " << endreq;
      return StatusCode::FAILURE;
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
  m_tree->Branch("ElecNoiseRescaled",&m_elecNoiseRescaled,"ElecNoiseRescaled/F");

  if( m_thistSvc->regTree("/file1/calonoise/mytree",m_tree).isFailure()) {
       log << MSG::ERROR << " cannot register ntuple " << endreq; 
       return StatusCode::FAILURE;
  }


  log << MSG::INFO << " end of CaloRescaleNoise::initialize " << endreq;
  return StatusCode::SUCCESS; 

}
//__________________________________________________________________________
StatusCode CaloRescaleNoise::execute()
{
  MsgStream log( messageService(), name() );
  log << MSG::DEBUG <<"CaloRescaleNoise execute()" << endreq;
  
  return StatusCode::SUCCESS; 
}

//__________________________________________________________________________
StatusCode CaloRescaleNoise::stop()
{
//.............................................

  MsgStream log( messageService(), name() );

   FILE* fp = fopen("calonoise.txt","w");

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

       float hvcorr=1.;
       float hvonline=1.;

       if (iCool<48) {
          hvcorr = m_hvCorrTool->Scale(id);
          hvonline = m_dd_HVScaleCorr->HVScaleCorr(id);
       }

      
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

          if (hvonline>0.) m_elecNoiseRescaled = m_elecNoise*hvcorr/hvonline;
          else m_elecNoiseRescaled = m_elecNoise ;

          if (iCool<48) fprintf(fp,"%5d %5d %5d %8.3f %8.3f\n",iCool,ii,gain,m_elecNoiseRescaled,m_pileupNoise);

          if (iCool<48 && m_elecNoise>0. && std::fabs(m_elecNoiseRescaled/m_elecNoise-1.)>0.05) {
              log << MSG::WARNING << " DifferentNoise  cell " << m_calo_id->show_to_string(id) <<
                 " layer/eta/phi " << m_layer << " " << m_eta << " " << m_phi << " OldNoise,NewNoise " <<
                m_elecNoise << " " << m_elecNoiseRescaled << endreq;
          }
          m_tree->Fill();

       }   // loop over gains

  }        // loop over cells

  fclose(fp);
  return StatusCode::SUCCESS;
 }

 StatusCode CaloRescaleNoise::finalize()
{
  return StatusCode::SUCCESS;
}

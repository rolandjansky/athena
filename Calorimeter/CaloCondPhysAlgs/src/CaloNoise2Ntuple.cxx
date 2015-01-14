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
  AthAlgorithm(name,pSvcLocator),
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
  ATH_MSG_DEBUG ( "CaloNoise2Ntuple destructor called" );
}
//__________________________________________________________________________
StatusCode CaloNoise2Ntuple::initialize()
{
  ATH_MSG_DEBUG ("CaloNoise2Ntuple initialize()" );

  ATH_CHECK( service("THistSvc",m_thistSvc) );

  ATH_CHECK( detStore()->retrieve( m_caloIdMgr ) );
  m_calo_id      = m_caloIdMgr->getCaloCell_ID();

  ATH_CHECK( detStore()->retrieve(m_calodetdescrmgr) );

  ATH_CHECK( m_noiseTool.retrieve() );

  if (!m_averageTool.empty())
    ATH_CHECK( m_averageTool.retrieve() );

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
  ATH_CHECK( m_thistSvc->regTree("/file1/calonoise/mytree",m_tree) );

  ATH_MSG_INFO ( " end of CaloNoise2Ntuple::initialize " );
  return StatusCode::SUCCESS; 

}
//__________________________________________________________________________
StatusCode CaloNoise2Ntuple::execute()
{
  ATH_MSG_DEBUG ("CaloNoise2Ntuple execute()" );
  
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
StatusCode CaloNoise2Ntuple::stop()
{
  ATH_MSG_INFO ( "  Run Number, lumiblock " << m_runNumber << " " << m_lumiBlock );

  int ncell=m_calo_id->calo_cell_hash_max();
  ATH_MSG_INFO ( " start loop over Calo cells " << ncell );
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

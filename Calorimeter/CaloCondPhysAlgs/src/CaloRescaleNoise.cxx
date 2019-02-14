/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#include "CaloRescaleNoise.h"
#include "CaloIdentifier/CaloGain.h"
#include "CaloEvent/CaloCell.h"
#include "Identifier/Identifier.h"


//Constructor
CaloRescaleNoise::CaloRescaleNoise(const std::string& name, ISvcLocator* pSvcLocator):
  AthAlgorithm(name,pSvcLocator),
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
  declareProperty("absScaling",m_absScaling=false);
}

//__________________________________________________________________________
//Destructor
CaloRescaleNoise::~CaloRescaleNoise()
{
  ATH_MSG_DEBUG ( "CaloRescaleNoise destructor called" );
}
//__________________________________________________________________________
StatusCode CaloRescaleNoise::initialize()
{
  ATH_MSG_DEBUG ("CaloRescaleNoise initialize()" );
  ATH_CHECK(service("THistSvc",m_thistSvc) );

  const CaloIdManager* mgr = nullptr;
  ATH_CHECK( detStore()->retrieve( mgr ) );
  m_calo_id      = mgr->getCaloCell_ID();

  ATH_CHECK( m_noiseTool.retrieve() );
  ATH_CHECK( m_hvCorrTool.retrieve() );
  ATH_CHECK( m_cablingKey.initialize());
  ATH_CHECK( detStore()->regHandle(m_dd_HVScaleCorr,m_keyHVScaleCorr) );

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

  ATH_CHECK( m_thistSvc->regTree("/file1/calonoise/mytree",m_tree) );
  ATH_MSG_INFO ( " end of CaloRescaleNoise::initialize " );
  return StatusCode::SUCCESS; 
}
//__________________________________________________________________________
StatusCode CaloRescaleNoise::execute()
{
  ATH_MSG_DEBUG ("CaloRescaleNoise execute()" );
  return StatusCode::SUCCESS; 
}

//__________________________________________________________________________
StatusCode CaloRescaleNoise::stop()
{
  const CaloDetDescrManager* calodetdescrmgr = nullptr;
  ATH_CHECK( detStore()->retrieve(calodetdescrmgr) );

  SG::ReadCondHandle<LArOnOffIdMapping> cablingHdl{m_cablingKey};
  const LArOnOffIdMapping* cabling{*cablingHdl};
  if(!cabling) {
     ATH_MSG_ERROR( "Do not have cabling mapping from key " << m_cablingKey.key() );
     return StatusCode::FAILURE;
  }

  FILE* fp = fopen("calonoise.txt","w");

  int ncell=m_calo_id->calo_cell_hash_max();
  ATH_MSG_INFO ( " start loop over Calo cells " << ncell );
  for (int i=0;i<ncell;i++) {
       IdentifierHash idHash=i;
       Identifier id=m_calo_id->cell_id(idHash);
       HWIdentifier hwid=cabling->createSignalChannelID(id);
       const CaloDetDescrElement* calodde = calodetdescrmgr->get_element(id);
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
          hvonline = m_dd_HVScaleCorr->HVScaleCorr(hwid);
       }

       const double inv_hvonline = (hvonline != 0) ? 1. / hvonline : 1;

      
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

          if (hvonline>0.) {
             if (m_absScaling) {
                m_elecNoiseRescaled = m_elecNoise*hvonline;
                if (iCool<48 && m_elecNoise>0. && hvonline > 1.) {
                    ATH_MSG_WARNING ( " Abs. scaled Noise cell " << m_Hash  <<
                                      " layer/ieta/iphi " << m_calo_id->sampling(id) << " " << m_calo_id->eta(id) << " " << m_calo_id->phi(id) << " OldNoise,NewNoise " <<
                                         m_elecNoise << " " << m_elecNoiseRescaled );
                }
             } else {
                m_elecNoiseRescaled = m_elecNoise*hvcorr*inv_hvonline;
                if (iCool<48 && m_elecNoise>0. && std::fabs(m_elecNoiseRescaled/m_elecNoise-1.)>0.05) {
                    ATH_MSG_WARNING ( " DifferentNoise  cell " << m_calo_id->show_to_string(id) <<
                                      " layer/eta/phi " << m_layer << " " << m_eta << " " << m_phi << " OldNoise,NewNoise " <<
                                         m_elecNoise << " " << m_elecNoiseRescaled );
                }
             }
          } else {
             m_elecNoiseRescaled = m_elecNoise ;
          }

          if (iCool<48) fprintf(fp,"%5d %5d %5d %8.3f %8.3f\n",iCool,ii,gain,m_elecNoiseRescaled,m_pileupNoise);

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

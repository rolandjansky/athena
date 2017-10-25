/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "CaloCondPhysAlgs/CaloAddCellPedShift.h"
#include "GaudiKernel/Property.h"
#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/NTuple.h"
#include "GaudiKernel/ListItem.h"
#include "GaudiKernel/IToolSvc.h"
#include "CaloIdentifier/CaloGain.h"
#include "CaloEvent/CaloCell.h"
#include "Identifier/Identifier.h"
#include "LArCabling/LArCablingService.h"
//=== AttributeList
#include "CoralBase/Blob.h"
#include "AthenaPoolUtilities/CondAttrListCollection.h"
//=== CaloCondBlod
#include "CaloCondBlobObjs/CaloCondBlobFlt.h"
#include "CaloCondBlobObjs/CaloCondUtils.h"



//Constructor
CaloAddCellPedShift::CaloAddCellPedShift(const std::string& name, ISvcLocator* pSvcLocator):
  AthAlgorithm(name,pSvcLocator),
  m_thistSvc(0),
  m_cablingService("LArCablingService"),
  m_calo_id(0),
  m_onlineID(0),
  m_caloCoolIdTool("CaloCoolIdTool"),
  m_iCool(0),
  m_SubHash(0),
  m_Hash(0),
  m_OffId(0),
  m_eta(0),
  m_phi(0),
  m_layer(0),
  m_Gain(0),
  m_bec(0),
  m_posneg(0),
  m_FT(0),
  m_slot(0),
  m_channel(0),
  m_ped1(0),
  m_ped1corr(0),
  m_ped2(0),
  m_tree(0)

{
   declareProperty("inputFile",m_fname);
   declareProperty("FolderName",m_folderName="/CALO/Pedestal/CellPedestal");
}

//__________________________________________________________________________
CaloAddCellPedShift::~CaloAddCellPedShift()
{
  ATH_MSG_DEBUG( "CaloAddCellPedShift destructor called"  );
}
//__________________________________________________________________________
StatusCode CaloAddCellPedShift::initialize()
{
  ATH_MSG_DEBUG ("CaloAddCellPedShift initialize()" );

  ATH_CHECK( detStore()->retrieve( m_caloIdMgr ) );
  m_calo_id      = m_caloIdMgr->getCaloCell_ID();

  ATH_CHECK( detStore()->retrieve(m_calodetdescrmgr) );

  ATH_CHECK( detStore()->regFcn(&CaloAddCellPedShift::updateMap, this, m_noiseAttrListColl, m_folderName) );
  ATH_MSG_INFO ( " registered a callback for " << m_folderName << " folder " );

  ATH_CHECK( m_caloCoolIdTool.retrieve() );
  ATH_CHECK( m_cablingService.retrieve() );
  ATH_CHECK( detStore()->retrieve(m_onlineID,"LArOnlineID") );
  ATH_CHECK( service("THistSvc",m_thistSvc) );

  m_tree = new TTree("mytree","Calo Ped ntuple");
  m_tree->Branch("iCool",&m_iCool,"iCool/I");
  m_tree->Branch("iSubHash",&m_SubHash,"iSubHash/I");
  m_tree->Branch("iHash",&m_Hash,"iHash/I");
  m_tree->Branch("iOffId",&m_OffId,"iOffId/I");
  m_tree->Branch("eta",&m_eta,"eta/F");
  m_tree->Branch("phi",&m_phi,"phi/F");
  m_tree->Branch("layer",&m_layer,"layer/I");
  m_tree->Branch("iGain",&m_Gain,"iGain/I");
  m_tree->Branch("bec",&m_bec,"bec/I");
  m_tree->Branch("posneg",&m_posneg,"posneg/I");
  m_tree->Branch("FT",&m_FT,"FT/I");
  m_tree->Branch("slot",&m_slot,"slot/I");
  m_tree->Branch("channel",&m_channel,"channel/I");
  m_tree->Branch("Pedestal",&m_ped1,"Pedestal/F");
  m_tree->Branch("PedestalCorr",&m_ped1corr,"PedestalCorr/F");
  m_tree->Branch("PedLumi",&m_ped2,"PedLumi/F");

  ATH_CHECK( m_thistSvc->regTree("/file1/calonoise/mytree",m_tree) );

  ATH_MSG_INFO ( " end of CaloAddCellPedShift::initialize " );
  return StatusCode::SUCCESS; 
}

// ===============================================================================

StatusCode CaloAddCellPedShift::updateMap(IOVSVC_CALLBACK_ARGS_K(keys) )
{
  msg() << MSG::INFO << " in updateMap ";
  std::list<std::string>::const_iterator itr;
  for (itr=keys.begin(); itr!=keys.end(); ++itr) {
    msg() << *itr << " ";
  }
  msg() << endmsg;

  //=== loop over collection (all cool channels)
  CondAttrListCollection::const_iterator iColl = m_noiseAttrListColl->begin();
  CondAttrListCollection::const_iterator last  = m_noiseAttrListColl->end();
  for (; iColl != last; ++iColl) {

    //=== COOL channel number is system id
    unsigned int sysId = static_cast<unsigned int>(iColl->first);

    //=== delete old CaloCondBlobFlt (which does not own the blob)
    std::map<unsigned int, const CaloCondBlobFlt*>::iterator iOld = m_noiseBlobMap.find(sysId);
    if(iOld != m_noiseBlobMap.end()){
      delete iOld->second;
    }

    //=== Get new CaloCondBlobFlt instance, interpreting current BLOB
    const coral::Blob& blob = (iColl->second)["CaloCondBlob16M"].data<coral::Blob>();
    const CaloCondBlobFlt* flt = CaloCondBlobFlt::getInstance(blob);

    //=== store new pointer in map
    m_noiseBlobMap[sysId] = flt;

  }//end iColl

  return StatusCode::SUCCESS;
}

//__________________________________________________________________________
StatusCode CaloAddCellPedShift::execute()
{
  ATH_MSG_DEBUG ("CaloAddCellPedShift execute()" );
  return StatusCode::SUCCESS; 
}

//__________________________________________________________________________
StatusCode CaloAddCellPedShift::stop()
{
  int ncell=m_calo_id->calo_cell_hash_max();

  std::vector<float> pedShiftValue;
  pedShiftValue.resize(ncell,0.);

  FILE* finput = fopen(m_fname.c_str(),"r");
  ATH_MSG_INFO ( " opened file " << m_fname );
  int bec;
  int pos_neg;
  int FT;
  int slot;
  int channel;
  float pedShift;
  while( fscanf(finput,"%d %d %d %d %d %f",&bec,&pos_neg,&FT,&slot,&channel,&pedShift) != EOF  ) {
    ATH_MSG_INFO ( " read linbe " << bec << " " << pos_neg << " " << FT << " " << slot << " " << channel << " " << pedShift );
    HWIdentifier hwid = m_onlineID->channel_Id(bec,pos_neg,FT,slot,channel);
    Identifier id     = m_cablingService->cnvToIdentifier( hwid);
    IdentifierHash idHash = m_calo_id->calo_cell_hash(id);
    int ii = (int) (idHash);
    pedShiftValue[ii] = pedShift;
  }
  fclose(finput);
  ATH_MSG_INFO ( " end of reading file" );
  

  FILE* fp = fopen("calopedestal.txt","w");
  ATH_MSG_INFO ( " start loop over Calo cells " << ncell );
  for (int i=0;i<ncell;i++) {
       IdentifierHash idHash=i;
       Identifier id=m_calo_id->cell_id(idHash);
       const CaloDetDescrElement* calodde = m_calodetdescrmgr->get_element(id);
       int subCalo;
       IdentifierHash idSubHash = m_calo_id->subcalo_cell_hash (idHash, subCalo);

#if 0
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
#endif
       int ii = (int) (idSubHash);

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

          unsigned int dbGain = CaloCondUtils::getDbCaloGain(gain);
          unsigned int subHash2;
          unsigned int iCool = m_caloCoolIdTool->getCoolChannelId(idHash,subHash2);
          const CaloCondBlobFlt* const flt = m_noiseBlobMap.find(iCool)->second;
          float ped1_old= flt->getData(subHash2,dbGain,0);
          float ped2= flt->getData(subHash2,dbGain,1);

          float ped1 = ped1_old + pedShiftValue[i];

          if (iCool<48) fprintf(fp,"%5u %5d %5d %8.3f %8.3f\n",iCool,ii,gain,ped1,ped2);

          m_iCool = iCool;
          m_SubHash=ii;
          m_Hash=i;
          m_OffId=(int)(id.get_identifier32().get_compact());
          m_eta = calodde->eta_raw();
          m_phi =  calodde->phi_raw();
          m_layer =  m_calo_id->calo_sample(id);
          m_Gain = gain;
          if (iCool<48) {
            HWIdentifier hwid = m_cablingService->createSignalChannelID(id);
            m_bec= m_onlineID->barrel_ec(hwid);
            m_posneg= m_onlineID->pos_neg(hwid);
            m_FT= m_onlineID->feedthrough(hwid);
            m_slot= m_onlineID->slot(hwid);
            m_channel= m_onlineID->channel(hwid);
          } else {
            m_bec=-1;
            m_posneg=-1;
            m_FT=-1;
            m_slot=-1;
            m_channel=-1;
          }
          m_ped1 = ped1_old;
          m_ped1corr = ped1;
          m_ped2 = ped2;
          m_tree->Fill();

         if (std::fabs(ped1-ped1_old)>1.)
           ATH_MSG_WARNING ( "  Pedestal shift found for cell " << m_OffId << " HWID: " << m_bec << " " << m_posneg << " " << m_FT << " " << m_slot << " " << m_channel << " New/Old pedestals "  << ped1 << " " << ped1_old );

       }   // loop over gains

  }        // loop over cells

  fclose(fp);
  return StatusCode::SUCCESS;
 }

 StatusCode CaloAddCellPedShift::finalize()
{
  return StatusCode::SUCCESS;
}

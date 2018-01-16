/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "LArCalibTools/LArCond2NtupleBase.h"

#include "LArIdentifier/LArOnlineID.h"
#include "LArIdentifier/LArOnline_SuperCellID.h"
#include "CaloIdentifier/CaloIdManager.h"
#include "LArCondUtils/LArFEBTempTool.h"
#include "CaloIdentifier/CaloCell_ID.h"

LArCond2NtupleBase::LArCond2NtupleBase(const std::string& name, ISvcLocator* pSvcLocator): 
  AthAlgorithm(name, pSvcLocator), m_initialized(false), m_nt(NULL),  
  m_detStore(NULL), m_emId(NULL), m_hecId(NULL), m_fcalId(NULL),m_onlineId(NULL),m_caloId(NULL),
  m_larCablingSvc(NULL), m_badChanTool("LArBadChanTool"),m_FEBTempTool("LArFEBTempTool"), m_isSC(false)
{
  declareProperty("BadChannelTool",m_badChanTool);
  declareProperty("AddBadChannelInfo",m_addBC=true);
  declareProperty("AddFEBTempInfo",m_addFEBTemp=false);
  declareProperty("isSC",m_isSC);
  declareProperty("isFlat", m_isFlat=false);
  declareProperty("OffId", m_OffId=false);
  declareProperty("AddHash",m_addHash=true);
}

LArCond2NtupleBase::~LArCond2NtupleBase() {
  ;
}

StatusCode LArCond2NtupleBase::initialize() {
  
  ATH_MSG_DEBUG("Initializing LArCond2Ntuple base class");
  
  if (m_ntpath.size()==0 || m_ntTitle.size()==0) {
    ATH_MSG_ERROR( "Need to set variable 'm_ntpath' and 'm_ntTitle' in constructor of deriving class!" );
    return StatusCode::FAILURE;
  }

  size_t i=m_ntpath.rfind('/');
  if (i==std::string::npos) {
    ATH_MSG_ERROR( "Expected at least on '/' in path " << m_ntpath );
    return StatusCode::FAILURE;
  }
  std::string basepath(m_ntpath.begin(),m_ntpath.begin()+i);
  //std::cout << "Basepath" << basepath << std::endl;


  NTupleFilePtr file1(ntupleSvc(),basepath);
  if (!file1){
    ATH_MSG_ERROR( "Could not get NTupleFilePtr with path " << basepath << " failed" );
    return StatusCode::FAILURE;
  }
  NTuplePtr nt(ntupleSvc(),m_ntpath);
  if (!nt) {
    nt=ntupleSvc()->book(m_ntpath,CLID_ColumnWiseTuple,m_ntTitle);
  }
  if (!nt){
    ATH_MSG_ERROR( "Booking of NTuple at "<< m_ntpath << " and name " << m_ntTitle << " failed" );
    return StatusCode::FAILURE; 
  }

  m_nt=nt;
  //std::cout << "Ntuple ptr:" << m_nt << std::endl;

  m_detStore= &(*detStore()); //for backward compatiblity 

  //StatusCode sc=service("DetectorStore",m_detStore);
  //if (sc!=StatusCode::SUCCESS) {
  //  ATH_MSG_ERROR( "Cannot get DetectorStore!" );
  //  return sc;
  //}

  const CaloIdManager *caloIdMgr=CaloIdManager::instance() ;
  if ( m_isSC ){
  m_emId=caloIdMgr->getEM_SuperCell_ID();
  m_fcalId=caloIdMgr->getFCAL_SuperCell_ID();
  m_hecId=caloIdMgr->getHEC_SuperCell_ID();
  }
  else{
  m_emId=caloIdMgr->getEM_ID();
  m_fcalId=caloIdMgr->getFCAL_ID();
  m_hecId=caloIdMgr->getHEC_ID();
  m_caloId=caloIdMgr->getCaloCell_ID();
  }

  if (!m_emId) {
    ATH_MSG_ERROR( "Could not access lar EM ID helper" );
    return StatusCode::FAILURE;
  }
  if (!m_fcalId) {
    ATH_MSG_ERROR( "Could not access lar FCAL ID helper" );
    return StatusCode::FAILURE;
  }
  if (!m_hecId) {
    ATH_MSG_ERROR( "Could not access lar HEC ID helper" );
    return StatusCode::FAILURE;
  }


  StatusCode sc;
  if ( m_isSC ){
  const LArOnline_SuperCellID* ll;
  sc = detStore()->retrieve(ll, "LArOnline_SuperCellID");
  if (sc.isFailure()) {
    ATH_MSG_ERROR( "Could not get LArOnlineID helper !" );
    return StatusCode::FAILURE;
  }
    else {
      m_onlineId = (const LArOnlineID_Base*)ll;
      ATH_MSG_DEBUG("Found the LArOnlineID helper");
    }

  ToolHandle<LArSuperCellCablingTool> tool("LArSuperCellCablingTool");
  sc = tool.retrieve();
  if (sc!=StatusCode::SUCCESS) {
    ATH_MSG_ERROR( " Can't get LArCablingSvc." );
    return sc;
  } else m_larCablingSvc = (LArCablingBase*)&(*tool);

  } else { // m_isSC
  const LArOnlineID* ll;
  sc = detStore()->retrieve(ll, "LArOnlineID");
  if (sc.isFailure()) {
    ATH_MSG_ERROR( "Could not get LArOnlineID helper !" );
    return StatusCode::FAILURE;
  }
   else {
      m_onlineId = (const LArOnlineID_Base*)ll;
      ATH_MSG_DEBUG(" Found the LArOnlineID helper. ");
    }

  ToolHandle<LArCablingService> tool("LArCablingService");
  sc = tool.retrieve();
  if (sc!=StatusCode::SUCCESS) {
    ATH_MSG_ERROR( " Can't get LArCablingSvc." );
    return sc;
  } else m_larCablingSvc = (LArCablingBase*)&(*tool);
  } // end of m_isSC if

  if (m_addBC) {
    sc = m_badChanTool.retrieve();
    if (sc!=StatusCode::SUCCESS) {
      ATH_MSG_ERROR( " Can't get BadChanTool." );
      return sc;
    }
  }

  if (m_addFEBTemp) {
    sc = m_FEBTempTool.retrieve();
    if (sc!=StatusCode::SUCCESS) {
      ATH_MSG_ERROR( " Can't get FEBTempTool." );
      return sc;
    }
  }

  //Online-identifier variables
  sc=nt->addItem("channelId",m_onlChanId,0x38000000,0x3A000000);
  if (sc!=StatusCode::SUCCESS) {
    ATH_MSG_ERROR( "addItem 'channelId' failed" );
    return StatusCode::FAILURE;
  }

  //Offline-identifier variables
  if ( m_OffId ) {
    sc=nt->addItem("offlineId",m_oflChanId,0x20000000,0x40000000);
    if (sc!=StatusCode::SUCCESS) {
      ATH_MSG_ERROR( "addItem 'channelId' failed" );
      return StatusCode::FAILURE;
    }
  }

  sc=nt->addItem("barrel_ec",m_barrel_ec,0,1);
  if (sc!=StatusCode::SUCCESS) {
    ATH_MSG_ERROR( "addItem 'barrel_ec' failed" );
    return StatusCode::FAILURE;
  }

  sc=nt->addItem("pos_neg",m_pos_neg,0,1);
  if (sc!=StatusCode::SUCCESS) {
    ATH_MSG_ERROR( "addItem 'pos_neg' failed" );
    return StatusCode::FAILURE;
   }

  sc=nt->addItem("FT",m_FT,0,32);
  if (sc!=StatusCode::SUCCESS) {
    ATH_MSG_ERROR( "addItem 'FT' failed" );
    return StatusCode::FAILURE;
  }

  sc=nt->addItem("slot",m_slot,1,15);
  if (sc!=StatusCode::SUCCESS) {
    ATH_MSG_ERROR( "addItem 'slot' failed" );
    return StatusCode::FAILURE;
   }

  sc=nt->addItem("channel",m_channel,0,127);
  if (sc!=StatusCode::SUCCESS){
    ATH_MSG_ERROR( "addItem 'channel' failed" );
    return StatusCode::FAILURE;
  }

  if ( !m_isSC) {
    sc=nt->addItem("calibLine",m_calibLine,0,127);
    if (sc!=StatusCode::SUCCESS) {
      ATH_MSG_ERROR( "addItem 'calibLine' failed" );
      return StatusCode::FAILURE;
    }
  }

  sc=nt->addItem("isConnected",m_isConnected,0,1);
  if (sc!=StatusCode::SUCCESS) {
    ATH_MSG_ERROR( "addItem 'isConnected' failed" );
    return StatusCode::FAILURE;
  }

  
  if (m_addHash) {
    sc=nt->addItem("channelHash",m_chanHash,0,200000);
    if (sc!=StatusCode::SUCCESS) {
      ATH_MSG_ERROR( "addItem 'channelHash' failed" );
      return StatusCode::FAILURE;
    }

    sc=nt->addItem("febHash",m_febHash,0,2000);
    if (sc!=StatusCode::SUCCESS) {
      ATH_MSG_ERROR( "addItem 'febHash' failed" );
      return StatusCode::FAILURE;
    }

    if (m_OffId) {
      sc=m_nt->addItem("oflHash",m_oflHash,0,200000);
      if (sc!=StatusCode::SUCCESS) {
	ATH_MSG_ERROR( "addItem 'oflHash' failed" );
	return StatusCode::FAILURE;
      }
    }
  }//end-if addHash


  //Offline-ID related variables
  sc=nt->addItem("layer",m_layer,0,4);
  if (sc!=StatusCode::SUCCESS) {
    ATH_MSG_ERROR( "addItem 'layer' failed" );
    return StatusCode::FAILURE;
   }
  sc=nt->addItem("ieta",m_eta,0,510);
  if (sc!=StatusCode::SUCCESS) {
   ATH_MSG_ERROR( "addItem 'ieta' failed" );
   return StatusCode::FAILURE;
  }
  sc=nt->addItem("iphi",m_phi,0,1023);
  if (sc!=StatusCode::SUCCESS) {
    ATH_MSG_ERROR( "addItem 'iphi' failed" );
    return StatusCode::FAILURE;
  }
  sc=nt->addItem("region",m_region,0,5);
  if (sc!=StatusCode::SUCCESS) {
    ATH_MSG_ERROR( "addItem 'region' failed" );
    return StatusCode::FAILURE;
  }
  sc=nt->addItem("detector",m_detector,0,2);
  if (sc!=StatusCode::SUCCESS) {
    ATH_MSG_ERROR( "addItem 'detector' failed" );
    return StatusCode::FAILURE;
  }

  if (m_addBC) {
    sc=nt->addItem("badChan",m_badChanWord);
    if (sc!=StatusCode::SUCCESS) {
      ATH_MSG_ERROR( "addItem 'badChan' failed" );
      return StatusCode::FAILURE;
    }
  }

  if (m_addFEBTemp) {
    sc=nt->addItem("FEBTemp1",m_FEBTemp1);
    if (sc!=StatusCode::SUCCESS) {
      ATH_MSG_ERROR( "addItem 'FEBTemp1' failed" );
      return StatusCode::FAILURE;
    }
    sc=nt->addItem("FEBTemp2",m_FEBTemp2);
    if (sc!=StatusCode::SUCCESS) {
      ATH_MSG_ERROR( "addItem 'FEBTemp2' failed" );
      return StatusCode::FAILURE;
    }
  }

  m_initialized=true;
  return StatusCode::SUCCESS;
}

bool LArCond2NtupleBase::fillFromIdentifier(const HWIdentifier& hwid) {

 m_onlChanId = hwid.get_identifier32().get_compact();
 
 m_barrel_ec = m_onlineId->barrel_ec(hwid);
 m_pos_neg   = m_onlineId->pos_neg(hwid);
 m_FT        = m_onlineId->feedthrough(hwid);
 m_slot      = m_onlineId->slot(hwid);
 m_channel   = m_onlineId->channel(hwid);

 if (m_addHash) {
   m_chanHash=m_onlineId->channel_Hash(hwid);
   m_febHash=m_onlineId->feb_Hash(m_onlineId->feb_Id(hwid));
 }

 if ( !m_isSC) {
 m_calibLine=NOT_VALID;
 const std::vector<HWIdentifier>& calibLineV=((LArCablingService*)m_larCablingSvc)->calibSlotLine(hwid);
 if(calibLineV.size()) m_calibLine = m_onlineId->channel(calibLineV[0]);
 }

 m_detector=NOT_VALID; 
 m_region=NOT_VALID;
 m_layer=NOT_VALID;
 m_eta=NOT_VALID;
 m_phi=NOT_VALID;
 //m_oflChanId=NOT_VALID;
 //m_oflHash=NOT_VALID;
 if (m_addBC) m_badChanWord=0;
 bool connected=false;

 try {
   if (m_larCablingSvc->isOnlineConnected(hwid)) {
     Identifier id=m_larCablingSvc->cnvToIdentifier(hwid);
     if ( m_OffId ) {
       m_oflChanId = id.get_identifier32().get_compact();
       if (m_addHash) 
	 m_oflHash=m_caloId->calo_cell_hash(id);
     }
   
     if (m_emId->is_lar_em(id)) {
       m_eta       = m_emId->eta(id);
       m_phi       = m_emId->phi(id);
       m_layer     = m_emId->sampling(id);
       m_region    = m_emId->region(id);
       m_detector  = std::abs(m_emId->barrel_ec(id)) - 1; //0-barrel, 1-EMEC-OW, 2-EMEC-IW
     }
     else if (m_hecId->is_lar_hec(id)) {
       m_eta       = m_hecId->eta(id);
       m_phi       = m_hecId->phi(id);
       m_layer     = m_hecId->sampling(id);
       m_region    = m_hecId->region(id);
       m_detector  = 3;
     }
     else if (m_fcalId->is_lar_fcal(id)) {
       m_eta       = m_fcalId->eta(id);
       m_phi       = m_fcalId->phi(id);
       m_layer     = m_fcalId->module(id);
       m_region    = 0;
       m_detector  = 4;
     }
     connected=true;
   }//end if is connected
 }catch (LArID_Exception & except) {}

 //bad-channel word
 if (m_addBC) m_badChanWord=m_badChanTool->status(hwid).packedData();
 // FEB temperatures
 if (m_addFEBTemp) {
     FEBTemp tv = m_FEBTempTool->getFebTemp(hwid);
     if( !tv.empty() ) {
       FEBTemp::const_iterator itb = tv.begin();
       FEBTemp::const_iterator ite = tv.end();
       for(;itb!=ite;++itb) {
          m_FEBTemp1 = (*itb).first;
          m_FEBTemp2 = (*itb).second;
       }
     }
 }

 m_isConnected = (long)connected;

 return connected;
}

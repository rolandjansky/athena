/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "LArCabling/LArSuperCellCablingTool.h"
#include "CaloIdentifier/CaloCell_SuperCell_ID.h"
#include "AthenaPoolUtilities/AthenaAttributeList.h"
#include "CoralBase/Blob.h"

#include "LArIdentifier/LArOnline_SuperCellID.h"

LArSuperCellCablingTool::LArSuperCellCablingTool( const std::string& type, 
						  const std::string& name, 
						  const IInterface* parent ) 
  : LArCablingBase(type,name,parent),
    m_sc2ccMappingTool("CaloSuperCellIDTool")
{
  declareInterface<LArSuperCellCablingTool>(this); 
  m_onOffIdKey="/LAR/IdentifierOfl/OnOffIdMap_SC";
}


LArSuperCellCablingTool::~LArSuperCellCablingTool() { } 


StatusCode LArSuperCellCablingTool::initialize () {
  ATH_MSG_DEBUG("Initializing LArSuperCellCablingTool");

  ATH_CHECK( m_sc2ccMappingTool.retrieve() );
  ATH_CHECK( m_larCablingSvc.retrieve() );

  const LArOnline_SuperCellID* onlineId;
  StatusCode sc=detStore()->retrieve(onlineId, "LArOnline_SuperCellID");
  if (sc.isFailure()) {
    msg() <<  MSG::FATAL << "Could not get LArSuperCellOnlineID helper !" << endmsg;
    return StatusCode::FAILURE;
  } 
  m_onlineId=onlineId; //Cast to base-class

  const CaloCell_SuperCell_ID* caloCell_ID;
  sc=detStore()->retrieve(caloCell_ID,"CaloCell_SuperCell_ID");
  if (sc.isFailure()) {
    msg() <<  MSG::FATAL << "Could not get CaloSuperCell_ID helper !" << endmsg;
    return StatusCode::FAILURE;
  } 
  m_caloId=caloCell_ID; //Cast to base-class
  
  sc=detStore()->regFcn(&LArSuperCellCablingTool::iovCallBack,this,m_attrOnOff,m_onOffIdKey,true);
  if (sc.isFailure()) {
    msg(MSG::ERROR) << "Failed to register callback on SG key" << m_onOffIdKey << endmsg;
    return sc;
  }
  else {
    msg(MSG::INFO) << "Successfully installed callback on folder" << m_onOffIdKey << endmsg; 
  }
  // Global INFO on initialize()
  msg() <<MSG::INFO<< "Sucessfully initialized LArCablingService "<< endmsg; //Add more info about regular or SC, with or wo calib-lines, etc.
  return StatusCode::SUCCESS;
}


StatusCode LArSuperCellCablingTool::iovCallBack(IOVSVC_CALLBACK_ARGS_K(keys)) {
  msg() << MSG::INFO<<" ====> iovCallBack " << endmsg;
  
  for (std::list<std::string>::const_iterator itr=keys.begin(); itr!=keys.end(); ++itr) {
    ATH_MSG_DEBUG("IOV callback for key " << *itr);
    if (*itr==m_onOffIdKey) {
      m_onOffValid=false;
      readOnlOffMap();
    }
    else
      msg(MSG::WARNING) << "Callback fired for unknown key " << *itr << endmsg;
  } //end loop over keys
  return StatusCode::SUCCESS;
}

const std::vector<HWIdentifier>& LArSuperCellCablingTool::calibSlotLine(const HWIdentifier & sid) {
  Identifier offline_SC = cnvToIdentifier( sid );

  std::vector<Identifier> cells_in_SC = m_sc2ccMappingTool->superCellToOfflineID( offline_SC );

  std::vector<HWIdentifier>* SC_calibSlotLines = new std::vector<HWIdentifier>;

  for (unsigned int i = 0; i < cells_in_SC.size(); ++i) {
    HWIdentifier cell = m_larCablingSvc->createSignalChannelID( cells_in_SC.at(i) );
    //std::cout << "Having cell " << cell.get_identifier32().get_compact() << std::endl;

    const std::vector<HWIdentifier>& calibChannelIDs=m_larCablingSvc->calibSlotLine( cell );
    /*for (unsigned int j = 0; j < calibChannelIDs.size(); ++j) {
      std::cout << "- " << (calibChannelIDs.at(j)).get_identifier32().get_compact() << std::endl;
    }*/

    SC_calibSlotLines->insert(SC_calibSlotLines->end(), calibChannelIDs.begin(), calibChannelIDs.end());
  }

  return *SC_calibSlotLines;
}


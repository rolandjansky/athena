/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "LArBadChannelTool/LArBadChannelDBAlg.h"

// AttributeList
#include "CoralBase/Attribute.h"
#include "CoralBase/Blob.h"
#include "CoralBase/AttributeListSpecification.h"
#include "AthenaPoolUtilities/AthenaAttributeList.h"
#include "StoreGate/StoreGate.h"
#include "AthenaPoolUtilities/CondAttrListCollection.h"

//#include "LArRecConditions/ILArBadChanTool.h"
#include "LArBadChannelTool/LArBadChanTool.h"
#include "LArBadChannelTool/LArBadChannelDBTools.h"

LArBadChannelDBAlg::LArBadChannelDBAlg(const std::string& name, ISvcLocator* pSvcLocator) :
  Algorithm( name, pSvcLocator),
  m_BadChanTool("LArBadChanTool"),
  m_detStore(0)
{
  declareProperty("BadChannelTool", m_BadChanTool, "public, shared BadChannelTool");
  declareProperty("DBFolder",m_dbFolder="/LAR/BadChannels/BadChannels");
  declareProperty("FEBFolder",m_dbFebFolder="/LAR/BadChannels/MissingFEBs");
  declareProperty("WritingMode",m_mode=0,"selects information written to DB: 0 for bad channels, 1 for missing febs");
}

LArBadChannelDBAlg::~LArBadChannelDBAlg() {}


StatusCode LArBadChannelDBAlg::initialize() {

  // Part 1: Get the messaging service, print where you are
  MsgStream log(msgSvc(), name());
  log << MSG::INFO << "initialize()" << endreq;

  // Part 3: Retrieve the tools using the ToolHandles
  if ( m_BadChanTool.retrieve().isFailure() ) {
    log << MSG::FATAL  << m_BadChanTool.propertyName() << ": Failed to retrieve tool " 
	<< m_BadChanTool << endreq;
    return StatusCode::FAILURE;
  } else {
    log << MSG::INFO << m_BadChanTool.propertyName() << ": Retrieved tool " 
	<< m_BadChanTool.type() << endreq;
  }

  // locate the conditions store ptr to it.
  if (service("DetectorStore", m_detStore).isFailure()) {
    log <<MSG::ERROR <<"Could not find DetStore" <<endreq;
    return StatusCode::FAILURE;
  }

  return StatusCode::SUCCESS;
}

StatusCode LArBadChannelDBAlg::execute() 
{return StatusCode::SUCCESS;}

StatusCode LArBadChannelDBAlg::finalize() {
  MsgStream log(msgSvc(), name());

  //if (m_mode == 0) {
    log << MSG::INFO << "Creating AttrListCollection in folder " << m_dbFolder << endreq;
    CondAttrListCollection* attrListColl = 
      LArBadChannelDBTools::createCoolCollection( m_BadChanTool->fullState());

    StatusCode sc = m_detStore->record( attrListColl, m_dbFolder);
    if (sc.isFailure()) {
      log <<MSG::ERROR <<"Could not record AttrListCollection" <<endreq;
      return( StatusCode::FAILURE);
    }
    //} else {
    log << MSG::INFO << "Creating AthenaAttributeList in folder " << m_dbFebFolder << endreq;
    const LArBadChanTool::BadFebVec& febs = m_BadChanTool->fullBadFebsState();
    if (!febs.empty()) {
      AthenaAttributeList* attrList = LArBadChannelDBTools::createFebPayload( febs);

      StatusCode sc = m_detStore->record( attrList, m_dbFebFolder);
      if (sc.isFailure()) {
	log <<MSG::ERROR <<"Could not record AthenaAttributeList" <<endreq;
	return( StatusCode::FAILURE);
      }
    }
    //}
  log << MSG::INFO << "exiting finalize successfully " << m_dbFebFolder << endreq;

  return StatusCode::SUCCESS;
}



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
  AthAlgorithm( name, pSvcLocator),
  m_BadChanTool("LArBadChanTool")
{
  declareProperty("BadChannelTool", m_BadChanTool, "public, shared BadChannelTool");
  declareProperty("DBFolder",m_dbFolder="/LAR/BadChannels/BadChannels");
  declareProperty("FEBFolder",m_dbFebFolder="/LAR/BadChannels/MissingFEBs");
  declareProperty("WritingMode",m_mode=0,"selects information written to DB: 0 for bad channels, 1 for missing febs");
}

LArBadChannelDBAlg::~LArBadChannelDBAlg() {}


StatusCode LArBadChannelDBAlg::initialize() {

  ATH_MSG_INFO ( "initialize()" );

  ATH_CHECK ( m_BadChanTool.retrieve() );
  ATH_MSG_INFO ( m_BadChanTool.propertyName() << ": Retrieved tool " 
                 << m_BadChanTool.type() );
  return StatusCode::SUCCESS;
}

StatusCode LArBadChannelDBAlg::execute() 
{return StatusCode::SUCCESS;}

StatusCode LArBadChannelDBAlg::finalize() {
  //if (m_mode == 0) {
  ATH_MSG_INFO ( "Creating AttrListCollection in folder " << m_dbFolder );
    CondAttrListCollection* attrListColl = 
      LArBadChannelDBTools::createCoolCollection( m_BadChanTool->fullState());

    ATH_CHECK( detStore()->record( attrListColl, m_dbFolder) );
    ATH_MSG_INFO ( "Creating AthenaAttributeList in folder " << m_dbFebFolder );
    const LArBadChanTool::BadFebVec& febs = m_BadChanTool->fullBadFebsState();
    if (!febs.empty()) {
      AthenaAttributeList* attrList = LArBadChannelDBTools::createFebPayload( febs);

      ATH_CHECK( detStore()->record( attrList, m_dbFebFolder) );
    }
    //}
    ATH_MSG_INFO ( "exiting finalize successfully " << m_dbFebFolder );

  return StatusCode::SUCCESS;
}



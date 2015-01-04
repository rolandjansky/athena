/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "CoolLumiUtilities/BunchGroupTool.h"

#include "CoolKernel/IObject.h"
#include "CoralBase/Blob.h"
#include "AthenaPoolUtilities/CondAttrListCollection.h"
#include "AthenaPoolUtilities/AthenaAttributeList.h"

BunchGroupTool::BunchGroupTool(const std::string& type,
			     const std::string& name,
			     const IInterface* parent) :
  AthAlgTool(type, name, parent), 
  m_bunchGroupFolderName("") // /TRIGGER/LVL1/BunchGroupContent")
{
  declareInterface<IBunchGroupTool>(this);
  declareProperty("BunchGroupFolderName", m_bunchGroupFolderName);

  m_bg0.clear();
  m_bg1.clear();
  m_bg2.clear();
  m_bg3.clear();
  m_bg4.clear();
  m_bg5.clear();
  m_bg6.clear();
  m_bg7.clear();
}

StatusCode
BunchGroupTool::initialize()
{
  ATH_MSG_DEBUG("BunchGroupTool::initialize() begin");

  if (m_bunchGroupFolderName.empty()) {
    // May not be configured, could be OK
    ATH_MSG_INFO("BunchGroupFolderName.empty() is TRUE, skipping...");
  } else {
    ATH_MSG_INFO("Registering callback on folder: " << m_bunchGroupFolderName);

    // Setup callback on COOL folder change
    // This must be done with the interface (IBunchGroupTool) so external code can use this to trigger
    // their own callbacks

    if (detStore()->contains<AthenaAttributeList>(m_bunchGroupFolderName)) {

      const DataHandle<AthenaAttributeList> aptr;

      CHECK(detStore()->regFcn(&IBunchGroupTool::updateCache, dynamic_cast<IBunchGroupTool*>(this) , aptr, m_bunchGroupFolderName));

      ATH_MSG_INFO( " Registered a callback for " << m_bunchGroupFolderName << " COOL folder " );
    } else {
      ATH_MSG_ERROR( " cannot find " << m_bunchGroupFolderName << " in DetectorStore" );
    }
  }

  ATH_MSG_DEBUG( "BunchGroupTool::initialize() end" );
  return StatusCode::SUCCESS;
}

unsigned int
BunchGroupTool::nBunchGroup0() const {
  return m_bg0.size();
}

unsigned int
BunchGroupTool::nBunchGroup1() const {
  return m_bg1.size();
}

unsigned int
BunchGroupTool::nBunchGroup2() const {
  return m_bg2.size();
}

unsigned int
BunchGroupTool::nBunchGroup3() const {
  return m_bg3.size();
}

unsigned int
BunchGroupTool::nBunchGroup4() const {
  return m_bg4.size();
}

unsigned int
BunchGroupTool::nBunchGroup5() const {
  return m_bg5.size();
}

unsigned int
BunchGroupTool::nBunchGroup6() const {
  return m_bg6.size();
}

unsigned int
BunchGroupTool::nBunchGroup7() const {
  return m_bg7.size();
}

const std::vector<unsigned int> &
BunchGroupTool::bunchGroup0() const {
  return m_bg0;
}

const std::vector<unsigned int> &
BunchGroupTool::bunchGroup1() const {
  return m_bg1;
}

const std::vector<unsigned int> &
BunchGroupTool::bunchGroup2() const {
  return m_bg2;
}

const std::vector<unsigned int> &
BunchGroupTool::bunchGroup3() const {
  return m_bg3;
}

const std::vector<unsigned int> &
BunchGroupTool::bunchGroup4() const {
  return m_bg4;
}

const std::vector<unsigned int> &
BunchGroupTool::bunchGroup5() const {
  return m_bg5;
}

const std::vector<unsigned int> &
BunchGroupTool::bunchGroup6() const {
  return m_bg6;
}

const std::vector<unsigned int> &
BunchGroupTool::bunchGroup7() const {
  return m_bg7;
}


// Callback function
StatusCode
BunchGroupTool::updateCache( IOVSVC_CALLBACK_ARGS_P(/*idx*/, /*keys*/) )
{
  
  ATH_MSG_DEBUG( "in updateCache()");

  // Clear old data
  m_bg0.clear();
  m_bg1.clear();
  m_bg2.clear();
  m_bg3.clear();
  m_bg4.clear();
  m_bg5.clear();
  m_bg6.clear();
  m_bg7.clear();

  // Shouldn't get here if folder name doesn't exist, but check anyways
  if (m_bunchGroupFolderName.empty()) {
    ATH_MSG_WARNING("updateCache called but BunchGroupFolderName.empty() is TRUE!");
    return StatusCode::SUCCESS;
  }

  const AthenaAttributeList* attrList = 0;
  CHECK(detStore()->retrieve(attrList, m_bunchGroupFolderName));

  if ((*attrList)["BunchCode"].isNull()) {
    ATH_MSG_WARNING( "BunchCode is NULL in " << m_bunchGroupFolderName << "!" );
    return StatusCode::SUCCESS;
  }

  // Do everything here for now, but should copy this to a vanilla object based on attrList
  const coral::Blob& blob = (*attrList)["BunchCode"].data<coral::Blob>();

  ATH_MSG_DEBUG( "Bunchgroup blob length: " << blob.size() );

  // Verify length
  // There have been many bugs, so just require at least 3564
  if ( static_cast<cool::UInt32>( blob.size() ) < 3564) {
    ATH_MSG_WARNING( "BunchCode length " << blob.size() << " < 3564!" );
    return StatusCode::SUCCESS;
  }

  // Decode all 8 bunch groups at once
  const unsigned char* p = static_cast<const unsigned char*>(blob.startingAddress());
  for (unsigned int bcid = 0; bcid < 3564; ++bcid, ++p) {

    // Read 8-bits at a time and decode                                                                     
    unsigned char mask = (*p);
    if (mask & 0x01) m_bg0.push_back(bcid);
    if (mask & 0x02) m_bg1.push_back(bcid);
    if (mask & 0x04) m_bg2.push_back(bcid);
    if (mask & 0x08) m_bg3.push_back(bcid);
    if (mask & 0x10) m_bg4.push_back(bcid);
    if (mask & 0x20) m_bg5.push_back(bcid);
    if (mask & 0x40) m_bg6.push_back(bcid);
    if (mask & 0x80) m_bg7.push_back(bcid);

  }

  // Print out physics bunch group
  if (msgLvl(MSG::DEBUG)) {
    msg(MSG::DEBUG) << "BunchGroup1 list: ";
    for (unsigned int i=0; i<m_bg1.size(); i++)
      msg(MSG::DEBUG) << m_bg1[i] << " ";
    msg(MSG::DEBUG) << endreq;
  }

  ATH_MSG_DEBUG( "Bunch Group 0 entries: " << m_bg0.size());
  ATH_MSG_DEBUG( "Bunch Group 1 entries: " << m_bg1.size());
  ATH_MSG_DEBUG( "Bunch Group 2 entries: " << m_bg2.size());
  ATH_MSG_DEBUG( "Bunch Group 3 entries: " << m_bg3.size());
  ATH_MSG_DEBUG( "Bunch Group 4 entries: " << m_bg4.size());
  ATH_MSG_DEBUG( "Bunch Group 5 entries: " << m_bg5.size());
  ATH_MSG_DEBUG( "Bunch Group 6 entries: " << m_bg6.size());
  ATH_MSG_DEBUG( "Bunch Group 7 entries: " << m_bg7.size());

  return StatusCode::SUCCESS;
}


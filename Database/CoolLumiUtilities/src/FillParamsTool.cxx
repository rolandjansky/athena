/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "CoolLumiUtilities/FillParamsTool.h"

#include "CoolKernel/IObject.h"
#include "CoralBase/Blob.h"
#include "AthenaPoolUtilities/CondAttrListCollection.h"
#include "AthenaPoolUtilities/AthenaAttributeList.h"

FillParamsTool::FillParamsTool(const std::string& type,
			     const std::string& name,
			     const IInterface* parent) :
  AthAlgTool(type, name, parent), 
  m_fillParamsFolderName("") // "/TDAQ/OLC/LHC/FILLPARAMS")
{
  declareInterface<IFillParamsTool>(this);
  declareProperty("FillParamsFolderName", m_fillParamsFolderName);

  m_beam1Bunches.clear();
  m_beam2Bunches.clear();
  m_luminousBunches.clear();
}

StatusCode
FillParamsTool::initialize()
{
  ATH_MSG_DEBUG("FillParamsTool::initialize() begin");

  if (m_fillParamsFolderName.empty()) {
    // May not be configured, could be OK
    ATH_MSG_INFO("FillparamsFolderName.empty() is TRUE, skipping...");
  } else {
    ATH_MSG_INFO("Registering callback on folder: " << m_fillParamsFolderName);

    // Setup callback on COOL folder change
    // This must be done with the interface (IFillParamsTool) so external code can use this to trigger
    // their own callbacks

    if (detStore()->contains<AthenaAttributeList>(m_fillParamsFolderName)) {

      const DataHandle<AthenaAttributeList> aptr;

      CHECK(detStore()->regFcn(&IFillParamsTool::updateCache, dynamic_cast<IFillParamsTool*>(this) , aptr, m_fillParamsFolderName));
    //CHECK(detStore()->regFcn(&FillParamsTool::updateCache, this , &IFillParamsTool::updateCache, dynamic_cast<IFillParamsTool*>(this)));

      ATH_MSG_INFO( " Registered a callback for " << m_fillParamsFolderName << " COOL folder " );
    } else {
      ATH_MSG_ERROR( " cannot find " << m_fillParamsFolderName << " in DetectorStore" );
    }
  }

  ATH_MSG_DEBUG( "FillParamsTool::initialize() end" );
  return StatusCode::SUCCESS;
}

unsigned int
FillParamsTool::nBeam1Bunches() const {
  return m_beam1Bunches.size();
}

unsigned int
FillParamsTool::nBeam2Bunches() const {
  return m_beam2Bunches.size();
}

unsigned int
FillParamsTool::nLuminousBunches() const {
  return m_luminousBunches.size();
}

const std::vector<unsigned int> &
FillParamsTool::beam1Bunches() const {
  return m_beam1Bunches;
}

const std::vector<unsigned int> &
FillParamsTool::beam2Bunches() const {
  return m_beam2Bunches;
}

const std::vector<unsigned int> &
FillParamsTool::luminousBunches() const {
  return m_luminousBunches;
}

// Callback function
StatusCode
FillParamsTool::updateCache( IOVSVC_CALLBACK_ARGS_P(/*idx*/, /*keys*/) )
{
  
  ATH_MSG_DEBUG( "in updateCache()");

  // Clear old data
  m_beam1Bunches.clear();
  m_beam2Bunches.clear();
  m_luminousBunches.clear();

  // Shouldn't get here if folder name is empty, but check anyways
  if (m_fillParamsFolderName.empty()) {
    ATH_MSG_WARNING("updateCache called but FillparamsFolderName.empty() is TRUE!");
    return StatusCode::SUCCESS;
  }

  const AthenaAttributeList* attrList = 0;
  CHECK(detStore()->retrieve(attrList, m_fillParamsFolderName));

  if ((*attrList)["BCIDmasks"].isNull()) {
    ATH_MSG_WARNING( "BCIDmasks is NULL in " << m_fillParamsFolderName << "!" );
    return StatusCode::SUCCESS;
  }

  // Do everything here for now, but should copy this to a vanilla object based on attrList
  cool::UInt32 nb1 = (*attrList)["Beam1Bunches"].data<cool::UInt32>();
  cool::UInt32 nb2 = (*attrList)["Beam2Bunches"].data<cool::UInt32>();
  cool::UInt32 ncol = (*attrList)["LuminousBunches"].data<cool::UInt32>();

  ATH_MSG_DEBUG( "Beam1Bunches: " << nb1 );
  ATH_MSG_DEBUG( "Beam2Bunches: " << nb2 );
  ATH_MSG_DEBUG( "LuminousBunches: " << ncol );

  const coral::Blob& blob = (*attrList)["BCIDmasks"].data<coral::Blob>();

  // Verify length
  if ( static_cast<cool::UInt32>( blob.size() ) != 2 * (nb1 + nb2 + ncol)) {
    ATH_MSG_WARNING( "BCIDmasks length " << blob.size() << " != 2 * " << (nb1+nb2+ncol) );
    return StatusCode::SUCCESS;
  }

  const uint16_t* p=static_cast<const uint16_t*>(blob.startingAddress());

  // Decode beam1 list
  if (msgLvl(MSG::DEBUG)) msg(MSG::DEBUG) << "Beam1 bunches list: ";
  for (unsigned int i = 0; i < nb1; i++, p++) {
    m_beam1Bunches.push_back(*p);
    if (msgLvl(MSG::DEBUG)) msg(MSG::DEBUG) << *p << " ";
  }
  if (msgLvl(MSG::DEBUG)) msg(MSG::DEBUG) << endreq;

  // Decode beam2 list
  if (msgLvl(MSG::DEBUG)) msg(MSG::DEBUG) << "Beam2 bunches list: ";
  for (unsigned int i = 0; i < nb2; i++, p++) {
    m_beam2Bunches.push_back(*p);
    if (msgLvl(MSG::DEBUG)) msg(MSG::DEBUG) << *p << " ";
  }
  if (msgLvl(MSG::DEBUG)) msg(MSG::DEBUG) << endreq;

  // Decode luminous list
  if (msgLvl(MSG::DEBUG)) msg(MSG::DEBUG) << "Luminous bunches list: ";
  for (unsigned int i = 0; i < ncol; i++, p++) {
    m_luminousBunches.push_back(*p);
    if (msgLvl(MSG::DEBUG)) msg(MSG::DEBUG) << *p << " ";
  }
  if (msgLvl(MSG::DEBUG)) msg(MSG::DEBUG) << endreq;

  return StatusCode::SUCCESS;
}


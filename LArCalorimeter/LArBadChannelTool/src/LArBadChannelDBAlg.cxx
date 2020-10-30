/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#include "LArBadChannelTool/LArBadChannelDBAlg.h"

// AttributeList
#include "CoralBase/Attribute.h"
#include "CoralBase/Blob.h"
#include "CoralBase/AttributeListSpecification.h"
#include "AthenaPoolUtilities/AthenaAttributeList.h"
#include "AthenaPoolUtilities/CondAttrListCollection.h"

#include "LArBadChannelTool/LArBadChannelDBTools.h"

#include "LArIdentifier/LArOnlineID.h"

LArBadChannelDBAlg::LArBadChannelDBAlg(const std::string& name, ISvcLocator* pSvcLocator) :
  AthAlgorithm( name, pSvcLocator), m_BCKey("LArBadChannel"), m_BFKey("LArBadFeb")
{
  declareProperty("BadChanKey",m_BCKey);
  declareProperty("MissingFEBKey",m_BFKey);
  declareProperty("DBFolder",m_dbFolder="/LAR/BadChannels/BadChannels");
  declareProperty("FEBFolder",m_dbFebFolder="/LAR/BadChannels/MissingFEBs");
  declareProperty("WritingMode",m_mode=0,"selects information written to DB: 0 for bad channels, 1 for missing febs");
}

LArBadChannelDBAlg::~LArBadChannelDBAlg() {}


StatusCode LArBadChannelDBAlg::initialize() {

  ATH_MSG_INFO ( "initialize()" );

  if(m_mode==0) {
     ATH_CHECK (m_BCKey.initialize() );
  }
  if(m_mode==1) {
     ATH_CHECK (m_BFKey.initialize() );
  }
  return StatusCode::SUCCESS;
}

StatusCode LArBadChannelDBAlg::execute() 
{return StatusCode::SUCCESS;}

StatusCode LArBadChannelDBAlg::finalize() {
  const DataHandle<LArOnlineID> onlineID; 
  ATH_CHECK( detStore()->retrieve(onlineID, "LArOnlineID") ); 
  if (m_mode == 0) {
    SG::ReadCondHandle<LArBadChannelCont> readHandle{m_BCKey};
    const LArBadChannelCont *bcCont {*readHandle};
    if ( bcCont == nullptr) {
       ATH_MSG_ERROR("Failed to retrieve BadChanCont with key "<<m_BCKey.key() );
       return StatusCode::FAILURE;
    }
    ATH_MSG_INFO ( "Creating AttrListCollection in folder " << m_dbFolder );
    LArBadChannelState bcState;
    LArBadChannelState::CoolChannelEnum coolchan;
    for (const auto& entry : bcCont->fullCont()) {
      const HWIdentifier hid=HWIdentifier(entry.first);
      if (onlineID->isEMBchannel(hid)) {
        if (onlineID->pos_neg(hid))
          coolchan = LArBadChannelState::CoolChannelEnum::EMBC; 
        else
          coolchan = LArBadChannelState::CoolChannelEnum::EMBA; 
      } else if (onlineID->isEMECchannel(hid)) {
        if (onlineID->pos_neg(hid))
          coolchan = LArBadChannelState::CoolChannelEnum::EMECC; 
        else
          coolchan = LArBadChannelState::CoolChannelEnum::EMECA; 
      } else if (onlineID->isHECchannel(hid)) {
        if (onlineID->pos_neg(hid))
          coolchan = LArBadChannelState::CoolChannelEnum::HECC; 
        else
          coolchan = LArBadChannelState::CoolChannelEnum::HECA; 
      } else if (onlineID->isFCALchannel(hid)) {
        if (onlineID->pos_neg(hid))
          coolchan = LArBadChannelState::CoolChannelEnum::FCALC; 
        else
          coolchan = LArBadChannelState::CoolChannelEnum::FCALA; 
      } else {
         ATH_MSG_WARNING ( "Wrong bad channel Id" << hid );
         continue;
      }

      bcState.add(LArBadChannelDBTools::BadChanEntry(hid,entry.second),coolchan);
   }

    CondAttrListCollection* attrListColl = 
      LArBadChannelDBTools::createCoolCollection(bcState);

    ATH_CHECK( detStore()->record( attrListColl, m_dbFolder) );
    ATH_MSG_INFO ( "Creating AthenaAttributeList in folder " << m_dbFebFolder );
  }
  if (m_mode == 1) {

    SG::ReadCondHandle<LArBadFebCont> readHandle{m_BFKey};
    const LArBadFebCont *bfCont {*readHandle};
    if ( bfCont == nullptr) {
       ATH_MSG_ERROR("Failed to retrieve BadFebCont with key "<<m_BFKey.key() );
       return StatusCode::FAILURE;
    }
    LArBadChannelDBTools::BadFebVec febs;
    for (const auto& entry : bfCont->fullCont()) {
      const HWIdentifier hid=HWIdentifier(entry.first);
      febs.emplace_back(LArBadChannelDBTools::BadFebEntry(hid,entry.second));
    }
    if (!febs.empty()) {
      AthenaAttributeList* attrList = LArBadChannelDBTools::createFebPayload( febs);

      ATH_CHECK( detStore()->record( attrList, m_dbFebFolder) );
    }
  }
    ATH_MSG_INFO ( "exiting finalize successfully " << m_dbFebFolder );

  return StatusCode::SUCCESS;
}



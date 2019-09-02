/*
 * Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration.
 */
/**
 * @file CoolLumiUtilities/src/BunchGroupCondAlg.cxx
 * @author scott snyder <snyder@bnl.gov>, from existing BunchGroupTool.
 * @date May, 2019
 * @brief Conditions algorithm to unpack bunch group data from COOL.
 */


#include "BunchGroupCondAlg.h"
#include "StoreGate/ReadCondHandle.h"
#include "StoreGate/WriteCondHandle.h"
#include "CoralBase/Blob.h"
#include "CoolKernel/IObject.h"
#include <stdint.h>


/**
 * @brief Gaudi initialize method.
 */
StatusCode
BunchGroupCondAlg::initialize()
{
  ATH_CHECK( m_bunchGroupFolderInputKey.initialize(SG::AllowEmpty) );
  ATH_CHECK( m_bunchGroupOutputKey.initialize() );
  return StatusCode::SUCCESS;
}


/**
 * @brief Algorithm execute method.
 * @param ctx Event Context.
 */
StatusCode
BunchGroupCondAlg::execute (const EventContext& ctx) const
{
  auto bg = std::make_unique<BunchGroupCondData>();
  const EventIDBase::number_type UNDEFNUM = EventIDBase::UNDEFNUM;
  const EventIDBase::event_number_t UNDEFEVT = EventIDBase::UNDEFEVT;
  EventIDRange range (EventIDBase (0, UNDEFEVT, UNDEFNUM, 0, 0),
                      EventIDBase (UNDEFNUM-1, UNDEFEVT, UNDEFNUM, 0, 0));

  if (!m_bunchGroupFolderInputKey.empty()) {
    SG::ReadCondHandle<AthenaAttributeList> bunchGroupFolder
      (m_bunchGroupFolderInputKey, ctx);
    ATH_CHECK( bunchGroupFolder.range (range) );

    if ((**bunchGroupFolder)["BunchCode"].isNull()) {
      ATH_MSG_ERROR( "BunchCode is NULL in " << m_bunchGroupFolderInputKey.key() << "!" );
      return StatusCode::FAILURE;
    }

    // Do everything here for now, but should copy this to a vanilla object based on attrList
    const coral::Blob& blob = (**bunchGroupFolder)["BunchCode"].data<coral::Blob>();

    ATH_MSG_DEBUG( "Bunchgroup blob length: " << blob.size() );

    // Verify length
    // There have been many bugs, so just require at least NBCID
    const unsigned int NBCID = BunchGroupCondData::NBCID;
    if ( static_cast<cool::UInt32>( blob.size() ) < NBCID) {
      ATH_MSG_ERROR( "BunchCode length " << blob.size()
                     << " < " << NBCID << "!" );
      return StatusCode::FAILURE;
    }

    // Decode all 8 bunch groups at once; read 8-bits at a time and decode.
    const uint8_t* p = static_cast<const uint8_t*>(blob.startingAddress());
    for (unsigned int bcid = 0; bcid < NBCID; ++bcid) {
      bg->addBCID (bcid, p[bcid]);
    }

    // Print out physics bunch group
    if (msgLvl(MSG::DEBUG)) {
      msg(MSG::DEBUG) << "BunchGroup1 list: ";
      for (unsigned int bcid : bg->bunchGroup(1)) {
        msg(MSG::DEBUG) << bcid << " ";
      }
      msg(MSG::DEBUG) << endmsg;
    }

    ATH_MSG_DEBUG( "Bunch Group 0 entries: " << bg->bunchGroup(0).size() );
    ATH_MSG_DEBUG( "Bunch Group 1 entries: " << bg->bunchGroup(1).size() );
    ATH_MSG_DEBUG( "Bunch Group 2 entries: " << bg->bunchGroup(2).size() );
    ATH_MSG_DEBUG( "Bunch Group 3 entries: " << bg->bunchGroup(3).size() );
    ATH_MSG_DEBUG( "Bunch Group 4 entries: " << bg->bunchGroup(4).size() );
    ATH_MSG_DEBUG( "Bunch Group 5 entries: " << bg->bunchGroup(5).size() );
    ATH_MSG_DEBUG( "Bunch Group 6 entries: " << bg->bunchGroup(6).size() );
    ATH_MSG_DEBUG( "Bunch Group 7 entries: " << bg->bunchGroup(7).size() );
  }

  bg->shrink();
  SG::WriteCondHandle<BunchGroupCondData> bunchGroupData
    (m_bunchGroupOutputKey, ctx);
  ATH_CHECK( bunchGroupData.record (range, std::move (bg)) );
  return StatusCode::SUCCESS;
}

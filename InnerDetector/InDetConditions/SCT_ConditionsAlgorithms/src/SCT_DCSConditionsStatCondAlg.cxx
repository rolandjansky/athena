/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#include "SCT_DCSConditionsStatCondAlg.h"

#include "Identifier/IdentifierHash.h"

#include "GaudiKernel/EventIDRange.h"

#include <memory>

SCT_DCSConditionsStatCondAlg::SCT_DCSConditionsStatCondAlg(const std::string& name, ISvcLocator* pSvcLocator)
  : ::AthReentrantAlgorithm(name, pSvcLocator)
  , m_condSvc{"CondSvc", name}
  , m_doState{true}
{
}

StatusCode SCT_DCSConditionsStatCondAlg::initialize() {
  ATH_MSG_DEBUG("initialize " << name());

  m_doState = ((m_readAllDBFolders.value() and m_returnHVTemp.value()) or (not m_readAllDBFolders.value() and not m_returnHVTemp.value()));

  // CondSvc
  ATH_CHECK(m_condSvc.retrieve());

  if (m_returnHVTemp.value()) {
    // Read Cond Handle (HV)
    ATH_CHECK(m_readKeyHV.initialize());
  }

  if (m_doState) {
    // Read Cond Handle (state)
    ATH_CHECK(m_readKeyState.initialize());
    // Write Cond Handle
    ATH_CHECK(m_writeKeyState.initialize());
    if (m_condSvc->regHandle(this, m_writeKeyState).isFailure()) {
      ATH_MSG_FATAL("unable to register WriteCondHandle " << m_writeKeyState.fullKey() << " with CondSvc");
      return StatusCode::FAILURE;
    }
  }

  if (m_useHV.value()) {
    m_hvLowLimit = m_useHVLowLimit;
    m_hvUpLimit = m_useHVUpLimit;
    m_chanstatCut = m_useHVChanCut;
    ATH_MSG_INFO("Using HV and Chanstat"<< m_chanstatCut << " for marking modules bad. >=Hvlow: "
                 << m_hvLowLimit<< " and <=Hv Up: " <<  m_hvUpLimit <<
                 ". Note: UseHV Overrides hv limit and chanstat values in joboptions!!");
  }

  return StatusCode::SUCCESS;
}

StatusCode SCT_DCSConditionsStatCondAlg::execute(const EventContext& ctx) const {
  ATH_MSG_DEBUG("execute " << name());

  if (not m_doState) {
    return StatusCode::SUCCESS;
  }

  // Write Cond Handle (state)
  SG::WriteCondHandle<SCT_DCSStatCondData> writeHandle{m_writeKeyState, ctx};
  // Do we have a valid Write Cond Handle for current time?
  if (writeHandle.isValid()) {
    ATH_MSG_DEBUG("CondHandle " << writeHandle.fullKey() << " is already valid."
                  << ". In theory this should not be called, but may happen"
                  << " if multiple concurrent events are being processed out of order.");
    return StatusCode::SUCCESS; 
  }

  // Read Cond Handle (state)
  SG::ReadCondHandle<CondAttrListCollection> readHandleState{m_readKeyState, ctx};
  const CondAttrListCollection* readCdoState{*readHandleState}; 
  if (readCdoState==nullptr) {
    ATH_MSG_FATAL("Null pointer to the read conditions object (state)");
    return StatusCode::FAILURE;
  }
  // Get the validitiy range (state)
  EventIDRange rangeState;
  if (not readHandleState.range(rangeState)) {
    ATH_MSG_FATAL("Failed to retrieve validity range for " << readHandleState.key());
    return StatusCode::FAILURE;
  }
  ATH_MSG_INFO("Size of CondAttrListCollection " << readHandleState.fullKey() << " readCdo->size()= " << readCdoState->size());
  ATH_MSG_INFO("Range of state input is " << rangeState);
  
  // Construct the output Cond Object and fill it in
  std::unique_ptr<SCT_DCSStatCondData> writeCdoState{std::make_unique<SCT_DCSStatCondData>()};

  // Read state info
  std::string paramState{"STATE"};
  CondAttrListCollection::const_iterator attrListState{readCdoState->begin()};
  CondAttrListCollection::const_iterator endState{readCdoState->end()};
  // CondAttrListCollection doesn't support C++11 type loops, no generic 'begin'
  for (; attrListState!=endState; ++attrListState) {
    // A CondAttrListCollection is a map of ChanNum and AttributeList
    CondAttrListCollection::ChanNum channelNumber{attrListState->first};
    const CondAttrListCollection::AttributeList &payload{attrListState->second};
    if (payload.exists(paramState) and not payload[paramState].isNull()) {
      unsigned int val{payload[paramState].data<unsigned int>()};
      unsigned int hvstate{val bitand 240};
      unsigned int lvstate{val bitand 15};
      if (   ( (m_chanstatCut=="NORM")  and not ((hvstate==16 or hvstate==48)                                and (lvstate==1 or lvstate==3))                             )
          or ( (m_chanstatCut=="NSTBY") and not ((hvstate==16 or hvstate==48 or hvstate==32)                 and (lvstate==1 or lvstate==3 or lvstate==2))               )
          or ( (m_chanstatCut=="LOOSE") and not ((hvstate==16 or hvstate==48 or hvstate==32 or hvstate==128) and (lvstate==1 or lvstate==3 or lvstate==2 or lvstate==8)) )) {
        writeCdoState->fill(channelNumber, paramState);
      } else {
        writeCdoState->remove(channelNumber, paramState);
      }
    } else {
      ATH_MSG_WARNING(paramState << " does not exist for ChanNum " << channelNumber);
    }
  }

  if (m_returnHVTemp.value()) {
    // Read Cond Handle 
    SG::ReadCondHandle<CondAttrListCollection> readHandleHV{m_readKeyHV, ctx};
    const CondAttrListCollection* readCdoHV{*readHandleHV};
    if (readCdoHV==nullptr) {
      ATH_MSG_FATAL("Null pointer to the read conditions object (HV)");
      return StatusCode::FAILURE;
    }
    // Get the validitiy range (HV)
    EventIDRange rangeHV;
    if (not readHandleHV.range(rangeHV)) {
      ATH_MSG_FATAL("Failed to retrieve validity range for " << readHandleHV.key());
      return StatusCode::FAILURE;
    }
    ATH_MSG_INFO("Size of CondAttrListCollection " << readHandleHV.fullKey() << " readCdo->size()= " << readCdoHV->size());
    ATH_MSG_INFO("Range of HV input is " << rangeHV);

    // Combined the validity ranges of state and range
    EventIDRange rangeIntersection{EventIDRange::intersect(rangeState, rangeHV)};
    if (rangeIntersection.stop().isValid() and rangeIntersection.start()>rangeIntersection.stop()) {
      ATH_MSG_FATAL("Invalid intersection range: " << rangeIntersection);
      return StatusCode::FAILURE;
    }
    rangeState = rangeIntersection;
  
    std::string paramHV{"HVCHVOLT_RECV"};
    CondAttrListCollection::const_iterator attrListHV{readCdoHV->begin()};
    CondAttrListCollection::const_iterator endHV{readCdoHV->end()};
    // CondAttrListCollection doesn't support C++11 type loops, no generic 'begin'
    for (; attrListHV!=endHV; ++attrListHV) {
      // A CondAttrListCollection is a map of ChanNum and AttributeList
      CondAttrListCollection::ChanNum channelNumber{attrListHV->first};
      const CondAttrListCollection::AttributeList &payload{attrListHV->second};
      if (payload.exists(paramHV) and not payload[paramHV].isNull()) {
        float hvval{payload[paramHV].data<float>()};
        if ((hvval<m_hvLowLimit) or (hvval>m_hvUpLimit)) {
          writeCdoState->fill(channelNumber, paramHV);
        } else {
          writeCdoState->remove(channelNumber, paramHV);
        }
      } else {
        ATH_MSG_WARNING(paramHV << " does not exist for ChanNum " << channelNumber);
      }
    }
  }

  // Record the output cond object
  if (writeHandle.record(rangeState, std::move(writeCdoState)).isFailure()) {
    ATH_MSG_FATAL("Could not record SCT_DCSStatCondData " << writeHandle.key() 
                  << " with EventRange " << rangeState
                  << " into Conditions Store");
    return StatusCode::FAILURE;
  }
  ATH_MSG_INFO("recorded new CDO " << writeHandle.key() << " with range " << rangeState << " into Conditions Store");

  return StatusCode::SUCCESS;
}

StatusCode SCT_DCSConditionsStatCondAlg::finalize()
{
  ATH_MSG_DEBUG("finalize " << name());
  return StatusCode::SUCCESS;
}

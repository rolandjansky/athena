/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "SCT_DCSConditionsStatCondAlg.h"

#include "Identifier/IdentifierHash.h"
#include "SCT_Cabling/SCT_OnlineId.h"

#include "GaudiKernel/EventIDRange.h"

SCT_DCSConditionsStatCondAlg::SCT_DCSConditionsStatCondAlg(const std::string& name, ISvcLocator* pSvcLocator)
  : ::AthAlgorithm(name, pSvcLocator)
  , m_readKeyHV{"/SCT/DCS/HV"}
  , m_readKeyState{"/SCT/DCS/CHANSTAT"}
  , m_writeKeyState{"SCT_DCSStatCondData"}
  , m_condSvc{"CondSvc", name}
  , m_readAllDBFolders{true}
  , m_returnHVTemp{true}
  , m_doState{true}
  , m_chanstatCut{"NORM"}
  , m_hvLowLimit{0.0}
  , m_hvUpLimit{1000000.0}
  , m_useHV{false}
  , m_useHVLowLimit{19.}
  , m_useHVUpLimit{1000000.0}
  , m_useHVChanCut{"LOOSE"}
{
  declareProperty("ReadAllDBFolders", m_readAllDBFolders);
  declareProperty("ReturnHVTemp", m_returnHVTemp);
  declareProperty("HVCutLow", m_hvLowLimit);
  declareProperty("HVCutUp", m_hvUpLimit);
  declareProperty("StateCut", m_chanstatCut);
  declareProperty("UseDefaultHV", m_useHV);
  declareProperty("useHVLow", m_useHVLowLimit);
  declareProperty("useHVUp", m_useHVUpLimit);
  declareProperty("useHVChan", m_useHVChanCut);
  
  declareProperty("ReadKeyHV", m_readKeyHV, "Key of input (raw) HV conditions folder");
  declareProperty("ReadKeyState", m_readKeyState, "Key of input (raw) State conditions folder");
  declareProperty("WriteKeyState", m_writeKeyState, "Key of output (derived) State conditions folder");
}

SCT_DCSConditionsStatCondAlg::~SCT_DCSConditionsStatCondAlg() {
}

StatusCode SCT_DCSConditionsStatCondAlg::initialize() {
  ATH_MSG_DEBUG("initialize " << name());

  m_doState = ((m_readAllDBFolders and m_returnHVTemp) or (not m_readAllDBFolders and not m_returnHVTemp));

  // CondSvc
  ATH_CHECK(m_condSvc.retrieve());

  if (m_returnHVTemp) {
    // Read Cond Handle (HV)
    ATH_CHECK(m_readKeyHV.initialize());
  }

  if (m_doState) {
    // Read Cond Handle (state)
    ATH_CHECK(m_readKeyState.initialize());
    // Write Cond Handle
    ATH_CHECK(m_writeKeyState.initialize());
    if(m_condSvc->regHandle(this, m_writeKeyState).isFailure()) {
      ATH_MSG_FATAL("unable to register WriteCondHandle " << m_writeKeyState.fullKey() << " with CondSvc");
      return StatusCode::FAILURE;
    }
  }

  if (m_useHV) {
    m_hvLowLimit = m_useHVLowLimit;
    m_hvUpLimit = m_useHVUpLimit;
    m_chanstatCut = m_useHVChanCut;
    ATH_MSG_INFO("Using HV and Chanstat"<< m_chanstatCut << " for marking modules bad. >=Hvlow: "
                 << m_hvLowLimit<< " and <=Hv Up: " <<  m_hvUpLimit <<
                 ". Note: UseHV Overrides hv limit and chanstat values in joboptions!!");
  }

  return StatusCode::SUCCESS;
}

StatusCode SCT_DCSConditionsStatCondAlg::execute() {
  ATH_MSG_DEBUG("execute " << name());

  if (not m_doState) {
    return StatusCode::SUCCESS;
  }

  // Write Cond Handle (state)
  SG::WriteCondHandle<SCT_DCSStatCondData> writeHandle{m_writeKeyState};
  // Do we have a valid Write Cond Handle for current time?
  if (writeHandle.isValid()) {
    // in theory this should never be called in MT
    writeHandle.updateStore();
    ATH_MSG_DEBUG("CondHandle " << writeHandle.fullKey() << " is already valid."
                  << ". In theory this should not be called, but may happen"
                  << " if multiple concurrent events are being processed out of order."
                  << " Forcing update of Store contents");
    return StatusCode::SUCCESS; 
  }

  // Read Cond Handle (state)
  SG::ReadCondHandle<CondAttrListCollection> readHandleState{m_readKeyState};
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
  SCT_DCSStatCondData* writeCdoState{new SCT_DCSStatCondData()};

  // Read state info
  std::string paramState{"STATE"};
  CondAttrListCollection::const_iterator attrListState{readCdoState->begin()};
  CondAttrListCollection::const_iterator endState{readCdoState->end()};
  // CondAttrListCollection doesn't support C++11 type loops, no generic 'begin'
  for (; attrListState!=endState; ++attrListState) {
    // A CondAttrListCollection is a map of ChanNum and AttributeList
    CondAttrListCollection::ChanNum channelNumber{attrListState->first};
    CondAttrListCollection::AttributeList payload{attrListState->second};
    if (payload.exists(paramState) and not payload[paramState].isNull()) {
      unsigned int val{payload[paramState].data<unsigned int>()};
      unsigned int hvstate{val bitand 240};
      unsigned int lvstate{val bitand 15};
      if (   ( (m_chanstatCut=="NORM")  and not ((hvstate==16 or hvstate==48)                                and (lvstate==1 or lvstate==3))                             )
          or ( (m_chanstatCut=="NSTBY") and not ((hvstate==16 or hvstate==48 or hvstate==32)                 and (lvstate==1 or lvstate==3 or lvstate==2))               )
          or ( (m_chanstatCut=="LOOSE") and not ((hvstate==16 or hvstate==48 or hvstate==32 or hvstate==128) and (lvstate==1 or lvstate==3 or lvstate==2 or lvstate==8)) )
             ) {
        writeCdoState->fill(channelNumber, paramState);
      } else {
        writeCdoState->remove(channelNumber, paramState);
      }
    } else {
      ATH_MSG_WARNING(paramState << " does not exist for ChanNum " << channelNumber);
    }
  }

  if (m_returnHVTemp) {
    // Read Cond Handle 
    SG::ReadCondHandle<CondAttrListCollection> readHandleHV{m_readKeyHV};
    const CondAttrListCollection* readCdoHV{*readHandleHV};
    if (readCdoHV==nullptr) {
      ATH_MSG_FATAL("Null pointer to the read conditions object (HV)");
      delete writeCdoState;
      return StatusCode::FAILURE;
    }
    // Get the validitiy range (HV)
    EventIDRange rangeHV;
    if (not readHandleHV.range(rangeHV)) {
      ATH_MSG_FATAL("Failed to retrieve validity range for " << readHandleHV.key());
      delete writeCdoState;
      return StatusCode::FAILURE;
    }
    ATH_MSG_INFO("Size of CondAttrListCollection " << readHandleHV.fullKey() << " readCdo->size()= " << readCdoHV->size());
    ATH_MSG_INFO("Range of HV input is " << rangeHV);

    // Combined the validity ranges of state and range
    EventIDRange rangeIntersection{EventIDRange::intersect(rangeState, rangeHV)};
    if(rangeIntersection.start()>rangeIntersection.stop()) {
      ATH_MSG_FATAL("Invalid intersection range: " << rangeIntersection);
      delete writeCdoState;
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
      CondAttrListCollection::AttributeList payload{attrListHV->second};
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
  if (writeHandle.record(rangeState, writeCdoState).isFailure()) {
    ATH_MSG_FATAL("Could not record SCT_DCSStatCondData " << writeHandle.key() 
                  << " with EventRange " << rangeState
                  << " into Conditions Store");
    delete writeCdoState;
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

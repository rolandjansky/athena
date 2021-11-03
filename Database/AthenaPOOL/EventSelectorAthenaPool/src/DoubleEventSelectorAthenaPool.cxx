/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

/**
 * @file DoubleEventSelectorAthenaPool.cxx
 * @brief This file contains the implementation for the DoubleEventSelectorAthenaPool class.
 * @author Peter van Gemmeren <gemmeren      -at- anl.gov>
 * @author John Detek Chapman <chapman       -at- hep.phy.cam.ac.uk>
 * @author Miha Muskinja      <miha.muskinja -at- cern.ch>
 * @author Tadej Novak        <tadej         -at- cern.ch>
 **/

#include "DoubleEventSelectorAthenaPool.h"
#include "PoolCollectionConverter.h"

// Framework
#include "GaudiKernel/FileIncident.h"

// Pool
#include "CollectionBase/ICollectionCursor.h"
#include "CollectionBase/CollectionRowBuffer.h"
#include "CollectionBase/TokenList.h"
#include "PersistentDataModel/DataHeader.h"
#include "PersistentDataModel/Token.h"


DoubleEventSelectorAthenaPool::DoubleEventSelectorAthenaPool(const std::string& name, ISvcLocator* pSvcLocator)
  : EventSelectorAthenaPool(name, pSvcLocator)
{
}


DoubleEventSelectorAthenaPool::~DoubleEventSelectorAthenaPool() {}


StatusCode DoubleEventSelectorAthenaPool::initialize()
{
  ATH_CHECK(EventSelectorAthenaPool::initialize());

  ATH_CHECK(m_secondarySelector.retrieve());
  if (dynamic_cast<EventSelectorAthenaPool *>(&*(m_secondarySelector)) == nullptr) {
    m_secondaryByteStream = true;
  }

  return StatusCode::SUCCESS;
}


StatusCode DoubleEventSelectorAthenaPool::next(IEvtSelector::Context& ctxt) const
{
  ATH_MSG_DEBUG("DoubleEventSelectorAthenaPool::next");

  std::lock_guard<CallMutex> lockGuard(m_callLock);

  for (const auto& tool : m_helperTools) {
    if (!tool->preNext().isSuccess()) {
        ATH_MSG_WARNING("Failed to preNext() " << tool->name());
    }
  }

  for (;;) {
    // Move in the primary file (with skipping)
    if (nextWithSkip(ctxt).isFailure()) {
      return StatusCode::FAILURE;
    }

    // Check if we're at the end of secondary file
    if (m_secondarySelector->nextWithSkip(ctxt).isFailure()) {
      return StatusCode::FAILURE;
    }

    // Record the attribute list
    if (!recordAttributeList().isSuccess()) {
      ATH_MSG_ERROR("Failed to record AttributeList.");
      return StatusCode::FAILURE;
    }

    StatusCode status = StatusCode::SUCCESS;
    for (const auto& tool : m_helperTools) {
      StatusCode toolStatus = tool->postNext();
      if (toolStatus.isRecoverable()) {
          ATH_MSG_INFO("Request skipping event from: " << tool->name());
          if (status.isSuccess()) {
            status = StatusCode::RECOVERABLE;
          }
      } else if (toolStatus.isFailure()) {
          ATH_MSG_WARNING("Failed to postNext() " << tool->name());
          status = StatusCode::FAILURE;
      }
    }
    if (status.isRecoverable()) {
      ATH_MSG_INFO("skipping event " << m_evtCount);
    } else if (status.isFailure()) {
      ATH_MSG_WARNING("Failed to postNext() HelperTool.");
    } else {
      if (!m_counterTool.empty() && !m_counterTool->postNext().isSuccess()) {
          ATH_MSG_WARNING("Failed to postNext() CounterTool.");
      }
      break;
    }
  }

  return StatusCode::SUCCESS;
}


StatusCode DoubleEventSelectorAthenaPool::next(IEvtSelector::Context& ctxt, int jump) const
{
  ATH_MSG_DEBUG("DoubleEventSelectorAthenaPool::next(jump)");

  if (jump > 0) {
    for (int i = 0; i < jump; i++) {
      if (!next(ctxt).isSuccess()) {
        return StatusCode::FAILURE;
      }
    }
    return StatusCode::SUCCESS;
  }

  return StatusCode::FAILURE;
}


StatusCode DoubleEventSelectorAthenaPool::seek(Context& ctxt, int evtNum) const
{
  ATH_MSG_DEBUG("DoubleEventSelectorAthenaPool::seek");

  ATH_CHECK(EventSelectorAthenaPool::seek(ctxt, evtNum));
  ATH_CHECK(m_secondarySelector->seek(ctxt, evtNum));

  return StatusCode::SUCCESS;
}


int DoubleEventSelectorAthenaPool::size(Context& ctxt) const
{
  ATH_MSG_DEBUG("DoubleEventSelectorAthenaPool::size");

  int sz1 = EventSelectorAthenaPool::size(ctxt);
  int sz2 = m_secondarySelector->size(ctxt);

  if (sz2 < sz1) {
    ATH_MSG_WARNING("Fewer secondary input events than primary input events. Expect trouble!");
  }

  return sz1;
}


StatusCode DoubleEventSelectorAthenaPool::recordAttributeList() const
{
  ATH_MSG_DEBUG("DoubleEventSelectorAthenaPool::recordAttributeList");

  // Get access to AttributeList
  ATH_MSG_DEBUG("Get AttributeList from the collection");
  // MN: accessing only attribute list, ignoring token list
  const coral::AttributeList& attrList = m_headerIterator->currentRow().attributeList();
  ATH_MSG_DEBUG("AttributeList size " << attrList.size());
  auto athAttrList = std::make_unique<AthenaAttributeList>(attrList);

  // Decide what to do based on the type of secondary file
  if (m_secondaryByteStream) {
    // Always add ByteStream as primary input
    ATH_CHECK(m_secondarySelector->fillAttributeList(athAttrList.get(), "", false));

    // Then fill the new attribute list from the primary file
    ATH_MSG_DEBUG("Append primary attribute list properties to the secondary one with a suffix: " << m_secondaryAttrListSuffix.value());
    ATH_CHECK(fillAttributeList(athAttrList.get(), "_" + m_secondaryAttrListSuffix.value(), true));
  } else {
    // Fill the new attribute list from the primary file
    ATH_CHECK(fillAttributeList(athAttrList.get(), "", false));

    // Fill the new attribute list from the secondary file
    ATH_MSG_DEBUG("Append secondary attribute list properties to the primary one with a suffix: " << m_secondaryAttrListSuffix.value());
    ATH_CHECK(m_secondarySelector->fillAttributeList(athAttrList.get(), "_" + m_secondaryAttrListSuffix.value(), true));
  }

  // Add info about secondary input
  athAttrList->extend("hasSecondaryInput", "bool");
  (*athAttrList)["hasSecondaryInput"].data<bool>() = true;

  SG::WriteHandle<AthenaAttributeList> wh(m_attrListKey.value(), eventStore()->name());
  if (!wh.record(std::move(athAttrList)).isSuccess()) {
    ATH_MSG_ERROR("Cannot record AttributeList to StoreGate " << StoreID::storeName(eventStore()->storeID()));
    return StatusCode::FAILURE;
  }

  return StatusCode::SUCCESS;
}


void DoubleEventSelectorAthenaPool::handle(const Incident& inc)
{
  ATH_MSG_DEBUG("DoubleEventSelectorAthenaPool::handle");

   if (not Atlas::hasExtendedEventContext(inc.context()) ) {
    ATH_MSG_WARNING("No extended event context available.");
    return;
  }

   SG::SourceID fid1;
   if (inc.type() == IncidentType::BeginProcessing) {
     if ( Atlas::hasExtendedEventContext(inc.context()) ) {
       fid1 = Atlas::getExtendedEventContext(inc.context()).proxy()->sourceID();
     }
     *m_sourceID1.get(inc.context()) = fid1;
   }
   else {
     fid1 = *m_sourceID1.get(inc.context());
   }

  if( fid1.empty() ) {
    ATH_MSG_WARNING("could not read event source ID from incident event context with key EventSelector");
    return;
  }

  ATH_MSG_DEBUG("**  MN Incident handler " << inc.type() << " Event source ID=" << fid1 );
  if( inc.type() == IncidentType::BeginProcessing ) {
    // increment the events-per-file counter for FID
    m_activeEventsPerSource[fid1]++;
  } else if( inc.type() == IncidentType::EndProcessing ) {
    m_activeEventsPerSource[fid1]--;
    disconnectIfFinished( fid1 );
    *m_sourceID1.get(inc.context()) = "";
  }
  if( msgLvl(MSG::DEBUG) ) {
    for( auto& source: m_activeEventsPerSource )
      msg(MSG::DEBUG) << "SourceID: " << source.first << " active events: " << source.second << endmsg;
  }

  // Nothing to do if secondary event selector is ByteStream
  if (m_secondaryByteStream) {
    return;
  }

  // Secondary guid
  SG::SourceID fid2;
  if (inc.type() == IncidentType::BeginProcessing) {
    if ( Atlas::hasExtendedEventContext(inc.context()) ) {
      fid2 = Atlas::getExtendedEventContext(inc.context()).proxy()->sourceID("SecondaryEventSelector");
    }
    *m_sourceID2.get(inc.context()) = fid2;
  }
  else {
    fid2 = *m_sourceID2.get(inc.context());
  }

  if( fid2.empty() ) {
    ATH_MSG_WARNING("could not read event source ID from incident event context with key SecondaryEventSelector");
    return;
  }

  ATH_MSG_DEBUG("**  MN Incident handler " << inc.type() << " Event source ID=" << fid2 );
  if( inc.type() == IncidentType::BeginProcessing ) {
    // increment the events-per-file counter for FID
    m_activeEventsPerSource[fid2]++;
  } else if( inc.type() == IncidentType::EndProcessing ) {
    m_activeEventsPerSource[fid2]--;
    m_secondarySelector->disconnectIfFinished( fid2 );
    *m_sourceID2.get(inc.context()) = "";
  }
  if( msgLvl(MSG::DEBUG) ) {
    for( auto& source: m_activeEventsPerSource )
      msg(MSG::DEBUG) << "SourceID: " << source.first << " active events: " << source.second << endmsg;
  }
}

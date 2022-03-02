/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/

/** @file StreamSelectorTool.cxx
 *  @brief This file contains the implementation for the StreamSelectorTool class.
 **/
#include <algorithm>

#include "GaudiKernel/ServiceHandle.h"
#include "StoreGate/StoreGateSvc.h"

#include "StreamSelectorTool.h"
#include "PersistentDataModel/AthenaAttributeList.h"
#include "PersistentDataModel/Token.h"
#include "PersistentDataModel/TokenAddress.h"
#include "PersistentDataModel/DataHeader.h"

#include "PoolSvc/IPoolSvc.h"

//___________________________________________________________________________
StreamSelectorTool::StreamSelectorTool(const std::string& type, const std::string& name, const IInterface* parent) : 
	AthAlgTool(type, name, parent)
{
   declareInterface<IAthenaSelectorTool>(this);
}
//___________________________________________________________________________
StreamSelectorTool::~StreamSelectorTool() {
}

//__________________________________________________________________________
StatusCode StreamSelectorTool::postNext() const {
   ATH_MSG_INFO("StreamSelectorTool::postNext");

   // Use attribute list to filter on selected stream, if it exists
   SG::ReadHandle<AthenaAttributeList> attrList(m_attrListKey.value(), evtStore()->name());
   if (attrList.isValid()) {
      if (attrList->specification().exists(m_streamName.value())) {
         if (!(*attrList)[m_streamName.value()].data<bool>()) {
            // Rejecting event
            ATH_MSG_DEBUG("Rejecting event");
            return(StatusCode::RECOVERABLE);
         }
      }
      // Accepting event
      ATH_MSG_DEBUG("Accepting event");
      if (attrList->specification().exists("eventRef")) {
         // Recording Extension DataHeader
         const std::string tokenStr = (*attrList)["eventRef"].data<std::string>();
         Token* token = new Token;
         token->fromString(tokenStr);
         token->setCont("POOLContainer_" + m_streamName.value() + "(DataHeader)");
         IOpaqueAddress* iop = new TokenAddress(POOL_StorageType, ClassID_traits<DataHeader>::ID(), "", m_streamName.value(), IPoolSvc::kInputStream, token);
         if (!evtStore()->recordAddress(iop).isSuccess()) {
            ATH_MSG_ERROR("Failed to record AthenaAttribute, name = " << token->contID() << ", eventRef = " << tokenStr);
            return(StatusCode::FAILURE);
         }
      }
   } else {
      ATH_MSG_ERROR("Could not find attribute list for " << m_attrListKey);
      return(StatusCode::FAILURE);
   }
   return(StatusCode::SUCCESS);
}

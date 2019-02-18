/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/** @file StreamSelectorTool.cxx
 *  @brief This file contains the implementation for the StreamSelectorTool class.
 *  $Id: StreamSelectorTool.cxx,v 1.5 2009-03-17 09:44:46 cranshaw Exp $
 **/

#include "StreamSelectorTool.h"
#include "PersistentDataModel/AthenaAttributeList.h"

//___________________________________________________________________________
StreamSelectorTool::StreamSelectorTool(const std::string& type, const std::string& name, const IInterface* parent) : 
	AthAlgTool(type, name, parent) 
{
   declareInterface<IAthenaSelectorTool>(this);
   declareProperty("AttributeListKey",    m_attrListKey = "Input");
   declareProperty("SelectedStream",    m_streamName = "NotSet");
}
//___________________________________________________________________________
StreamSelectorTool::~StreamSelectorTool() {
}

//__________________________________________________________________________
StatusCode StreamSelectorTool::postNext() const {
   ATH_MSG_INFO("StreamSelectorTool::postNext");
   StatusCode retc = StatusCode::SUCCESS;
   const DataHandle<AthenaAttributeList> attrList;
   ATH_MSG_INFO(evtStore()->dump());
   StatusCode sc = evtStore()->retrieve(attrList,m_attrListKey);
   if (sc.isSuccess()) {
      ATH_MSG_DEBUG("Found attribute list");
      // dump spec
      /*
      coral::AttributeList::const_iterator it = attrList->begin();
      while (it != attrList->end()) {
        //ATH_MSG_INFO("Spec element " << it->specification().name());
        std::ostringstream value;
        it->toOutputStream(value);
        ATH_MSG_INFO("Spec element " << value.str());
        ++it;
      }
      */
      // Search for stream name in attribute list, and check flag
      if ( attrList->specification().exists(m_streamName) ) {
         if ((*attrList)[m_streamName].data<bool>()!=true) {
            retc = StatusCode::RECOVERABLE;
            ATH_MSG_INFO("Rejecting event");
         }
      } else {
         ATH_MSG_ERROR("Stream decision for " << m_streamName << " does not exist in input");
      }
   }
   else {
      ATH_MSG_WARNING("Could not find attribute list for " << m_attrListKey);
   }
   return retc;
}
//__________________________________________________________________________
StatusCode StreamSelectorTool::preNext() const {
   return(StatusCode::SUCCESS);
}
//__________________________________________________________________________

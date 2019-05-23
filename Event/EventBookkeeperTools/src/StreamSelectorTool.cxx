/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

/** @file StreamSelectorTool.cxx
 *  @brief This file contains the implementation for the StreamSelectorTool class.
 *  $Id: StreamSelectorTool.cxx,v 1.5 2009-03-17 09:44:46 cranshaw Exp $
 **/
#include <algorithm>

#include "GaudiKernel/ServiceHandle.h"
#include "StoreGate/StoreGateSvc.h"

#include "StreamSelectorTool.h"
#include "PersistentDataModel/AthenaAttributeList.h"
#include "xAODCutFlow/CutBookkeeperContainer.h"

//___________________________________________________________________________
StreamSelectorTool::StreamSelectorTool(const std::string& type, const std::string& name, const IInterface* parent) : 
	AthAlgTool(type, name, parent),
        m_cutflow("CutFlowSvc/CutFlowSvc",name) 
{
   declareInterface<IAthenaSelectorTool>(this);
   declareProperty("CutFlowSvc", m_cutflow,
                   "handle to the ICutFlowSvc instance this filtering algorithm"
                   " will use for building the flow of cuts.");
   declareProperty("AcceptStreams",m_streamName,"Name of stream to be used as a ACCEPT, defaluts to ALL");
   declareProperty("VetoStreams",m_unstreamName,"Name of stream to be used as a VETO, defaluts to ALL");
}
//___________________________________________________________________________
StreamSelectorTool::~StreamSelectorTool() {
}

//__________________________________________________________________________
StatusCode StreamSelectorTool::postInitialize() {
   ATH_MSG_DEBUG("... postInitialize");

   ATH_CHECK(m_cutflow.retrieve());
   m_cutid = m_cutflow->registerFilter(this->name(),"Stream selection");
   if (0 == m_cutid) {
     ATH_MSG_WARNING("Unable to register filter tool");
   }

   return StatusCode::SUCCESS;
}
//__________________________________________________________________________
StatusCode StreamSelectorTool::postNext() const {
   ATH_MSG_INFO("StreamSelectorTool::postNext");

   StatusCode retc = StatusCode::SUCCESS;

   // if default, just return immediately
   //if (m_streamName.size()>0 && m_streamName[0]=="ALL") return retc;

   // Use attribute list to filter on selected stream
   const DataHandle<AthenaAttributeList> attrList;
   StatusCode sc = evtStore()->retrieve(attrList,m_attrListKey);
   if (sc.isSuccess()) {
      ATH_MSG_DEBUG("Found attribute list");
      // fill the input bookkeeper
      float weight = (*attrList)["EventWeight"].data<float>();
      // Search for stream name in attribute list, and check flag
      for (auto sel = m_streamName.begin(); sel != m_streamName.end(); ++sel) {
         // check veto
         if ( attrList->specification().exists(*sel) ) {
           bool dec(false);
           bool veto(false);
           // First check for vetoes
           for (auto ivet = m_unstreamName.begin(); ivet != m_unstreamName.end(); ++ivet) {
             try {
               if ((*attrList)[*ivet].data<bool>()) {
                 ATH_MSG_DEBUG("Stream " << *ivet << " vetoed event");
                 veto=true;
                 break; 
               }
             }
             catch (...) {
               ATH_MSG_WARNING("Unable to access " << m_streamName << " decision");
             }
           }
           // if not vetoed, then look for accept
           if (!veto) {
             try {
               dec = (*attrList)[*sel].data<bool>();
             }
             catch (...) {
               ATH_MSG_WARNING("Unable to access " << m_streamName << " decision");
             }
           } else {
             // exit loop as reject if already vetoed
             retc = StatusCode::RECOVERABLE;
             break;
           }
           // if not vetoed and decision found in attribute list 
           if (dec!=true) {
             // Rejecting event
             retc = StatusCode::RECOVERABLE;
             ATH_MSG_DEBUG("Rejecting event");
           } else {
             // Accepting event
             ATH_MSG_DEBUG("Accepting event");
             m_cutflow->addEvent(m_cutid,weight);
           }  
         } else {
           ATH_MSG_ERROR("Stream decision for " << m_streamName << " does not exist in input");
         }
      }
      // Get pointer to metadata store
      ServiceHandle<StoreGateSvc> mstore("StoreGateSvc/MetaDataStore",this->name());
      ATH_CHECK(mstore.retrieve());
      xAOD::CutBookkeeperContainer* fileBook = nullptr;
      if (mstore->retrieve(fileBook,m_cutflow->SGKey()).isSuccess())
      {
         xAOD::CutBookkeeperContainer::iterator it = fileBook->begin();
         xAOD::CutBookkeeperContainer::iterator ite = fileBook->end();
         while (it != ite) {
            if ((*it)->uniqueIdentifier()==m_cutid) {
               ATH_MSG_DEBUG(this->name()<<": For "<<m_streamName<<" cutflow events = " 
                                   << (*it)->nAcceptedEvents());
               break;
            }
            ++it;
         }
      }
   }
   else {
      ATH_MSG_WARNING("Could not find attribute list for " << m_attrListKey);
      ATH_MSG_ERROR("Cannot do attributelist bookkeeping");
   }
   return retc;
}
//__________________________________________________________________________
StatusCode StreamSelectorTool::preNext() const {
   return(StatusCode::SUCCESS);
}
//__________________________________________________________________________

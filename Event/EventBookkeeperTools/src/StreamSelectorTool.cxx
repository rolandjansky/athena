/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

/** @file StreamSelectorTool.cxx
 *  @brief This file contains the implementation for the StreamSelectorTool class.
 *  $Id: StreamSelectorTool.cxx,v 1.5 2009-03-17 09:44:46 cranshaw Exp $
 **/

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
   if (m_streamName=="ALL") return retc;

   // Use attribute list to filter on selected stream
   const DataHandle<AthenaAttributeList> attrList;
   StatusCode sc = evtStore()->retrieve(attrList,m_attrListKey);
   if (sc.isSuccess()) {
      ATH_MSG_DEBUG("Found attribute list");
      // fill the input bookkeeper
      float weight = (*attrList)["EventWeight"].data<float>();
      // Search for stream name in attribute list, and check flag
      if ( attrList->specification().exists(m_streamName) ) {
         bool dec(false);
         try {
           dec = (*attrList)[m_streamName].data<bool>();
         }
         catch (...) {
           ATH_MSG_WARNING("Unable to access " << m_streamName << " decision");
         }
         if (dec!=true) {
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
      //const std::pair<unsigned int, unsigned long long> re = std::make_pair(run,ev);
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

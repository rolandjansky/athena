/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/** @file MakeInputDataHeader.cxx
 *  @brief This file contains the implementation for the MakeInputDataHeader class.
 *  @author Peter van Gemmeren <gemmeren@anl.gov>
 *  $Id: MakeInputDataHeader.cxx,v 1.6 2008-06-26 17:00:22 gemmeren Exp $
 **/

#include "MakeInputDataHeader.h"

#include "StoreGate/StoreGateSvc.h"
#include "PersistentDataModel/DataHeader.h"

//___________________________________________________________________________
MakeInputDataHeader::MakeInputDataHeader(const std::string& name, ISvcLocator* pSvcLocator) 
	: ::AthAlgorithm(name, pSvcLocator), p_SGevent("StoreGateSvc", name) {
   // Declare the properties
   declareProperty("StreamName", m_streamName);
   declareProperty("KeepCurrentInput", m_keepInput = false);
}
//___________________________________________________________________________
MakeInputDataHeader::~MakeInputDataHeader() {
}
//___________________________________________________________________________
StatusCode MakeInputDataHeader::initialize() {
   ATH_MSG_INFO("Initializing " << name() << " - package version " << PACKAGE_VERSION);
   // Locate the StoreGateSvc
   if (!p_SGevent.retrieve().isSuccess()) {
      ATH_MSG_FATAL("Could not find StoreGateSvc");
      return(StatusCode::FAILURE);
   }
   // Print out the stream name
   ATH_MSG_INFO("Name of Stream to be made Input: " << m_streamName.value());
   return(StatusCode::SUCCESS);
}
//___________________________________________________________________________
StatusCode MakeInputDataHeader::execute() {
   const DataHandle<DataHeader> dh, dhEnd;
   if (!m_keepInput.value()) {
      if (!p_SGevent->retrieve(dh, dhEnd).isSuccess()) {
         ATH_MSG_FATAL("Could not find any DataHeader");
         return(StatusCode::FAILURE);
      }
      for ( ; dh != dhEnd; dh++) {
         if (p_SGevent->transientContains<DataHeader>(dh.key()) && dh->checkStatus(DataHeader::Primary)) {
            dh->setStatus(DataHeader::Input);
         }
      }
   }
   if (!p_SGevent->retrieve(dh, m_streamName.value()).isSuccess()) {
      ATH_MSG_FATAL("Could not find DataHeader: " << m_streamName.value());
      return(StatusCode::FAILURE);
   }
   dh->setStatus(DataHeader::Primary);
   return(StatusCode::SUCCESS);
}
//___________________________________________________________________________
StatusCode MakeInputDataHeader::finalize() {
   ATH_MSG_DEBUG("in finalize()");
   // Release the StoreGateSvc
   if (!p_SGevent.release().isSuccess()) {
      ATH_MSG_WARNING("Could not release StoreGateSvc");
   }
   return(StatusCode::SUCCESS);
}

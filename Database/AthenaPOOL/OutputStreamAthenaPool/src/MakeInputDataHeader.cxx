/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

/** @file MakeInputDataHeader.cxx
 *  @brief This file contains the implementation for the MakeInputDataHeader class.
 *  @author Peter van Gemmeren <gemmeren@anl.gov>
 **/

#include "MakeInputDataHeader.h"

#include "StoreGate/ReadHandle.h"
#include "PersistentDataModel/DataHeader.h"

//___________________________________________________________________________
MakeInputDataHeader::MakeInputDataHeader(const std::string& name, ISvcLocator* pSvcLocator) 
  : ::AthReentrantAlgorithm(name, pSvcLocator)
{
}
//___________________________________________________________________________
StatusCode MakeInputDataHeader::initialize() {

   // Print out the stream name
   ATH_MSG_INFO("Name of Stream to be made Input: " << m_streamName.key());

   m_aliasName = m_streamName.key() + "_Input";

   ATH_CHECK( m_streamName.initialize() );
   ATH_CHECK( m_aliasName.initialize() );
   return(StatusCode::SUCCESS);
}
//___________________________________________________________________________
StatusCode MakeInputDataHeader::execute (const EventContext& ctx) const
{
  SG::ReadHandle<DataHeader> dh (m_streamName, ctx);
  ATH_CHECK( dh.alias (m_aliasName) );
  return(StatusCode::SUCCESS);
}

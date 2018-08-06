/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/** @file MakeInputDataHeader.cxx
 *  @brief This file contains the implementation for the MakeInputDataHeader class.
 *  @author Peter van Gemmeren <gemmeren@anl.gov>
 *  $Id: MakeInputDataHeader.cxx,v 1.6 2008-06-26 17:00:22 gemmeren Exp $
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
MakeInputDataHeader::~MakeInputDataHeader() {
}
//___________________________________________________________________________
StatusCode MakeInputDataHeader::initialize() {
   ATH_MSG_INFO("Initializing " << name() << " - package version " << PACKAGE_VERSION);
   // Print out the stream name
    ATH_MSG_INFO("Name of Stream to be made Input: " << m_streamName.key());

   m_aliasName = m_streamName.key() + "_Input";

   ATH_CHECK( m_streamName.initialize() );
   ATH_CHECK( m_aliasName.initialize() );
   return(StatusCode::SUCCESS);
}
//___________________________________________________________________________
StatusCode MakeInputDataHeader::execute_r (const EventContext& ctx) const
{
  SG::ReadHandle<DataHeader> dh (m_streamName, ctx);
  ATH_CHECK( dh.alias (m_aliasName) );
  return(StatusCode::SUCCESS);
}
//___________________________________________________________________________
StatusCode MakeInputDataHeader::finalize() {
   ATH_MSG_DEBUG("in finalize()");
   return(StatusCode::SUCCESS);
}

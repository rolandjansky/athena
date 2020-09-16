/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

/** @file AthenaSharedWriter.cxx
 *  @brief This file contains the implementation for the AthenaSharedWriter class.
 *  @author Peter van Gemmeren <gemmeren@anl.gov>
 **/

#include "AthenaSharedWriter.h"
#include "AthenaKernel/IAthenaSharedWriterSvc.h"
#include "AthenaKernel/IDataShare.h"
#include "AthenaBaseComps/AthCnvSvc.h"

//___________________________________________________________________________
AthenaSharedWriter::AthenaSharedWriter(const std::string& name, ISvcLocator* pSvcLocator)
  : AthFilterAlgorithm(name, pSvcLocator) {
}
//___________________________________________________________________________
StatusCode AthenaSharedWriter::initialize() {
   ATH_MSG_INFO("in initialize()");
   // Initialize IConversionSvc
   ATH_CHECK(m_cnvSvc.retrieve());
   // Use IDataShare to make ConversionSvc a Share Server
   IDataShare* dataShare = dynamic_cast<IDataShare*>(m_cnvSvc.get());
   if (dataShare == nullptr || !dataShare->makeServer(-m_numberOfClients.value() - 1).isSuccess()) {
      ATH_MSG_FATAL("Could not make AthenaPoolCnvSvc a share server: " << dataShare);
      return StatusCode::FAILURE;
   } else {
      ATH_MSG_DEBUG("Successfully made the conversion service a share server");
   }
   // Initialize IAthenaSharedWriterSvc
   ATH_CHECK(m_sharedWriterSvc.retrieve());
   ATH_CHECK(m_sharedWriterSvc->share(m_numberOfClients.value()));
   return StatusCode::SUCCESS;
}
//___________________________________________________________________________
StatusCode AthenaSharedWriter::execute() {
   ATH_MSG_DEBUG("in execute()");
   setFilterPassed(false); // don't output events
   return StatusCode::SUCCESS;
}
//___________________________________________________________________________
StatusCode AthenaSharedWriter::finalize() {
   ATH_MSG_INFO("in finalize()");
   AthCnvSvc* cnvSvc = dynamic_cast<AthCnvSvc*>(m_cnvSvc.get());
   if (cnvSvc == nullptr || !cnvSvc->disconnectOutput("").isSuccess()) {
      ATH_MSG_FATAL("Could not disconnectOutput");
      return StatusCode::FAILURE;
   }
   // Release IAthenaSharedWriterSvc
   if (!m_sharedWriterSvc.release().isSuccess()) {
      ATH_MSG_WARNING("Could not release IAthenaSharedWriterSvc");
   }
   return StatusCode::SUCCESS;
}

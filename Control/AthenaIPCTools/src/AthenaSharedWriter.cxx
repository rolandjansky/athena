/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

/** @file AthenaSharedWriter.cxx
 *  @brief This file contains the implementation for the AthenaSharedWriter class.
 *  @author Peter van Gemmeren <gemmeren@anl.gov>
 **/

#include "AthenaBaseComps/AthCnvSvc.h"
#include "AthenaKernel/IDataShare.h"

#include "AthenaSharedWriter.h"

//___________________________________________________________________________
AthenaSharedWriter::AthenaSharedWriter(const std::string& name, ISvcLocator* pSvcLocator)
  : AthFilterAlgorithm(name, pSvcLocator) , m_cnvSvc(nullptr) {
}
//___________________________________________________________________________
AthenaSharedWriter::~AthenaSharedWriter() {
}
//___________________________________________________________________________
StatusCode AthenaSharedWriter::initialize() {
   ATH_MSG_INFO("in initialize()");

   StatusCode sc = serviceLocator()->service("AthenaPoolCnvSvc", m_cnvSvc);
   if (sc.isFailure() || m_cnvSvc == nullptr) {
      ATH_MSG_FATAL("Could not retrieve AthenaPoolCnvSvc");
      return StatusCode::FAILURE;
   }
   // Use IDataShare to make ConversionSvc a Share Server
   IDataShare* dataShare = dynamic_cast<IDataShare*>(m_cnvSvc);
   if (dataShare == nullptr || !dataShare->makeServer(-m_numberOfClients.value() - 1).isSuccess()) {
      ATH_MSG_FATAL("Could not make AthenaPoolCnvSvc a share server: " << dataShare);
      return StatusCode::FAILURE;
   } else {
      ATH_MSG_DEBUG("Successfully made the conversion service a share server");
   }
   return StatusCode::SUCCESS;
}
//___________________________________________________________________________
StatusCode AthenaSharedWriter::execute() {
   ATH_MSG_DEBUG("in execute()");
   int counter = 0;
   bool doCommit = false;
   StatusCode sc = m_cnvSvc->commitOutput("", false);
   ATH_MSG_VERBOSE("Start commitOutput loop");
   while (sc.isSuccess() || sc.isRecoverable()) {
      if (sc.isRecoverable()) {
         usleep(100);
      } else {
         counter++;
         if (m_autoSend.value() > 0 && counter% m_autoSend.value() == 0) {
            doCommit = true;
            ATH_MSG_INFO("commitOutput sending data.");
         } else {
            doCommit = false;
         }
      }
      sc = m_cnvSvc->commitOutput("", doCommit);
   }

   AthCnvSvc* cnvSvc = dynamic_cast<AthCnvSvc*>(m_cnvSvc);
   if (cnvSvc == nullptr || !cnvSvc->disconnectOutput("").isSuccess()) {
      ATH_MSG_FATAL("Could not disconnectOutput");
      return StatusCode::FAILURE;
   }

   setFilterPassed(false); // don't output events
   return StatusCode::SUCCESS;
}
//___________________________________________________________________________
StatusCode AthenaSharedWriter::finalize() {
   ATH_MSG_INFO("in finalize()");
   return StatusCode::SUCCESS;
}

/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

/** @file AthenaRootSharedWriter.cxx
 *  @brief This file contains the implementation for the AthenaRootSharedWriter class.
 *  @author Peter van Gemmeren <gemmeren@anl.gov>
 **/

#include "AthenaBaseComps/AthCnvSvc.h"
#include "AthenaKernel/IDataShare.h"

#include "AthenaRootSharedWriter.h"

#include "TFile.h"
#include "TFileMerger.h"
#include "TMessage.h"
#include "TMemFile.h"
#include "TMonitor.h"
#include "TServerSocket.h"
#include "TSocket.h"

/* Code from ROOT tutorials/net/parallelMergeServer.C, reduced to handle TTrees only */

struct ParallelFileMerger : public TObject
{
   TString       fFilename;
   TFileMerger   fMerger;

   ParallelFileMerger(const char *filename) : fFilename(filename), fMerger(kFALSE, kTRUE)
   {
      fMerger.OutputFile(filename, "RECREATE");
   }

   ~ParallelFileMerger()
   {
   }

   ULong_t  Hash() const
   {
      return fFilename.Hash();
   }

   const char* GetName() const
   {
      return fFilename;
   }

   Bool_t MergeTrees(TFile *input)
   {
      fMerger.AddFile(input);
      Bool_t result = fMerger.PartialMerge(TFileMerger::kIncremental | TFileMerger::kResetable);
      return result;
   }
};

//___________________________________________________________________________
AthenaRootSharedWriter::AthenaRootSharedWriter(const std::string& name, ISvcLocator* pSvcLocator)
  : AthFilterAlgorithm(name, pSvcLocator) , m_cnvSvc(nullptr) {
}
//___________________________________________________________________________
AthenaRootSharedWriter::~AthenaRootSharedWriter() {
}
//___________________________________________________________________________
StatusCode AthenaRootSharedWriter::initialize() {
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

   m_rootServerSocket = new TServerSocket(1095, true, 100);
   if (m_rootServerSocket == nullptr || !m_rootServerSocket->IsValid()) {
      ATH_MSG_FATAL("Could not create ROOT TServerSocket");
      return StatusCode::FAILURE;
   }
   m_rootMonitor = new TMonitor;
   m_rootMonitor->Add(m_rootServerSocket);
   ATH_MSG_DEBUG("Successfully created ROOT TServerSocket and added it to TMonitor: ready to accept connections!");

   return StatusCode::SUCCESS;
}
//___________________________________________________________________________
StatusCode AthenaRootSharedWriter::execute() {
   ATH_MSG_DEBUG("in execute()");
   int counter = 0;
   bool doCommit = false;
   StatusCode sc = m_cnvSvc->commitOutput("", false);
   ATH_MSG_VERBOSE("Start commitOutput loop");

   int clientIndex = 0;
   int clientCount = 0;
   THashTable mergers;

   while (sc.isSuccess() || sc.isRecoverable()) {
      if (sc.isRecoverable()) {
         ATH_MSG_VERBOSE("commitOutput not doing anything.");
         TSocket* socket = m_rootMonitor->Select(1);
         if (socket == nullptr || socket == (TSocket*)-1) {
            ATH_MSG_VERBOSE("ROOT Monitor did not do anything.");
         } else {
            ATH_MSG_DEBUG("ROOT Monitor got: " << socket);
            if (socket->IsA() == TServerSocket::Class()) {
               TSocket* client = ((TServerSocket*)socket)->Accept();
               client->Send(clientIndex, 0);
               client->Send(1, 1);
               ++clientIndex;
               ++clientCount;
               m_rootMonitor->Add(client);
               ATH_MSG_INFO("ROOT Monitor add client: " << clientIndex << ", " << client);
            } else {
               TMessage* message;
               socket->Recv(message);
               if (message == nullptr) {
                  ATH_MSG_WARNING("ROOT Monitor got no message from socket: " << socket);
               } else if (message->What() == kMESS_STRING) {
                  char str[64];
                  message->ReadString(str, 64);
                  ATH_MSG_INFO("ROOT Monitor client: " << socket << ", " << str);
                  m_rootMonitor->Remove(socket);
                  ATH_MSG_INFO("ROOT Monitor client: " << socket << ", " << socket->GetBytesRecv() << ", " << socket->GetBytesSent());
                  socket->Close();
                  --clientCount;
                  if (m_rootMonitor->GetActive() == 0 || clientCount == 0) {
                     ATH_MSG_INFO("ROOT Monitor: No more active clients...");
                  }
               } else if (message->What() == kMESS_ANY) {
                  long long length;
                  TString filename;
                  int clientId;
                  message->ReadInt(clientId);
                  message->ReadTString(filename);
                  message->ReadLong64(length);
                  ATH_MSG_DEBUG("ROOT Monitor client: " << socket << ", " << clientId << ": " << filename << ", " << length);
                  TMemFile *transient = new TMemFile(filename, message->Buffer() + message->Length(), length, "UPDATE");
                  message->SetBufferOffset(message->Length() + length);
                  ParallelFileMerger* info = static_cast<ParallelFileMerger*>(mergers.FindObject(filename));
                  if (!info) {
                     info = new ParallelFileMerger(filename);
                     mergers.Add(info);
                     ATH_MSG_DEBUG("ROOT Monitor ParallelFileMerger: " << info << ", for: " << filename);
                  }
                  info->MergeTrees(transient);
                  delete transient; transient = nullptr;
               }
            }
         }
      } else {
         counter++;
         if (m_autoSend.value() > 0 && counter% m_autoSend.value() == 0) {
            doCommit = true;
            ATH_MSG_INFO("commitOutput sending data.");
         } else {
            doCommit = false;
            ATH_MSG_DEBUG("commitOutput not sending data.");
         }
      }
      sc = m_cnvSvc->commitOutput("", doCommit);
   }
   ATH_MSG_INFO("End commitOutput loop");

   mergers.Delete();

   AthCnvSvc* cnvSvc = dynamic_cast<AthCnvSvc*>(m_cnvSvc);
   if (cnvSvc == nullptr || !cnvSvc->disconnectOutput("").isSuccess()) {
      ATH_MSG_FATAL("Could not disconnectOutput");
      return StatusCode::FAILURE;
   }

   setFilterPassed(false); // don't output events
   return StatusCode::SUCCESS;
}
//___________________________________________________________________________
StatusCode AthenaRootSharedWriter::finalize() {
   ATH_MSG_INFO("in finalize()");
   delete m_rootMonitor; m_rootMonitor = nullptr;
   delete m_rootServerSocket; m_rootServerSocket = nullptr;
   return StatusCode::SUCCESS;
}

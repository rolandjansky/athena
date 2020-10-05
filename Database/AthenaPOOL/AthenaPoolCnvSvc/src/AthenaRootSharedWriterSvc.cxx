/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

/** @file AthenaRootSharedWriterSvc.cxx
 *  @brief This file contains the implementation for the AthenaRootSharedWriterSvc class.
 *  @author Peter van Gemmeren <gemmeren@anl.gov>
 **/

#include "AthenaRootSharedWriterSvc.h"

#include "TBranch.h"
#include "TClass.h"
#include "TFile.h"
#include "TFileMerger.h"
#include "TKey.h"
#include "TLeaf.h"
#include "TMemFile.h"
#include "TMessage.h"
#include "TMonitor.h"
#include "TServerSocket.h"
#include "TSocket.h"
#include "TTree.h"

/// Definiton of a branch descriptor from RootTreeContainer
struct BranchDesc {
public:
   TClass* clazz;
   using dummy_ptr_t = std::unique_ptr<void, std::function<void(void*)> >;
   std::unique_ptr<void, std::function<void(void*)> > dummyptr;
   void* dummy = 0;

   BranchDesc(TClass* cl) : clazz(cl) {}

   void*     dummyAddr()
   {
      if (clazz) {
         std::function<void(void*)> del = std::bind(&TClass::Destructor, clazz, std::placeholders::_1, false);
         dummyptr = std::unique_ptr<void, std::function<void(void*)> >(clazz->New(), std::move(del));
         dummy = dummyptr.get();
         return &dummy;
      }
      return nullptr;
   }
};

/* Code from ROOT tutorials/net/parallelMergeServer.C, reduced to handle TTrees only */

struct ParallelFileMerger : public TObject
{
   TString       fFilename;
   TFileMerger   fMerger;

   ParallelFileMerger(const char *filename, int compress = ROOT::RCompressionSetting::EDefaults::kUseCompiledDefault) : fFilename(filename), fMerger(kFALSE, kTRUE)
   {
      fMerger.OutputFile(filename, "RECREATE", compress);
   }

   ~ParallelFileMerger()
   {
   }

   ULong_t Hash() const
   {
      return fFilename.Hash();
   }

   const char* GetName() const
   {
      return fFilename;
   }

// Add missing branches to client tree and BackFill before merging
   bool syncBranches(TTree* fromTree, TTree* toTree)
   {
      bool updated = false;
      const TObjArray* fromBranches = fromTree->GetListOfBranches();
      const TObjArray* toBranches = toTree->GetListOfBranches();
      int nBranches = fromBranches->GetEntriesFast();
      for (int k = 0; k < nBranches; ++k) {
         TBranch* branch = static_cast<TBranch*>(fromBranches->UncheckedAt(k));
         if (toBranches->FindObject(branch->GetName()) == nullptr) {
            TBranch* newBranch = nullptr;
            TClass* cl = TClass::GetClass(branch->GetClassName());
            BranchDesc desc(cl);
            void* empty = desc.dummyAddr();
            char buff[32];
            if (strlen(branch->GetClassName()) > 0) {
               newBranch = toTree->Branch(branch->GetName(), branch->GetClassName(), nullptr, branch->GetBasketSize(), branch->GetSplitLevel());
               newBranch->SetAddress(empty);
            } else {
               TObjArray* outLeaves = branch->GetListOfLeaves();
               TLeaf* leaf = static_cast<TLeaf*>(outLeaves->UncheckedAt(0));
               std::string type = leaf->GetTypeName();
               std::string attr = leaf->GetName();
               if (type == "Int_t") type = attr + "/I";
               else if (type == "Short_t") type = attr + "/S";
               else if (type == "Long_t") type = attr + "/L";
               else if (type == "UInt_t") type = attr + "/i";
               else if (type == "UShort_t") type = attr + "/s";
               else if (type == "UShort_t") type = attr + "/s";
               else if (type == "Float_t") type = attr + "/F";
               else if (type == "Double_t") type = attr + "/D";
               else if (type == "Char_t") type = attr + "/B";
               else if (type == "UChar_t") type = attr + "/b";
               else if (type == "Bool_t") type = attr + "/O";
               newBranch = toTree->Branch(branch->GetName(), buff, type.c_str(), 2048);
            }
            int nEntries = toTree->GetEntries();
            for (int m = 0; m < nEntries; ++m) {
               newBranch->BackFill();
            }
            updated = true;
         }
      }
      return updated;
   }

   Bool_t MergeTrees(TFile *input)
   {
      fMerger.AddFile(input);
      TTree* outCollTree = static_cast<TTree*>(fMerger.GetOutputFile()->Get("CollectionTree"));
      TTree* inCollTree = static_cast<TTree*>(input->Get("CollectionTree"));
      if (inCollTree != nullptr && outCollTree != nullptr) {
         if (syncBranches(outCollTree, inCollTree)) {
            input->Write();
         }
         if (syncBranches(inCollTree, outCollTree)) {
            fMerger.GetOutputFile()->Write();
         }
      }
      Bool_t result = fMerger.PartialMerge(TFileMerger::kIncremental | TFileMerger::kResetable | TFileMerger::kKeepCompression);
      TIter nextKey(input->GetListOfKeys());
      while (TKey* key = static_cast<TKey*>(nextKey())) {
         TClass* cl = TClass::GetClass(key->GetClassName());
         if (0 != cl->GetResetAfterMerge()) {
            key->Delete();
            input->GetListOfKeys()->Remove(key);
            delete key;
         }
      }
      return result;
   }
};

//___________________________________________________________________________
AthenaRootSharedWriterSvc::AthenaRootSharedWriterSvc(const std::string& name, ISvcLocator* pSvcLocator)
  : AthService(name, pSvcLocator)
  , m_rootServerSocket(nullptr), m_rootMonitor(nullptr), m_rootMergers(), m_rootClientIndex(0), m_rootClientCount(0) {
}
//___________________________________________________________________________
StatusCode AthenaRootSharedWriterSvc::initialize() {
   ATH_MSG_INFO("in initialize()");

   // Initialize IConversionSvc
   ATH_CHECK(m_cnvSvc.retrieve());
   IProperty* propertyServer = dynamic_cast<IProperty*>(m_cnvSvc.get());
   if (propertyServer == nullptr) {
      ATH_MSG_ERROR("Unable to cast conversion service to IProperty");
      return StatusCode::FAILURE;
   } else {
      std::string propertyName = "StreamMetaDataOnly";
      bool streamMetaDataOnly(false);
      BooleanProperty streamMetaDataOnlyProp(propertyName, streamMetaDataOnly);
      if (propertyServer->getProperty(&streamMetaDataOnlyProp).isFailure()) {
         ATH_MSG_INFO("Conversion service does not have StreamMetaDataOnly property");
      } else if(streamMetaDataOnlyProp.value()) {
         m_rootServerSocket = new TServerSocket(1095, true, 100);
         if (m_rootServerSocket == nullptr || !m_rootServerSocket->IsValid()) {
            ATH_MSG_FATAL("Could not create ROOT TServerSocket");
            return StatusCode::FAILURE;
         }
         m_rootMonitor = new TMonitor;
         m_rootMonitor->Add(m_rootServerSocket);
         ATH_MSG_DEBUG("Successfully created ROOT TServerSocket and added it to TMonitor: ready to accept connections!");
      }
   }

   return StatusCode::SUCCESS;
}
//___________________________________________________________________________
StatusCode AthenaRootSharedWriterSvc::share(int numClients) {
   ATH_MSG_VERBOSE("Start commitOutput loop");
   StatusCode sc = m_cnvSvc->commitOutput("", false);
   while (m_rootClientCount > 0 || (m_rootClientIndex < numClients && (sc.isSuccess() || sc.isRecoverable()))) {
      if (sc.isSuccess()) {
         ATH_MSG_VERBOSE("Success in commitOutput loop");
      } else if (m_rootMonitor != nullptr) {
         TSocket* socket = m_rootMonitor->Select(1);
         if (socket != nullptr && socket != (TSocket*)-1) {
            ATH_MSG_DEBUG("ROOT Monitor got: " << socket);
            if (socket->IsA() == TServerSocket::Class()) {
               TSocket* client = ((TServerSocket*)socket)->Accept();
               client->Send(m_rootClientIndex, 0);
               client->Send(1, 1);
               ++m_rootClientIndex;
               ++m_rootClientCount;
               m_rootMonitor->Add(client);
               ATH_MSG_INFO("ROOT Monitor add client: " << m_rootClientIndex << ", " << client);
            } else {
               TMessage* message = nullptr;
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
                  --m_rootClientCount;
                  if (m_rootMonitor->GetActive() == 0 || m_rootClientCount == 0) {
                     ATH_MSG_INFO("ROOT Monitor: No more active clients...");
                  }
               } else if (message->What() == kMESS_ANY) {
                  long long length;
                  TString filename;
                  int clientId;
                  message->ReadInt(clientId);
                  message->ReadTString(filename);
                  message->ReadLong64(length);
                  ATH_MSG_INFO("ROOT Monitor client: " << socket << ", " << clientId << ": " << filename << ", " << length);
                  std::unique_ptr<TMemFile> transient(new TMemFile(filename, message->Buffer() + message->Length(), length, "UPDATE"));
                  message->SetBufferOffset(message->Length() + length);
                  ParallelFileMerger* info = static_cast<ParallelFileMerger*>(m_rootMergers.FindObject(filename));
                  if (!info) {
                     info = new ParallelFileMerger(filename, transient->GetCompressionSettings());
                     m_rootMergers.Add(info);
                     ATH_MSG_INFO("ROOT Monitor ParallelFileMerger: " << info << ", for: " << filename);
                  }
                  info->MergeTrees(transient.get());
               }
               delete message; message = nullptr;
            }
         }
      } else if (m_rootMonitor == nullptr) {
         usleep(100);
      }
      sc = m_cnvSvc->commitOutput("", false);
   }
   ATH_MSG_INFO("End commitOutput loop");
   return StatusCode::SUCCESS;
}
//___________________________________________________________________________
StatusCode AthenaRootSharedWriterSvc::stop() {
   m_rootMergers.Delete();
   return StatusCode::SUCCESS;
}
//___________________________________________________________________________
StatusCode AthenaRootSharedWriterSvc::finalize() {
   ATH_MSG_INFO("in finalize()");
   delete m_rootMonitor; m_rootMonitor = nullptr;
   delete m_rootServerSocket; m_rootServerSocket = nullptr;
   return StatusCode::SUCCESS;
}
//___________________________________________________________________________
StatusCode AthenaRootSharedWriterSvc::queryInterface(const InterfaceID& riid, void** ppvInterface) {
   if ( IAthenaSharedWriterSvc::interfaceID().versionMatch(riid) ) {
      *ppvInterface = (IAthenaSharedWriterSvc*)this;
   } else {
      // Interface is not directly available: try out a base class
      return(AthService::queryInterface(riid, ppvInterface));
   }
   addRef();
   return(StatusCode::SUCCESS);
}

///////////////////////// -*- C++ -*- /////////////////////////////

/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

// RootNtupleOutputMetadataTool.cxx 
// Implementation file for class Athena::RootNtupleOutputMetadataTool
// Author Jack Cranshaw <cranshaw@anl.gov>
/////////////////////////////////////////////////////////////////// 

// AthenaRootComps includes
#include "RootNtupleOutputMetadataTool.h"
#include "RootBranchAddress.h"

// Gaudi
#include "GaudiKernel/IConversionSvc.h"
#include "GaudiKernel/IOpaqueAddress.h"
#include "GaudiKernel/IRegistry.h"
#include "GaudiKernel/DataObject.h"

// Athena
#include "AthenaKernel/IClassIDSvc.h"
#include "AthenaKernel/CLASS_DEF.h"
#include "AthenaRootKernel/IIoSvc.h"
#include "StoreGate/StoreGateSvc.h"
#include "SGTools/BuiltinsClids.h"

#include "TString.h"
#include "AthenaRootComps/TransferTree.h"

// ROOT includes
#include "TROOT.h"
#include "TFile.h"
#include "TTree.h"

namespace Athena {

RootNtupleOutputMetadataTool::RootNtupleOutputMetadataTool(const std::string& type,
                                           const std::string& name,
                                           const IInterface* parent) : 
  base_class(type, name, parent),
  m_imetaStore("InputMetaDataStore", name),
  m_ometaStore("MetaDataStore", name),
  m_clidSvc("ClassIDSvc", name), 
  m_metaWritten(false)
{
  // Properties
  declareProperty("InputStore", 
                  m_imetaStore,
                  "Input metadata store");
  declareProperty("OutputStore", 
                  m_ometaStore,
                  "Output metadata store");
  declareProperty("StreamName",
                  m_tupleName = "StreamNTUP_RED",
                  "Name of the output stream tool associated with FIXME");
  declareProperty("TupleName",
                  m_tupleName = "atlas_ntuple",
                  "Name of the output n-tuple FIXME");
  declareProperty("OutputFile",
                  m_outputName,
                  "Name of the output file FIXME");
}

RootNtupleOutputMetadataTool::~RootNtupleOutputMetadataTool() 
{}

StatusCode 
RootNtupleOutputMetadataTool::initialize()
{
  ATH_MSG_INFO("Initializing " << name() << " - package version " << PACKAGE_VERSION);

  if (!::AthAlgTool::initialize().isSuccess()) {
    ATH_MSG_FATAL("Cannot initialize AlgTool base class.");
    return(StatusCode::FAILURE);
  }
  // Get the ClassID service
  if (!m_clidSvc.retrieve().isSuccess()) {
    ATH_MSG_FATAL("Cannot get ClassID service via IClassIDSvc interface.");
    return(StatusCode::FAILURE);
  } else {
    ATH_MSG_DEBUG("Found ClassID service.");
  }
  // Get the storegate services
  if (!m_imetaStore.retrieve().isSuccess()) {
    ATH_MSG_FATAL("Failure retrieving input metadata store.");
    return(StatusCode::FAILURE);
  } else {
    ATH_MSG_DEBUG("Found input store.");
  }
  if (!m_ometaStore.retrieve().isSuccess()) {
    ATH_MSG_FATAL("Failure retrieving output metadata store.");
    return(StatusCode::FAILURE);
  } else {
    ATH_MSG_DEBUG("Found output store.");
  }
  ServiceHandle<IIncidentSvc> incSvc("IncidentSvc", this->name());
  if (!incSvc.retrieve().isSuccess()) {
    ATH_MSG_ERROR("Cannot get the IncidentSvc");
    return StatusCode::FAILURE;
  } else {
    ATH_MSG_INFO("Retrieved IncidentSvc");
  }
  incSvc->addListener(this, "BeginInputFile", 50);
  incSvc->addListener(this, "EndInputFile", 50);
  m_treesWritten.clear();

  return(StatusCode::SUCCESS);
}

StatusCode
RootNtupleOutputMetadataTool::stop()
{
  ATH_MSG_DEBUG("stop() for metadata write in Tool");
  // Make sure metadata is written
  if(writeMetadata().isFailure()) {
     ATH_MSG_ERROR("Could not connect for metadata writing");
     return StatusCode::RECOVERABLE;
  }

  return StatusCode::SUCCESS;
}

StatusCode
RootNtupleOutputMetadataTool::finalize()
{
  // Release the data store service
  if (m_imetaStore != 0) {
    if (!m_imetaStore.release().isSuccess()) {
      ATH_MSG_WARNING("Could not release " << m_imetaStore.type() << " store.");
    }
  }
  if (m_ometaStore != 0) {
    if (!m_ometaStore.release().isSuccess()) {
      ATH_MSG_WARNING("Could not release " << m_ometaStore.type() << " store.");
    }
  }
  // Release the ClassID service
  if (!m_clidSvc.release().isSuccess()) {
    ATH_MSG_WARNING("Cannot release ClassID service.");
  }
  return(::AthAlgTool::finalize());
}

StatusCode RootNtupleOutputMetadataTool::postInitialize() {return StatusCode::SUCCESS;}
StatusCode RootNtupleOutputMetadataTool::preFinalize()    {return StatusCode::SUCCESS;}
StatusCode RootNtupleOutputMetadataTool::preExecute()     {return StatusCode::SUCCESS;}
StatusCode RootNtupleOutputMetadataTool::preStream()      {return StatusCode::SUCCESS;}
StatusCode RootNtupleOutputMetadataTool::postExecute()    {return StatusCode::SUCCESS;}

void
RootNtupleOutputMetadataTool::handle(const Incident& inc)
{
  ATH_MSG_DEBUG("handle() incident type: " << inc.type());
  if (inc.type()=="BeginInputFile") {
    // Have to clean out any previous file metadata trees 
    SG::ConstIterator<TransferTree> titer; 
    SG::ConstIterator<TransferTree> tend; 
    StatusCode pc = m_ometaStore->retrieve(titer,tend); 
    if (pc.isSuccess()) { 
      for (; titer != tend; titer++) { 
        if (m_ometaStore->removeDataAndProxy(&*titer).isFailure()) { 
          ATH_MSG_ERROR("Unable to remove TransferTree after writing"); 
        } 
      } 
    } 
    m_treesWritten.clear(); 
  } 
  else if (inc.type()=="EndInputFile") {
     if(copyMetadata().isFailure()) ATH_MSG_ERROR("Could not copy input metadata");
     if(writeMetadata().isFailure()) ATH_MSG_ERROR("Could not connect for metadata writing");
  }
}

StatusCode
RootNtupleOutputMetadataTool::writeMetadata() 
{
  ATH_MSG_INFO("RootNtupleOutputMetadataTool::writeMetadata outputName = [" 
                  << m_outputName <<"]");
  // open the file thru the i/o svc
  ServiceHandle<IIoSvc> iosvc("IoSvc/AthIoSvc", name());
  if (!iosvc.retrieve().isSuccess()) {
    ATH_MSG_ERROR("could not retrieve the AthIoSvc");
    return StatusCode::FAILURE;
  }
  IIoSvc::Fd fd = iosvc->open(m_outputName, IIoSvc::RECREATE);
  if (fd < 0) {
    ATH_MSG_ERROR("could not open-recreate file [" << m_outputName << "]");
    return StatusCode::FAILURE;
  }
  if (!m_metaWritten) {
    // Write the strings
    SG::ConstIterator<std::string> iter;
    SG::ConstIterator<std::string> end;
    StatusCode pc = m_ometaStore->retrieve(iter,end);
    bool failure = false; 
    if (pc.isSuccess()) {
      for (; iter != end; iter++) {
        std::string key = iter.key();
        if (this->addMetadata(key,&(*iter),typeid(std::string)).isFailure()) failure=true;
      }
    }
    if (failure) {
      ATH_MSG_ERROR("Problem writing metadata");
      return StatusCode::FAILURE;
    }
    m_metaWritten = true;
  }
  SG::ConstIterator<TransferTree> titer;
  SG::ConstIterator<TransferTree> tend;
  StatusCode pc = m_ometaStore->retrieve(titer,tend);
  bool failure = false; 
  if (pc.isSuccess()) {
    for (; titer != tend; titer++) {
      std::string key = titer.key();
      if (m_treesWritten.find(key) == m_treesWritten.end()) {
        const TTree* x = (TTree*)titer->tree(); 
        try { 
          if (this->addMetadata(key,x,typeid(TTree)).isFailure()) failure=true; 
        } 
        catch (...) { 
          ATH_MSG_INFO("Error adding metadata for TTree " << key); 
        } 
        m_treesWritten.insert(key);
      }
      else {ATH_MSG_WARNING("Tree " << key << " already written");}
    }
  }
  if (failure) {
    ATH_MSG_ERROR("Problem writing metadata trees");
    return StatusCode::FAILURE;
  }

  return(StatusCode::SUCCESS);
}

StatusCode
RootNtupleOutputMetadataTool::copyMetadata() 
{
  SG::ConstIterator<std::string> iter;
  SG::ConstIterator<std::string> end;
  StatusCode pc = m_imetaStore->retrieve(iter,end);
  bool failure = false; 
  if (pc.isSuccess()) {
    for (; iter != end; iter++) {
      if (!m_ometaStore->contains<std::string>(iter.key())) {
        std::string* toCopy = new std::string(*iter);
        if (m_ometaStore->record(toCopy,iter.key()).isFailure()) failure=true;
      }
    }
  }
  if (failure) {
    ATH_MSG_ERROR("Problem copying metadata");
    return StatusCode::FAILURE;
  }
  SG::ConstIterator<TransferTree> titer;
  SG::ConstIterator<TransferTree> tend;
  pc = m_imetaStore->retrieve(titer,tend);
  failure = false; 
  if (pc.isSuccess()) {
    for (; titer != tend; titer++) {
      auto toCopy = std::make_unique<TransferTree>(*titer);
      if (!m_ometaStore->contains<TransferTree>(titer.key())) {
        if (m_ometaStore->record(std::move(toCopy),titer.key()).isFailure()) failure=true;
      }
      else ATH_MSG_INFO("Retrieve TTree with null pointer from input metadata store");
    }
  }
  if (failure) {
    ATH_MSG_ERROR("Problem copying metadata trees");
    return StatusCode::FAILURE;
  }

  return(StatusCode::SUCCESS);
}

StatusCode
RootNtupleOutputMetadataTool::addMetadata(const std::string& /*key*/, const void* obj, const std::type_info& /*ti*/) {
  // HACK for compilation, set m_file and m_tree = 0
  //TFile* m_file = 0;
  //TTree* m_tree = 0;
  // Reject null pointers
  if (obj==0) {
    REPORT_MESSAGE (MSG::ERROR) << "Attempt to write null pointer metadata";
    return StatusCode::FAILURE;
  }
  // Will crash past here anyway if m_file and m_tree are both null.
  std::abort();
#if 0
  // Determine directory to write into
  std::string metaname;
  TDirectory* dir = 0;
  if (m_tree != 0) {
    dir = m_tree->GetDirectory();
  }
  else {
    REPORT_MESSAGE (MSG::WARNING) << "No TTree to navigate from";
    dir = m_file->CurrentDirectory();
    if (dir == 0) {
      REPORT_MESSAGE (MSG::ERROR) << "No file directory to navigate from";
      return StatusCode::FAILURE;
    }
  }
  std::string thekey = key;
  std::string::size_type sep = key.find('/');
  if (sep != std::string::npos) {
    metaname = key.substr (0, sep);

    if (m_tree != 0) {
      // Go to the root directory of the file.
      while (dynamic_cast<TDirectoryFile*> (dir) != 0 &&
             dir->GetMotherDir())
        dir = dir->GetMotherDir();
      thekey = m_tree->GetName();
    }
  }
  else {
    if (m_tree != 0) {
      metaname = m_tree->GetName();
    }
    else {
      metaname = "eventless";
    }
    metaname += "Meta";
  }

  TDirectory::TContext ctx (dir);
  TDirectory* metadir = dir->GetDirectory (metaname.c_str());
  if (!metadir) {
    metadir = dir->mkdir (metaname.c_str());
    if (!metadir) {
      REPORT_MESSAGE (MSG::ERROR)
        << "Can't create metadata dir " << metaname
        << "in dir " << dir->GetName();
      return StatusCode::RECOVERABLE;
    }
  }

  // Set info and casts for appropriate type
  TClass* cls = gROOT->GetClass(ti);
  if (!cls)
    return StatusCode::RECOVERABLE;

  // If we're writing strings in a common directory, make sure name is unique.
  if (ti == typeid(TString) || ti == typeid(std::string)) {
    TObjString ostmp;
    if (ti == typeid(TString)) {
      ostmp.String() = *reinterpret_cast<const TString*> (obj);
      obj = &ostmp;
      cls = gROOT->GetClass ("TObjString");
    }
    else if (ti == typeid(std::string)) {
      ostmp.String() = *reinterpret_cast<const std::string*> (obj);
      obj = &ostmp;
      cls = gROOT->GetClass ("TObjString");
    }
    if (key.size() > 0 && key[key.size()-1] == '/') {
      int i = 1;
      while (metadir->FindObject (thekey.c_str())) {
        ++i;
        std::ostringstream ss;
        if (m_tree)
          ss << m_tree->GetName();
        else
          ss << dir->GetName();
        ss << "-" << i;
        thekey = ss.str();
      }
    }

    // Treat like hadd and create new version each time
    if (metadir->WriteObjectAny (obj, cls, thekey.c_str(), "new") == 0) {
      REPORT_MESSAGE (MSG::ERROR)
        << "Can't write metadata object " << thekey
        << " for file " << metadir->GetFile()->GetName();
      return StatusCode::RECOVERABLE;
    }
  }
  else if (ti == typeid(TTree)) {
    TTree* readTree = ((const TTree*)obj)->GetTree();
    metadir->cd();

    // If we're writing in a common directory, make sure name is unique.
    if (key.size() > 0) {
      TTree* outTree = (TTree*)metadir->FindObject (thekey.c_str());
      if (outTree == 0) {
        outTree = readTree->CloneTree();
      }
      else {
        TList tc;
        tc.Add((TObject*)readTree);
        Long64_t temp = outTree->Merge((TCollection*)&tc);
        if (temp==0) {
          REPORT_MESSAGE (MSG::ERROR) << "Unable to merge with existing tree in file";
          return StatusCode::RECOVERABLE;
        }
      }
      outTree->Write();
    }
    else {
      REPORT_MESSAGE (MSG::ERROR) << "Did not use proper key for metadata tree ";
      return StatusCode::RECOVERABLE;
    }
  }
  else {
    REPORT_MESSAGE (MSG::ERROR) << "addMetadata typeid not supported";
    return StatusCode::FAILURE;
  }
  return(StatusCode::SUCCESS);
#endif
}

}//> namespace Athena

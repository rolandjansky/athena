///////////////////////// -*- C++ -*- /////////////////////////////

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// RootNtupleOutputMetadataTool.cxx 
// Implementation file for class Athena::RootNtupleOutputMetadataTool
// Author Jack Cranshaw <cranshaw@anl.gov>
/////////////////////////////////////////////////////////////////// 

// AthenaRootComps includes
#include "RootNtupleOutputMetadataTool.h"
#include "RootSvc.h"
#include "RootConnection.h"
#include "RootBranchAddress.h"

// stl
#include "CxxUtils/unordered_set.h" // move to stl
#include "CxxUtils/make_unique.h"

// Gaudi
#include "GaudiKernel/IConversionSvc.h"
#include "GaudiKernel/IOpaqueAddress.h"
#include "GaudiKernel/IRegistry.h"
#include "GaudiKernel/DataObject.h"
#include "GaudiKernel/IJobOptionsSvc.h"

// Athena
#include "AthenaKernel/IClassIDSvc.h"
#include "AthenaRootKernel/IIoSvc.h"
#include "StoreGate/StoreGateSvc.h"
#include "SGTools/BuiltinsClids.h"
#include "SGTools/CLASS_DEF.h"

#include "TString.h"
#include "AthenaRootComps/TransferTree.h"

namespace Athena {

RootNtupleOutputMetadataTool::RootNtupleOutputMetadataTool(const std::string& type,
                                           const std::string& name,
                                           const IInterface* parent) : 
  ::AthAlgTool(type, name, parent),
  m_imetaStore("InputMetaDataStore", name),
  m_ometaStore("MetaDataStore", name),
  m_clidSvc("ClassIDSvc", name), 
  m_metaWritten(false)
{
  // Declare IAthenaOutputStreamTool interface
  declareInterface<IAthenaOutputTool>(this);
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
StatusCode RootNtupleOutputMetadataTool::postExecute()    {return StatusCode::SUCCESS;}

void
RootNtupleOutputMetadataTool::handle(const Incident& inc)
{
  ATH_MSG_DEBUG("handle() incident type: " << inc.type());
  if (inc.type()=="BeginInputFile") {
    // Have to clean out any previous file metadata trees 
    const DataHandle<TransferTree> titer; 
    const DataHandle<TransferTree> tend; 
    StatusCode pc = m_ometaStore->retrieve(titer,tend); 
    if (pc.isSuccess()) { 
      for (; titer != tend; titer++) { 
        if (titer.cptr()!=0) { 
          if (m_ometaStore->removeDataAndProxy(titer.cptr()).isFailure()) { 
            ATH_MSG_ERROR("Unable to remove TransferTree after writing"); 
          } 
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
  ServiceHandle<IRootSvc> rootsvc("Athena::RootSvc/AthenaRootSvc", name());
  if (!rootsvc.retrieve().isSuccess()) {
    ATH_MSG_ERROR("could not retrieve the AthenaRootSvc");
    return StatusCode::FAILURE;
  }
  // create a RootConnection
  Athena::RootSvc* rsvc = dynamic_cast<Athena::RootSvc*>(&*rootsvc);
  if (!rsvc) {
    ATH_MSG_ERROR("could not dyn-cast to Athena::RootSvc");
    return StatusCode::FAILURE;
  }
  // connection should already be part of rootsvc from output stream
  Athena::RootConnection* conn = rsvc->connection(fd);
  if (conn == NULL) {
    // Try to make the connection
    if (rsvc->connect(fd).isSuccess()) conn = rsvc->connection(fd);
    if (conn == NULL) {
      // OK, can't get to the file
      ATH_MSG_ERROR("connection for fd=" << fd 
                     << " fname=[" << m_outputName << "] not available from RootSvc");
      return StatusCode::FAILURE;
    }
    conn->setTreeName(m_tupleName);
  }
  else {
    ATH_MSG_INFO("connection established for " << m_outputName);
  }
  if (!m_metaWritten) {
    // Write the strings
    const DataHandle<std::string> iter;
    const DataHandle<std::string> end;
    StatusCode pc = m_ometaStore->retrieve(iter,end);
    bool failure = false; 
    if (pc.isSuccess()) {
      for (; iter != end; iter++) {
        std::string key = iter.key();
        if (conn->addMetadata(key,&(*iter),typeid(std::string)).isFailure()) failure=true;
      }
    }
    if (failure) {
      ATH_MSG_ERROR("Problem writing metadata");
      return StatusCode::FAILURE;
    }
    m_metaWritten = true;
  }
  const DataHandle<TransferTree> titer;
  const DataHandle<TransferTree> tend;
  StatusCode pc = m_ometaStore->retrieve(titer,tend);
  bool failure = false; 
  if (pc.isSuccess()) {
    for (; titer != tend; titer++) {
      std::string key = titer.key();
      if (m_treesWritten.find(key) == m_treesWritten.end()) {
        if (titer.cptr()!=0) {
          const TTree* x = (TTree*)titer.cptr()->tree(); 
          try { 
            if (conn->addMetadata(key,x,typeid(TTree)).isFailure()) failure=true; 
          } 
          catch (...) { 
            ATH_MSG_INFO("Error adding metadata for TTree " << key); 
          } 
        }
        else ATH_MSG_INFO("Retrieve TTree with null pointer from input metadata store");
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
  const DataHandle<std::string> iter;
  const DataHandle<std::string> end;
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
  const DataHandle<TransferTree> titer;
  const DataHandle<TransferTree> tend;
  pc = m_imetaStore->retrieve(titer,tend);
  failure = false; 
  if (pc.isSuccess()) {
    for (; titer != tend; titer++) {
      if (titer.cptr()!=0) {
        auto toCopy = CxxUtils::make_unique<TransferTree>(*titer);
        if (!m_ometaStore->contains<TransferTree>(titer.key())) {
          if (m_ometaStore->record(std::move(toCopy),titer.key()).isFailure()) failure=true;
        }
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

}//> namespace Athena

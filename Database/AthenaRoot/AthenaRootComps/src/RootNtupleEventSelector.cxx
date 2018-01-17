///////////////////////// -*- C++ -*- /////////////////////////////

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// RootNtupleEventSelector.cxx 
// Implementation file for class RootNtupleEventSelector
// Author: S.Binet<binet@cern.ch>
/////////////////////////////////////////////////////////////////// 

// STL includes
#include <sstream>
#include <string>
#include <vector>
#include <stdint.h>
#include <unordered_map>
#include <unordered_set>

// ROOT includes
#include "TROOT.h"
#include "RootUtils/TBranchElementClang.h"
#include "TClass.h"
#include "TClassEdit.h"
#include "TFile.h"
#include "TKey.h"
#include "TLeaf.h"

// Framework includes
//#include "GaudiKernel/GenericAddress.h"
#include "GaudiKernel/FileIncident.h"
#include "GaudiKernel/IIoComponentMgr.h"
#include "GaudiKernel/ISvcLocator.h"
#include "GaudiKernel/ITHistSvc.h"
#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/StatusCode.h"
#include "GaudiKernel/System.h"
#include "AthenaKernel/IClassIDSvc.h"
#include "AthenaKernel/IDictLoaderSvc.h"

// StoreGate includes

#include "SGTools/BuiltinsClids.h"   // to make sure we have their clids
#include "SGTools/CLASS_DEF.h"
#include "SGTools/StlMapClids.h"     // to make sure we have their clids
#include "SGTools/StlVectorClids.h"  // to make sure we have their clids
#include "SGTools/TransientAddress.h"
#include "StoreGate/StoreGate.h" 

#include "TObject.h"
#include "TTree.h"
CLASS_DEF( TObject,    74939790 , 1 )
#include "AthenaRootComps/TransferTree.h"

// EventInfo includes
#include "EventInfo/EventInfo.h"
#include "EventInfo/EventType.h"
#include "EventInfo/EventID.h"
#include "xAODEventInfo/EventInfo.h"
#include "xAODEventInfo/EventAuxInfo.h"


// Package includes
#include "RootNtupleEventSelector.h"
#include "RootBranchAddress.h"
#include "RootGlobalsRestore.h"

#include "boost/tokenizer.hpp"

namespace {
  std::string
  root_typename(const std::string& root_type_name)
  {
    static std::unordered_map<std::string,std::string> s;
    static bool first = true;
    if (first) {
      first = false;

      s["Int_t"] = System::typeinfoName(typeid(Int_t));
      s["UInt_t"] = System::typeinfoName(typeid(UInt_t));

      s["Long_t"] = System::typeinfoName(typeid(Long_t));
      s["ULong_t"] = System::typeinfoName(typeid(ULong_t));

      s["Long64_t"] = System::typeinfoName(typeid(Long64_t));
      s["ULong64_t"] = System::typeinfoName(typeid(ULong64_t));

      s["Float_t"] = System::typeinfoName(typeid(Float_t));
      s["Float16_t"] = System::typeinfoName(typeid(Float16_t));
      s["Double_t"] = System::typeinfoName(typeid(Double_t));
      s["Double32_t"] = System::typeinfoName(typeid(Double32_t));
      
      s["Bool_t"] = System::typeinfoName(typeid(Bool_t));
      s["Char_t"] = System::typeinfoName(typeid(Char_t));
      s["UChar_t"] = System::typeinfoName(typeid(UChar_t));
      
      s["Short_t"] = System::typeinfoName(typeid(Short_t));
      s["UShort_t"] = System::typeinfoName(typeid(UShort_t));

    }
    return s[root_type_name];
  }

#if 0
  std::vector<std::string>
  get_active_leaves(TTree *tuple) 
  {
    std::vector<std::string> active;
    TObjArray *leaves = tuple->GetListOfLeaves(); 
    if (leaves) { 
      // loop over leaves 
      for (Int_t i = 0; i < leaves->GetEntries(); ++i) { 
        TLeaf *leaf = (TLeaf *)leaves->At(i); 
        TBranch *branch = leaf->GetBranch(); 
        if (branch) { 
          const char *brname = branch->GetName(); 
          if (tuple->GetBranchStatus(brname)) { 
            active.push_back(std::string(brname));
          } 
        }
      }
    }
    return active;
  }
#endif

}

namespace Athena {

/** @class RootNtupleEventContext 
 *  ROOT specific event selector context
 */
class RootNtupleEventContext : 
    public ::IEvtSelector::Context
{
public:
  /// definition of the file container
  typedef std::vector<std::string> FileNames_t;

private:
  /// reference to the hosting event selector instance
  const RootNtupleEventSelector* m_evtsel;

  /// the file container managed by this context
  //FileNames_t m_files;

  /// current collection index (into `m_inputCollectionsName`)
  long m_collIdx;

  /// current tuple index (into `m_tupleNames')
  long m_tupleIdx;

  /// current entry of current file
  //int64_t m_entry;

  /// reference to top-level tree
  //TTree* m_tree;

  /// connection FID
  std::string m_fid;

public:
  
  /// standard c-tor with initialization
  RootNtupleEventContext(const RootNtupleEventSelector* sel) :
    m_evtsel(sel),
    //m_files(),
    m_collIdx(0),
    m_tupleIdx(0),
    //m_entry(-1),
    //m_tree(NULL),
    m_fid("")
  {}

  /// standard d-tor
  virtual ~RootNtupleEventContext() {}

  // access to the container of files
  const FileNames_t& files() const 
  { return m_evtsel->m_inputCollectionsName.value(); }

  // /// set the container of files
  // void setFiles(const FileNames_t& fnames) 
  // {
  //   //m_files = fnames;
  //   //m_fidx = 0;
  // }

  /// context identifier
  virtual void* identifier() const 
  { return (void*)(m_evtsel); }

  /// access to the file iterator
  std::size_t fileIndex() const
  { return m_collIdx; }

  /// set file iterator
  void setFileIndex(std::size_t idx) 
  { m_collIdx = idx; }

  std::size_t tupleIndex() const
  { return m_tupleIdx; }

  void setTupleIndex(std::size_t idx) 
  { m_tupleIdx = idx; }

  /// access to the current event entry number
  int64_t entry() const { return m_evtsel->m_curEvt; }

  // /// set the current event entry number
  // void setEntry(int64_t entry) { m_entry = entry; }

  /// set connection FID
  void setFID(const std::string& fid) { m_fid = fid; }

  /// access to the connection FID
  const std::string& fid() const { return m_fid; }

  /// access to the tree used to iterate
  TTree* tree() const { return m_evtsel->m_tuple; }

  /// set the tree used to iterate
  void setTree(TTree* tree) {
    // make sure we clean-up and close the file holding
    // the previous tree, if any.
    // dont close if it is the same tree though!
    TTree *cur = m_evtsel->m_tuple;
    if (cur && tree != cur) {
      TFile *old_file = cur->GetCurrentFile();
      if (old_file) {
        old_file->Close();
      }
    }

    m_evtsel->m_tuple = tree; 
  }
};

  typedef RootNtupleEventContext::FileNames_t FileNames_t;

/////////////////////////////////////////////////////////////////// 
// Public methods: 
/////////////////////////////////////////////////////////////////// 

// Constructors
////////////////

RootNtupleEventSelector::RootNtupleEventSelector( const std::string& name,
                                                  ISvcLocator* svcLoc ) :
  extends ( name,    svcLoc ),
  m_dataStore( "StoreGateSvc/StoreGateSvc", name ),
  m_imetaStore( "StoreGateSvc/InputMetaDataStore", name ),
  m_ometaStore( "StoreGateSvc/MetaDataStore", name ),
  m_clidsvc  ( "ClassIDSvc",   name ),
  m_dictsvc  ( "AthDictLoaderSvc", name ),
  m_incsvc   ( "IncidentSvc", name ),
  m_nbrEvts  ( 0 ),
  m_curEvt   ( 0 ),
  m_collEvts (   ),
  m_tuple    (NULL),
  m_needReload (true),
  m_fireBIF  (true)
{
  declareProperty( "DataStore",
		   m_dataStore,
		   "Store where to publish data");

  declareProperty( "InputMetaStore",
		   m_imetaStore,
		   "Store where to publish (input) metadata");

  declareProperty( "MetaStore",
		   m_ometaStore,
		   "Store where to publish (output) metadata");

  declareProperty( "InputCollections", 
                   m_inputCollectionsName,
                   "List of input (ROOT) file names" );
  m_inputCollectionsName.declareUpdateHandler
    ( &RootNtupleEventSelector::setupInputCollection, this );

  declareProperty( "TupleName",
                   m_tupleName = "CollectionTree",
                   "Name of the TTree to load/read from input file(s).  "
                   "May be a semicolon-separated string to read multiple TTrees.");

  declareProperty( "SkipEvents",           
                   m_skipEvts = 0,
                   "Number of events to skip at the beginning" );

  declareProperty( "ActiveBranches",
                   m_activeBranchNames,
                   "List of branch names to activate" );
}

// Destructor
///////////////
RootNtupleEventSelector::~RootNtupleEventSelector()
{}

StatusCode RootNtupleEventSelector::initialize()
{
  ATH_MSG_INFO ("Enter RootNtupleEventSelector initialization...");

  // retrieve clidsvc
  if ( !m_clidsvc.retrieve().isSuccess() ) {
    ATH_MSG_ERROR
      ("Could not retrieve [" << m_clidsvc.typeAndName() << "]");
    return StatusCode::FAILURE;
  }

  // retrieve dictsvc
  if ( !m_dictsvc.retrieve().isSuccess() ) {
    ATH_MSG_ERROR
      ("Could not retrieve [" << m_dictsvc.typeAndName() << "]");
    return StatusCode::FAILURE;
  }

  typedef boost::tokenizer<boost::char_separator<char> > tokenizer;
  boost::char_separator<char> sep (" ;");
  tokenizer tokens (m_tupleName.value(), sep);
  m_tupleNames.assign (tokens.begin(), tokens.end());

  if ( m_tupleNames.empty() ) {
    ATH_MSG_ERROR
      ("You have to give a TTree name to read from the ROOT files !");
    return StatusCode::FAILURE;
  }

  CHECK( m_incsvc.retrieve() );
   m_incsvc->addListener(this,IncidentType::BeginEvent,99); //used to trigger BeginInputFile on start of first event of file - 99 priority so AFTER storegatesvc done


  setupInputCollection( m_inputCollectionsName );
  const std::size_t nbrInputFiles = m_inputCollectionsName.value().size();
  if ( nbrInputFiles < 1 ) {
    ATH_MSG_ERROR
      ("You need to give at least 1 input file !!" << endmsg
       << "(Got [" << nbrInputFiles << "] file instead !)");
    return StatusCode::FAILURE;
  } else {
    ATH_MSG_INFO
      ("Selector configured to read [" << nbrInputFiles << "] file(s)..."
       << endmsg
       << "                      TTree [" << m_tupleName.value() << "]");
  }

  {
    // register this service for 'I/O' events
    ServiceHandle<IIoComponentMgr> iomgr("IoComponentMgr", name());
    if (!iomgr.retrieve().isSuccess()) {
      ATH_MSG_FATAL("Could not retrieve IoComponentMgr !");
      return StatusCode::FAILURE;
    }
    if (!iomgr->io_register(this).isSuccess()) {
      ATH_MSG_FATAL("Could not register myself with the IoComponentMgr !");
      return StatusCode::FAILURE;
    }
    // register input file's names with the I/O manager
    const std::vector<std::string>& incol = m_inputCollectionsName.value();
    bool allGood = true;
    for (std::size_t icol = 0, imax = incol.size(); icol < imax; icol++) {
      if (!iomgr->io_register(this, 
                              IIoComponentMgr::IoMode::READ, 
                              incol[icol]).isSuccess()) {
        ATH_MSG_FATAL("could not register [" << incol[icol] << "] for output !");
        allGood = false;
      } else {
        ATH_MSG_VERBOSE("io_register[" << this->name() << "](" << incol[icol] << ") [ok]");
      }
    }
    if (!allGood) {
      return StatusCode::FAILURE;
    }
  }

  if (!do_init_io().isSuccess()) {
    return StatusCode::FAILURE;
  }

  // retrieve event store
  // this needs to happen *after* having initialized the i/o
  // as our branches (which need a valid m_ntuple pointer)
  // may be asked to be registered as we are a ProxyProvider.
  // retrieving the event store will poke the ProxyProviderSvc...
  /*
  if ( !m_dataStore.retrieve().isSuccess() ) {
    ATH_MSG_ERROR
      ("Could not retrieve [" << m_dataStore.typeAndName() << "] !!");
    return StatusCode::FAILURE;
  }

  // ditto for (input) meta data store
  if (!m_imetaStore.retrieve().isSuccess()) {
    ATH_MSG_ERROR
      ("Could not retrieve [" << m_imetaStore.typeAndName() << "] !!");
    return StatusCode::FAILURE;
  }

  // ditto for (output) meta data store
  if (!m_ometaStore.retrieve().isSuccess()) {
    ATH_MSG_ERROR
      ("Could not retrieve [" << m_ometaStore.typeAndName() << "] !!");
    return StatusCode::FAILURE;
  }
  */

  //ensure the Athena::NtupleCnvSvc is in the EventPersistencySvc
  ServiceHandle<IProperty> epSvc("EventPersistencySvc",name());
  std::vector<std::string> propVal;
  CHECK( Gaudi::Parsers::parse( propVal , epSvc->getProperty("CnvServices").toString() ) );
  bool foundSvc(false);
  for(auto s : propVal) {
    if(s=="Athena::xAODCnvSvc") { foundSvc=true; break; }
  }
  if(!foundSvc) {
    propVal.push_back("Athena::NtupleCnvSvc");
    CHECK( epSvc->setProperty("CnvServices", Gaudi::Utils::toString( propVal ) ));
  }

  //we should also add ourself as a proxy provider
  ServiceHandle<IProxyProviderSvc> ppSvc("ProxyProviderSvc",name());
  CHECK( ppSvc.retrieve() );
  ppSvc->addProvider( this );



  return StatusCode::SUCCESS;
}

StatusCode RootNtupleEventSelector::finalize()
{
  ATH_MSG_INFO ("Finalize...");
  // FIXME: this should be tweaked/updated if/when a selection function
  //        or filtering predicate is applied (one day?)
  ATH_MSG_INFO ("Total events read: " << (m_nbrEvts - m_skipEvts));

  // Explicitly delete all the files we created.
  // If we leave it up to root, then xrootd can get cleaned up before
  // the root destructors run, leading to a crash.
  for (TFile* f : m_files)
    delete f;
  m_files.clear();

  return StatusCode::SUCCESS;
}

/////////////////////////////////////////////////////////////////// 
// Const methods: 
///////////////////////////////////////////////////////////////////

StatusCode RootNtupleEventSelector::endInputFile (RootNtupleEventContext* rctx) const
{
  const FileNames_t& fnames = rctx->files();
  std::size_t fidx = rctx->fileIndex();
  m_incsvc->fireIncident(FileIncident(name(), "EndInputFile", fnames[fidx]));

  // prepare for next file, if any...
  // std::cout << "=========================================================="
  //           << std::endl;
  // std::cerr << "::switch to next file...\n";

  // iterate over proxies and
  // mark as garbage and drop the RootBranchAddress (as a side effect of
  // ::setAddress(NULL).
  // this way, the next time we hit ::createRootBranchAddress or ::updateAddress
  // all internal states are kosher.
  for (const SG::DataProxy* cdp : m_dataStore->proxies()) {
    if (dynamic_cast<Athena::RootBranchAddress*> (cdp->address()) != nullptr) {
      if (SG::DataProxy* dp = m_dataStore->proxy_exact (cdp->sgkey())) {
        dp->setAddress (nullptr);
      }
    }
  }

  const bool forceRemove = false;
  CHECK( m_dataStore->clearStore(forceRemove) ); //must clear the storegate so that any tampering user did in EndInputFile incident is cleared
  m_needReload = true;m_fireBIF=true;

  return StatusCode::SUCCESS;
}

StatusCode
RootNtupleEventSelector::next( IEvtSelector::Context& ctx ) const
{
  // std::cout << "::next(fidx=" << ctx->fileIndex() << ", eidx=" << m_curEvt << ")"
  //           << std::endl;
  ATH_MSG_DEBUG ("next() : iEvt " << m_curEvt);

  // get evt context
  RootNtupleEventContext* rctx = dynamic_cast<RootNtupleEventContext*>(&ctx);
  if ( 0 == rctx ) {
    ATH_MSG_ERROR ("Could not dyn-cast to RootNtupleEventContext !!");
    throw "RootNtupleEventSelector: Unable to get RootNtupleEventContext";
  }
  
  TTree *tree = rctx->tree();
  if (!tree) {
    const FileNames_t& fnames = rctx->files();
    std::size_t fidx = rctx->fileIndex();
    rctx->setTree(NULL);
    //rctx->setEntry(-1);

    while (!tree && rctx->tupleIndex() < m_tupleNames.size()) {
      if (fidx < rctx->files().size()) {
        const std::string& fname = fnames[fidx];
        tree = fetchNtuple(fname, m_tupleNames[rctx->tupleIndex()]);
        if (!tree) {
          throw "RootNtupleEventSelector: Unable to get tree";
        }
        rctx->setTree(tree);

      }
      else {
        // end of collections; go to next tuple.
        rctx->setTupleIndex (rctx->tupleIndex()+1);
        rctx->setFileIndex (0);
        fidx = 0;
      }
    }

    if (!tree) {
      return StatusCode::FAILURE;
    }
  }
  int64_t global_entry = rctx->entry();
  size_t collIdx = rctx->fileIndex();
  size_t tupleIdx = rctx->tupleIndex();
  int64_t entry = global_entry;
  if (m_collEvts[tupleIdx][collIdx].min_entries < 0) {
    // need to trigger collmetadata...
    long coll_idx, tuple_idx;
    const_cast<RootNtupleEventSelector*>(this)->find_coll_idx(entry,
                                                              coll_idx,
                                                              tuple_idx);
  }
  // rctx::entry is the *global* entry number.
  // we need the local one...
  entry = global_entry - m_collEvts[tupleIdx][collIdx].min_entries;

  Long64_t nentries = tree->GetEntriesFast();
  // std::cout << "::entry=" << global_entry 
  //           << ", nentries=" << nentries
  //           << ", local=" << entry
  //           << " (min=" << m_collEvts[collIdx].min_entries
  //           << ", max=" << m_collEvts[collIdx].max_entries << ")"
  //           << " (tree=" << tree << ")"
  //           << std::endl;
  if ( nentries > entry ) {

    // load data from tuple
    //std::cout << "--load-data--" << " " << tree->GetReadEntry() << std::endl;
    if (tree->LoadTree(entry) < 0) {
      ATH_MSG_ERROR
        ("Problem loading tree for event [" << m_curEvt << "] !!");
      throw "RootNtupleEventSelector: Problem loading input tree";
    } else {
      ATH_MSG_DEBUG("==> loaded-tree(" << m_curEvt << ")");
    }

    ++m_nbrEvts;
    m_curEvt = global_entry + 1;

    // std::cout << "--event-info--" << std::endl;
    // event info
    EventType* evtType = new EventType;
    const std::size_t runNbr = 0;
    EventInfo* evtInfo = new EventInfo(new EventID(runNbr, m_curEvt-1, 0), evtType);
    if ( !m_dataStore->record( evtInfo, "TTreeEventInfo" ).isSuccess() ) {
      ATH_MSG_ERROR ("Could not record TTreeEventInfo !");
      delete evtInfo; evtInfo = 0;
      return StatusCode::FAILURE;
    }

    {
      auto ei = std::make_unique<xAOD::EventInfo>();
      auto ei_store = std::make_unique<xAOD::EventAuxInfo>();
      ei->setStore (ei_store.get());
      ei->setRunNumber (runNbr);
      ei->setEventNumber (global_entry);

      static const SG::AuxElement::Accessor<std::string> tupleName ("tupleName");
      static const SG::AuxElement::Accessor<std::string> collName ("collectionName");
      tupleName(*ei) = m_tupleNames[tupleIdx];
      collName(*ei) = m_inputCollectionsName[collIdx];
      
      CHECK( m_dataStore->record (std::move(ei), "EventInfo") );
      CHECK( m_dataStore->record (std::move(ei_store), "EventInfoAux.") );
    }
    
    // now the data has been loaded into the store, we can
    // notify clients and fire the BeginInputFile incident
   //MOVED TO handle method
    /*if (m_fireBIF) {
      m_fireBIF = false;
      const FileNames_t& fnames = rctx->files();
      std::size_t fidx = rctx->fileIndex();
      const std::string& fname = fnames[fidx];
      // notify other clients
      // std::cout << "::switchED to next file..." << std::endl;
      m_incsvc->fireIncident(FileIncident(name(), "BeginInputFile", fname));
      // std::cerr << ":: new file: [" << fname << "]\n";
    }*/
    return StatusCode::SUCCESS;

  } else {
    // file is depleted
    CHECK( endInputFile (rctx) );
    rctx->setFileIndex (rctx->fileIndex() + 1);
    rctx->setTree(NULL);
    return next(*rctx);
  }
  
  // NOT REACHED
  // std::cout << "***end of collections***" << std::endl;
  // end of collections
  //return StatusCode::FAILURE;
}

StatusCode RootNtupleEventSelector::next( Context& ctx, int jump ) const
{
  ATH_MSG_DEBUG ("next(" << jump << ") : iEvt " << m_curEvt);

  if (self()->seek(ctx, m_curEvt + jump).isSuccess()) {
    return StatusCode::FAILURE;
  }
  return next(ctx);
}

StatusCode
RootNtupleEventSelector::previous( IEvtSelector::Context& ctx ) const 
{
  return next( ctx, -1 );
}

StatusCode 
RootNtupleEventSelector::previous( Context& ctx, int jump ) const 
{
  return next( ctx, -jump );
}

StatusCode
RootNtupleEventSelector::last( Context& /*ctxt*/ ) const
{
  ATH_MSG_ERROR ("............. Last Event Not Implemented .............");
  return StatusCode::FAILURE;
}


StatusCode 
RootNtupleEventSelector::rewind( Context& ctxt ) const 
{
  return self()->seek(ctxt, 0);
}

StatusCode
RootNtupleEventSelector::createContext( Context*& refCtx ) const
{
  RootNtupleEventContext *ctx = new RootNtupleEventContext(this);
  refCtx = ctx;
  return StatusCode::SUCCESS;
}

StatusCode
RootNtupleEventSelector::createAddress( const Context& /*refCtx*/, 
                                        IOpaqueAddress*& /*addr*/ ) const 
{
  //std::cerr << "::TTES::createAddress()...\n";
  return StatusCode::SUCCESS;
}

StatusCode
RootNtupleEventSelector::releaseContext( Context*& refCtxt ) const
{
  RootNtupleEventContext *ctx  = dynamic_cast<RootNtupleEventContext*>(refCtxt);
  if ( ctx )   {
    delete ctx; ctx = 0;
    return StatusCode::SUCCESS;
  }
  
  return StatusCode::FAILURE;
}

StatusCode
RootNtupleEventSelector::resetCriteria( const std::string&, Context& ) const 
{
  ATH_MSG_ERROR ("............. resetCriteria Not Implemented .............");
  return StatusCode::FAILURE;
}

/////////////////////////////////////////////////////////////////// 
// Non-const methods: 
/////////////////////////////////////////////////////////////////// 

/**
 * @brief Seek to a given event number.
 * @param evtnum  The event number to which to seek.
 */
StatusCode
RootNtupleEventSelector::seek (Context& ctx, int evtnum) const
{
  RootNtupleEventContext* rctx = dynamic_cast<RootNtupleEventContext*>(&ctx);

  // std::cout << "::seek - evtnum=" << evtnum 
  //           << " curevt=" << m_curEvt 
  //           << " curcol=" << rctx->fileIndex()
  //           << std::endl;
  long coll_idx, tuple_idx;
  find_coll_idx(evtnum, coll_idx, tuple_idx);
  // std::cout << "::seek - evtnum=" << evtnum 
  //           << " curevt=" << m_curEvt 
  //           << " curcol=" << rctx->fileIndex()
  //           << " colidx=" << coll_idx
  //           << std::endl;
  if ((coll_idx == -1 || tuple_idx == -1) && evtnum < m_curEvt) {
    coll_idx = rctx->fileIndex();
    tuple_idx = rctx->tupleIndex();
  }

  if (coll_idx == -1 || tuple_idx == -1) {
    ATH_MSG_INFO("seek: reached end of input.");
    return StatusCode::RECOVERABLE;
  }

  if (coll_idx != static_cast<int>(rctx->fileIndex()) ||
      tuple_idx != static_cast<int>(rctx->tupleIndex()))
  {
    // tell everyone we switched files...
    m_tuple = NULL;
    CHECK( endInputFile (rctx) );
  }

  rctx->setFileIndex (coll_idx);
  rctx->setTupleIndex (tuple_idx);
  m_curEvt = evtnum;

  return StatusCode::SUCCESS;
}

/**
 * @brief return the current event number.
 * @return The current event number.
 */
int 
RootNtupleEventSelector::curEvent (const Context& /*refCtxt*/) const
{
  return m_curEvt;
}

/// Callback method to reinitialize the internal state of the component 
/// for I/O purposes (e.g. upon @c fork(2))
StatusCode 
RootNtupleEventSelector::io_reinit()
{
  ATH_MSG_INFO("I/O reinitialization...");

  ServiceHandle<IIoComponentMgr> iomgr("IoComponentMgr", name());
  if (!iomgr.retrieve().isSuccess()) {
    ATH_MSG_FATAL("Could not retrieve IoComponentMgr !");
    return StatusCode::FAILURE;
  }
  if (!iomgr->io_hasitem(this)) {
    ATH_MSG_FATAL("IoComponentMgr does not know about myself !");
    return StatusCode::FAILURE;
  }
  std::vector<std::string> inputCollections = m_inputCollectionsName.value();

  for (std::size_t 
         i = 0, 
         imax = m_inputCollectionsName.value().size(); 
       i < imax; 
       ++i) {
    std::string &fname = inputCollections[i];
    // std::cout << "--retrieve new name for [" << fname << "]...\n";
    if (!iomgr->io_contains(this, fname)) {
      ATH_MSG_ERROR("IoComponentMgr does not know about [" << fname << "] !");
      return StatusCode::FAILURE;
    }
    if (!iomgr->io_retrieve(this, fname).isSuccess()) {
      ATH_MSG_FATAL("Could not retrieve new value for [" << fname << "] !");
      return StatusCode::FAILURE;
    }
    // std::cout << "--> [" << fname << "]\n" << std::flush;
  }
  // all good... copy over.
  m_inputCollectionsName = inputCollections;

  // remove our EventInfo if any...
  // {
  //   const bool force_remove = true;
  //   if (!m_dataStore->clearStore(force_remove).isSuccess()) {
  //     ATH_MSG_ERROR("could not clear event store!");
  //     return StatusCode::FAILURE;
  //   } else {
  //     ATH_MSG_INFO("sgdump: \n" << m_dataStore->dump()); 
  //   }
  // }

  // std::cout << "--> do_init_io...\n" << std::flush;
  if (!do_init_io().isSuccess()) {
    return StatusCode::FAILURE;
  }
  
  ATH_MSG_INFO("I/O reinitialization... [done]");
  return StatusCode::SUCCESS;
}


///@{
/// @c IAddressProvider interface
///get all addresses from Provider : Called before Begin Event
StatusCode 
RootNtupleEventSelector::preLoadAddresses(StoreID::type /*storeID*/, 
                                          tadList& /*tads*/)
{
  // std::cerr << "TTES::preLoadAddresses(" << int(storeID)
  // 	    << "," << tads.size()
  // 	    << ")...\n";
  return StatusCode::SUCCESS;
}
 
/// get all new addresses from Provider for this Event.
StatusCode 
RootNtupleEventSelector::loadAddresses(StoreID::type storeID, tadList& tads)
{
  if (m_needReload) {
    return createRootBranchAddresses(storeID, tads);
  }

  return StatusCode::SUCCESS;
}
 
/// update a transient Address
StatusCode 
RootNtupleEventSelector::updateAddress(StoreID::type /*storeID*/, SG::TransientAddress* tad,
                                       const EventContext& /*ctx*/)
{
  if (tad) {
    if (m_dataStore->proxy_exact (tad->sgkey())) {
      return StatusCode::SUCCESS;
    }
    return StatusCode::FAILURE;
  }
  // do nothing. 
  return StatusCode::SUCCESS;
}
///@}

/////////////////////////////////////////////////////////////////// 
// Protected methods: 
/////////////////////////////////////////////////////////////////// 

/// callback to synchronize the list of input files
void 
RootNtupleEventSelector::setupInputCollection( Property& /*inputCollectionsName*/ )
{
  // nothing ?
  return;
}

StatusCode 
RootNtupleEventSelector::createRootBranchAddresses(StoreID::type storeID,
                                                   tadList &tads)
{
  if (storeID != StoreID::EVENT_STORE) {
    ATH_MSG_INFO("-- not the event-store --");
    return(StatusCode::SUCCESS);
  }

  if (0 == m_tuple) {
    ATH_MSG_ERROR("null pointer to n-tuple !");
    return StatusCode::FAILURE;
  }

  TObjArray *leaves = m_tuple->GetListOfLeaves();
  if (!leaves) {
    ATH_MSG_INFO("no leaves!!");
    return StatusCode::SUCCESS;
  }

  // loop over leaves
  for (Int_t i = 0; i < leaves->GetEntries(); ++i) {
    TLeaf *leaf = (TLeaf *)leaves->At(i);
    TBranch *branch = leaf->GetBranch();
    if (branch) {
      
      CLID id = 0;
      const void* value_ptr = m_tuple;
      const std::string type_name = leaf->GetTypeName();
      const std::string br_name = branch->GetName();
      const std::string sg_key = br_name;//m_tupleName.value()+"/"+br_name;
      TClass *cls = TClass::GetClass(type_name.c_str());
      const std::type_info *ti = 0;

      if (cls) {
        ti = cls->GetTypeInfo();
        // first, try to load a dict for that class...
        if (ti) {
          m_dictsvc->load_type(*ti);
        }
        if (!ti) {
          ATH_MSG_WARNING("could not find a type-info for [" << 
                          type_name << "]");
          continue;
        }
        std::string ti_typename = System::typeinfoName(*ti);
        if (!m_clidsvc->getIDOfTypeInfoName(ti_typename, id)
            .isSuccess()) {
          // try another one...
          ti_typename = TClassEdit::ShortType(ti_typename.c_str(),
                                              TClassEdit::kDropAllDefault);
          if (!m_clidsvc->getIDOfTypeInfoName(ti_typename, id)
              .isSuccess()) {
            ATH_MSG_DEBUG("** could not find a CLID from type-info ["
                         << System::typeinfoName(*ti) << "]");
            ATH_MSG_DEBUG("** could not find a CLID from type-info-alias ["
                         << ti_typename << "]");
            continue;
          }
        }
      } else {
        // probably a built-in type...
        if (!m_clidsvc->getIDOfTypeName(::root_typename(type_name), id)
            .isSuccess()) {
          ATH_MSG_DEBUG("** could not find a CLID for type-name [" 
                       << type_name << "]");
          continue;
        }
      }
      if (id == 0) {
        ATH_MSG_DEBUG("** could not find a CLID for type-name ["
                     << type_name << "]");
        continue;
      }
      Athena::RootBranchAddress* addr = new Athena::RootBranchAddress
        (ROOT_StorageType, id,
         m_tuple->GetName(),
         br_name, 
         (unsigned long)(value_ptr),
         (unsigned long)(m_curEvt-1));

      // recycle old rootaddress, if any.
      SG::DataProxy* proxy = m_dataStore->proxy (id, sg_key);
      if (proxy) {
        proxy->setAddress (addr);
      }
      else {
        auto taddr = new SG::TransientAddress(id, sg_key, addr, false);
        taddr->setProvider(this, storeID);
        // only add the *new* TransientAddress to the input list as the *old* ones
        // are already tracked by the datastore (via the sticky proxies)
        tads.push_back(taddr);
      }
    }
  }
  m_needReload = false;
  // remember that we need to fire a BeginInputFile incident.
  // we can't fire it just now as some client may need the tree and its
  // content loaded in the evtstore when their ::handle method is
  // called.
  // so we do it later.
  //MOVED TO handle method - which is fired on BeginEvent after StoreGateSvc
  //m_fireBIF = true;

  return StatusCode::SUCCESS;
}

StatusCode 
RootNtupleEventSelector::createMetaDataRootBranchAddresses(StoreGateSvc *store,
							   TTree *tree,
							   const std::string& prefix) const
{
  if (0 == store) {
    ATH_MSG_ERROR("null pointer to store !");
    return StatusCode::FAILURE;
  }

  if (0 == tree) {
    ATH_MSG_ERROR("null pointer to n-tuple !");
    return StatusCode::FAILURE;
  }

  // Record tree in Storegate for later writing
  TransferTree* temp = new TransferTree(tree);
  if (store->record(temp,prefix).isFailure()) ATH_MSG_ERROR("Unable to record metadata tree " << tree->GetName());

  const std::string tree_name = tree->GetName();
  TObjArray *branches = tree->GetListOfBranches();
  if (!branches) {
    ATH_MSG_INFO("no branches!!");
    return StatusCode::SUCCESS;
  }

  // loop over branches
  for (Int_t i = 0; i < branches->GetEntries(); ++i) {
    TBranch *branch = (TBranch *)branches->At(i);
    if (branch) {
      
      CLID id = 0;
      const void* value_ptr = tree;
      const std::string type_name = branch->GetClassName();
      const std::string br_name = branch->GetName();
      const std::string sg_key = prefix.empty() 
        ? br_name
        : prefix + "/" + br_name;
      TClass *cls = NULL;
      if (!type_name.empty()) {
        cls = TClass::GetClass(type_name.c_str());
      }
      const std::type_info *ti = 0;

      if (cls) {
        ti = cls->GetTypeInfo();
        // first, try to load a dict for that class...
        if (ti) {
          m_dictsvc->load_type(*ti);
        }
        if (!ti) {
          ATH_MSG_WARNING("could not find a type-info for [" << 
                          type_name << "]");
          continue;
        }
        std::string ti_typename = System::typeinfoName(*ti);
        if (!m_clidsvc->getIDOfTypeInfoName(ti_typename, id)
            .isSuccess()) {
          // try another one...
          ti_typename = TClassEdit::ShortType(ti_typename.c_str(),
                                              TClassEdit::kDropAllDefault);
          if (!m_clidsvc->getIDOfTypeInfoName(ti_typename, id)
              .isSuccess()) {
            ATH_MSG_INFO("** could not find a CLID from type-info ["
                         << System::typeinfoName(*ti) << "]");
            ATH_MSG_INFO("** could not find a CLID from type-info-alias ["
                         << ti_typename << "]");
            continue;
          }
        }
      } else {
        // probably a built-in type...
        TObjArray *leaves = branch->GetListOfLeaves();
        if (leaves && 
            leaves->GetEntries() == 1) {
          const std::string type_name = ((TLeaf*)leaves->At(0))->GetTypeName();
          if (!m_clidsvc->getIDOfTypeName(::root_typename(type_name), id)
              .isSuccess()) {
            ATH_MSG_INFO("** could not find a CLID for type-name [" 
                         << type_name << "]");
            continue;
          }
        }
      }
      if (id == 0) {
        ATH_MSG_INFO("** could not find a CLID for type-name ["
                     << type_name << "]");
        continue;
      }
      Athena::RootBranchAddress* addr = new Athena::RootBranchAddress
        (ROOT_StorageType, id, 
         tree_name, 
         br_name, 
         (unsigned long)(value_ptr),
         (unsigned long)(0));
      if (!store->recordAddress(sg_key, addr, true).isSuccess()) {
        ATH_MSG_ERROR("could not record address at [" << sg_key << "] in store ["
                      << store->name() << "]");
        delete addr; addr = 0;
      }
      // SG::TransientAddress* taddr = new SG::TransientAddress
      //   (id, sg_key, addr);
      // taddr->setProvider(this);
      // taddr->clearAddress(true);
      // tads.push_back(taddr);
    }
  }
  return StatusCode::SUCCESS;
}

TTree*
RootNtupleEventSelector::fetchNtuple(const std::string& fname,
                                     const std::string& tupleName) const
{
  // std::cout << "::fetchNtuple(" << fname << ")..." << std::endl;
  TTree* tree = NULL;
  RootGlobalsRestore rgr;
  // std::cout << "::TFile::Open()..." << std::endl;
  TFile *f = (TFile*)gROOT->GetListOfFiles()->FindObject(fname.c_str());
  TFile* fnew = nullptr;
  if (!f) {
    f = TFile::Open(fname.c_str(), "READ");
    fnew = f;
    if (f) {
      f->SetName(fname.c_str());
    }
  }
  if (!f || f->IsZombie()) {
    ATH_MSG_ERROR("could not open next file in input collection ["
                  << fname << "]");
    if (f) {
      f->Close();
    }
    return tree;
  }
  // std::cout << "::TFile::GetTree(" << m_tupleName << ")..." << std::endl;
  tree = (TTree*)f->Get(tupleName.c_str());
  if (!tree) {
    ATH_MSG_ERROR("could not retrieve tree [" << tupleName << "]"
                  << " from file [" << fname << "]");
    f->Close();
    return tree;
  }

  if (fnew)
    m_files.push_back(fnew);

  // std::cout << "::TTree::SetBranchStatus()..." << std::endl;
  // disable all branches
  tree->SetBranchStatus("*", 0);

  if (!m_imetaStore->clearStore().isSuccess()) {
    ATH_MSG_INFO("could not clear store [" << m_imetaStore.typeAndName() << "]");
    return tree;
  }

  addMetadataFromDirectoryName(tupleName+"Meta", f);
  addMetadataFromDirectoryName("Lumi", f, "Lumi");
  return tree;
}

void RootNtupleEventSelector::addMetadataFromDirectoryName(const std::string &metadirname, TFile *fileObj, const std::string &prefix) const
{
  TDirectoryFile *metadir = (TDirectoryFile*)fileObj->Get(metadirname.c_str());
  if (!metadir) return;
  addMetadataFromDirectory(metadir, prefix);
}

void RootNtupleEventSelector::addMetadataFromDirectory(TDirectoryFile *metadir, const std::string &prefix) const
{
  std::unordered_set<std::string> meta_keys;
  const TList *keys = metadir->GetListOfKeys();
  for (Int_t i=0; i < keys->GetSize(); ++i) {
    TKey* key = dynamic_cast<TKey*>(keys->At(i));
    if (!key) {
      continue;
    }

    const std::string meta_key = key->GetName();
    if (meta_keys.find(meta_key) != meta_keys.end()) {
      // meta_key is another cycle from a previous key entry.
      // *ASSUME* the highest cycle is the one we are interested in
      // *AND* that it was the previous one...
      continue;
    } else {
      meta_keys.insert(meta_key);
    }

    std::string fullPrefix(prefix);
    if (prefix != "") fullPrefix += "/";
    const std::string path = fullPrefix + key->GetName();

    TString fullKeyName(TString::Format("%s;%hi", key->GetName(), key->GetCycle()));
    TObject *objRef = metadir->Get(fullKeyName);

    TTree *metatree = dynamic_cast<TTree*>(objRef);
    if (metatree) {
      addMetadata(metatree, path);
      continue;
    } 
    
    TObjString *metaObjString = dynamic_cast<TObjString*>(objRef);
    if (metaObjString) {
      addMetadata(metaObjString, path);
      continue;
    }

    ATH_MSG_WARNING("Unsupported metadata type: " << objRef->ClassName());
  }
}

void RootNtupleEventSelector::addMetadata(TTree *metatree, const std::string &path) const
{
  if (metatree->GetEntry(0) < 0) {
    ATH_MSG_INFO("Problem retrieving data from metadata-tree [" << path << "] !!");
    return;
  }

  if (!createMetaDataRootBranchAddresses(m_imetaStore.get(), metatree, path).isSuccess()) {
    ATH_MSG_INFO("Could not create metadata for tree [" << path << "]");
  }
}

void RootNtupleEventSelector::addMetadata(TObjString *metastring, const std::string &path) const
{
  std::string *converted = new std::string(metastring->String());
  if (!m_imetaStore->record(converted, path).isSuccess()) {
    ATH_MSG_INFO("Could not create metadata for string [" << path << "]");
  }
}

StatusCode
RootNtupleEventSelector::do_init_io()
{
  // std::cout << "::fetchNtuple..." << std::endl;

  // initialize some helper structures and data
  {
    CollMetaData zero; 
    zero.min_entries = -1;
    zero.max_entries = -1;
    zero.entries = -1;
    m_collEvts.resize (m_tupleNames.size());
    for (size_t i = 0; i < m_collEvts.size(); i++) {
      m_collEvts[i].resize(m_inputCollectionsName.value().size(), zero);
    }
  }

  m_tuple = fetchNtuple(m_inputCollectionsName.value()[0],
                        m_tupleNames[0]);
  if (!m_tuple) {
    throw "RootNtupleEventSelector: Unable to fetch Ntuple";
  }

  // std::cout << "::clear-root-addresses..." << std::endl;
  // reset the list of branches
  m_needReload = true;

  // skip events we are asked to skip
  m_curEvt  = m_skipEvts;
  m_nbrEvts = 0;

  // std::cout << "::fetchNtuple...[done]" << std::endl;
  return StatusCode::SUCCESS;
}

/// helper method to get the collection index (into `m_inputCollectionsName`)
/// and tuple index (into `m_tupleNames')
/// for a given event index `evtidx`.
/// returns -1 if not found.
void
RootNtupleEventSelector::find_coll_idx (int evtidx,
                                        long& coll_idx,
                                        long& tuple_idx) const
{
  coll_idx = -1;
  tuple_idx = -1;

  // std::cout << "--find_coll_idx(" << evtidx << ")..." << std::endl
  //           << "--collsize: " << m_collEvts.size() << std::endl;
  for (size_t ituple = 0; ituple < m_collEvts.size(); ++ituple) {
    for (size_t icoll = 0; icoll < m_collEvts[ituple].size(); ++icoll) {
      CollMetaData &itr = m_collEvts[ituple][icoll];
      if (itr.min_entries == -1) {
        TTree *tree = fetchNtuple(m_inputCollectionsName.value()[icoll],
                                  m_tupleNames[ituple]);
        if (tree) {
          long offset = 0;
          if (icoll > 0) {
            offset = m_collEvts[ituple][icoll-1].max_entries;
          }
          else if (ituple > 0) {
            offset = m_collEvts[ituple-1].back().max_entries;
          }
          itr.entries = tree->GetEntriesFast();
          itr.min_entries = offset;
          itr.max_entries = offset + itr.entries;
        } else {
          throw "RootNtupleEventSelector: Unable to fetch ntuple";
        }
      }
      // std::cout << "--[" << i << "] => [" << itr.min_entries << ", "
      //           << itr.max_entries << ") evtidx=[" << evtidx << "]"
      //           << std::endl;
      if (itr.min_entries <= evtidx && evtidx < itr.max_entries) {
        coll_idx = icoll;
        tuple_idx = ituple;
        return;
      }
    }
  }
}

///return total number of events in all TTree
int RootNtupleEventSelector::size (Context& /*refCtxt*/) const {
  //use find_coll_idx to trigger a population of the m_collEvts
  long coll_idx, tuple_idx;
  find_coll_idx(-1, coll_idx, tuple_idx);
  return m_collEvts.back().back().max_entries;
}


void RootNtupleEventSelector::handle(const Incident& incident) {
   if(m_fireBIF && incident.type() == IncidentType::BeginEvent) {
      //fire the BeginInputFile
      m_incsvc->fireIncident(FileIncident(name(), "BeginInputFile", m_tuple->GetCurrentFile()->GetName())); 
      m_fireBIF=false;
   }
}

} //> namespace Athena

///////////////////////// -*- C++ -*- /////////////////////////////

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// TTreeEventSelector.cxx 
// Implementation file for class TTreeEventSelector
// Author: S.Binet<binet@cern.ch>
/////////////////////////////////////////////////////////////////// 

// STL includes
#include <sstream>
#include <unordered_map>

// ROOT includes
#include "TChain.h"
#include "TClass.h"
#include "TClassEdit.h"
#include "RootUtils/TBranchElementClang.h"
#include "TLeaf.h"
#include "TFile.h"

// Framework includes
#include "GaudiKernel/ISvcLocator.h"
//#include "GaudiKernel/GenericAddress.h"
#include "GaudiKernel/StatusCode.h"
#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/ITHistSvc.h"
#include "GaudiKernel/System.h"
#include "AthenaKernel/IClassIDSvc.h"
#include "AthenaKernel/IDictLoaderSvc.h"

// StoreGate includes
#include "StoreGate/StoreGate.h" 
#include "SGTools/TransientAddress.h"
#include "SGTools/BuiltinsClids.h"   // to make sure we have their clids
#include "SGTools/StlVectorClids.h"  // to make sure we have their clids
#include "SGTools/StlMapClids.h"     // to make sure we have their clids

#include "SGTools/CLASS_DEF.h"

#include "TObject.h"
CLASS_DEF( TObject,    74939790 , 1 )

// EventInfo includes
#include "EventInfo/EventInfo.h"
#include "EventInfo/EventType.h"
#include "EventInfo/EventID.h"

// Package includes
#include "TTreeEventSelector.h"
#include "TTreeEventContext.h"
#include "TTreeEventSelectorHelperSvc.h"
#include "RootBranchAddress.h"

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
}
/////////////////////////////////////////////////////////////////// 
// Public methods: 
/////////////////////////////////////////////////////////////////// 

// Constructors
////////////////

TTreeEventSelector::TTreeEventSelector( const std::string& name,
                                        ISvcLocator* svcLoc ) :
  AthService ( name,    svcLoc ),
  m_storeGate( "StoreGateSvc", name ),
  m_clidsvc  ( "ClassIDSvc",   name ),
  m_dictsvc  ( "AthDictLoaderSvc", name ),
  m_tupleSvc ( "THistSvc",     name ),
  m_nbrEvts     ( 0 ),
  m_totalNbrEvts( 0 ),
  m_tuple       ( 0 ),
  m_rootAddresses ()
{
  declareProperty( "InputCollections", 
                   m_inputCollectionsName,
                   "List of input (ROOT) file names" );
  m_inputCollectionsName.declareUpdateHandler
    ( &TTreeEventSelector::setupInputCollection, this );

  declareProperty( "TupleName",
                   m_tupleName = "",
                   "Name of the TTree to load/read from input file(s)" );

  declareProperty( "SkipEvents",           
                   m_skipEvts = 0,
                   "Number of events to skip at the beginning" );

  declareProperty( "ActiveBranches",
                   m_activeBranchNames,
                   "List of branch names to activate" );
}

// Destructor
///////////////
TTreeEventSelector::~TTreeEventSelector()
{
  delete m_tuple; m_tuple = 0;
}

StatusCode TTreeEventSelector::initialize()
{
  ATH_MSG_INFO ("Enter TTreeEventSelector initialization...");

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

  // retrieve ITHistSvc
  if ( !m_tupleSvc.retrieve().isSuccess() ) {
    ATH_MSG_ERROR
      ("Could not retrieve [" << m_tupleSvc.typeAndName() << "] !!");
    return StatusCode::FAILURE;
  }

  if ( m_tupleName.value().empty() ) {
    ATH_MSG_ERROR
      ("You have to give a TTree name to read from the ROOT files !");
    return StatusCode::FAILURE;
  }

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
       << "                      tuple [" << m_tupleName.value() << "]");
  }

  // retrieve the TTreeEventSelectorHelperSvc to make sure the TTree
  // has been registered with the ITHistSvc
  {
    typedef ServiceHandle<TTreeEventSelectorHelperSvc> HelperSvc_t;
    HelperSvc_t svc("TTreeEventSelectorHelperSvc", name());
    if ( !svc.retrieve().isSuccess() ) {
      ATH_MSG_ERROR ("Could not retrieve [" << svc.typeAndName() << "] !!");
      return StatusCode::FAILURE;
    }
  }
  
  // load tree/chain
  delete m_tuple; m_tuple = 0;

  {
    std::ostringstream tupleName;
    tupleName << "/temp/TTreeStream/" << m_tupleName.value();
  
    const ITHistSvc* svc = &*m_tupleSvc;
    TTree* tuple = 0;
    if ( !svc->getTree(tupleName.str(), tuple).isSuccess() ||
         0 == tuple ) {
      ATH_MSG_ERROR
        ("Could not re-access tuple at [" << tupleName.str() << "] !!");
      return StatusCode::FAILURE;
    }
    if ( 0 == (m_tuple = dynamic_cast<TChain*>(tuple)) ) {
      delete tuple; tuple = 0;
      ATH_MSG_ERROR ("Could not dyn-cast tuple to a TChain !");
      return StatusCode::FAILURE;
    }
  }

  // retrieve event store
  // this needs to happen *after* having registered the n-tuple with the
  // ITHistSvc service as our branches (which need a valid m_ntuple pointer)
  // may be asked to be registered as we are a ProxyProvider.
  // retrieving the event store will poke the ProxyProviderSvc...
  if ( !m_storeGate.retrieve().isSuccess() ) {
    ATH_MSG_ERROR
      ("Could not retrieve [" << m_storeGate.typeAndName() << "] !!");
    return StatusCode::FAILURE;
  }

  // get total number of events:
  m_totalNbrEvts = m_tuple->GetEntries();
  ATH_MSG_INFO ("Total events: " << m_totalNbrEvts);

  if ( m_skipEvts >= m_totalNbrEvts ) {
    ATH_MSG_ERROR 
      ("Number of events to skip (" << m_skipEvts << ") is greater than "
       << "the total number of events in the input collections ("
       << m_totalNbrEvts << ") !"
       << endmsg
       << "Please correct your jobOptions file");
    return StatusCode::FAILURE;
  }

  // reset the list of branches
  m_rootAddresses.clear();

  // skip events we are asked to skip
  m_nbrEvts += m_skipEvts;

  return StatusCode::SUCCESS;
}

StatusCode TTreeEventSelector::finalize()
{
  ATH_MSG_INFO ("Finalize...");

  if (msgLvl(MSG::DEBUG)) {
    TObjArray *leaves = m_tuple->GetListOfLeaves();
    if (leaves) {
      // loop over leaves
      for (Int_t i = 0; i < leaves->GetEntries(); ++i) {
        TLeaf *leaf = (TLeaf *)leaves->At(i);
        TBranch *branch = leaf->GetBranch();
        if (branch) {
          const char *brname = branch->GetName();
          if (m_tuple->GetBranchStatus(brname)) {
            ATH_MSG_DEBUG("branch [" << brname << "] was active.");
          } else {
            ATH_MSG_DEBUG("branch [" << brname << "] was NOT active.");
          }
        }
      }
    }
  }

  // Cleaning-up: make the tuple disappear from the tuple service's registery
  if ( !m_tupleSvc->deReg( m_tuple ).isSuccess() ) {
    ATH_MSG_WARNING 
      ("Could not unregister our (chain of) tuple(s) !" << endmsg
       << "Tuple service may barf.");
  }

  // FIXME: this should be tweaked/updated if/when a selection function
  //        or filtering predicate is applied (one day?)
  ATH_MSG_INFO ("Total events read: " << (m_nbrEvts - m_skipEvts));
  return StatusCode::SUCCESS;
}

// Query the interfaces.
//   Input: riid, Requested interface ID
//          ppvInterface, Pointer to requested interface
//   Return: StatusCode indicating SUCCESS or FAILURE.
// N.B. Don't forget to release the interface after use!!!
StatusCode 
TTreeEventSelector::queryInterface( const InterfaceID& riid, 
                                    void** ppvInterface )
{
  if ( IEvtSelector::interfaceID().versionMatch(riid) ) {
    *ppvInterface = dynamic_cast<IEvtSelector*>(this);
  } else if ( IEventSeek::interfaceID().versionMatch(riid) ) {
    *ppvInterface = dynamic_cast<IEventSeek*>(this);
  } else {
    // Interface is not directly available : try out a base class
    return AthService::queryInterface(riid, ppvInterface);
  }
  addRef();
  return StatusCode::SUCCESS;
}

/////////////////////////////////////////////////////////////////// 
// Const methods: 
///////////////////////////////////////////////////////////////////

StatusCode TTreeEventSelector::next( IEvtSelector::Context& ctx ) const
{
  return this->next(ctx, 0);
}

StatusCode TTreeEventSelector::next( Context& ctx, int jump ) const
{
  ATH_MSG_DEBUG ("next(" << jump << ") : iEvt " << m_nbrEvts);

  // get evt context
  TTreeEventContext* ct = dynamic_cast<TTreeEventContext*>(&ctx);
  if ( 0 == ct ) {
    ATH_MSG_ERROR ("Could not dyn-cast to TTreeEventContext !!");
    return StatusCode::FAILURE;
  }

  // jump to right position
  if ( ( (m_nbrEvts + jump) >= 0 ) &&
       ( (m_nbrEvts + jump) <  m_totalNbrEvts ) ) {
    // adjust pointer
    m_nbrEvts += jump;
    
    // load data from tuple
    if ( m_tuple->LoadTree (m_nbrEvts) < 0 ) {
      ATH_MSG_ERROR
        ("Problem loading tree for event [" << m_nbrEvts << "] !!");
      return StatusCode::FAILURE;
    } else {
      ATH_MSG_DEBUG("==> loaded-tree(" << m_nbrEvts << ")");
    }

    if ( m_tuple->GetEntry( m_nbrEvts ) < 0 ) {
      ATH_MSG_ERROR
        ("Problem retrieving data from tree for entry [" << m_nbrEvts 
         << "] !!");
      return StatusCode::FAILURE;
    } else {
      ATH_MSG_DEBUG("==> got-tree(" << m_nbrEvts << ")");
    }
    ++m_nbrEvts;

    // FIXME: we should try to get the RunNumber/EventNumber from
    //        the ntuple if these branches exist...

    // event info
    EventType* evtType = new EventType;
    const std::size_t runNbr = 0;
    EventInfo* evtInfo = new EventInfo( new EventID( runNbr, m_nbrEvts-1, 0 ),
                                        evtType );
    if ( !m_storeGate->record( evtInfo, "TTreeEventInfo" ).isSuccess() ) {
      ATH_MSG_ERROR ("Could not record TTreeEventInfo !");
      delete evtInfo; evtInfo = 0;
      return StatusCode::FAILURE;
    }
    return StatusCode::SUCCESS;
  }

  // still here ? So this is it. EOF.
  return StatusCode::FAILURE;
}

StatusCode
TTreeEventSelector::previous( IEvtSelector::Context& ctx ) const 
{
  return next( ctx, -1 );
}

StatusCode 
TTreeEventSelector::previous( Context& ctx, int jump ) const 
{
  return next( ctx, -jump );
}

StatusCode
TTreeEventSelector::last( Context& /*ctxt*/ ) const
{
  ATH_MSG_ERROR ("............. Last Event Not Implemented .............");
  return StatusCode::FAILURE;
}


StatusCode 
TTreeEventSelector::rewind( Context& /*ctxt*/ ) const 
{
  ATH_MSG_ERROR 
    ("............. ::rewind(Context) Not Implemented .............");
  return StatusCode::FAILURE;
}

StatusCode
TTreeEventSelector::createContext( Context*& refCtx ) const
{
  refCtx = new TTreeEventContext(this);
  return StatusCode::SUCCESS;
}

StatusCode
TTreeEventSelector::createAddress( const Context& /*refCtx*/, 
                                   IOpaqueAddress*& /*addr*/ ) const 
{
  //std::cerr << "::TTES::createAddress()...\n";
  return StatusCode::SUCCESS;
}

StatusCode
TTreeEventSelector::releaseContext( Context*& refCtxt ) const
{
  TTreeEventContext *ctx  = dynamic_cast<TTreeEventContext*>(refCtxt);
  if ( ctx )   {
    delete ctx; ctx = 0;
    return StatusCode::SUCCESS;
  }
  
  return StatusCode::FAILURE;
}

StatusCode
TTreeEventSelector::resetCriteria( const std::string&, Context& ) const 
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
TTreeEventSelector::seek (int evtnum)
{
  
  // jump to right position
  if ( ( evtnum >= 0 ) &&
       ( evtnum <  m_totalNbrEvts ) ) {
    // adjust pointer
    m_nbrEvts = evtnum;
    
    // load data from tuple
    if ( m_tuple->LoadTree (m_nbrEvts) < 0 ) {
      ATH_MSG_ERROR
        ("Problem loading tree for event [" << m_nbrEvts << "] !!");
      return StatusCode::FAILURE;
    }

    if ( m_tuple->GetEntry( m_nbrEvts ) < 0 ) {
      ATH_MSG_ERROR
        ("Problem retrieving data from tree for entry [" << m_nbrEvts 
         << "] !!");
      return StatusCode::FAILURE;
    }
  } else {
    return StatusCode::FAILURE;
  }

  return StatusCode::SUCCESS;
}

/**
 * @brief return the current event number.
 * @return The current event number.
 */
int 
TTreeEventSelector::curEvent() const
{
  return m_nbrEvts;
}

///@{
/// @c IAddressProvider interface
///get all addresses from Provider : Called before Begin Event
StatusCode 
TTreeEventSelector::preLoadAddresses(StoreID::type storeID, tadList& tads)
{
  // std::cerr << "TTES::preLoadAddresses(" << int(storeID)
  // 	    << "," << tads.size()
  // 	    << ")...\n";

  return createRootBranchAddresses(storeID, tads);
}
 
/// get all new addresses from Provider for this Event.
StatusCode 
TTreeEventSelector::loadAddresses(StoreID::type /*storeID*/, tadList& /*tads*/)
{
  // do nothing. already done by preLoadAddresses.
  return StatusCode::SUCCESS;
}
 
/// update a transient Address
StatusCode 
TTreeEventSelector::updateAddress(StoreID::type /*storeID*/, SG::TransientAddress* tad,
                                  const EventContext& /*ctx*/)
{
  // FIXME: check if we couldn't just use TTree::GetListOfBranches...
  // check memory usage/cpu consumption tradeoff

  // check if this tad is known to us.
  if (tad) {
    if ( m_rootAddresses.find(tad) != m_rootAddresses.end() ) {
      return StatusCode::SUCCESS;
    }
    ATH_MSG_DEBUG("updateAddress: address [" << tad->clID() << "#"
		  << tad->name() << ") NOT known to us.");
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
TTreeEventSelector::setupInputCollection( Property& /*inputCollectionsName*/ )
{
  // nothing ?
  return;
}

StatusCode 
TTreeEventSelector::createRootBranchAddresses(StoreID::type storeID,
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
            ATH_MSG_INFO("** could not find a CLID from type-info ["
                         << System::typeinfoName(*ti) << "]");
            ATH_MSG_INFO("** could not find a CLID from type-info-alias ["
                         << ti_typename << "]");
            continue;
          }
        }
      } else {
        // probably a built-in type...
        if (!m_clidsvc->getIDOfTypeName(::root_typename(type_name), id)
            .isSuccess()) {
          ATH_MSG_INFO("** could not find a CLID for type-name [" 
                       << type_name << "]");
          continue;
        }
      }
      if (id == 0) {
        ATH_MSG_INFO("** could not find a CLID for type-name ["
                     << type_name << "]");
        continue;
      }
      Athena::RootBranchAddress* addr = new Athena::RootBranchAddress
        (ROOT_StorageType, id, 
         m_tupleName.value(), 
         br_name, 
         (unsigned long)(value_ptr),
         (unsigned long)(m_nbrEvts-1));
      SG::TransientAddress* taddr = new SG::TransientAddress
        (id, sg_key, addr);
      taddr->setProvider(this, storeID);
      taddr->clearAddress(false);
      tads.push_back(taddr);
      // note: we can store this taddr *b/c* we don't clearAddress it
      // ie: b/c we just called clearAddress(false) so it will be recycled
      // over the events.
      m_rootAddresses.insert(taddr);
    }
  }
  return StatusCode::SUCCESS;
}

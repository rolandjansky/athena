/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/*----------------------------------------------------
 * TRT_DAQ_ConditionsSvc.cxx
 * Denver Whittington
 *///-------------------------------------------------

// Header Includes
#include "GaudiKernel/ToolFactory.h"
#include "GaudiKernel/IIncidentSvc.h"
#include "EventInfo/EventInfo.h"
#include "EventInfo/EventID.h"

#include "TRT_DAQ_ConditionsSvc.h"
#include "AthenaPoolUtilities/CondAttrListCollection.h"

//anonymous namespace for file scope functions, variables
namespace{
	const std::string run1FolderName("/TDAQ/EnabledResources/ATLAS/TRT/Robins"); //a multi-channel, single version folder in the DB
	const std::string run2FolderName("/TDAQ/Resources/ATLAS/TRT/Robins");
}

//////////
/// Constructor
/////
TRT_DAQ_ConditionsSvc::TRT_DAQ_ConditionsSvc( const std::string& name, ISvcLocator* pSvcLocator ) :
  AthService( name, pSvcLocator ),
  m_evtStore("StoreGateSvc",name),
  m_detStore("DetectorStore",name),
  m_EventInfoKey("ByteStreamEventInfo"),
  m_FolderName(run1FolderName),
  m_TRT_ID_Helper(0),
  m_EnabledRods(0)
{
  // Get properties from job options
  declareProperty( "EventInfoKey", m_EventInfoKey );
  declareProperty( "FolderName", m_FolderName );
  declareProperty( "EventStore", m_evtStore );
  declareProperty( "DetectorStore", m_detStore );
}

//////////
/// Destructor
/////
TRT_DAQ_ConditionsSvc::~TRT_DAQ_ConditionsSvc() {}

//////////
/// Initialize
/////
StatusCode TRT_DAQ_ConditionsSvc::initialize() {
  StatusCode sc(StatusCode::SUCCESS);
  // Retrieve the EventStore and DetectorStore
  ATH_CHECK(m_evtStore.retrieve());
  ATH_CHECK(m_detStore.retrieve());
	
	//Determine the folder name by seeing which folder is in the detStore
	const bool option1Exists = m_detStore->contains<CondAttrListCollection>(run1FolderName);
	const bool option2Exists = m_detStore->contains<CondAttrListCollection>(run2FolderName);
  const bool nonsense = (option1Exists == option2Exists); //both exist, or neither exist
  if (nonsense) {
    if (option1Exists) { //both exist
      ATH_MSG_ERROR("The folders "<<run1FolderName<<" and "<<run2FolderName<<" are both present!");
    } else { //neither exist
      ATH_MSG_ERROR("Neither "<<run1FolderName<<" nor "<<run2FolderName<<" exist");
    }
    return StatusCode::FAILURE;
  }
  m_FolderName = option1Exists ? run1FolderName : run2FolderName;
	ATH_MSG_INFO("TRT_DAQ_ConditionsSvc will use the folder "<<m_FolderName);
	
  // Get the TRT Identifier Helper.
  ATH_CHECK( m_detStore->retrieve( m_TRT_ID_Helper, "TRT_ID" ));
 
  // Register a callback for "BeginRun"
  IIncidentSvc* incSvc(0);
  sc = service( "IncidentSvc", incSvc );
  if ( sc.isFailure() or (incSvc==0)) {
    ATH_MSG_ERROR( "Couldn't get the IncidentSvc." );
    return sc;
  }
  incSvc->addListener( this, std::string("BeginEvent") );

  return sc;
}

//////////
/// Return ROD identifier
/////
unsigned int TRT_DAQ_ConditionsSvc::RODid( const Identifier& ident ) {
  int barrel_ec      = m_TRT_ID_Helper->barrel_ec( ident );
  int phi_module     = m_TRT_ID_Helper->phi_module( ident );
  int layer_or_wheel = m_TRT_ID_Helper->layer_or_wheel( ident );
  int straw_layer    = m_TRT_ID_Helper->straw_layer( ident );

  int partition = 0;
  if      ( barrel_ec ==  1 ) partition = 0x31; // Barrel A
  else if ( barrel_ec == -1 ) partition = 0x32; // Barrel C
  else if ( barrel_ec ==  2 ) partition = 0x33; // Endcap A
  else if ( barrel_ec == -2 ) partition = 0x34; // Endcap C
  else {
    ATH_MSG_WARNING( "Invalid Barrel/EC identifier requested in RODid." );
    return 0;
  }

  int phi = phi_module + 1; // [0,31] -> [1,32]

  int rodHalf = 0;
  if ( abs(barrel_ec) == 2 ) {
    if ( layer_or_wheel >= 0 && layer_or_wheel < 4 ) rodHalf = 2;
    if ( layer_or_wheel == 4 && straw_layer >= 0 && straw_layer <  8 ) rodHalf = 1;
    if ( layer_or_wheel == 4 && straw_layer >= 8 && straw_layer < 16 ) rodHalf = 2;
    if ( layer_or_wheel == 5 && straw_layer >= 0 && straw_layer <  8 ) rodHalf = 2;
    if ( layer_or_wheel == 5 && straw_layer >= 8 && straw_layer < 16 ) rodHalf = 1;
    if ( layer_or_wheel >= 6 && layer_or_wheel < 14 ) rodHalf = 1;
  }

  int thisRODid = partition*0x10000 + phi*0x100 + rodHalf;
  return thisRODid;
}

//////////
/// StatusFlag for TRT_ConditionsSummarySvc (by Identifier)
/////
InDet::TRT_CondFlag TRT_DAQ_ConditionsSvc::condSummaryStatus(
					    const Identifier& ident ) {

  if ( !m_EnabledRods ) return InDet::TRT_COND_NOINFO;

  unsigned int thisRODid = RODid( ident );

  return condSummaryStatus( thisRODid );
}

//////////
/// StatusFlag for conditions summary (by ROD ID)
/////
InDet::TRT_CondFlag TRT_DAQ_ConditionsSvc::condSummaryStatus( unsigned int thisRODid ) {

  if ( !m_EnabledRods ) return InDet::TRT_COND_NOINFO;

  CondAttrListCollection::name_const_iterator chanNameMapItr;
  if ( m_EnabledRods->name_size() == 0 ) {
    ATH_MSG_WARNING( m_FolderName << " has no entries in it's ChanNameMap." );
    return InDet::TRT_COND_NOINFO;
  }
  int chanNum = -1;
  std::string chanName = "";
  for ( chanNameMapItr = m_EnabledRods->name_begin();
	chanNameMapItr != m_EnabledRods->name_end(); ++chanNameMapItr ) {
    chanNum = (*chanNameMapItr).first;
    chanName = (*chanNameMapItr).second;
    // Pull off last 6 characters from chanName and convert
    // to an integer for comparison to identifier.
    std::string ROD_Part = std::string( chanName, chanName.length()-6, 2 );
    std::string ROD_Phi  = std::string( chanName, chanName.length()-4, 2 );
    std::string ROD_Half = std::string( chanName, chanName.length()-2, 2 );

    unsigned int rod = 0;

    // Convert string representation of hex representation of partition (detector) to integer
    /* Partition number in string from database comes as hex number.
     * Need to convert it to an integer. */
    int ROD_Part_int;
    std::istringstream iss(ROD_Part);
    iss >> ROD_Part_int;
    switch ( ROD_Part_int ) {
     case 31:
       rod += 0x10000 * 0x31;
       break;
     case 32:
       rod += 0x10000 * 0x32;
       break;
     case 33:
       rod += 0x10000 * 0x33;
       break;
     case 34:
       rod += 0x10000 * 0x34;
       break;
    }

    // Convert string representation of hex representation of phi stack to integer
    /* Phi stack number in string from database comes as hex number.
     * Need to convert it to an integer. */
    char* extra = NULL;
    int ROD_Phi_int = strtol( ROD_Phi.c_str(), &extra, 16 );
    rod += ROD_Phi_int * 0x100;

    rod += atoi(ROD_Half.c_str());

    ATH_MSG_VERBOSE( "Comparing " << rod << " to " << thisRODid );
    if ( rod == thisRODid ) break;
  }
  if ( chanNameMapItr == m_EnabledRods->name_end() ) {
    ATH_MSG_VERBOSE( thisRODid << " not found in chanNameMap" );
    return InDet::TRT_COND_BAD; // SHOULD THIS BE NOINFO??
    // Either this is unnecessary (ROD absence won't show up here)
    // or the next checking step is redundant.
  }

  // Check for absence of AttributeList for this ROD.
  CondAttrListCollection::const_iterator chanAttrListPair;
  chanAttrListPair = m_EnabledRods->chanAttrListPair( chanNum );
  if ( chanAttrListPair == m_EnabledRods->end() ) {
    ATH_MSG_VERBOSE( "Channel " << chanNum << " (" << chanName << ") not found." );
    return InDet::TRT_COND_BAD;
  }

  // Must have been present = GOOD.
  ATH_MSG_VERBOSE( "Channel " << chanNum << " (" << chanName << ") enabled in this event." );
  return InDet::TRT_COND_GOOD;
}

//////////
/// Finalize
/////
StatusCode TRT_DAQ_ConditionsSvc::finalize() {
  return StatusCode::SUCCESS;
}

//////////
/// IncidentSvc incident handler
/////
void TRT_DAQ_ConditionsSvc::handle( const Incident& inc ) {
  StatusCode sc(StatusCode::SUCCESS);

  // BeginEvent handler
  if ( inc.type() == "BeginEvent" ) {

    // Retrieve COOL Folder at beginning of event to cut down on StoreGate accesses.
    // Contents won't change during event.
    sc= m_detStore->retrieve( m_EnabledRods, m_FolderName );
    if (sc.isFailure()) ATH_MSG_ERROR("The folder "<<m_FolderName<<" could not be retrieved");
   

    /* FOR TESTING THE CALLBACK
    // Get the run number
    const EventInfo* evtInfo;
    sc = m_evtStore->retrieve(evtInfo,m_EventInfoKey);
    if ( sc.isFailure() || !evtInfo ) {
      ATH_MSG_WARNING( "Couldn't get " << m_EventInfoKey << " from StoreGate." );
      return;
    }
    if ( !evtInfo->event_ID() ) {
      ATH_MSG_WARNING( m_EventInfoKey << " object has no EventID object." );
      return;
    }
    int runNum = evtInfo->event_ID()->run_number();
    int lumiBlock = evtInfo->event_ID()->lumi_block();

    ATH_MSG_INFO( "Run " << runNum << ", lumiblock " << lumiBlock );
    */

    /* FOR TESTING FOLDER ACCESS
    CondAttrListCollection::name_const_iterator chanNameMapItr;
    if ( m_EnabledRods->name_size() == 0 ) {
      ATH_MSG_FATAL( "This CondAttrListCollection has no entries in it's ChanNameMap." );
      return;
    }
    for ( chanNameMapItr = m_EnabledRods->name_begin();
	  chanNameMapItr != m_EnabledRods->name_end(); ++chanNameMapItr ) {
      int chanNum = (*chanNameMapItr).first;
      std::string chanName = (*chanNameMapItr).second;
      CondAttrListCollection::iov_const_iterator chanIOVPair;
      chanIOVPair = m_EnabledRods->chanIOVPair( chanNum );
      if ( chanIOVPair == m_EnabledRods->iov_end() ) {
	ATH_MSG_INFO( "Channel " << chanNum << " (" << chanName << ") not found." );
	return;
      }
      ATH_MSG_INFO( "Channel " << chanNum << " (" << chanName << ") enabled in this event." );
    }
    */

  }

  return;
}

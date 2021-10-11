/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

/*----------------------------------------------------
 * TRT_DAQ_ConditionsSvc.cxx
 * Denver Whittington
 *///-------------------------------------------------

// Header Includes

#include "TRT_DAQ_ConditionsSvc.h"
#include "StoreGate/ReadCondHandle.h"

//////////
/// Constructor
/////
TRT_DAQ_ConditionsSvc::TRT_DAQ_ConditionsSvc( const std::string& name, ISvcLocator* pSvcLocator ) :
  AthService( name, pSvcLocator ),
  m_detStore("DetectorStore",name),
  m_FolderName("/TDAQ/Resources/ATLAS/TRT/Robins") // For run1 the name is /TDAQ/EnabledResources/ATLAS/TRT/Robins
{
  // Get properties from job options
  declareProperty( "FolderName", m_FolderName );
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
	
  // Read key for Robins folder
  ATH_CHECK(m_RobinsReadKey.initialize());

  // Get the TRT Identifier Helper.
  ATH_CHECK( m_detStore->retrieve( m_TRT_ID_Helper, "TRT_ID" ));
 
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

  SG::ReadCondHandle<CondAttrListCollection> readHandle(m_RobinsReadKey);
  const CondAttrListCollection* EnabledRods{*readHandle};
  if ( !EnabledRods ) return InDet::TRT_COND_NOINFO;

  unsigned int thisRODid = RODid( ident );

  return condSummaryStatus( thisRODid );
}

//////////
/// StatusFlag for conditions summary (by ROD ID)
/////
InDet::TRT_CondFlag TRT_DAQ_ConditionsSvc::condSummaryStatus( unsigned int thisRODid ) {

  SG::ReadCondHandle<CondAttrListCollection> readHandle(m_RobinsReadKey);
  const CondAttrListCollection* EnabledRods{*readHandle};

  if ( !EnabledRods ) return InDet::TRT_COND_NOINFO;

  CondAttrListCollection::name_const_iterator chanNameMapItr;
  if ( EnabledRods->name_size() == 0 ) {
    ATH_MSG_WARNING( m_FolderName << " has no entries in it's ChanNameMap." );
    return InDet::TRT_COND_NOINFO;
  }
  int chanNum = -1;
  std::string chanName = "";
  for ( chanNameMapItr = EnabledRods->name_begin();
	chanNameMapItr != EnabledRods->name_end(); ++chanNameMapItr ) {
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
    char* extra = nullptr;
    int ROD_Phi_int = strtol( ROD_Phi.c_str(), &extra, 16 );
    rod += ROD_Phi_int * 0x100;

    rod += atoi(ROD_Half.c_str());

    ATH_MSG_VERBOSE( "Comparing " << rod << " to " << thisRODid );
    if ( rod == thisRODid ) break;
  }
  if ( chanNameMapItr == EnabledRods->name_end() ) {
    ATH_MSG_VERBOSE( thisRODid << " not found in chanNameMap" );
    return InDet::TRT_COND_BAD; // SHOULD THIS BE NOINFO??
    // Either this is unnecessary (ROD absence won't show up here)
    // or the next checking step is redundant.
  }

  // Check for absence of AttributeList for this ROD.
  CondAttrListCollection::const_iterator chanAttrListPair;
  chanAttrListPair = EnabledRods->chanAttrListPair( chanNum );
  if ( chanAttrListPair == EnabledRods->end() ) {
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


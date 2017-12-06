/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/



#include "TRT_RodDecoder.h"

#include "CoralBase/Attribute.h"
// COOL API include files (CoolKernel)
#include "CoolKernel/DatabaseId.h"
#include "CoolKernel/Exception.h"
#include "CoolKernel/IDatabaseSvc.h"
#include "CoolKernel/IDatabase.h"
#include "CoolKernel/IFolder.h"
#include "CoolKernel/IObject.h"
#include "CoolKernel/IObjectIterator.h"
#include "CoolKernel/Record.h"
#include "CoolKernel/RecordSpecification.h"
#include "CoolApplication/DatabaseSvcFactory.h"
// COOL API include files (CoolApplication)
#include "CoolApplication/Application.h"


#include "AthenaPoolUtilities/AthenaAttributeList.h"
#include "AthenaPoolUtilities/CondAttrListCollection.h"

/*
 * TRT Specific detector manager to get layout information
 */
#include "InDetReadoutGeometry/TRT_DetectorManager.h"  

/*
 * Event info to check if ByteStream is Simulation
 */
#include "EventInfo/EventInfo.h"
#include "EventInfo/EventID.h"
#include "EventInfo/EventType.h"

#include "PathResolver/PathResolver.h"
#include <fstream>

/* -------------------------------------------------------
 * constructor
 * -------------------------------------------------------
 */

TRT_RodDecoder::TRT_RodDecoder
( const std::string& type, const std::string& name,const IInterface* parent )
  :  AthAlgTool              ( type,name,parent ),
     m_CablingSvc            ( "TRT_CablingSvc", name ),
     m_bsErrSvc              ( "TRT_ByteStream_ConditionsSvc", name ),
     m_incsvc                ( "IncidentSvc",name ),
     m_recordBSErrors        ( true ),
     m_lookAtSidErrors       ( true ),
     m_lookAtErrorErrors     ( false ),
     m_lookAtL1idErrors      ( true ),
     m_lookAtBcidErrors      ( true ),
     m_lookAtMissingErrors   ( true ),
     m_loadCompressTableFile ( false ),
     m_loadCompressTableDB   ( true ),
     m_compressTableFolder   ( "/TRT/Onl/ROD/Compress" ),
     m_maxCompressionVersion ( 255 ),
     m_forceRodVersion       ( -1 ),
     //     m_Nsymbols              ( 0 ),
     m_escape_marker         ( 0x8000000 ),
     m_Nrdos                 ( 0 )
{
  declareProperty ( "TRT_Cabling", m_CablingSvc );
  declareProperty ( "BSCondSvc", m_bsErrSvc );
  declareProperty ( "RecordByteStreamErrors", m_recordBSErrors );
  declareProperty ( "LookAtSidErrors",        m_lookAtSidErrors );
  declareProperty ( "LookAtErrorErrors",      m_lookAtErrorErrors );
  declareProperty ( "LookAtL1idErrors",       m_lookAtL1idErrors );
  declareProperty ( "LookAtBcidErrors",       m_lookAtBcidErrors );
  declareProperty ( "LookAtMissingErrors",    m_lookAtMissingErrors );
  declareProperty ( "LoadCompressTableFile",  m_loadCompressTableFile );
  declareProperty ( "LoadCompressTableDB",    m_loadCompressTableDB );
  declareProperty ( "ForceRodVersion",        m_forceRodVersion );
  declareProperty ( "LoadCompressTableVersions", m_LoadCompressTableVersions );

  for (int i=0; i<256; i++)
     m_compressTableLoaded[i] = false;
  /*
  m_CompressionTables[0] = NULL;
  m_CompressionTables[1] = NULL;
  m_CompressionTables[2] = NULL;
  m_CompressionTables[3] = NULL;
  m_CompressionTables[4] = NULL;
  m_CompressionTables[5] = NULL;
  m_CompressionTables[6] = NULL;
  m_CompressionTables[7] = NULL;
  m_CompressionTables[8] = NULL;
  m_CompressionTables[9] = NULL;
  m_CompressionTables[10] = NULL;
  m_CompressionTables[11] = NULL;
  m_CompressionTables[12] = NULL;
  m_CompressionTables[13] = NULL;
  m_CompressionTables[14] = NULL;
  m_CompressionTables[15] = NULL;
  */

  declareInterface<ITRT_RodDecoder>( this );
}

/* ----------------------------------------------------------
 * destructor  
 * ----------------------------------------------------------
 */
TRT_RodDecoder::~TRT_RodDecoder()
{}


/* ----------------------------------------------------------
 * initialize
 * ----------------------------------------------------------
 */

StatusCode TRT_RodDecoder::initialize()
{
  ATH_MSG_DEBUG( " initialize " );
  StatusCode sc;

  sc = AlgTool::initialize(); 
  if (sc.isFailure())
  {
    ATH_MSG_FATAL( "Failed to init baseclass" );
    return StatusCode::FAILURE;
  }

  /*
   * Retrieve id mapping 
   */
  if ( m_CablingSvc.retrieve().isFailure() )
  {
    ATH_MSG_FATAL( "Failed to retrieve tool " << m_CablingSvc );
    return StatusCode::FAILURE;
  } else 
    ATH_MSG_INFO( "Retrieved tool " << m_CablingSvc );


  /*
   * Retrieve conditions tool
   */
  if ( m_bsErrSvc.retrieve().isFailure() )
  {
    ATH_MSG_FATAL( "Failed to retrieve service " << m_bsErrSvc );
    return StatusCode::FAILURE;
  } else
    ATH_MSG_INFO( "Retrieved service " << m_bsErrSvc );


  /*
   * get detector manager
   */
   const InDetDD::TRT_DetectorManager* indet_mgr;
   sc = detStore()->retrieve(indet_mgr,"TRT");
   if (sc.isFailure()) 
   {
     ATH_MSG_FATAL( "Cannot retrieve TRT_DetectorManager!" );
     return StatusCode::FAILURE;
   }



  // get the helper
  if (detStore()->retrieve(m_trt_id, "TRT_ID").isFailure()) 
  {
    ATH_MSG_FATAL( "Could not get TRT ID helper" );
    return StatusCode::FAILURE;
  }
  m_straw_layer_context = m_trt_id->straw_layer_context();


  // get layout
  std::string Layout;
  Layout = indet_mgr->getLayout();
  if ( Layout == "TestBeam" ||
       Layout == "SR1" ||
       Layout == "SR1-EndcapC" )
  {
     ATH_MSG_INFO( "Creating TRT_TB04_RawData RDOs." );
     m_TB04_RawData = true;
     m_LoLumRawData = false;
  }
  else
  {
     ATH_MSG_INFO( "Creating TRT_LoLumRawData RDOs." );
     // ME: bugfix
     m_TB04_RawData = false;
     m_LoLumRawData = true;
  }

  /*
   * Show Look At Me's
   */
  ATH_MSG_INFO( "Look at Sid Errors    : " << m_lookAtSidErrors );
  ATH_MSG_INFO( "Look at Error Errors  : " << m_lookAtErrorErrors );
  ATH_MSG_INFO( "Look at L1ID Errors   : " << m_lookAtL1idErrors );
  ATH_MSG_INFO( "Look at BCID Errors   : " << m_lookAtBcidErrors );
  ATH_MSG_INFO( "Look at Missing Errors: " << m_lookAtMissingErrors );

  /*
   * listen for BeginRun in order to decide if data is simulated.
   */
  if( m_incsvc.retrieve().isFailure() )
  {
    ATH_MSG_FATAL( "Failed to retrieve service " << m_incsvc );
    return StatusCode::FAILURE;
  } else 
    m_incsvc->addListener( this, "BeginRun");


  m_eventTypeIsSim = false;

  if ( m_loadCompressTableFile )
  {
    if  ( m_LoadCompressTableVersions.size() == 0 ) 
    {
      m_LoadCompressTableVersions.push_back( 4 );
    }

    std::vector<int>::iterator it;

    for ( it=m_LoadCompressTableVersions.begin() ; 
	  it < m_LoadCompressTableVersions.end(); it++ )
     {
       if ( (*it < 4) || (*it > m_maxCompressionVersion) )
       {
	 ATH_MSG_INFO( "Not reading bad Compress Table Version: " << *it );
       }
       else
       {
	 std::ostringstream ssVersion;
	 ssVersion << *it;
	 std::string compressTableFile = "Compress_Table-" + ssVersion.str() + ".dat";

	 ATH_MSG_INFO( "Reading Compress Table: " << compressTableFile );

	 sc = ReadCompressTableFile( compressTableFile );
       }
     }
  }

  if ( m_loadCompressTableDB )
  {
  // Register callback function for cache updates - RToT:
    const DataHandle<CondAttrListCollection> cptr_Ctab;
    if ( StatusCode::SUCCESS == 
	 detStore()->regFcn(&TRT_RodDecoder::update, this, 
			   cptr_Ctab, m_compressTableFolder) )
    {
      ATH_MSG_DEBUG ("Registered callback for TRT_RodDecoder - CTable.");
    }
    else
    {
      ATH_MSG_ERROR ("Callback registration failed for TRT_RodDecoder - CTable! ");
    }

    // register incident handler for begin run
    //    IIncidentSvc* incsvc;
    //    if ( StatusCode::SUCCESS != service( m_incsvc, incsvc) )
    //      ATH_MSG_FATAL( "Incident service not found" );
    //    long int pri=100;
    //    incsvc->addListener(this,"BeginRun",pri);
  }

  if ( m_forceRodVersion > 0 )
  {
    ATH_MSG_WARNING( "****************************" );
    ATH_MSG_WARNING( "****************************" );
    ATH_MSG_WARNING( "* Forcing ROD Version to " << m_forceRodVersion << " *" );
    ATH_MSG_WARNING( "****************************" );
    ATH_MSG_WARNING( "****************************" );
  }

  return sc;
}


/* ----------------------------------------------------------
 * finalize
 * ----------------------------------------------------------
 */
StatusCode TRT_RodDecoder::finalize() {

  ATH_MSG_VERBOSE( "in TRT_RodDecoder::finalize" );
  ATH_MSG_INFO( "Number of TRT RDOs created: " << m_Nrdos );

  return StatusCode::SUCCESS;
}


/* ----------------------------------------------------------
 * Incident Handle
 * ----------------------------------------------------------
 */
void
TRT_RodDecoder::handle(const Incident& inc) {

  /** Look at first event to determine if it is data or simulation,
   * in order to complain later if the RodBlock version is wrong.
   **/

  if ( inc.type() == "BeginRun" ) {
    const EventInfo* currentEvent = nullptr;
    StatusCode sc = evtStore()->retrieve(currentEvent);
    if ( sc.isFailure() ) 
       ATH_MSG_ERROR( "Could not get current event" );
#ifdef TRT_BSC_DEBUG    
    ATH_MSG_DEBUG( "Current Run.Event,Time: " \
	  << "[" << currentEvent->event_ID()->run_number() \
	  << "." << currentEvent->event_ID()->event_number() \
	  << "," << currentEvent->event_ID()->time_stamp() \
	  << "]   " <<currentEvent->event_type()->typeToString() \
	  << "..." );
#endif
    m_eventTypeIsSim =  currentEvent->event_type()->test(EventType::IS_SIMULATION);
  }
  return;
}



/* ----------------------------------------------------------
 * main method to fill the collections into the IDC
 *
 * this is just a switch to call the correct decoding based on the
 * format of the BS.  The version is based on the low order 8 bits of
 * the rod_version header word:
 *       0  : Only used for certain (old) simulation files. Expanded Mode
 *       1,2: Expanded Mode.  1 does not have a modern status block
 *       3  : Minimal Compression Mode.
 * If the version is none of these we complain and return FAILURE.
 * ----------------------------------------------------------
 */
StatusCode
TRT_RodDecoder::fillCollection ( const ROBFragment* robFrag,
				 TRT_RDO_Container* rdoIdc,
				 std::vector<IdentifierHash>* vecHash )
{
  StatusCode sc;

  int    RodBlockVersion          = (robFrag->rod_version() & 0xff);

  if ( m_forceRodVersion > 0 )
    RodBlockVersion = m_forceRodVersion;

  uint32_t robid = robFrag->rod_source_id();

  //  ATH_MSG_INFO( "fC: robid = " 
  //		<< MSG::hex 
  //		<< robid << " L1ID = " << robFrag->rod_lvl1_id()
  //		<< MSG::dec );

  static int  err_count                = 0;

  /*
   * Save non-zero rob status to TRT BS Conditions Services
   */
  if ( robFrag->nstatus() )
  { 
    const uint32_t* rob_status;
    robFrag->status(rob_status);

    if ( *rob_status )
    {

      m_bsErrSvc->add_rob_error( robFrag->rob_source_id(), *rob_status ); 


      /*
       * This is a hack to only print once per event.  It will work the
       * vast majority of the time, but it may miss an occasional event.
       */
      static uint32_t Last_print_L1ID = 0xffffffff;
      static uint32_t Last_print_BCID = 0xffffffff;

      if ( (Last_print_L1ID != robFrag->rod_lvl1_id()) || 
      	   (Last_print_BCID != robFrag->rod_bc_id()) )
      {
	Last_print_L1ID = robFrag->rod_lvl1_id();
	Last_print_BCID = robFrag->rod_bc_id();

	ATH_MSG_INFO( "Non-Zero ROB status word for ROB " 
		      << MSG::hex 
		      << robFrag->rob_source_id() 
		      << " ( " << *rob_status << " ) "
		      << MSG::dec
		      << "  Skipping decoding..." );
      }

      sc = StatusCode::RECOVERABLE;   // XXX - Evil, but cannot trust anything
      return sc;
    }
  }


  // get version to decide which method to use to decode !
  if ( 3 < RodBlockVersion && m_maxCompressionVersion >= RodBlockVersion )     // Full Compression
  {
    if ( m_compressTableLoaded[RodBlockVersion] )
      sc = int_fillFullCompress( robFrag, rdoIdc, 
				 m_CompressionTables[RodBlockVersion],
				 vecHash );  
    else
    {
      if ( err_count < 100 )
      {
	ATH_MSG_WARNING( "Rod Version: " << RodBlockVersion		\
			 << ", but Compression Table not loaded!  ROD ID = " \
			 << MSG::hex << robid << MSG::dec );
	err_count++;
      }
      else if ( 100 == err_count )
      {
	ATH_MSG_WARNING( "Too many Rod Version messages.  "	\
			 << "Turning message off." );
	err_count++;
      }
      
      sc = StatusCode::FAILURE;
    }
  }
  else if ( 3 == RodBlockVersion )
    sc = int_fillMinimalCompress( robFrag, rdoIdc, vecHash );
  else if ( (2 == RodBlockVersion) || (1 == RodBlockVersion) )
    sc = int_fillExpanded( robFrag, rdoIdc, vecHash );
  else if ( 0 == RodBlockVersion )
  {
    if ( 0 == robFrag->rod_ndata() )
      return sc;


    if (! m_eventTypeIsSim)
    {
       ATH_MSG_FATAL( "ROD Format Version Number is ZERO.  " \
	    << "and event_type is not EventType::IS_SIMULATION, " \
	    << "ROD ID = " << MSG::hex << robid << MSG::dec );
      return StatusCode::FAILURE;
    }

    sc = int_fillExpanded( robFrag, rdoIdc, vecHash );
  }
  else
  {
  }
  
  /*
   * Decode Status Block, only if we have a modern block and 
   * are OK at this point
   */
  if ( (RodBlockVersion >= 2) && (sc == StatusCode::SUCCESS) )
    {
      /*
       * Error counters
       */
      int sid_errors     = 0;
      int error_errors   = 0;
      int l1id_errors    = 0;
      int bcid_errors    = 0;
      int missing_errors = 0;

      if ( m_recordBSErrors )
      {
	uint32_t rod_L1ID = robFrag->rod_lvl1_id();
	uint32_t rod_BCID = robFrag->rod_bc_id();
	uint32_t rod_SourceID = robFrag->rod_source_id();
      
	OFFLINE_FRAGMENTS_NAMESPACE::PointerType vint;
	robFrag->rod_status( vint );
      
	//     uint32_t v_size = robFrag->rod_nstatus();
	int v_index=0;
      
	/*
	 * skip mandatory status word
	 */
	//uint32_t mandatory = vint[v_index++];
	v_index++;
      
	uint32_t n_status = vint[v_index++];
      
      //     cout << "TRT v_size: " << v_size << " & n_status: " << n_status << endl;
      
      //     for (uint32_t ii=0; ii<v_size; ii++)
      //     	cout << "TRT vint[" << ii << "] = " << hex << vint[ii] << dec << endl;
      
	uint32_t i=1;
	while( i < n_status )
	{
	  uint32_t word = vint[v_index++];
	  
	  for ( int j=0; j<2; j++ )
	  {
	    uint32_t stat = (word >> (16*j)) & 0xffff;
	    if ( stat )   // Skip 0 words, bug in ROD???
	    {
	      int DTMROC_index = stat & 0x7f;
	      int DTMROC_head = (stat >> 7) & 0x1ff;
		  
	      uint32_t Index = (rod_SourceID << 8) | DTMROC_index;
		  
		  //	      cout << i << "/" << j << ":" << hex << Index << dec << " " 
		  //		   << hex << DTMROC_head << dec << ": " << endl;
		  
	      if ( DTMROC_head )
	      {
		int D_sid = DTMROC_head & 0x100;
		uint32_t D_L1ID = (DTMROC_head >> 5) & 0x7;
		uint32_t D_BCID = (DTMROC_head >> 1) & 0xf;
		int D_error = !(DTMROC_head & 1);
		      
		      
		if ( m_lookAtSidErrors && D_sid )
		{
		  //		    cout << "sid ";
		  m_bsErrSvc->add_sid_error( Index );
		  sid_errors++;
		}
		      
		if ( m_lookAtErrorErrors && D_error )
		{
		  //		    cout << "err ";
		  m_bsErrSvc->add_error_error( Index );
		  error_errors++;
		}
		      
		if ( m_lookAtL1idErrors && (D_L1ID != (rod_L1ID & 0x7)) )
		{
		  //		    cout << "l1(" << hex << D_L1ID << "/" 
		  //			 << (rod_L1ID & 0x7) << dec;
		  m_bsErrSvc->add_l1id_error( Index, D_L1ID );
		  l1id_errors++;
		}

		/*
		 * We need to account for the fact that we EXPECT the BCIDs
		 * to be off by 12 for the first 7*16 bunch crossing due to
		 * the way our timing works.  ugh!
		 */
		uint32_t expected_BCID;
		if ( rod_BCID < 7*16 )
		  expected_BCID = (rod_BCID + 12) & 0xf;
		else
		  expected_BCID = rod_BCID & 0xf;

		if ( m_lookAtBcidErrors && (D_BCID != expected_BCID) )
		{
		  //		    cout << "bc(" << hex << D_BCID << "/" 
		  //			 << expected_BCID << dec;
		  m_bsErrSvc->add_bcid_error( Index, D_BCID );
		  bcid_errors++;
		}
	      } 
	      else if ( m_lookAtMissingErrors )
	      {
		//		 cout << "mis ";
		m_bsErrSvc->add_missing_error( Index );
		missing_errors++;
	      }
		  
	      //	      cout << endl;
	    } // end of non-zero status check
	  }
	  
	  i++;
	} // End of loop over status words
      }   // End of if on m_recordBSErrors

      uint32_t errorWord = 0;
      
      errorWord  = l1id_errors;
      errorWord |= (bcid_errors << 7);
      errorWord |= (missing_errors << 14);
      errorWord |= (error_errors << 21);
      if ( sid_errors > 0xf ) 
	sid_errors = 0xf;
      errorWord |= ((sid_errors & 0xf) << 28);   // Only report first 15
      
      if ( errorWord )
      { 
	 //	 std::cout << "ROD id: " << std:: hex <<  rod_SourceID
	 //		   << "errorWord: " << errorWord << std::dec << std::endl;
	 sc=StatusCode::RECOVERABLE;
      }
      
    }

  //  ATH_MSG_INFO( "out of fillCollection: robid = " << MSG::hex << robid << MSG::dec );
  
    return sc;
}


/* ----------------------------------------------------------
 * internal method to fill the collections into the IDC, used for
 * Expanded Mode data.
 *
 * This mode pads the most significant 5 bits with 0s or 1s, depending
 * on the version of the software/hardware generating the BS.
 *
 * If we fail to add a collection to the IDC, we print an ERROR and
 * return the StatusCode from IDC::addCollection()
 *
 * If we have been set up in an inconsistant state, wrt type of RDO to
 * create, we have a *serious* problem.  We print a FATAL and return
 * FAILURE.
 * ----------------------------------------------------------
 */
StatusCode
TRT_RodDecoder::int_fillExpanded( const ROBFragment* robFrag,
				  TRT_RDO_Container* rdoIdc,
				  std::vector<IdentifierHash>* vecHash )
{
  // get the ROBid
  uint32_t robid = robFrag->rod_source_id();

  // get the ROD version. It could be used to decode the data in one
  // way or another
  //  eformat::helper::Version rodVersion(robFrag->rod_version()); 
  //  const uint16_t rodMinorVersion= rodVersion.minor(); 

  // get the phasetime for CTB/Cosmics
  int phasetime = 0;
  if ( m_TB04_RawData ) phasetime = robFrag->rod_lvl1_trigger_type();

#ifdef TRT_BSC_DEBUG
  ATH_MSG_DEBUG( "fillCollections for " << MSG::hex << robid << MSG::dec );
#endif
  
  uint32_t         word;
  uint32_t         digit;
  Identifier       idStraw;
  IdentifierHash   idHash, skipHash=0xffffffff,lastHash=0xffffffff;
  const Identifier NULLId(0);

  TRT_RDORawData*                   rdo     = 0;
  TRT_RDO_Collection*               theColl = 0;
  TRT_RDO_Container::const_iterator itColl;

  // get the data of the fragment
  OFFLINE_FRAGMENTS_NAMESPACE::PointerType vint;
  robFrag->rod_data( vint );
  
  // loop over the data in the fragment
  unsigned int i;
  uint32_t     v_size       = robFrag->rod_ndata();
  int          bufferOffset = -1;
  for ( i=0; i<v_size; i++ )
    {
      // increment offest
      bufferOffset++;

      // get the raw data bit word
      word  = vint[i];
      // mask it
      digit = word & 0x07ffffff;          // We only use 27 bits
      if ( !digit ) continue;             // Empty Straw

      
#ifdef TRT_BSC_DEBUG
      ATH_MSG_VERBOSE( (hex) << robid << " " << bufferOffset << " " << word \
		       << " " << idHash << (dec) << " " << m_trt_id->print_to_string(idStraw) )
#endif
      
      // Make an Identifier for the RDO and get the IdHash
      idStraw = m_CablingSvc->getIdentifier( (eformat::SubDetector) 0 /*unused*/, robid, 
					    bufferOffset, idHash );

      if ( NULLId == idStraw )
	{
#ifdef TRT_BSC_DEBUG
	   ATH_MSG_DEBUG( "Null Identifier for bufferOffset " \
			  << bufferOffset << " word " << MSG::hex << word \
			  << MSG::dec )
#endif
	  continue;
	}

#ifdef TRT_BSC_DEBUG
      ATH_MSG_DEBUG( " Collection ID = " << idHash  \
	     << " Straw ID = " << m_trt_id->show_to_string( idStraw ) );
#endif
      
      // this option is for the trigger, if there is a vecHash* given, test it !
      if (vecHash)
	{
	  if (idHash == skipHash)
	    {
#ifdef TRT_BSC_DEBUG
	       ATH_MSG_DEBUG( "Collection for Hash not to be decoded, skip" );
#endif
	      continue;
	    }
	  else if (idHash != lastHash)
	    {
#ifdef TRT_BSC_DEBUG
	       ATH_MSG_DEBUG( "New hash, see if we should decode it" );
#endif
	      lastHash = idHash;
	      // maybe the new hash is not in the list, so test it
	      std::vector<IdentifierHash>::iterator p = find(vecHash->begin(),vecHash->end(),idHash);
	      if (p == vecHash->end())
		{
#ifdef TRT_BSC_DEBUG
		   ATH_MSG_DEBUG( "Collection for Hash not to be decoded, skip" );
#endif
		  // remember this one, so that we do not find(...) forever
		  skipHash = idHash;
		  continue;
		}
	    }
	}

      // get the collection
      itColl = rdoIdc->indexFind( idHash );

      // Check if the Collection is already created.
      if ( itColl != rdoIdc->end() )
	// this is really nasty !
	theColl = const_cast<TRT_RDO_Collection*>(&**itColl);
      else
	{
#ifdef TRT_BSC_DEBUG
	   ATH_MSG_DEBUG( " Collection ID = " << idHash \
			  << " does not exist, create it " );
#endif
	  // create new collection
	  theColl = new TRT_RDO_Collection ( idHash );
	  // get identifier from the hash, this is not nice
	  Identifier ident;
	  m_trt_id->get_id(idHash,ident,&m_straw_layer_context);
	  // get the Identifier to be nice to downstream clients
	  theColl->setIdentifier(ident);
	  // add collection into IDC
	  StatusCode sc = rdoIdc->addCollection(theColl, idHash);
	  if ( sc.isFailure() )
	  {
	     ATH_MSG_WARNING ( "Failed to add TRT RDO collection to container" );
	    return sc;
	  }
	}

      // Now the Collection is there for sure. Create RDO and push it
      // into Collection. 
      if ( m_TB04_RawData && ! m_LoLumRawData )
	rdo = new TRT_TB04_RawData( idStraw, digit, phasetime );
      else if ( m_LoLumRawData && ! m_TB04_RawData )
	rdo = new TRT_LoLumRawData( idStraw, digit ); 
      else
	{
	   ATH_MSG_FATAL( " Inconsistient setting of decoder, this is a bug" );
	  return StatusCode::FAILURE;
	}

      // add the RDO
      theColl->push_back( rdo );
      
    } // End of loop over all words in ROD
  
  return StatusCode::SUCCESS;
}


/* ----------------------------------------------------------
 * internal method to fill the collections into the IDC, used for
 * Minimal Compression Mode data.
 *
 * This is a reduced case of Full Compression Mode.  Straw words that
 * are 0 are compressed to 1 bit: "1".  Everything else is expanded by
 * prepending 5 "0" bits to the literal straw word.  
 *
 * Note that the input data words are filled "backwords": from LSB to
 * MSB.  For literal straw words, this means that we reading from LSB
 * to MSB, we have 5 0s, followed by the LSB of the straw word.
 *
 * If the incoming BS is not valid, ie the first 0 bit isn't followed
 * by 4 more 0s, we print an ERROR and return FAILURE.
 *
 * If we fail to add a collection to the IDC, we print an ERROR and
 * return the StatusCode from IDC::addCollection()
 *
 * If we have been set up in an inconsistant state, wrt type of RDO to
 * create, we have a *serious* problem.  We print a FATAL and return
 * FAILURE.
 * ----------------------------------------------------------
 */

StatusCode
TRT_RodDecoder::int_fillMinimalCompress( const ROBFragment *robFrag,
					TRT_RDO_Container* rdoIdc,
					std::vector<IdentifierHash>* vecHash)
{
  static int err_count = 0;
  
  uint32_t robid = robFrag->rod_source_id();
  
  // get the ROD version. It could be used to decode the data in one
  // way or another
  //  eformat::helper::Version rodVersion(m_robFrag->rod_version()); 
  //  const uint16_t rodMinorVersion= rodVersion.minor(); 
  
  int phasetime = 0;
  if ( m_TB04_RawData ) phasetime = robFrag->rod_lvl1_trigger_type();
  
#ifdef TRT_BSC_DEBUG
  ATH_MSG_DEBUG( "fillCollection3 for " \
		 << MSG::hex << robid << MSG::dec );
#endif
  
  uint32_t         word;
  uint32_t         digit;
  Identifier       idStraw;
  IdentifierHash   idHash, skipHash=0xffffffff, lastHash=0xffffffff;
  const Identifier NULLId(0);
  
  TRT_RDORawData*                   rdo     = 0;
  TRT_RDO_Collection*               theColl = 0;
  TRT_RDO_Container::const_iterator itColl;
  
  // get the data of the fragment
  OFFLINE_FRAGMENTS_NAMESPACE::PointerType vint;
  robFrag->rod_data( vint );
  
  int bit=0;
  int v;
  
  // loop over the data in the fragment and decode the bits
  unsigned int in_ptr       = 0;
  unsigned int out_ptr      = 0;
  int          bufferOffset = -1;
  uint32_t     v_size       = robFrag->rod_ndata();

  //  ATH_MSG_WARNING( "vint[" << in_ptr << "] = " << MSG::hex << vint[in_ptr] << MSG::dec );

  while ( (in_ptr < v_size) && (out_ptr < 1920) )    // XXX -- avoid HardCode!
    {
      // increment buffer offset
      bufferOffset++;
      out_ptr++;

      // get the next word from the bits
      word = 0;
      v    = (vint[in_ptr] >> bit) & 0x1;
      bit++;
      if ( bit > 31 ) {
	in_ptr++;
	bit = 0;
	//  ATH_MSG_WARNING( "vint[" << in_ptr << "] = " << MSG::hex << vint[in_ptr] << MSG::dec );
      }
      if ( v ) continue;
      else {
	for ( int i=0; i<4; i++ ) {          // Look for 4 more 0's
	  v = (vint[in_ptr] >> bit) & 0x1;
	  bit++;
	  if ( bit > 31 ) {
	    in_ptr++;
	    bit = 0;
	    //  ATH_MSG_WARNING( "vint[" << in_ptr << "] = " << MSG::hex << vint[in_ptr] << MSG::dec );
	  }
	  if ( v ) {
	    if ( err_count < 100 ) {
	       ATH_MSG_WARNING( "Invalid ByteStream, ROD ID = " \
				<< MSG::hex << robid << MSG::dec );
	      err_count++;
	    } else if ( 100 == err_count ) {
	       ATH_MSG_WARNING( "Too many Invalid ByteStream messages  " \
				<< "Turning message off." );
	      err_count++;
	    }
	    return StatusCode::RECOVERABLE;
	  }
	}
	for ( int i=0; i<27; i++ ) {
	  word = word  | (((vint[in_ptr] >> bit) & 0x1) << i);
	  bit++;
	  if ( bit > 31 ) {
	    in_ptr++;
	    bit = 0;
	    //  ATH_MSG_WARNING( "vint[" << in_ptr << "] = " << MSG::hex << vint[in_ptr] << MSG::dec );
	  }
	}
      }
      
      // get data word
      digit = word;                       // We only use 27 bits

      
#ifdef TRT_BSC_DEBUG
      ATH_MSG_VERBOSE( (hex) << robid << " " << bufferOffset << " " << word \
		       << " " << idHash << (dec) << " " << m_trt_id->print_to_string(idStraw) );
#endif
    
      // Make an Identifier for the RDO and get the IdHash
      idStraw = m_CablingSvc->getIdentifier( (eformat::SubDetector) 0 /*unused*/, robid, 
					    bufferOffset, idHash );
    
      if ( NULLId == idStraw )
	{
#ifdef TRT_BSC_DEBUG
	   ATH_MSG_DEBUG( "Null Identifier for bufferOffset " << bufferOffset );
#endif
	continue;
      }

#ifdef TRT_BSC_DEBUG
      ATH_MSG_DEBUG( " Collection ID = " << idHash  \
		    << " Straw ID = " << m_trt_id->show_to_string( idStraw ) );
#endif
      
      // this option is for the trigger, if there is a vecHash* given, test it !
      if (vecHash)
	{
	  if (idHash == skipHash)
	    {
#ifdef TRT_BSC_DEBUG
	       ATH_MSG_DEBUG( "Collection for Hash not to be decoded, skip" );
#endif
	      continue;
	    }
	  else if (idHash != lastHash)
	    {
#ifdef TRT_BSC_DEBUG
	       ATH_MSG_DEBUG( "New hash, see if we should decode it" );
#endif
	      lastHash = idHash;
	      // maybe the new hash is not in the list, so test it
	      std::vector<IdentifierHash>::iterator p = find(vecHash->begin(),vecHash->end(),idHash);
	      if (p == vecHash->end())
		{
#ifdef TRT_BSC_DEBUG
		   ATH_MSG_DEBUG( "Collection for Hash not to be decoded, skip" );
#endif
		  // remember this one, so that we do not find(...) forever
		  skipHash = idHash;
		  continue;
		}
	    }
	}
      
      // get the collection
      itColl = rdoIdc->indexFind( idHash );
      
      // Check if the Collection is already created.
      if ( itColl != rdoIdc->end() )
	// this is really nasty !
	theColl = const_cast<TRT_RDO_Collection*>(&**itColl);
      else
	{
#ifdef TRT_BSC_DEBUG
	   ATH_MSG_DEBUG( " Collection ID = " << idHash \
			  << " does not exist, create it " );
#endif
	  // create new collection
	  theColl = new TRT_RDO_Collection ( idHash );
	  // get identifier from the hash, this is not nice
	  Identifier ident;
	  m_trt_id->get_id( idHash, ident, &m_straw_layer_context );
	  // get the Identifier to be nice to downstream clients
	  theColl->setIdentifier(ident);
	  // add collection into IDC
	  StatusCode sc = rdoIdc->addCollection(theColl, idHash);
	  if ( sc.isFailure() )
	  {
	     ATH_MSG_WARNING ( "Failed to add TRT RDO collection to container" );
	    return sc;
	  }
	}
      
      // Now the Collection is there for sure. Create RDO and push it
      // into Collection. 

      //ATH_MSG_INFO ( "idStraw: " << idStraw 
      //               << " digit: " << MSG::hex << digit << MSG::dec );

      if ( m_TB04_RawData )
      {
	rdo = new TRT_TB04_RawData( idStraw, digit, phasetime );
	m_Nrdos++;
      }
      else if ( m_LoLumRawData )
      {
	rdo = new TRT_LoLumRawData( idStraw, digit ); 
	m_Nrdos++;
      }
      else
      {
	ATH_MSG_FATAL( " Inconsistient setting of decoder, this is a bug" );
	return StatusCode::FAILURE;
      }
      
      // get the collection
      // add the RDO
      theColl->push_back( rdo );
      
    }  //   End of loop over all words in ROD
  
  return StatusCode::SUCCESS;
}


/* ----------------------------------------------------------
 * internal method to fill the collections into the IDC, used for
 * Full Compression Mode data.
 *
 * Note that the input data words are filled "backwords": from LSB to
 * MSB.  For literal straw words, this means that we reading from LSB
 * to MSB, we have 5 0s, followed by the LSB of the straw word.
 *
 * If the incoming BS is not valid, ie the first 0 bit isn't followed
 * by 4 more 0s, we print an ERROR and return FAILURE.
 *
 * If we fail to add a collection to the IDC, we print an ERROR and
 * return the StatusCode from IDC::addCollection()
 *
 * If we have been set up in an inconsistant state, wrt type of RDO to
 * create, we have a *serious* problem.  We print a FATAL and return
 * FAILURE.
 * ----------------------------------------------------------
 */

StatusCode
TRT_RodDecoder::int_fillFullCompress( const ROBFragment *robFrag,
				      TRT_RDO_Container* rdoIdc,
				      t_CompressTable* Ctable,
				      std::vector<IdentifierHash>* vecHash)
{
  static int err_count = 0;

  int phase;
  for ( phase=0; phase<2; phase++ )
  {

  uint32_t robid = robFrag->rod_source_id();
  
  // get the ROD version. It could be used to decode the data in one
  // way or another
  //  eformat::helper::Version rodVersion(m_robFrag->rod_version()); 
  //  const uint16_t rodMinorVersion= rodVersion.minor(); 
  
  int phasetime = 0;
  if ( m_TB04_RawData ) phasetime = robFrag->rod_lvl1_trigger_type();
  
#ifdef TRT_BSC_DEBUG
  ATH_MSG_DEBUG( "fillCollection3 for " \
		 << MSG::hex << robid << MSG::dec );
#endif
  
  uint32_t         word;
  uint32_t         digit;
  Identifier       idStraw;
  IdentifierHash   idHash, skipHash=0xffffffff, lastHash=0xffffffff;
  const Identifier NULLId(0);
  
  TRT_RDORawData*                   rdo     = 0;
  TRT_RDO_Collection*               theColl = 0;
  TRT_RDO_Container::const_iterator itColl;
  
  // get the data of the fragment
  OFFLINE_FRAGMENTS_NAMESPACE::PointerType vint;
  robFrag->rod_data( vint );
  
  int bit=0;
  int v,l;
  int i;
  
  // loop over the data in the fragment and decode the bits
  unsigned int in_ptr       = 0;
  unsigned int out_ptr      = 0;
  int          bufferOffset = -1;
  uint32_t     v_size       = robFrag->rod_ndata();

  //  ATH_MSG_WARNING( "vint[" << in_ptr << "] = " << MSG::hex << vint[in_ptr] << MSG::dec );

  while ( (in_ptr < v_size) && (out_ptr < 1920) )    // XXX -- avoid HardCode!
  {
    // increment buffer offset
    bufferOffset++;
    out_ptr++;

    // get the next word from the bits
    word = 0;
    v    = (vint[in_ptr] >> bit) & 0x1;
    bit++;
    if ( bit > 31 ) 
    {
      in_ptr++;
      bit = 0;
      //  ATH_MSG_WARNING( "vint[" << in_ptr << "] = " << MSG::hex << vint[in_ptr] << MSG::dec );
    }
    l = 1;

    //ATH_MSG_INFO( "l, firstcode, v " << l << " " 
    //		  << MSG::hex << Ctable->m_firstcode[l] << " " << v << MSG::dec );

    while ( v < Ctable->m_firstcode[l] )
    {
      v = 2 * v + ((vint[in_ptr] >> bit) & 0x1);
      
      bit++;
      if ( bit > 31 ) 
      {
	in_ptr++;
	bit = 0;
	//  ATH_MSG_WARNING( "vint[" << in_ptr << "] = " << MSG::hex << vint[in_ptr] << MSG::dec );
      }

      l++;

      //ATH_MSG_INFO( "l, firstcode, v " << l << " " 
      //		  << MSG::hex << Ctable->m_firstcode[l] << " " << v << MSG::dec );
    }

    int idx = Ctable->m_lengths_integral[l] + (v - Ctable->m_firstcode[l]);

    //ATH_MSG_INFO ( "lengths_int, idx, syms " << 
    //	   Ctable->m_lengths_integral[l] << " " << idx << " " << MSG::hex <<
    //	   idx << " " << Ctable->m_syms[idx] << MSG::dec );

    if ( idx <= Ctable->m_Nsymbols )
      word = Ctable->m_syms[idx];
    else
    {
      if ( err_count < 100 ) 
      {
	ATH_MSG_WARNING( "Invalid ByteStream, ROD ID = "		\
			 << MSG::hex << robid << MSG::dec );
	err_count++;
      }
      else if ( 100 == err_count ) 
      {
	ATH_MSG_WARNING( "Too many Invalid ByteStream messages  "	\
			 << "Turning message off." );
	err_count++;
      }

      return StatusCode::RECOVERABLE;
    }


    /*
     * Handle case of escaped literal
     */
    if ( word == m_escape_marker )
    {
      word = 0;
      for ( i=0; i<27; i++ )
      {
	word = word  | (((vint[in_ptr] >> bit) & 0x1) << i);
	bit++;
	if ( bit > 31 )
	{
	  in_ptr++;
	  bit = 0;
	}
      }
    }


    if ( 1 == phase )
    {

    if ( word )
    {
      //      std::cout << "PTK: " << bufferOffset << " : " << out_ptr << " : " << word << std::endl;

      // get data word
      digit = word;                       // We only use 27 bits

      
#ifdef TRT_BSC_DEBUG
      ATH_MSG_VERBOSE( (hex) << robid << " " << bufferOffset << " " << word \
		       << " " << idHash << (dec) << " " << m_trt_id->print_to_string(idStraw) );
#endif
    
      // Make an Identifier for the RDO and get the IdHash
      idStraw = m_CablingSvc->getIdentifier( (eformat::SubDetector) 0 /*unused*/, robid, 
					    bufferOffset, idHash );
    
      if ( NULLId == idStraw )
	{
#ifdef TRT_BSC_DEBUG
	   ATH_MSG_DEBUG( "Null Identifier for bufferOffset " << bufferOffset );
#endif
	continue;
      }

#ifdef TRT_BSC_DEBUG
      ATH_MSG_DEBUG( " Collection ID = " << idHash  \
		    << " Straw ID = " << m_trt_id->show_to_string( idStraw ) );
#endif
      
      // this option is for the trigger, if there is a vecHash* given, test it !
      if (vecHash)
	{
	  if (idHash == skipHash)
	    {
#ifdef TRT_BSC_DEBUG
	       ATH_MSG_DEBUG( "Collection for Hash not to be decoded, skip" );
#endif
	      continue;
	    }
	  else if (idHash != lastHash)
	    {
#ifdef TRT_BSC_DEBUG
	       ATH_MSG_DEBUG( "New hash, see if we should decode it" );
#endif
	      lastHash = idHash;
	      // maybe the new hash is not in the list, so test it
	      std::vector<IdentifierHash>::iterator p = find(vecHash->begin(),vecHash->end(),idHash);
	      if (p == vecHash->end())
		{
#ifdef TRT_BSC_DEBUG
		   ATH_MSG_DEBUG( "Collection for Hash not to be decoded, skip" );
#endif
		  // remember this one, so that we do not find(...) forever
		  skipHash = idHash;
		  continue;
		}
	    }
	}
      
      // get the collection
      itColl = rdoIdc->indexFind( idHash );
      
      // Check if the Collection is already created.
      if ( itColl != rdoIdc->end() )
	// this is really nasty !
	theColl = const_cast<TRT_RDO_Collection*>(&**itColl);
      else
	{
#ifdef TRT_BSC_DEBUG
	   ATH_MSG_DEBUG( " Collection ID = " << idHash \
			  << " does not exist, create it " );
#endif
	  // create new collection
	  theColl = new TRT_RDO_Collection ( idHash );
	  // get identifier from the hash, this is not nice
	  Identifier ident;
	  m_trt_id->get_id( idHash, ident, &m_straw_layer_context );
	  // get the Identifier to be nice to downstream clients
	  theColl->setIdentifier(ident);
	  // add collection into IDC
	  StatusCode sc = rdoIdc->addCollection(theColl, idHash);
	  if ( sc.isFailure() )
	  {
	     ATH_MSG_WARNING ( "Failed to add TRT RDO collection to container" );
	    return sc;
	  }
	}
      
      // Now the Collection is there for sure. Create RDO and push it
      // into Collection. 

      //      ATH_MSG_INFO ( "idStraw: " << idStraw 
      //		     << " digit: " << MSG::hex << digit << MSG::dec );

      if ( m_TB04_RawData )
      {
	rdo = new TRT_TB04_RawData( idStraw, digit, phasetime );
	m_Nrdos++;
      }
      else if ( m_LoLumRawData )
      {
	rdo = new TRT_LoLumRawData( idStraw, digit ); 
	m_Nrdos++;
      }
      else
	{
	   ATH_MSG_FATAL( " Inconsistient setting of decoder, this is a bug" );
	  return StatusCode::FAILURE;
	}
      
      // get the collection
      // add the RDO
      theColl->push_back( rdo );
    }
    } // if phase == 1
  }  //   End of loop over all words in ROD


  //  ATH_MSG_INFO( "Input: " << in_ptr << " / " << v_size << "   Output: " << out_ptr << " / 1920" );

  if ( (out_ptr != 1920) || ((in_ptr == v_size) && bit != 0) ||
       ((in_ptr == (v_size - 1)) && (bit == 0)) || (in_ptr < (v_size -1)) )
  {
    ATH_MSG_WARNING( "Decode error: " 
		     << "L1ID = " << MSG::hex << robFrag->rod_lvl1_id()
		     << " ROD = " << robFrag->rod_source_id() << MSG::dec
		     << " bit = " << bit << "  "
		     << in_ptr << " / " << v_size 
		     << " : " << out_ptr << " / 1920" );
    return StatusCode::RECOVERABLE;
  }

  }

  return StatusCode::SUCCESS;
}


/*
 * Read Compression Table from file
 */
StatusCode
TRT_RodDecoder::ReadCompressTableFile(  std::string 
#ifdef TRT_READCOMPTABLE_FILE
TableFilename
#endif
 )
{  
   ATH_MSG_FATAL( "Reading Compression Table from File is not supported anymore!" );

   return StatusCode::FAILURE;

#ifdef TRT_READCOMPTABLE_FILE

  t_CompressTable *Ctable = new t_CompressTable;

  ATH_MSG_INFO( "Reading Compress Table File: " << TableFilename );

  std::string file = PathResolver::find_file ( TableFilename, "DATAPATH" );
  std::ifstream inFile ( file.c_str() );

  if (!inFile.is_open())
  {
     ATH_MSG_FATAL( "Could not open Compression Table File " 
		    << TableFilename );

     delete Ctable;

     return StatusCode::FAILURE;
  }


#define MAXLINE 1024

  char line[MAXLINE];
  char *tok;

  int *lengths=0;                // Array of codeword lengths
  int *codewords=0;              // Array of codewords


  Ctable->m_Nsymbols = 0;

  while( ! inFile.eof() )
  {
    inFile.getline( line, MAXLINE );

    tok = strtok( line, " \t\n<" );
    if ( ! tok )
      continue;


    /*************************************/
    if ( ! strncmp( tok, "Version", 7 ) )
    {
      tok = strtok( NULL, " \t\n" );
      Ctable->m_TableVersion = atoi( tok );

      ATH_MSG_DEBUG( "Table Version = " << Ctable->m_TableVersion );

      tok = strtok( NULL, " \t\n" );
      if ( ! tok )
      {
	inFile.getline( line, MAXLINE );

	tok = strtok( line, " \t\n" );
      }

      if ( strncmp( tok, ">", 1 ) )
      {
	ATH_MSG_WARNING( "Invalid file format in Version!" );
	inFile.close();

	if ( lengths )
	  delete[] lengths;

	if( codewords )
	  delete[] codewords;

	delete Ctable;

	return StatusCode::FAILURE;
      }

      Ctable->m_syms       = new unsigned int[ Ctable->m_Nsymbols ];

      if ( lengths )
	 delete[] lengths;
      lengths    = new int[ Ctable->m_Nsymbols ];

      if ( codewords )
	 delete[] codewords;
      codewords  = new int[ Ctable->m_Nsymbols ];
    }


    /*************************************/
    if ( ! strncmp( tok, "Nsymbols", 8 ) )
    {
      tok = strtok( NULL, " \t\n" );
      Ctable->m_Nsymbols = atoi( tok );

      ATH_MSG_DEBUG( "Nsymbols = " << Ctable->m_Nsymbols );

      tok = strtok( NULL, " \t\n" );
      if ( ! tok )
      {
	inFile.getline( line, MAXLINE );

	tok = strtok( line, " \t\n" );
      }

      if ( strncmp( tok, ">", 1 ) )
      {
	ATH_MSG_WARNING( "Invalid file format in Nsymbols!" );
	inFile.close();

	if ( lengths )
	  delete[] lengths;

	if( codewords )
	  delete[] codewords;

	delete Ctable;

	return StatusCode::FAILURE;
      }

      Ctable->m_syms       = new unsigned int[ Ctable->m_Nsymbols ];

      if ( lengths )
	 delete[] lengths;
      lengths    = new int[ Ctable->m_Nsymbols ];

      if ( codewords )
	 delete[] codewords;
      codewords  = new int[ Ctable->m_Nsymbols ];
    }


    /*************************************/
    if ( ! strncmp( tok, "syms", 4 ) )
    {

      if ( ! Ctable->m_syms )
      {
	ATH_MSG_WARNING( "Invalid file format Nsymbols must come first!" );
	inFile.close();
	
	if ( lengths )
	  delete[] lengths;

	if( codewords )
	  delete[] codewords;

	delete Ctable;

	return StatusCode::FAILURE;
      }

      int i=0;

      tok = strtok( NULL, " \t\n" );
      while( i < Ctable->m_Nsymbols )
      {
	while ( (tok) && (i < Ctable->m_Nsymbols) )
	{
	  Ctable->m_syms[i++] = atoi( tok );
	  tok = strtok( NULL, " \t\n" );
	}

	if ( !tok )
	{
	  inFile.getline( line, MAXLINE );
	  tok = strtok( line, " \t\n" );
	}
      }

      if ( strncmp( tok, ">", 1 ) )
      {
	ATH_MSG_WARNING( "Invalid file format in syms!" );
	inFile.close();

	if ( lengths )
	  delete[] lengths;

	if( codewords )
	  delete[] codewords;

	delete Ctable;

	return StatusCode::FAILURE;
      }
    }

    /*************************************/
    if ( ! strncmp( tok, "codewords", 9 ) )
    {
      if ( ! codewords )
      {
	ATH_MSG_WARNING( "Invalid file format Nsymbols must come first!" );
	inFile.close();
	
	if ( lengths )
	  delete[] lengths;

	delete Ctable;

	return StatusCode::FAILURE;
      }


      int i=0;

      tok = strtok( NULL, " \t\n" );
      while( i < Ctable->m_Nsymbols )
      {
	while ( (tok) && (i < Ctable->m_Nsymbols) )
	{
	  codewords[i++] = atoi( tok );
	  tok = strtok( NULL, " \t\n" );
	}

	if ( !tok )
	{
	  inFile.getline( line, MAXLINE );
	  tok = strtok( line, " \t\n" );
	}
      }

      if ( strncmp( tok, ">", 1 ) )
      {
	ATH_MSG_WARNING( "Invalid file format in codewords!" );
	inFile.close();

	if ( lengths )
	  delete[] lengths;

	if( codewords )
	  delete[] codewords;

	delete Ctable;

	return StatusCode::FAILURE;
      }

    }


    /*************************************/
    if ( ! strncmp( tok, "firstcode",9 ) )
    {
      int i=1;

      tok = strtok( NULL, " \t\n" );
      while( i < 33 )
      {
	while ( (tok) && (i < 33) )
	{
	  Ctable->m_firstcode[i++] = atoi( tok );
	  tok = strtok( NULL, " \t\n" );
	}

	if ( !tok )
	{
	  inFile.getline( line, MAXLINE );
	  tok = strtok( line, " \t\n" );
	}
      }

      if ( strncmp( tok, ">", 1 ) )
      {
	ATH_MSG_WARNING( "Invalid file format in firstcode" );
	inFile.close();

	if ( lengths )
	  delete[] lengths;

	if( codewords )
	  delete[] codewords;

	delete Ctable;

	return StatusCode::FAILURE;
      }

    }


    /*************************************/
    if ( ! strncmp( tok, "lengths_integral", 16 ) )
    {
      int i=1;

      tok = strtok( NULL, " \t\n" );
      while( i < 33 )
      {
	while ( (tok) && (i < 33) )
	{
	  Ctable->m_lengths_integral[i++] = atoi( tok );
	  tok = strtok( NULL, " \t\n" );
	}

	if ( !tok )
	{
	  inFile.getline( line, MAXLINE );
	  tok = strtok( line, " \t\n" );
	}
      }

      if ( strncmp( tok, ">", 1 ) )
      {
	ATH_MSG_WARNING( "Invalid file format in lengths_integral!" );
	inFile.close();

	if ( lengths )
	  delete[] lengths;

	if( codewords )
	  delete[] codewords;

	delete Ctable;

	return StatusCode::FAILURE;
      }

    }

    /*************************************/
    if ( ! strncmp( tok, "lengths", 7 ) )
    {
      if ( ! lengths )
      {
	ATH_MSG_WARNING( "Invalid file format Nsymbols must come first!" );
	inFile.close();
	
	if( codewords )
	  delete[] codewords;

	delete Ctable;

	return StatusCode::FAILURE;
      }

      int i=0;

      tok = strtok( NULL, " \t\n" );
      while( i < Ctable->m_Nsymbols )
      {
	while ( (tok) && (i < Ctable->m_Nsymbols) )
	{
	  lengths[i++] = atoi( tok );
	  tok = strtok( NULL, " \t\n" );
	}

	if ( !tok )
	{
	  inFile.getline( line, MAXLINE );
	  tok = strtok( line, " \t\n" );
	}
      }

      if ( strncmp( tok, ">", 1 ) )
      {
	ATH_MSG_WARNING( "Invalid file format in lengths!" );
	inFile.close();

	if ( lengths )
	  delete[] lengths;

	if( codewords )
	  delete[] codewords;

	delete Ctable;

	return StatusCode::FAILURE;
      }

    }

  }

  inFile.close();



  if ( ! codewords || ! lengths )
  {
    ATH_MSG_WARNING( "Invalid file format Nsymbols must come first!" );
	
    if ( lengths )
      delete[] lengths;

    if( codewords )
      delete[] codewords;

    delete Ctable;

    return StatusCode::FAILURE;
  }



  /*
   * Recover escape info from the table
   */
  int i=Ctable->m_Nsymbols-1;
  int escape_length=0;
  //  uint32_t escape_codeword;  // Set but not used

  while ( (m_escape_marker != Ctable->m_syms[i]) && (i >=0) )
    i--;

  if ( i < 0 )
  {
    ATH_MSG_WARNING( "Escape code not found!" );

    if ( lengths )
      delete[] lengths;

    if( codewords )
      delete[] codewords;

    return StatusCode::FAILURE;
  }
  else
  {
     //    escape_codeword = codewords[i];   // Set but not used
    escape_length = lengths[i];

    if ( escape_length != 5 )
      ATH_MSG_WARNING( "WARNING!  Escape code length is " << escape_length \
		       << " rather than 5!" );
  }

#ifdef PRINT_TABLE
  for ( int j=0; j<Ctable->m_Nsymbols; j++ )
  {
    ATH_MSG_INFO( "Table: " << j << " " 
		  << lengths[j] << " "  
		  << MSG::hex << codewords[j] << " " 
		  << Ctable->m_syms[j] << std::dec );
  }
#endif // PRINT_TABLE

  if ( lengths )
    delete[] lengths;

  if( codewords )
    delete[] codewords;


  if ( Ctable->m_TableVersion  > m_maxCompressionVersion )
  {
    ATH_MSG_WARNING( "Invalid Compression Table Version: " <<
		     Ctable->m_TableVersion );

    delete Ctable;

    return StatusCode::FAILURE;
  }


  if ( m_compressTableLoaded[Ctable->m_TableVersion] ) 
  {
    ATH_MSG_WARNING( "Table " << Ctable->m_TableVersion 
		     << " already loaded!  Not overwriting" );
    delete Ctable;
  }
  else
  {
    m_CompressionTables[Ctable->m_TableVersion] = Ctable;
    ATH_MSG_INFO( "Loaded Compress Table Version: " << Ctable->m_TableVersion );

    m_compressTableLoaded[Ctable->m_TableVersion] = true;
  }


  return StatusCode::SUCCESS;

#endif /* TRT_READCOMPTABLE_FILE */

}



/*
 * Read Compression Table from DB on IOV change
 */
StatusCode
TRT_RodDecoder::update( IOVSVC_CALLBACK_ARGS_P(I,keys) )
{  

  ATH_MSG_INFO ("Updating TRT_RodDecoder Compression Table from DB");
 
  /*
   * Callback function to update compression table when condDB data changes:
   */

  for( std::list<std::string>::const_iterator key=keys.begin(); 
       key != keys.end(); ++key)
  {
    ATH_MSG_INFO("IOVCALLBACK for key " << *key << " number " << I);
  }
 

  const DataHandle<CondAttrListCollection> catrlist;

  if ( StatusCode::SUCCESS == 
       detStore()->retrieve( catrlist, m_compressTableFolder )
       && catrlist != 0) 
  {

    CondAttrListCollection::const_iterator catrIt (catrlist->begin());
    CondAttrListCollection::const_iterator last_catr (catrlist->end());

    while ( catrIt != last_catr )
    {
       t_CompressTable *Ctable = new t_CompressTable;

       const AthenaAttributeList atrlist(catrIt->second);
     

       Ctable->m_TableVersion = (atrlist)["Version"].data<cool::Int32>();


       if ( Ctable->m_TableVersion  > m_maxCompressionVersion )
       {
	 ATH_MSG_WARNING( "Invalid Compression Table Version: " <<
			  Ctable->m_TableVersion );

	 delete Ctable;
	 catrIt++;

	 continue;
       }


       if ( m_compressTableLoaded[Ctable->m_TableVersion] ) 
       {
	 ATH_MSG_WARNING( "Table " << Ctable->m_TableVersion 
			  << " already loaded!  Not overwriting" );
	 delete Ctable;
	 catrIt++;

	 continue;
       }


       Ctable->m_Nsymbols = (atrlist)["Nsymbols"].data<cool::Int32>();
       ATH_MSG_DEBUG( "Nsymbols = " << Ctable->m_Nsymbols );


       Ctable->m_syms       = new unsigned int[ Ctable->m_Nsymbols ];

       const cool::Blob16M& blob = (atrlist)["syms"].data<cool::Blob16M> ();

       if ( blob.size() != (unsigned int) (Ctable->m_Nsymbols * sizeof(unsigned int)) )
       {
	  ATH_MSG_ERROR( "Unexpected size of symbol table! ( " << blob.size()
			 << " != " 
			 << (Ctable->m_Nsymbols * sizeof(unsigned int))
			 << " )" );

	  delete[] Ctable->m_syms;
	  delete Ctable;

	  return StatusCode::FAILURE;
       }

       const unsigned char* BlobStart =
	  static_cast<const unsigned char*> (blob.startingAddress());
       int j = 0;
       for (int i = 0; (i < blob.size()) && (j < Ctable->m_Nsymbols);
	    i += sizeof(unsigned int))
       {
	  Ctable->m_syms[j++] = *((unsigned int*) (BlobStart + i));
       }

       std::istringstream 
	  iss((atrlist)["firstcode"].data<cool::String4k>());
       std::string tok;
       int i = 1;
       while ( getline(iss, tok, ' ') && (i < CTABLE_FC_LENGTH) ) 
       {
	  Ctable->m_firstcode[i++] = atoi(tok.c_str());
       }


       std::istringstream 
	  iss2((atrlist)["lengths_integral"].data<cool::String4k>());
       i = 1;
       while ( getline(iss2, tok, ' ') && (i < CTABLE_LI_LENGTH) ) 
       {
	  Ctable->m_lengths_integral[i++] = atoi(tok.c_str());
       }


       m_CompressionTables[Ctable->m_TableVersion] = Ctable;
       ATH_MSG_INFO( "Loaded Compress Table Version: " <<
		     Ctable->m_TableVersion );

       m_compressTableLoaded[Ctable->m_TableVersion] = true;


#ifdef NOTDEF
       if ( 0 )
       {
	  ATH_MSG_INFO( "Compress Table Version : " << Ctable->m_TableVersion );
	  ATH_MSG_INFO( "Compress Table Nsymbols: " << Ctable->m_Nsymbols );

	  int i;
	  for ( i=0; i<32; i++ )
	     ATH_MSG_INFO( "Compress Table firstcode  : " << i 
			   << " " << Ctable->m_firstcode[i] );

	  for ( i=0; i<32; i++ )
	     ATH_MSG_INFO( "Compress Table lengths_int: " << i
			   << " " << Ctable->m_lengths_integral[i] );


	  for ( i=0; i<10; i++ )
	     ATH_MSG_INFO( "Compress Table syms: " << i
			   << " " << Ctable->m_syms[i] );

	  ATH_MSG_INFO( "Compress Table syms: [...]" );

	  for ( i=0; i<10; i++ )
	     ATH_MSG_INFO( "Compress Table syms: " << (Ctable->m_Nsymbols - 10 + i)
			   << " " << Ctable->m_syms[(Ctable->m_Nsymbols - 10 + i)] );
       }
#endif /* NOTDEF */

       catrIt++;

    }
  }
  else
  {
     ATH_MSG_ERROR( "Could not read Compression Table from DB" );

     return StatusCode::FAILURE;
  }

  return StatusCode::SUCCESS;
}

#ifdef NOTDEF
void
TRT_RodDecoder::handle( const Incident &inc )
{  
  if ( inc.type() != "BeginRun" )
    return;

  t_CompressTable *Ctable = new t_CompressTable;

  ATH_MSG_INFO ("Updating TRT_RodDecoder Compression Table from DB in BeginRun handle");
 
 
   const AthenaAttributeList* atrlist=0;
     
   if ( StatusCode::SUCCESS == 
	detStore()->retrieve( atrlist, m_compressTableFolder )
	&& atrlist != 0) 
   {
     Ctable->m_TableVersion = (atrlist)["Version"].data<cool::Int32>();
     Ctable->m_Nsymbols = (atrlist)["Nsymbols"].data<cool::Int32>();

     Ctable->m_syms       = new unsigned int[ Ctable->m_Nsymbols ];

     const cool::Blob16M& blob = (atrlist)["syms"].data<cool::Blob16M> ();
     const unsigned char* BlobStart =
       static_cast<const unsigned char*> (blob.startingAddress());
     int j = 0;
     for (int i = 0; i < blob.size(); i += sizeof(unsigned int))
     {
       Ctable->m_syms[j++] = *((unsigned int*) (BlobStart + i));
     }

     std::istringstream 
       iss((atrlist)["firstcode"].data<cool::String4k>());
     std::string tok;
     int i = 0;
     while (getline(iss, tok, ' ')) 
     {
       Ctable->m_firstcode[i++] = atoi(tok.c_str());
     }


     std::istringstream 
       iss2((atrlist)["lengths_integral"].data<cool::String4k>());
     i = 0;
     while (getline(iss2, tok, ' ')) 
     {
       Ctable->m_lengths_integral[i++] = atoi(tok.c_str());
     }
   }


   return;
}
#endif /* NOTDEF */

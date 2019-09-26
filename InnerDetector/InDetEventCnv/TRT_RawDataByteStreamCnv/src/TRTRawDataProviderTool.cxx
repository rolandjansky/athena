/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/


//#include "TRT_ConditionsServices/ITRT_ByteStream_ConditionsSvc.h"

#include "TRTRawDataProviderTool.h"
#include "GaudiKernel/IToolSvc.h"
#include "InDetRawData/TRT_RDORawData.h"
#include "ByteStreamData/RawEvent.h" 
#include "GaudiKernel/ServiceHandle.h"
#include "StoreGate/StoreClearedIncident.h"
#include "StoreGate/WriteHandle.h"

using OFFLINE_FRAGMENTS_NAMESPACE::ROBFragment;


// -------------------------------------------------------
//



// -------------------------------------------------------
// default contructor

TRTRawDataProviderTool::TRTRawDataProviderTool
( const std::string& type, const std::string& name,const IInterface* parent )
  :  AthAlgTool( type, name, parent ),
     m_decoder   ("TRT_RodDecoder",this),
     m_bsErrSvc ("TRT_ByteStream_ConditionsSvc", name),
     m_LastLvl1ID(),
     m_LVL1Collection(nullptr),
     m_BCCollection(nullptr),
     m_storeInDetTimeColls(true)
{
  declareProperty ("Decoder", m_decoder);
  declareProperty ("BSCondSvc", m_bsErrSvc);
  declareProperty ("StoreInDetTimeCollections", m_storeInDetTimeColls);


  declareInterface< ITRTRawDataProviderTool >( this );   
}

// -------------------------------------------------------
// destructor 

TRTRawDataProviderTool::~TRTRawDataProviderTool()
{}
 
// -------------------------------------------------------
// initialize

StatusCode TRTRawDataProviderTool::initialize()
{

   StatusCode sc = AlgTool::initialize(); 
   if (sc.isFailure())
   {
      ATH_MSG_FATAL( "Failed to init baseclass" );
     return StatusCode::FAILURE;
   }

   // Retrieve decoder
   if (m_decoder.retrieve().isFailure()) {
      ATH_MSG_FATAL( "Failed to retrieve tool " << m_decoder );
     return StatusCode::FAILURE;
   } else 
      ATH_MSG_INFO( "Retrieved tool " << m_decoder );


  /*
   * Retrieve BS Error Service
   */
   if ( m_bsErrSvc.retrieve().isFailure() )
   {
     ATH_MSG_FATAL( "Failed to retrieve service " << m_bsErrSvc );
     return StatusCode::FAILURE;
   } else
     ATH_MSG_INFO( "Retrieved service " << m_bsErrSvc );


  //initialize LastLvl1ID, to keep track of new events.
  m_LastLvl1ID = 0xffffffff;

  //initialize write handles
  ATH_CHECK(m_lvl1idkey.initialize());
  ATH_CHECK(m_bcidkey.initialize());
  
  return StatusCode::SUCCESS;
}

// -------------------------------------------------------
// finalize

StatusCode TRTRawDataProviderTool::finalize()
{
   StatusCode sc = AlgTool::finalize(); 
   return sc;
}

// -------------------------------------------------------
// convert method

StatusCode TRTRawDataProviderTool::convert( std::vector<const ROBFragment*>& vecRobs,
					    TRT_RDO_Container*               rdoIdc )
{

  static std::atomic_int DecodeErrCount = 0;

  if(vecRobs.size() == 0) 
     return StatusCode::SUCCESS;



  std::vector<const ROBFragment*>::const_iterator rob_it = vecRobs.begin();
  
  // are we working on a new event ?
  bool isNewEvent = ((*rob_it)->rod_lvl1_id() != m_LastLvl1ID);

  if ( isNewEvent )
  {
#ifdef TRT_BSC_DEBUG
     ATH_MSG_DEBUG( " New event, reset the collection set" );
#endif
    // remember last Lvl1ID
    m_LastLvl1ID = (*rob_it)->rod_lvl1_id();

    if ( m_storeInDetTimeColls )
    {
      // Create Collections for per ROD vectors on L1ID and BCID  
      m_LVL1Collection = new InDetTimeCollection();
      m_LVL1Collection->reserve(vecRobs.size());
  
      m_BCCollection = new InDetTimeCollection();
      m_BCCollection->reserve(vecRobs.size());  
    }
  }

  // loop over the ROB fragments
  for(; rob_it!=vecRobs.end(); ++rob_it)
  {

    uint32_t robid = (*rob_it)->rod_source_id();


      if ( isNewEvent && m_storeInDetTimeColls )
      {
	 /*
	  * Add to vector containing pairs of (ROD ID, [L1ID,BCID])
	  */

	 unsigned int lvl1id = (*rob_it)->rod_lvl1_id();
	 std::pair<uint32_t, unsigned int>* lvl1Pair = 
	    new std::pair<uint32_t, unsigned int>(std::make_pair( robid, lvl1id ));
	 m_LVL1Collection->push_back( lvl1Pair ) ;
	
	 unsigned int bcid = (*rob_it)->rod_bc_id();  
	 std::pair<uint32_t, unsigned int>* bcidPair = 
	    new std::pair<uint32_t, unsigned int>(std::make_pair( robid, bcid ));
	 m_BCCollection->push_back( bcidPair );
      
#ifdef TRT_BSC_DEBUG
	 ATH_MSG_DEBUG( "Stored LVL1ID " << lvl1id << " and BCID " << bcid << " in InDetTimeCollections" );
#endif
      }

      StatusCode sc = m_decoder->fillCollection( &**rob_it, rdoIdc);
      if ( sc == StatusCode::FAILURE )
      {
	if ( DecodeErrCount < 100 )
	{
	   ATH_MSG_INFO( "Problem with TRT ByteStream Decoding!" );
	   DecodeErrCount++;
	}
	else if ( 100 == DecodeErrCount )
	{
	   ATH_MSG_INFO( "Too many Problem with TRT Decoding messages.  Turning message off." );
	   DecodeErrCount++;
	}

	// return sc;  Don't return on single ROD failure
      }
  }

  /*
   * On first event, record per ROD L1ID and BCID collections
   */ 
  if ( isNewEvent )
  {
    StatusCode sc;

    if ( m_storeInDetTimeColls )
    {

      SG::WriteHandle<InDetTimeCollection> lvl1id(m_lvl1idkey);
      auto mylvl1 = std::make_unique<InDetTimeCollection>(*m_LVL1Collection);
      sc = lvl1id.record(std::move(mylvl1));
      if ( sc.isFailure() ) 
      {   
	ATH_MSG_WARNING( "failed to record LVL1ID TimeCollection" );
	return sc;   
      }

      SG::WriteHandle<InDetTimeCollection> bcid(m_bcidkey);
      auto mybc = std::make_unique<InDetTimeCollection>(*m_BCCollection);
      sc = bcid.record(std::move(mybc));
      if ( sc.isFailure() ) 
      {   
	ATH_MSG_WARNING( "failed to record BCID TimeCollection" );
	return sc;   
      }
    }

     sc = m_bsErrSvc->recordData();
     if (sc.isFailure() ) 
     {
	ATH_MSG_ERROR( "failed to record BS errors" );
	return sc;   
     }
  }
 
  return StatusCode::SUCCESS; 
}

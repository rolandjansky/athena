/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/


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
     m_storeInDetTimeColls(true),
     m_doEventCheck(true)
{
  declareProperty ("Decoder", m_decoder);
  declareProperty ("StoreInDetTimeCollections", m_storeInDetTimeColls);
  declareProperty ("checkLVL1ID", m_doEventCheck);

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

StatusCode TRTRawDataProviderTool::convert(const std::vector<const ROBFragment*>& vecRobs,
					   TRT_RDO_Container* rdoIdc, 
					   TRT_BSErrContainer* bserr)
{

  static std::atomic_int DecodeErrCount = 0;

  if(vecRobs.size() == 0) 
     return StatusCode::SUCCESS;


  std::unique_ptr<InDetTimeCollection> LVL1Collection;
  std::unique_ptr<InDetTimeCollection> BCCollection;
  std::vector<const ROBFragment*>::const_iterator rob_it = vecRobs.begin();

  const EventContext& ctx{Gaudi::Hive::currentContext()};
  std::lock_guard<std::mutex> lock{m_mutex};
  CacheEntry* ent{m_cache.get(ctx)};
  if (ent->m_evt!=ctx.evt()) { // New event in this slot
    ent->m_LastLvl1ID = 0xffffffff;
    ent->m_evt = ctx.evt();
  }
  
  // are we working on a new event ?
  bool isNewEvent = (m_doEventCheck ? ((*rob_it)->rod_lvl1_id() != ent->m_LastLvl1ID) : true);

  if ( isNewEvent )
  {
#ifdef TRT_BSC_DEBUG
     ATH_MSG_DEBUG( " New event, reset the collection set" );
#endif
    // remember last Lvl1ID
    ent->m_LastLvl1ID = (*rob_it)->rod_lvl1_id();
    // and clean up the identifable container !
    rdoIdc->cleanup(); //TODO REMOVE AFTER TRIGGER LEGACY CODE RETIRED

    if ( m_storeInDetTimeColls )
    {
      // Create Collections for per ROD vectors on L1ID and BCID  
      LVL1Collection = std::make_unique<InDetTimeCollection>();
      LVL1Collection->reserve(vecRobs.size());
  
      BCCollection = std::make_unique<InDetTimeCollection>();
      BCCollection->reserve(vecRobs.size());
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
	 LVL1Collection->emplace_back( robid, lvl1id ) ;
	
	 unsigned int bcid = (*rob_it)->rod_bc_id();  
	 BCCollection->emplace_back( robid, bcid );
      
#ifdef TRT_BSC_DEBUG
	 ATH_MSG_DEBUG( "Stored LVL1ID " << lvl1id << " and BCID " << bcid << " in InDetTimeCollections" );
#endif
      }

      StatusCode sc = m_decoder->fillCollection( &**rob_it, rdoIdc, bserr);
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
      sc = lvl1id.record(std::move(LVL1Collection));
      if ( sc.isFailure() ) 
      {   
	ATH_MSG_WARNING( "failed to record LVL1ID TimeCollection" );
	return sc;   
      }

      SG::WriteHandle<InDetTimeCollection> bcid(m_bcidkey);
      sc = bcid.record(std::move(BCCollection));
      if ( sc.isFailure() ) 
      {   
	ATH_MSG_WARNING( "failed to record BCID TimeCollection" );
	return sc;   
      }
    }
  }
 
  return StatusCode::SUCCESS; 
}

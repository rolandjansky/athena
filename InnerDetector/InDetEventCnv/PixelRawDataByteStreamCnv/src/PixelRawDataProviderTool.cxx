/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "PixelRawDataProviderTool.h"

#include "PixelRawDataByteStreamCnv/IPixelRodDecoder.h"
#include "PixelConditionsServices/IPixelByteStreamErrorsSvc.h"

using OFFLINE_FRAGMENTS_NAMESPACE::ROBFragment;

//#define PIXEL_DEBUG
//#define PLOTS

// -------------------------------------------------------
// default contructor

PixelRawDataProviderTool::PixelRawDataProviderTool
( const std::string& type, const std::string& name,const IInterface* parent )
  :  AthAlgTool(type,name,parent),
     m_decoder   ("PixelRodDecoder"),
     m_bsErrSvc  ("PixelByteStreamErrorsSvc",name),
     m_robIdSet(),
     m_LVL1Collection(nullptr),
     m_BCIDCollection(nullptr),
     m_DecodeErrCount(0),
     m_LastLvl1ID(0xffffffff)
{
  declareProperty ("Decoder", m_decoder);
  declareProperty ("ErrorsSvc", m_bsErrSvc);
  declareInterface< IPixelRawDataProviderTool >( this );   
}

// -------------------------------------------------------
// destructor 

PixelRawDataProviderTool::~PixelRawDataProviderTool()
{}
 
// -------------------------------------------------------
// initialize

StatusCode PixelRawDataProviderTool::initialize()
{
   StatusCode sc = AthAlgTool::initialize(); 
   if (sc.isFailure()) {
     msg(MSG::FATAL) << "Failed to init baseclass" << endmsg;
     return StatusCode::FAILURE;
   }

   // Retrieve decoder
   if (m_decoder.retrieve().isFailure()) {
     msg(MSG::FATAL) << "Failed to retrieve tool " << m_decoder << endmsg;
     return StatusCode::FAILURE;
   } else 
     msg(MSG::INFO) << "Retrieved tool " << m_decoder << endmsg;
   
   return StatusCode::SUCCESS;
}

// -------------------------------------------------------
// finalize

StatusCode PixelRawDataProviderTool::finalize()
{
   StatusCode sc = AthAlgTool::finalize(); 
   return sc;
}

// -------------------------------------------------------
// convert method

StatusCode PixelRawDataProviderTool::convert( std::vector<const ROBFragment*>& vecRobs,
					    IPixelRDO_Container*               rdoIdc )
{
  if(vecRobs.size() == 0)
    return StatusCode::SUCCESS;

  std::vector<const ROBFragment*>::const_iterator rob_it = vecRobs.begin();
  
#ifdef PIXEL_DEBUG
  msg(MSG::DEBUG)
	  << " New ROD collection found: LVL1=" << (*rob_it)->rod_lvl1_id()
	  << " Size=" << vecRobs.size()
	  << endmsg;
#endif 
#ifdef PLOTS
  std::cout << " New ROD collection found: LVL1=" << (*rob_it)->rod_lvl1_id() << " Size=" << vecRobs.size() << std::endl;
#endif

    //    are we working on a new event ?
  bool isNewEvent = ((*rob_it)->rod_lvl1_id() != m_LastLvl1ID);
  if ( isNewEvent ) {

    m_LVL1Collection = new InDetTimeCollection();
    m_LVL1Collection->reserve(vecRobs.size());
    m_BCIDCollection = new InDetTimeCollection();
    m_BCIDCollection->reserve(vecRobs.size());  

#ifdef PIXEL_DEBUG
    msg(MSG::DEBUG) << " New event, reset the collection set" << endmsg;
#endif
    // remember last Lvl1ID
    m_LastLvl1ID = (*rob_it)->rod_lvl1_id();
    // reset list of known robIds
    m_robIdSet.clear();
    // and clean up the identifable container !
    rdoIdc->cleanup();
  }

  // loop over the ROB fragments
  for(; rob_it!=vecRobs.end(); ++rob_it) {

    uint32_t robid = (*rob_it)->rob_source_id();
#ifdef PIXEL_DEBUG
    msg(MSG::DEBUG) << "Found ROB " 
	  << std::hex << robid << std::dec << endmsg;
#endif

    if (isNewEvent) {

      unsigned int lvl1id = (*rob_it)->rod_lvl1_id();
      std::pair<uint32_t, unsigned int>* lvl1Pair = new std::pair<uint32_t, unsigned int>(robid,lvl1id);
      m_LVL1Collection->push_back(lvl1Pair) ;
    
      unsigned int bcid = (*rob_it)->rod_bc_id();  
      std::pair<uint32_t, unsigned int>* bcidPair = new std::pair<uint32_t, unsigned int>(robid,bcid);
      m_BCIDCollection->push_back(bcidPair);
      
#ifdef PIXEL_DEBUG
      msg(MSG::DEBUG) << "Stored LVL1ID "<<lvl1id<<" and BCID "<<bcid<<" in InDetTimeCollections"<<endmsg;
#endif
    }


    // check if this ROBFragment was already decoded (EF case in ROIs
    if (!m_robIdSet.insert(robid).second) {

#ifdef PIXEL_DEBUG
      msg(MSG::DEBUG) << " ROB Fragment with ID  "
	    << std::hex<<robid<<std::dec<< " already decoded, skip" << endmsg; 
#endif

    } else {
      
      // here the code for the timing monitoring should be reinserted
      // using 1 container per event und subdetector


      StatusCode sc = m_decoder->fillCollection( &**rob_it, rdoIdc);
      if ( sc == StatusCode::FAILURE )
	{
	  if ( m_DecodeErrCount < 100 )
	    {
	      msg(MSG::INFO) << "Problem with Pixel ByteStream Decoding!" 
		    << endmsg;
	      m_DecodeErrCount++;
	    }
	  else if ( 100 == m_DecodeErrCount )
	    {
	      msg(MSG::INFO) 
		    << "Too many Problems with Pixel Decoding messages.  "
		    << "Turning message off." << endmsg;
	      m_DecodeErrCount++;
	    }
	}
    }
  }
  if (isNewEvent) {
    StatusCode sc = evtStore()->record(m_LVL1Collection,"PixelLVL1ID");  
    if (sc.isFailure() ) {   
      ATH_MSG_ERROR ( "failed to record LVL1ID TimeCollection" );   
      return sc;   
    }
    sc = evtStore()->record(m_BCIDCollection,"PixelBCID");  
    if (sc.isFailure() ) {   
      ATH_MSG_ERROR ( "failed to record BCID TimeCollection" );   
     return sc;   
    }
  }
  StatusCode sc = m_bsErrSvc->recordData();
  if (sc.isFailure() ) {
    ATH_MSG_ERROR( "failed to record Pixel BS errors");
  } 
  return StatusCode::SUCCESS; 
}

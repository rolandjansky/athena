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

PixelRawDataProviderTool::PixelRawDataProviderTool(const std::string& type, const std::string& name, const IInterface* parent):
  AthAlgTool(type,name,parent),
  m_decoder("PixelRodDecoder"),
  m_bsErrSvc("PixelByteStreamErrorsSvc",name),
  m_robIdSet(),
  m_LVL1CollectionKey("PixelLVL1ID"),
  m_BCIDCollectionKey("PixelBCID"),
  m_DecodeErrCount(0),
  m_LastLvl1ID(0xffffffff)
{
  declareProperty("Decoder", m_decoder);
  declareProperty("ErrorsSvc", m_bsErrSvc);
  declareProperty("LVL1CollectionName",m_LVL1CollectionKey);
  declareProperty("BCIDCollectionName",m_BCIDCollectionKey);
  declareInterface<IPixelRawDataProviderTool>(this);   
}

PixelRawDataProviderTool::~PixelRawDataProviderTool() {}
 
StatusCode PixelRawDataProviderTool::initialize() {
  CHECK(AthAlgTool::initialize());
  ATH_MSG_DEBUG("PixelRawDataProviderTool::initialize()");
  CHECK(m_decoder.retrieve());
  ATH_MSG_INFO("Retrieved tool " << m_decoder);
  
  ATH_CHECK(m_LVL1CollectionKey.initialize());
  ATH_CHECK(m_BCIDCollectionKey.initialize());

  return StatusCode::SUCCESS;
}

StatusCode PixelRawDataProviderTool::finalize() {
  ATH_MSG_DEBUG("PixelRawDataProviderTool::finalize()");
  return StatusCode::SUCCESS;
}

StatusCode PixelRawDataProviderTool::convert(std::vector<const ROBFragment*>& vecRobs, IPixelRDO_Container* rdoIdc) {
  if (vecRobs.size()==0) { return StatusCode::SUCCESS; }

  std::vector<const ROBFragment*>::const_iterator rob_it = vecRobs.begin();

#ifdef PIXEL_DEBUG
  ATH_MSG_DEBUG(" New ROD collection found: LVL1=" << (*rob_it)->rod_lvl1_id() << " Size=" << vecRobs.size());
#endif 
#ifdef PLOTS
  std::cout << " New ROD collection found: LVL1=" << (*rob_it)->rod_lvl1_id() << " Size=" << vecRobs.size() << std::endl;
#endif

  //    are we working on a new event ?
  bool isNewEvent = ((*rob_it)->rod_lvl1_id() != m_LastLvl1ID);
  if (isNewEvent) {
    m_LVL1Collection = SG::makeHandle(m_LVL1CollectionKey);
    ATH_CHECK(m_LVL1Collection.record(std::make_unique<InDetTimeCollection>()));
    ATH_MSG_DEBUG("InDetTimeCollection " << m_LVL1Collection.name() << " registered in StoreGate");
    m_LVL1Collection->reserve(vecRobs.size());

    m_BCIDCollection = SG::makeHandle(m_BCIDCollectionKey);
    ATH_CHECK(m_BCIDCollection.record(std::make_unique<InDetTimeCollection>()));
    ATH_MSG_DEBUG("InDetTimeCollection " << m_BCIDCollection.name() << " registered in StoreGate");
    m_BCIDCollection->reserve(vecRobs.size());  

#ifdef PIXEL_DEBUG
    ATH_MSG_DEBUG(" New event, reset the collection set");
#endif
    // remember last Lvl1ID
    m_LastLvl1ID = (*rob_it)->rod_lvl1_id();
    // reset list of known robIds
    m_robIdSet.clear();
    // and clean up the identifable container !
    rdoIdc->cleanup();
  }

  // loop over the ROB fragments
  for (; rob_it!=vecRobs.end(); ++rob_it) {
    uint32_t robid = (*rob_it)->rob_source_id();
#ifdef PIXEL_DEBUG
    ATH_MSG_DEBUG("Found ROB " << std::hex << robid << std::dec);
#endif

    if (isNewEvent) {
      unsigned int lvl1id = (*rob_it)->rod_lvl1_id();
      std::pair<uint32_t, unsigned int>* lvl1Pair = new std::pair<uint32_t, unsigned int>(robid,lvl1id);
      m_LVL1Collection->push_back(lvl1Pair) ;

      unsigned int bcid = (*rob_it)->rod_bc_id();  
      std::pair<uint32_t, unsigned int>* bcidPair = new std::pair<uint32_t, unsigned int>(robid,bcid);
      m_BCIDCollection->push_back(bcidPair);
      
#ifdef PIXEL_DEBUG
      ATH_MSG_DEBUG("Stored LVL1ID "<<lvl1id<<" and BCID "<<bcid<<" in InDetTimeCollections");
#endif
    }

    // check if this ROBFragment was already decoded (EF case in ROIs
    if (!m_robIdSet.insert(robid).second) {
#ifdef PIXEL_DEBUG
      ATH_MSG_DEBUG(" ROB Fragment with ID  " << std::hex<<robid<<std::dec<< " already decoded, skip");
#endif
    } 
    else {
      // here the code for the timing monitoring should be reinserted
      // using 1 container per event und subdetector
      StatusCode sc = m_decoder->fillCollection(&**rob_it, rdoIdc);
      if (sc==StatusCode::FAILURE) {
        if (m_DecodeErrCount<100) {
          ATH_MSG_INFO("Problem with Pixel ByteStream Decoding!");
          m_DecodeErrCount++;
        }
        else if (100==m_DecodeErrCount) {
          ATH_MSG_INFO("Too many Problems with Pixel Decoding messages.  Turning message off.");
          m_DecodeErrCount++;
        }
      }
    }
  }
  if (isNewEvent) {
    CHECK(m_bsErrSvc->recordData());
  }
  return StatusCode::SUCCESS; 
}

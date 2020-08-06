/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#include "PixelRawDataProviderTool.h"
#include "StoreGate/WriteHandle.h"

using OFFLINE_FRAGMENTS_NAMESPACE::ROBFragment;

//#define PIXEL_DEBUG
//#define PLOTS

// -------------------------------------------------------
// default contructor

PixelRawDataProviderTool::PixelRawDataProviderTool(const std::string& type, const std::string& name, const IInterface* parent):
  AthAlgTool(type,name,parent),
  m_DecodeErrCount(0)
{
  declareProperty("checkLVL1ID", m_checkLVL1ID = true);
  declareInterface<IPixelRawDataProviderTool>(this);   
}

PixelRawDataProviderTool::~PixelRawDataProviderTool() {}
 
StatusCode PixelRawDataProviderTool::initialize() {
  CHECK(AthAlgTool::initialize());
  ATH_MSG_DEBUG("PixelRawDataProviderTool::initialize()");
  ATH_CHECK(m_decoder.retrieve());
  ATH_MSG_INFO("Retrieved tool " << m_decoder);
  
  ATH_CHECK(m_LVL1CollectionKey.initialize());
  ATH_CHECK(m_BCIDCollectionKey.initialize());

  return StatusCode::SUCCESS;
}

StatusCode PixelRawDataProviderTool::finalize() {
  ATH_MSG_DEBUG("PixelRawDataProviderTool::finalize()");
  return StatusCode::SUCCESS;
}

StatusCode PixelRawDataProviderTool::convert(std::vector<const ROBFragment*>& vecRobs, IPixelRDO_Container* rdoIdc,
					     IDCInDetBSErrContainer& decodingErrors ) const {
  if (vecRobs.size()==0) { return StatusCode::SUCCESS; }

  std::vector<const ROBFragment*>::const_iterator rob_it = vecRobs.begin();

#ifdef PIXEL_DEBUG
  ATH_MSG_DEBUG(" New ROD collection found: LVL1=" << (*rob_it)->rod_lvl1_id() << " Size=" << vecRobs.size());
#endif 
#ifdef PLOTS
  std::cout << " New ROD collection found: LVL1=" << (*rob_it)->rod_lvl1_id() << " Size=" << vecRobs.size() << std::endl;
#endif

  SG::WriteHandle<InDetTimeCollection> LVL1Collection;
  SG::WriteHandle<InDetTimeCollection> BCIDCollection;

  const EventContext& ctx{Gaudi::Hive::currentContext()};
  std::lock_guard<std::mutex> lock{m_mutex};
  CacheEntry* ent{m_cache.get(ctx)};
  if (ent->m_evt!=ctx.evt()) { // New event in this slot
    ent->m_LastLvl1ID = 0xffffffff;
    ent->m_evt = ctx.evt();
  }
  //    are we working on a new event ?
  bool isNewEvent = (m_checkLVL1ID ? ((*rob_it)->rod_lvl1_id() != ent->m_LastLvl1ID) : true);
  if (isNewEvent) {
    LVL1Collection = SG::makeHandle(m_LVL1CollectionKey);
    ATH_CHECK(LVL1Collection.record(std::make_unique<InDetTimeCollection>()));
    ATH_MSG_DEBUG("InDetTimeCollection " << LVL1Collection.name() << " registered in StoreGate");
    LVL1Collection->reserve(vecRobs.size());

    BCIDCollection = SG::makeHandle(m_BCIDCollectionKey);
    ATH_CHECK(BCIDCollection.record(std::make_unique<InDetTimeCollection>()));
    ATH_MSG_DEBUG("InDetTimeCollection " << BCIDCollection.name() << " registered in StoreGate");
    BCIDCollection->reserve(vecRobs.size());  

#ifdef PIXEL_DEBUG
    ATH_MSG_DEBUG(" New event, reset the collection set");
#endif
    // remember last Lvl1ID
    ent->m_LastLvl1ID = (*rob_it)->rod_lvl1_id();

    // and clean up the identifable container !
    rdoIdc->cleanup();//TODO Remove this when legacy trigger code is removed

  }

  // loop over the ROB fragments
  for (; rob_it!=vecRobs.end(); ++rob_it) {
    uint32_t robid = (*rob_it)->rob_source_id();
#ifdef PIXEL_DEBUG
    ATH_MSG_DEBUG("Found ROB " << std::hex << robid << std::dec);
#endif

    if (isNewEvent) {
      unsigned int lvl1id = (*rob_it)->rod_lvl1_id();
      LVL1Collection->emplace_back(robid,lvl1id) ;

      unsigned int bcid = (*rob_it)->rod_bc_id();  
      BCIDCollection->emplace_back(robid,bcid);
      
#ifdef PIXEL_DEBUG
      ATH_MSG_DEBUG("Stored LVL1ID "<<lvl1id<<" and BCID "<<bcid<<" in InDetTimeCollections");
#endif
    }

    // here the code for the timing monitoring should be reinserted
    // using 1 container per event and subdetector
    StatusCode sc = m_decoder->fillCollection(&**rob_it, rdoIdc, decodingErrors);

    const int issuesMessageCountLimit = 100;
    if (sc==StatusCode::FAILURE) {
      if (m_DecodeErrCount < issuesMessageCountLimit) {
        ATH_MSG_INFO("Problem with Pixel ByteStream Decoding!");
        m_DecodeErrCount++;
      }
      else if (issuesMessageCountLimit == m_DecodeErrCount) {
        ATH_MSG_INFO("Too many Problems with Pixel Decoding messages.  Turning message off.");
        m_DecodeErrCount++;
      }
    }
  }
  return StatusCode::SUCCESS; 
}

int PixelRawDataProviderTool::SizeOfIDCInDetBSErrContainer() const {
  //=========================================================
  //  Size of Pixel BS Error container
  //
  //  The error would be stored not only for module but also each FE (x16) per module.
  //  In addition, IBL FEI4 provides error counter between trigger, this also extends 
  //  the size beyond nominal module + FE ID. These numbers come from the hardware 
  //  specification so that there is no easy way to document in the header file.
  //  Rather, put the hardcoded number here.
  //
  //      Total number of pixel modules: 2048
  //      Number of FE chips per module:   16
  //     -------------------------------------
  //          2048 x 17 (module + FE) = 34816
  //
  //      IBL extra error information  : 
  //          280(module) x 2(FE) x 32(error counter) = 35840
  //     -------------------------------------
  //                             Total : 70656
  //=========================================================
  return 70656;
}

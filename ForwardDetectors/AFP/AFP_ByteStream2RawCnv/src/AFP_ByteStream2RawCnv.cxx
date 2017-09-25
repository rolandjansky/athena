/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// AFP_ByteStream2RawCnv includes
#include "AFP_ByteStream2RawCnv/AFP_ByteStream2RawCnv.h"
//#include "AFP_ByteStream2RawCnv/ReadOut.h"
#include <algorithm>

static const InterfaceID IID_IAFP_ByteStream2RawCnv("AFP_ByteStream2RawCnv", 1,
                                                    0);
const InterfaceID &AFP_ByteStream2RawCnv::interfaceID() {
  return IID_IAFP_ByteStream2RawCnv;
}

//////////////////////////
// constructor
//////////////////////////

AFP_ByteStream2RawCnv::AFP_ByteStream2RawCnv(const std::string &type,
                                             const std::string &name,
                                             const IInterface *parent)
    : AthAlgTool(type, name, parent),
      m_robDataProvider("ROBDataProviderSvc", name), m_AFP_RawDataReadOut(0),
      m_AFP_RawDataCollectionReadOut(0), m_AFP_RawDataContainerReadOut(0)

{
  declareInterface<AFP_ByteStream2RawCnv>(this);
}

//////////////////////////
//// destructor
//////////////////////////

AFP_ByteStream2RawCnv::~AFP_ByteStream2RawCnv() {}

//////////////////////////
//// initialize()
//////////////////////////

StatusCode AFP_ByteStream2RawCnv::initialize() {
  ATH_MSG_INFO("Initializing " << name() << "...");

  StatusCode sc = AthAlgTool::initialize();
  ATH_MSG_DEBUG("AFP_ByteStream2RawCnv::initialize");
  if (sc.isFailure()) {
    ATH_MSG_WARNING("Failed to initialize");
    return StatusCode::SUCCESS;
  }

  if (m_robDataProvider.retrieve().isFailure()) {
    ATH_MSG_WARNING("Failed to retrieve service " << m_robDataProvider
                                                  << "...");
    return StatusCode::SUCCESS;
  } else {
    ATH_MSG_DEBUG("Retrieved service " << m_robDataProvider << "...");
  }

  if (StatusCode::SUCCESS !=
      serviceLocator()->service("StoreGateSvc", m_EvtStore)) {
    ATH_MSG_WARNING("Can't get StoreGateSvc");
    return StatusCode::SUCCESS;
  }

  m_fragment_number = 0;

  m_AFP_RawDataReadOut = new AFP_RawDataReadOut();
  m_AFP_RawDataCollectionReadOut = new AFP_RawDataCollectionReadOut();
  m_AFP_RawDataContainerReadOut = new AFP_RawDataContainerReadOut();

  m_count_hits = 0;
  return StatusCode::SUCCESS;
}

//////////////////////////
////// finalize()
//////////////////////////

StatusCode AFP_ByteStream2RawCnv::finalize() {
  ATH_MSG_DEBUG("AFP_ByteStream2RawCnv: finalizing ");
  ATH_MSG_DEBUG(" Bytestream summary:" << m_fragment_number
                                       << " fragments found");
  if (m_AFP_RawDataReadOut) {
    delete m_AFP_RawDataReadOut;
  }
  if (m_AFP_RawDataCollectionReadOut) {
    delete m_AFP_RawDataCollectionReadOut;
  }
  if (m_AFP_RawDataContainerReadOut) {
    delete m_AFP_RawDataContainerReadOut;
  }

  return StatusCode::SUCCESS;
}

//////////////////////////
////// fillCollection()
//////////////////////////

StatusCode AFP_ByteStream2RawCnv::fillCollection(const ROBFragment *robFrag,
                                                 AFP_RawDataContainer *rdoCont,
                                                 std::vector<unsigned int> *) {
  ATH_MSG_DEBUG("AFP_ByteStream2RawCnv::fillColelction rob_source_id: in decimal="<<std::dec<<robFrag->rob_source_id()<<",  in hex=0x"<<std::hex<<robFrag->rob_source_id()<<std::dec);

  try {
    robFrag->check();
  } catch (...) {
    ATH_MSG_WARNING("An exception occurred");
    return StatusCode::SUCCESS;
  }

  uint32_t nstat = robFrag->nstatus();
  if (nstat) {
    const uint32_t *it;
    robFrag->status(it);
    if (*it) {
      ATH_MSG_WARNING(" Error in ROB status word: 0x");
      return StatusCode::SUCCESS;
    }
  }

  if (!rdoCont) {
    ATH_MSG_WARNING("NULL pointer passed in rdoCont argument.");
    return StatusCode::SUCCESS;
  }

  // --- end of check input parameters ---

  OFFLINE_FRAGMENTS_NAMESPACE::PointerType vint;
  robFrag->rod_data(vint);

  uint32_t size = robFrag->rod_ndata();

  OFFLINE_FRAGMENTS_NAMESPACE::PointerType vint_status;
  robFrag->rod_status(vint_status);
  uint32_t size_status = robFrag->rod_nstatus();

  if (size_status <= 0) {
    ATH_MSG_WARNING("Buffer size <= 0!");
    return StatusCode::SUCCESS;
  }

  const uint32_t wordPos = 0;
  m_AFP_RawDataCollectionReadOut->decodeWord(vint[wordPos]);
  m_AFP_RawDataReadOut->decodeWord(vint[wordPos]);

  const eformat::FullEventFragment<const uint32_t *> *event =
      m_robDataProvider->getEvent();

  if (!event) {
    ATH_MSG_WARNING("NULL event retrived from m_robDataProvider");
    return StatusCode::SUCCESS;
  }

  const uint32_t Time_StampID = event->bc_time_seconds();
  const uint32_t Time_StampnsID = event->bc_time_nanoseconds();
  const uint32_t BC_ID = event->bc_id();
  const uint32_t LumiBlock_ID = event->lumi_block();
  const uint32_t Lvl1_ID = event->lvl1_id();

  rdoCont->SetTimeStamp(Time_StampID);
  rdoCont->SetTimeStampns(Time_StampnsID);
  rdoCont->SetBCId(BC_ID);
  rdoCont->SetLumiBlock(LumiBlock_ID);
  rdoCont->SetLvl1Id(Lvl1_ID);
  AFP_RawDataCollection *collection = nullptr;
  unsigned int collection_number = 0;
  const uint32_t noHitMarker = 15;


int first_BCID = 0;
int current_BCID = 0;
int hitLvl1 = 0;

  
  for (unsigned i = 0; i < size; i++) {

    m_AFP_RawDataCollectionReadOut->decodeWord(vint[i]);
    if (m_AFP_RawDataCollectionReadOut->is_BOB()) {
      collection = getCollection(collection_number, robFrag->rob_source_id(), rdoCont);

      if (!collection) {
        ATH_MSG_WARNING(
            "NULL pointer returned by getCollection(collection_number = "
            << collection_number << ", rdoCont)");

        return StatusCode::SUCCESS;
      }

      collection->Set_lvl1Id(m_AFP_RawDataCollectionReadOut->lvl1Id());
      collection->Set_link_header(m_AFP_RawDataCollectionReadOut->link());
      collection->Set_flag(m_AFP_RawDataCollectionReadOut->flag());
      collection->Set_bcid(m_AFP_RawDataCollectionReadOut->bcid());
      collection->Set_header(collection_number);
      collection->Set_robID(robFrag->rob_source_id());
      collection_number++;
      if(collection_number == 1) {first_BCID = m_AFP_RawDataCollectionReadOut->bcid();}
      current_BCID = m_AFP_RawDataCollectionReadOut->bcid();
      hitLvl1 = 0;

      if(current_BCID >= first_BCID)
      {hitLvl1 = (current_BCID-first_BCID);}
      else if (first_BCID > current_BCID)
      {hitLvl1 = 1024 + (current_BCID - first_BCID);}

    }

    else if (m_AFP_RawDataCollectionReadOut->is_LWC() && collection) {
      if (m_AFP_RawDataCollectionReadOut->ToT1() != noHitMarker) {
        AFP_RawData *rawData = new AFP_RawData(i);

        rawData->addData(vint[i]);
        rawData->Set_DiscConf(m_AFP_RawDataCollectionReadOut->DiscConf());
        rawData->Set_link(m_AFP_RawDataCollectionReadOut->link());
        rawData->Set_column(m_AFP_RawDataCollectionReadOut->row());
        rawData->Set_row(m_AFP_RawDataCollectionReadOut->column());
	rawData->Set_lvl1(hitLvl1);        
 
        if (m_AFP_RawDataCollectionReadOut->ToT1() <= 13) {
        rawData->Set_ToT(m_AFP_RawDataCollectionReadOut->ToT1() + m_AFP_RawDataCollectionReadOut->DiscConf() + 1);
      }
	else if (m_AFP_RawDataCollectionReadOut->ToT1() == 14) {
	rawData->Set_ToT(m_AFP_RawDataCollectionReadOut->DiscConf());
      }
        collection->push_back(rawData);
      }

      if (m_AFP_RawDataCollectionReadOut->ToT2() != noHitMarker) {
        AFP_RawData *rawData = new AFP_RawData(i);
        rawData->addData(vint[i]);
        rawData->Set_DiscConf(m_AFP_RawDataCollectionReadOut->DiscConf());
        rawData->Set_link(m_AFP_RawDataCollectionReadOut->link());
        rawData->Set_column(m_AFP_RawDataCollectionReadOut->row());
        rawData->Set_row(m_AFP_RawDataCollectionReadOut->column() + 1);
	rawData->Set_lvl1(hitLvl1);
    
        if (m_AFP_RawDataCollectionReadOut->ToT2() <= 13) {
        rawData->Set_ToT(m_AFP_RawDataCollectionReadOut->ToT2() + m_AFP_RawDataCollectionReadOut->DiscConf() + 1);
      }
        else if (m_AFP_RawDataCollectionReadOut->ToT2() == 14) {
        rawData->Set_ToT(m_AFP_RawDataCollectionReadOut->DiscConf());
      } 

        collection->push_back(rawData);
      }
    }
  } // end of loop
  return StatusCode::SUCCESS;
}

//////////////////////////
///////// getCollection()
//////////////////////////

AFP_RawDataCollection *
AFP_ByteStream2RawCnv::getCollection(const unsigned int columnNum, const unsigned int robID,
                                     AFP_RawDataContainer *cont) {

  if (!cont) {
    ATH_MSG_WARNING(
        "NULL pointer passed in argument: cont. NULL pointer returned");
    return nullptr;
  }

  for (const AFP_RawDataCollection* collection : *cont) {
    if (collection->Get_header_number_POT() == columnNum && collection->Get_robID() == robID) {
      ATH_MSG_WARNING("Collection already in container, although it should not be there.");
      return nullptr;
    }
  }

  AFP_RawDataCollection *coll = new AFP_RawDataCollection(columnNum);
  cont->push_back(coll);
  return coll;
}

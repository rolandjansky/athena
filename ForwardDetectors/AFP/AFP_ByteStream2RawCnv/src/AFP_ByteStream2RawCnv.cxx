/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// AFP_ByteStream2RawCnv includes
#include "AFP_ByteStream2RawCnv/AFP_ByteStream2RawCnv.h"

#include "AFP_RawEv/AFP_RawDataCommonHead.h"

#include "AFP_RawEv/AFP_SiRawData.h"
#include "AFP_RawEv/AFP_SiRawCollection.h"

#include "AFP_RawEv/AFP_ToFRawData.h"
#include "AFP_RawEv/AFP_ToFRawCollection.h"

#include <algorithm>

const InterfaceID &AFP_ByteStream2RawCnv::interfaceID() {
  static const InterfaceID IID_IAFP_ByteStream2RawCnv("AFP_ByteStream2RawCnv", 1, 0);
  return IID_IAFP_ByteStream2RawCnv;
}

AFP_ByteStream2RawCnv::AFP_ByteStream2RawCnv(const std::string &type,
                                             const std::string &name,
                                             const IInterface *parent)
  : AthAlgTool(type, name, parent),
    m_robDataProvider("ROBDataProviderSvc", name)
{
  declareInterface<AFP_ByteStream2RawCnv>(this);
}

AFP_ByteStream2RawCnv::~AFP_ByteStream2RawCnv() {}

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

  return StatusCode::SUCCESS;
}

StatusCode AFP_ByteStream2RawCnv::finalize() {
  ATH_MSG_DEBUG("AFP_ByteStream2RawCnv: finalizing ");

  return StatusCode::SUCCESS;
}

StatusCode AFP_ByteStream2RawCnv::fillCollection(const OFFLINE_FRAGMENTS_NAMESPACE::ROBFragment *robFrag, AFP_RawContainer *rawContainer) {
  ATH_MSG_DEBUG("AFP_ByteStream2RawCnv::fillColelction rob_source_id: in decimal="<<std::dec<<robFrag->rob_source_id()<<",  in hex=0x"<<std::hex<<robFrag->rob_source_id()<<std::dec);

  try {
    robFrag->check();
  } catch (...) {
    ATH_MSG_WARNING("An exception occurred");
    return StatusCode::SUCCESS;
  }

  const uint32_t nStat = robFrag->nstatus();
  if (nStat) {
    const uint32_t *it;
    robFrag->status(it);
    if (*it) {
      ATH_MSG_WARNING(" Error in ROB status word: 0x");
      return StatusCode::SUCCESS;
    }
  }

  if (!rawContainer) {
    ATH_MSG_WARNING("NULL pointer passed in rawContainer argument.");
    return StatusCode::SUCCESS;
  }

  // --- end of check input parameters ---

  OFFLINE_FRAGMENTS_NAMESPACE::PointerType vint;
  robFrag->rod_data(vint);

  if (robFrag->rod_nstatus() <= 0) {
    ATH_MSG_WARNING("Buffer size <= 0!");
    return StatusCode::SUCCESS;
  }

  const eformat::FullEventFragment<const uint32_t*> *event = m_robDataProvider->getEvent();

  if (!event) {
    ATH_MSG_WARNING("NULL event retrived from m_robDataProvider");
    return StatusCode::SUCCESS;
  }

  // set information about event in the RawContainer
  rawContainer->setTimeStamp(event->bc_time_seconds());
  rawContainer->setTimeStampNS(event->bc_time_nanoseconds());
  rawContainer->setBCId(event->bc_id());
  rawContainer->setLumiBlock(event->lumi_block());
  rawContainer->setLvl1Id(event->lvl1_id());

  // fill container with collections
  AFP_SiRawCollection *collectionSi = nullptr;
  AFP_ToFRawCollection *collectionToF = nullptr;

  const uint32_t size = robFrag->rod_ndata();
  for (unsigned i = 0; i < size; i++) {
    m_wordReadout.setWord(vint[i]);

    if (m_wordReadout.isHeader()) {
      AFP_RawCollectionHead* collectionHead = nullptr;
      if ( isLinkToF (m_wordReadout.link()) ) {
	// prepare collection for time-of-flight
	collectionToF = getCollectionToF(m_wordReadout.link(), robFrag->rob_source_id(), rawContainer);
	collectionHead = collectionToF;
      }
      else if ( isLinkSi (m_wordReadout.link()) ) {
	// prepare collection for silicon detector
	collectionSi = getCollectionSi(m_wordReadout.link(), robFrag->rob_source_id(), rawContainer);
	collectionHead = collectionSi;
      }
      else {
        ATH_MSG_WARNING("Unidentified value of link="<<m_wordReadout.link()<<" for header record.");
        return StatusCode::SUCCESS;
      }

      if (!collectionHead) {
        ATH_MSG_WARNING("nullptr returned by getCollection(link = "
			<< m_wordReadout.link() << ", robID = " << robFrag->rob_source_id() <<")");
        return StatusCode::SUCCESS;
      }

      // set head collection informaiton
      collectionHead->setLvl1Id(m_wordReadout.getBits(14, 10));
      collectionHead->setLink(m_wordReadout.link());
      collectionHead->setFrontendFlag(m_wordReadout.getBits(15, 15));
      collectionHead->setBcId(m_wordReadout.getBits(9, 0));
      collectionHead->setRobId(robFrag->rob_source_id());
    }
    else if (m_wordReadout.isData()) {
      // fill time-of-flight collection
      if ( isLinkToF (m_wordReadout.link()) ) {

	// check if collection is available
	if ( !collectionToF ) {
	  ATH_MSG_WARNING("No ToF collection available to fill data.");
	  return StatusCode::SUCCESS;
	}

	AFP_ToFRawData& ToFData = collectionToF->newDataRecord();
	ToFData.setHeader( m_wordReadout.getBits (23, 21) );
	ToFData.setEdge( m_wordReadout.getBits (20, 20) );
	ToFData.setChannel( m_wordReadout.getBits (19, 16) );
	ToFData.setPulseLength( m_wordReadout.getBits (15, 10) );
	ToFData.setTime( m_wordReadout.getBits (9, 0) );

	setDataHeader (&ToFData);
      }
      else if ( isLinkSi (m_wordReadout.link()) ) {
	// fill silicon detector collection

	// check if collection is available
	if ( !collectionSi ) {
	  ATH_MSG_WARNING("No silicon detector collection available to fill data.");
	  return StatusCode::SUCCESS;
	}

	// check first silicon hit information
	if (m_wordReadout.getBits(7, 4) != s_siNoHitMarker) {
	  AFP_SiRawData& siData = collectionSi->newDataRecord();
	  siData.setColumn (m_wordReadout.getBits(23, 17));
	  siData.setRow (m_wordReadout.getBits(16, 8));
	  siData.setTimeOverThreshold (m_wordReadout.getBits(7, 4));
	  
	  setDataHeader (&siData);
	}

	// check second silicon hit information
	if (m_wordReadout.getBits(3, 0) != s_siNoHitMarker) {
	  AFP_SiRawData& siData = collectionSi->newDataRecord();
	  siData.setColumn (m_wordReadout.getBits(23, 17));
	  siData.setRow (m_wordReadout.getBits(16, 8) + 1);
	  siData.setTimeOverThreshold (m_wordReadout.getBits(3, 0));

	  setDataHeader (&siData);
	}
      }
      else {
	ATH_MSG_WARNING("Not recognised value of link="<<m_wordReadout.link()<<" for data record.");
	return StatusCode::SUCCESS;
      }

    } // end is data
    
  } // end of loop
  return StatusCode::SUCCESS;
}

AFP_SiRawCollection *
AFP_ByteStream2RawCnv::getCollectionSi(const unsigned int link, const unsigned int robId,
				       AFP_RawContainer *container) {

  if (!container) {
    ATH_MSG_WARNING("NULL pointer passed in argument: container. NULL pointer returned.");
    return nullptr;
  }

  for (const AFP_SiRawCollection& collection : container->collectionsSi()) {
    if (collection.link() == link && collection.robId() == robId) {
      ATH_MSG_WARNING("Silicon collection link="<<link<<" robId="<<robId<<" already in container, although it should not be there.");
      return nullptr;
    }
  }

  AFP_SiRawCollection& newCollection = container->newCollectionSi();
  return &newCollection;
}

AFP_ToFRawCollection *
AFP_ByteStream2RawCnv::getCollectionToF(const unsigned int link, const unsigned int robId,
					AFP_RawContainer *container) {

  if (!container) {
    ATH_MSG_WARNING("NULL pointer passed in argument: container. NULL pointer returned.");
    return nullptr;
  }

  for (const AFP_ToFRawCollection& collection : container->collectionsToF()) {
    if (collection.link() == link && collection.robId() == robId) {
      ATH_MSG_WARNING("Silicon collection link="<<link<<" robId="<<robId<<" already in container, although it should not be there.");
      return nullptr;
    }
  }

  AFP_ToFRawCollection& newCollection = container->newCollectionToF();
  return &newCollection;
}

void AFP_ByteStream2RawCnv::setDataHeader (AFP_RawDataCommonHead* dataHead) const
{
  dataHead->setLink (m_wordReadout.link());
  dataHead->setHitDiscConfig (m_wordReadout.getBits(29, 28));
}

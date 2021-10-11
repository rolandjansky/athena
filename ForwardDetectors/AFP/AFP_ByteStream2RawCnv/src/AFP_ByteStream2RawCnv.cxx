/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
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

   if (m_wordReadout.retrieve().isFailure()) {
    ATH_MSG_WARNING("Failed to retrieve service " << m_wordReadout
                    << "...");
    return StatusCode::SUCCESS;
  } else {
    ATH_MSG_DEBUG("Retrieved service " << m_wordReadout << "...");
  }

  return StatusCode::SUCCESS;
}

StatusCode AFP_ByteStream2RawCnv::finalize() {
  ATH_MSG_DEBUG("AFP_ByteStream2RawCnv: finalizing ");

  return StatusCode::SUCCESS;
}

StatusCode AFP_ByteStream2RawCnv::fillCollection(const OFFLINE_FRAGMENTS_NAMESPACE::ROBFragment *robFrag, AFP_RawContainer *rawContainer) const {
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

  // this information will be present only in offline reconstruction
  // not be at HLT; however the remaining AFP information will be
  // present both online and offline
  if (event) {
    // set information about event in the RawContainer
    rawContainer->setTimeStamp(event->bc_time_seconds());
    rawContainer->setTimeStampNS(event->bc_time_nanoseconds());
    rawContainer->setBCId(event->bc_id());
    rawContainer->setLumiBlock(event->lumi_block());
    rawContainer->setLvl1Id(event->lvl1_id());
  }


  // fill container with collections
  AFP_SiRawCollection *collectionSi = nullptr;
  AFP_ToFRawCollection *collectionToF = nullptr;

  std::vector<std::vector<uint16_t>> picoTDC1_channels, picoTDC2_channels;
  std::vector<bool> picoTDC_hasTrigger;
  std::vector<int> ToF_links;

  const uint32_t size = robFrag->rod_ndata();
  for (unsigned i = 0; i < size; i++) {
    uint32_t the_word=vint[i];
    uint32_t the_link=m_wordReadout->link(the_word);

    if (m_wordReadout->isHeader(the_word)) {
      AFP_RawCollectionHead* collectionHead = nullptr;
      if ( isLinkToF (the_link) ) {
        // prepare collection for time-of-flight
        collectionToF = getCollectionToF(//m_wordReadout->link(), robFrag->rob_source_id(), 
                                           rawContainer);
        collectionHead = collectionToF;
        
        std::vector<uint16_t> helper;
        picoTDC1_channels.push_back(helper);
        picoTDC2_channels.push_back(helper);
        picoTDC_hasTrigger.push_back(false);
        ToF_links.push_back(the_link);
      }
      else if ( isLinkSi (the_link) ) {
        // prepare collection for silicon detector
        collectionSi = getCollectionSi(//m_wordReadout->link(), robFrag->rob_source_id(), 
                                         rawContainer);
        collectionHead = collectionSi;
      }
      else {
        ATH_MSG_ERROR("Unidentified value of link="<<the_link<<" for header record.");
        return StatusCode::FAILURE;
      }

      if (!collectionHead) {
        ATH_MSG_WARNING("nullptr returned by getCollection(link = "
                        << the_link << ", robID = " << robFrag->rob_source_id() <<")");
        return StatusCode::SUCCESS;
      }

      // set head collection informaiton
      collectionHead->setLvl1Id(m_wordReadout->getBits(the_word, 14, 10));
      collectionHead->setLink(the_link);
      collectionHead->setFrontendFlag(m_wordReadout->getBits(the_word, 15, 15));
      collectionHead->setBcId(m_wordReadout->getBits(the_word, 9, 0));
      collectionHead->setRobId(robFrag->rob_source_id());
    }
    else if (m_wordReadout->isData(the_word)) {
      // fill time-of-flight collection
      if ( isLinkToF (the_link) ) {

        // check if collection is available
        if ( !collectionToF ) {
          ATH_MSG_WARNING("No ToF collection available to fill data.");
          return StatusCode::SUCCESS;
        }

        uint32_t bit23=m_wordReadout->getBits(the_word, 23, 23);
        if(!bit23)
        {
          // HPTDC 2017
          AFP_ToFRawData& ToFData = collectionToF->newDataRecord();
          ToFData.setHeader( m_wordReadout->getBits(the_word, 23, 21) );
          ToFData.setEdge( m_wordReadout->getBits(the_word, 20, 20) );
          ToFData.setChannel( m_wordReadout->getBits(the_word, 19, 16) );
          ToFData.setPulseLength( m_wordReadout->getBits(the_word, 15, 10) );
          ToFData.setTime( m_wordReadout->getBits(the_word, 9, 0) );

          setDataHeader (the_word, &ToFData);
        }
        else
        {
          // picoTDC
          
          uint32_t bits19_22=m_wordReadout->getBits(the_word, 22, 19);
            
          if(bits19_22==0 || bits19_22==1)
          {
            // picoTDC #1 (==0) or picoTDC #2 (==1)
            uint16_t channel=m_wordReadout->getBits(the_word, 18, 13);
              
            // find entry with the same channel number
            auto ToFData_itr=std::find_if( collectionToF->begin(), collectionToF->end(),
                                           [&](const AFP_ToFRawData entry){return entry.channel()==channel;});

            if(ToFData_itr==collectionToF->end())
            {
              // create a new entry if such channel number doesn't exist
              auto& ToFData = collectionToF->newDataRecord();
                  
              ToFData.setHeader( bit23 );
              ToFData.setEdge( 0 );
              ToFData.setChannel( m_wordReadout->getBits(the_word, 18, 13) );
              ToFData.setPulseLength( 0 );
              ToFData.setTime( 0 );
              setDataHeader (the_word, &ToFData);
              
              ToFData_itr = std::prev(collectionToF->end());
            }
              
            if(!bits19_22)
            {
              // picoTDC #1
              if(ToFData_itr->time() !=0 )
              {
                ATH_MSG_WARNING("trying to set time to "<<m_wordReadout->getBits(the_word, 12, 0)<<", but it is already set to = "<<ToFData_itr->time()<<", will not overwrite");
              }
              else
              {
                ToFData_itr->setTime( m_wordReadout->getBits(the_word, 12, 0) );
                picoTDC1_channels.back().push_back(channel);
              }
            }
            else
            {
              // picoTDC #2
              if(ToFData_itr->pulseLength() !=0 )
              {
                ATH_MSG_WARNING("trying to set pulseLength to "<<m_wordReadout->getBits(the_word, 12, 0)<<", but it is already set to = "<<ToFData_itr->pulseLength()<<", will not overwrite");
              }
              else
              {
                ToFData_itr->setPulseLength( m_wordReadout->getBits(the_word, 12, 0) );
                picoTDC2_channels.back().push_back(channel);
              }
            }
          }
          else if(bits19_22==4)
          {              
            // check if there's already some other trigger word
            auto ToFData_itr=std::find_if( collectionToF->begin(), collectionToF->end(),
                                           [&](const AFP_ToFRawData e){return e.isTrigger();});
            if(ToFData_itr!=collectionToF->end())
            {
              // there shouldn't be any other trigger word
              ATH_MSG_WARNING("already found a trigger word with delayedTrigger = "<<ToFData_itr->delayedTrigger()<<" and triggerPattern = "<<ToFData_itr->triggerPattern()<<", will ignore new word with delayedTrigger = "<<m_wordReadout->getBits(the_word, 18,16)<<" and triggerPattern = "<<m_wordReadout->getBits(the_word, 15, 0));
            }
            else
            {
              // if there isn't any other trigger word, create a new entry
              auto& ToFData = collectionToF->newDataRecord();
            
              ToFData.setHeader( bit23 );
              ToFData.setEdge( 0 );
              ToFData.setTrigger(); // mark this entry as a trigger entry; must be done before setting delayedTrigger or triggerPattern
              ToFData.setDelayedTrigger( m_wordReadout->getBits(the_word, 18,16) );
              ToFData.setTriggerPattern( m_wordReadout->getBits(the_word, 15, 0) );
              picoTDC_hasTrigger.back()=true;
            }
          }
          else
          {
            ATH_MSG_WARNING("unknown pattern in bits 19-22 = "<<bits19_22<<", ignoring word = "<<the_word);
          }
        }
      }
      else if ( isLinkSi (the_link) ) {
        // fill silicon detector collection

        // check if collection is available
        if ( !collectionSi ) {
          ATH_MSG_WARNING("No silicon detector collection available to fill data.");
          return StatusCode::SUCCESS;
        }

        // check first silicon hit information
        if (m_wordReadout->getBits(the_word, 7, 4) != s_siNoHitMarker) {
          AFP_SiRawData& siData = collectionSi->newDataRecord();
          siData.setColumn (m_wordReadout->getBits(the_word, 23, 17));
          siData.setRow (m_wordReadout->getBits(the_word, 16, 8));
          siData.setTimeOverThreshold (m_wordReadout->getBits(the_word, 7, 4));
          
          setDataHeader (the_word, &siData);
        }

        // check second silicon hit information
        if (m_wordReadout->getBits(the_word, 3, 0) != s_siNoHitMarker) {
          AFP_SiRawData& siData = collectionSi->newDataRecord();
          siData.setColumn (m_wordReadout->getBits(the_word, 23, 17));
          siData.setRow (m_wordReadout->getBits(the_word, 16, 8) + 1);
          siData.setTimeOverThreshold (m_wordReadout->getBits(the_word, 3, 0));

          setDataHeader (the_word, &siData);
        }
      }
      else {
        ATH_MSG_ERROR("Not recognised value of link="<<the_link<<" for data record.");
        return StatusCode::FAILURE;
      }

    } // end is data
  } // end of loop
  
  // in case of picoTDC, check we always have both words
  for(unsigned int i=0;i<picoTDC1_channels.size();++i)
  {
    if(picoTDC1_channels.at(i).size()>0 || picoTDC2_channels.at(i).size())
    {
      for(auto ch1 : picoTDC1_channels.at(i))
      {
        if(std::find(picoTDC2_channels.at(i).begin(),picoTDC2_channels.at(i).end(),ch1) == picoTDC2_channels.at(i).end())
        {
          ATH_MSG_WARNING("Cannot find channel "<<ch1<<" from picoTDC #1 in picoTDC #2 in ToF collections with link nr. "<<ToF_links.at(i)<<", pulseLength is very probably not set");
        }
      }
      for(auto ch2 : picoTDC2_channels.at(i))
      {
        if(std::find(picoTDC1_channels.at(i).begin(),picoTDC1_channels.at(i).end(),ch2) == picoTDC1_channels.at(i).end())
        {
          ATH_MSG_WARNING("Cannot find channel "<<ch2<<" from picoTDC #2 in picoTDC #1 in ToF collections with link nr. "<<ToF_links.at(i)<<", time is very probably not set");
        }
      }
      
      if(!picoTDC_hasTrigger.at(i))
	  {
	    ATH_MSG_WARNING("Cannot find trigger word in ToF collections with link nr. "<<ToF_links.at(i));
	  }
    }
  }
  
  return StatusCode::SUCCESS;
}

AFP_SiRawCollection *
AFP_ByteStream2RawCnv::getCollectionSi(//const unsigned int link, const unsigned int robId,
                                       AFP_RawContainer *container) const {

  if (!container) {
    ATH_MSG_WARNING("NULL pointer passed in argument: container. NULL pointer returned.");
    return nullptr;
  }

  // Code below is commented, because it was impossible to
  // reconstruct several bunch crossings in one event with it. In
  // standard data taking only one bunch crossing is saved and this
  // protection should not be necessary. However in case of big
  // readout problems the code may be needed, that is why it stays
  // commented here.

  // for (const AFP_SiRawCollection& collection : container->collectionsSi())
  //   if (collection.link() == link && collection.robId() == robId)
  //     ATH_MSG_WARNING("Silicon collection link="<<link<<" robId="<<robId<<" already in container, although it should not be there. Anyway creating new collection. (Reading several BCX?)");

  AFP_SiRawCollection& newCollection = container->newCollectionSi();
  return &newCollection;
}

AFP_ToFRawCollection *
AFP_ByteStream2RawCnv::getCollectionToF(// const unsigned int link, const unsigned int robId,
                                        AFP_RawContainer *container) const {

  if (!container) {
    ATH_MSG_WARNING("NULL pointer passed in argument: container. NULL pointer returned.");
    return nullptr;
  }
  
  // Code below is commented, because it was impossible to
  // reconstruct several bunch crossings in one event with it. In
  // standard data taking only one bunch crossing is saved and this
  // protection should not be necessary. However in case of big
  // readout problems the code may be needed, that is why it stays
  // commented here.

  // for (const AFP_ToFRawCollection& collection : container->collectionsToF())
  //   if (collection.link() == link && collection.robId() == robId)
  //     ATH_MSG_WARNING("ToF collection link="<<link<<" robId="<<robId<<" already in container, although it should not be there. Anyway creating new collection (Reading several BCX?)");

  AFP_ToFRawCollection& newCollection = container->newCollectionToF();
  return &newCollection;
}

void AFP_ByteStream2RawCnv::setDataHeader (uint32_t the_word, AFP_RawDataCommonHead* dataHead) const
{
  dataHead->setLink (m_wordReadout->link(the_word));
  dataHead->setHitDiscConfig (m_wordReadout->getBits(the_word, 29, 28));
}

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/*****************************************************************************
Name    : GlobalTriggerTagTool.cxx
Package : offline/PhysicsAnalysis/EventTag/EventTagUtils
Author  : Ketevi A. Assamagan
Created : January 2006
Purpose : create a GlobalTriggerTag - The Tag information associated to the event
          is built here

*****************************************************************************/
#include "EventTagUtils/GlobalTriggerTagTool.h"

#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/Property.h"
#include "GaudiKernel/IIncidentSvc.h"
#include "CLHEP/Units/SystemOfUnits.h"


#include "xAODEventInfo/EventInfo.h"

#include "EventInfo/TriggerInfo.h"
#include "EventInfo/EventInfo.h"
#include "EventInfo/EventID.h"
#include "EventInfo/EventIncident.h"


#include "TrigDecisionTool/ChainGroup.h"
#include "TrigDecisionTool/TDTUtilities.h"
#include "TrigDecisionTool/Conditions.h"

#include "DBDataModel/CollectionMetadata.h"
#include "AthenaKernel/IOVRange.h"
#include "AthenaPoolUtilities/TagMetadataKey.h"

#include "TagEvent/GlobalTriggerAttributeNames.h"
#include "AthenaPoolUtilities/AthenaAttributeSpecification.h"

#include <sstream>

/** the constructor */
GlobalTriggerTagTool::GlobalTriggerTagTool (const std::string& type, const
					    std::string& name, const IInterface* parent) :
  AthAlgTool( type, name, parent ),
  m_trigDec("Trig::TrigDecisionTool/TrigDecisionTool"),
  m_pMetaDataStore ("StoreGateSvc/MetaDataStore",      name)
{
  declareInterface<GlobalTriggerTagTool>( this );
  declareProperty("TrigDecisionTool", m_trigDec, "The tool to access TrigDecision");
}

/** initialization - called once at the beginning */
StatusCode  GlobalTriggerTagTool::initialize() 
{

  ATH_MSG_DEBUG("in initialize()");

  StatusCode sc = m_pMetaDataStore.retrieve();
  if (sc.isFailure() || 0 == m_pMetaDataStore) {
     ATH_MSG_ERROR("Could not find MetaDataStore");
     return sc;
  }

  sc = m_trigDec.retrieve();
  if (sc.isFailure() ) {
    ATH_MSG_ERROR("Unable to retrieve TrigDecisionTool!");
    return sc;
  }

  // Listen for end of run
  ServiceHandle<IIncidentSvc> incSvc("IncidentSvc", this->name());
  sc = incSvc.retrieve();
  if (sc.isFailure()) {
    ATH_MSG_ERROR("Unable to get the IncidentSvc");
    return(sc);
  }
  int PRIORITY = 100;
  incSvc->addListener(this, "BeginRun", PRIORITY);
  incSvc->addListener(this, "EndRun", PRIORITY);

  m_trigDec->ExperimentalAndExpertMethods()->enable();

  return StatusCode::SUCCESS;
}


/** build the attribute list  for Trigger */
void GlobalTriggerTagTool::handle ( const Incident& incident ) {

  ATH_MSG_DEBUG("in beginRun()");

  if (incident.type()==IncidentType::BeginRun) {
     m_ccnameL1 = getChainCounter("L1_.*");
     m_ccnameHLT = getChainCounter("HLT_.*");

     CollectionMetadataContainer* bitmaplist = 0;

     if (m_pMetaDataStore->contains<CollectionMetadataContainer>(this->name())) {
        ATH_MSG_DEBUG("Pre-existing CollectionMetadataContainer found");
        StatusCode status = m_pMetaDataStore->retrieve(bitmaplist,this->name());
        if (!status.isSuccess()) {
           ATH_MSG_ERROR("Could not retrieve CollectionMetadataContainer for " << this->name());
        }
     }
     else {
        bitmaplist = new CollectionMetadataContainer;
        StatusCode sc = m_pMetaDataStore->record(bitmaplist,this->name());
        if (!sc.isSuccess()) {
           ATH_MSG_ERROR("Could not store stream bit map in Metadata store for " << this->name());
        }
        else {
           ATH_MSG_INFO("stream bit map copied to MetaDataStore");
        }
     }
     const EventIncident* evtinc = dynamic_cast<const EventIncident*>(&incident);
     if(evtinc==0) ATH_MSG_ERROR("Couldn't get EventIncident object => Event Info not available");
     unsigned long run = incident.context().eventID().run_number();

     unsigned int index = 0;
     std::map<std::string,std::string> bitmap;
     std::vector<std::string> streams = m_trigDec->getListOfStreams();
     for (std::vector<std::string>::const_iterator it = streams.begin(); it != streams.end(); ++it) {
        if (m_TDTstreamMap.find(*it)==m_TDTstreamMap.end()) {
           TagMetadataKey tkey;
           std::stringstream bitname; 
           bitname << "(b=" << index << ")";
           IOVRange iov(IOVTime(run,0),IOVTime(run,-1));
           tkey.setKey("SFO_StreamBit");
           tkey.setQual(bitname.str());
           tkey.setIOV((std::string)iov);
           bitmap.insert(std::make_pair(tkey.toString(),*it));
           m_TDTstreamMap.insert(std::make_pair(*it,index));
           ++index;
        }
        else {
           ATH_MSG_INFO("Stream " << *it << " already present");
        }
        ATH_MSG_DEBUG("TrigDecisionTool returns stream = " << *it);
     }

     // Add it to the list
     //CollectionMetadata* newmap = new CollectionMetadata(bitmap); 
     //bitmaplist->push_back(newmap);
  }
  else {
     ATH_MSG_DEBUG(incident.type() << " not handled");
  }

}

std::map<int,std::string> GlobalTriggerTagTool::getChainCounter(std::string level) { 

  ATH_MSG_DEBUG("in getchaincounter ");

  // get the chains for each trigger level.
  const Trig::ChainGroup* group = m_trigDec->getChainGroup(level);
  std::vector<std::string> name = group->getListOfTriggers();
  
  std::map<int,std::string> ccname;
  for(unsigned int i=0; i<name.size(); ++i) {
    int cc;
    if ( level.substr(0,2) == "L1" ) {
      cc = m_trigDec->ExperimentalAndExpertMethods()->getItemConfigurationDetails(name[i])->ctpId();
    } else {
      cc = m_trigDec->ExperimentalAndExpertMethods()->getChainConfigurationDetails(name[i])->chain_counter();
    }
    ccname[cc]=name[i];
  }
  return ccname;
}
 

/** build the attribute list  for Trigger */
StatusCode GlobalTriggerTagTool::attributeSpecification(std::map<std::string,AthenaAttributeType>& attrMap,
                                                        const std::vector<int>& max) 
{
  ATH_MSG_DEBUG("in attributeSpecification()");

  std::string level1TriggerType ="Level1TriggerType";
  attrMap[ level1TriggerType ] = AthenaAttributeType("unsigned int", "", TriggerAttributeGroupNames[Trg::LVL1]);
  m_lv1StrType.push_back( level1TriggerType );

  std::ostringstream os;
  int maxNumber =0;
  if ( max.size() > 0 ) maxNumber = max[0]; 
  for (int i=0; i<maxNumber; ++i) {
    /** LVL1 */
    os.str("");
    if (i < 16) os << TriggerAttributeNames[Trg::LVL1] << "TBP" << std::dec << i;
    else if (i < 32) os << TriggerAttributeNames[Trg::LVL1] << "TAP" << std::dec << (i%16);
    else  os << TriggerAttributeNames[Trg::LVL1] << "TAV" << std::dec << (i%16);
    attrMap[ os.str() ] = AthenaAttributeType("unsigned int", TriggerAttributeUnitNames[Trg::LVL1], TriggerAttributeGroupNames[Trg::LVL1]);
    m_lv1Str.push_back( os.str() );
  }




  if ( max.size() > 2 ) maxNumber = max[1];
  for (int i=0; i<maxNumber; ++i) { 
    /** HLT */
    os.str("");
    os << TriggerAttributeNames[Trg::HLT] << std::dec << i;
    attrMap[ os.str() ] = AthenaAttributeType("unsigned long long", TriggerAttributeUnitNames[Trg::HLT], TriggerAttributeGroupNames[Trg::HLT]);
    m_hltStr.push_back( os.str() );

  }
  // Add Stream info
  attrMap[TriggerAttributeNames[Trg::Stream] ] = AthenaAttributeType("unsigned int",TriggerAttributeUnitNames[Trg::Stream], TriggerAttributeGroupNames[Trg::Stream]);

  // Add BunchGroup
  attrMap[TriggerAttributeNames[Trg::BunchGroup] ] = AthenaAttributeType("unsigned int",TriggerAttributeUnitNames[Trg::BunchGroup], TriggerAttributeGroupNames[Trg::BunchGroup]);

  ATH_MSG_DEBUG("initialisation finished: attributes specified");

  return StatusCode::SUCCESS;
}

/** execute - called on every event */
StatusCode  GlobalTriggerTagTool::execute(TagFragmentCollection& globalTriggerTag, const std::vector<int>& max) {

  ATH_MSG_DEBUG("in execute() for global Trigger Info");

  /** retrieve event info */
  const xAOD::EventInfo* eventInfo;
  StatusCode sc = evtStore()->retrieve( eventInfo );
  if (sc.isFailure()) {
    const EventInfo* oeventInfo;
    sc = evtStore()->retrieve( oeventInfo );
    if (sc.isFailure()) {
      ATH_MSG_DEBUG("Cannot get event info.");
      return StatusCode::FAILURE;
    }
    else {
      globalTriggerTag.insert( m_lv1StrType[0], oeventInfo->trigger_info()->level1TriggerType() );
      // Put stream decisions into a bit mask
      unsigned int stream = 0;

      for (const auto& STit :  oeventInfo->trigger_info()->streamTags()) {
        // check if stream is in list, if not add it, otherwise increment it
        std::map<std::string,unsigned int>::const_iterator bitshift = m_TDTstreamMap.find(STit.name());
        if (bitshift != m_TDTstreamMap.end()) {
          ATH_MSG_DEBUG("Insertting for " << STit.name());
          stream += 1 << bitshift->second;
        }
        else {
          ATH_MSG_WARNING("StreamTag " << STit.name() << " not found in StreamMap from TDT");
        }
      }
      globalTriggerTag.insert(TriggerAttributeNames[Trg::Stream], stream );
    }
  }
  else {
    globalTriggerTag.insert( m_lv1StrType[0], eventInfo->level1TriggerType() );
    // Put stream decisions into a bit mask
    uint64_t stream = 0;

    for (const auto& STit :  eventInfo->streamTags()) {
      // check if stream is in list, if not add it, otherwise increment it
      std::map<std::string,unsigned int>::const_iterator bitshift = m_TDTstreamMap.find(STit.name());
      if (bitshift != m_TDTstreamMap.end()) {
         ATH_MSG_DEBUG("Inserting for " << STit.name());
         stream += 1ULL << bitshift->second;
      }
      else {
        ATH_MSG_WARNING("StreamTag " << STit.name() << " not found in StreamMap from TDT");
      }
    }
  
    globalTriggerTag.insert(TriggerAttributeNames[Trg::Stream], stream );
  }


  /** Level 1 */
  unsigned int maxNumber = 0;
  if ( max.size()>0 ) maxNumber = max[0];
  std::vector<unsigned int> test;
  test.resize(maxNumber,0x0);

  // Fill the 3 sets of words for Level 1
  for( std::map<int, std::string>::iterator it=m_ccnameL1.begin(); it!=m_ccnameL1.end(); ++it) {
    unsigned int bit = 1u << ( (*it).first % 32 );
    unsigned int word = ((*it).first)/32;
    if ( (word+32)<maxNumber ) {
      if (this->isPassedBits(((*it).second), TrigDefs::L1_isPassedBeforePrescale)) test[word]    |= bit;
      if (this->isPassedBits(((*it).second), TrigDefs::L1_isPassedAfterPrescale))  test[word+16]  |= bit;
      if (this->isPassedBits(((*it).second), TrigDefs::L1_isPassedAfterVeto))      test[word+32] |= bit;
    } else {
      ATH_MSG_DEBUG("Word is not within the limit : " << word);
    }
  }

  for (unsigned int i=0; i<maxNumber; i++) {
    if ( i<m_ccnameL1.size() ) {
      // careful: we are converting the 'int' bitfield to 'double' here
      globalTriggerTag.insert( m_lv1Str[i], (double)test[i] );
    } else {
      globalTriggerTag.insert( m_lv1Str[i], 0.0 );
    }

  }


  /** HLT */
  if ( max.size()>1 )  maxNumber = max[1]; 
  test.clear(); test.resize(maxNumber,0x0);

  for( std::map<int, std::string>::iterator it=m_ccnameHLT.begin(); it!=m_ccnameHLT.end(); ++it) {
    unsigned int bit = 1u << ( (*it).first % 32 );
    unsigned int word = ((*it).first)/32;
    if ( word<maxNumber ) {
      //(RawAccepted && ! Prescaled && ! Resurrected) || PassedThrough
      bool passedPhysics = m_trigDec->isPassed((*it).second);
      bool passedPT      = this->isPassed((*it).second, TrigDefs::passedThrough);
      //bool passedRes     = this->isPassedBits((*it).second, TrigDefs::hlt_resurrected);

      if( passedPhysics || passedPT  ) test[word] |= bit;
    } else {
      ATH_MSG_DEBUG("Word is not within the limit : " << word);
    }
  }

  for (unsigned int i=0; i<maxNumber; i++) {
    if ( i<m_ccnameHLT.size()) {
        // careful: we are converting the 'int' bitfield to 'double' here
      globalTriggerTag.insert( m_hltStr[i], (double)test[i]);
    } else {
      globalTriggerTag.insert( m_hltStr[i], 0.0 );
    }
  }
 

  // Put stream decisions into a bit mask
  uint64_t stream = 0;
  //std::vector<TriggerInfo::StreamTag>::const_iterator STit  = eventInfo->streamTags().begin();
  //std::vector<TriggerInfo::StreamTag>::const_iterator STend = eventInfo->streamTags().end();
  //while (STit != STend) {

  for (const auto& STit :  eventInfo->streamTags()) {
    // check if stream is in list, if not add it, otherwise increment it
    std::map<std::string,unsigned int>::const_iterator bitshift = m_TDTstreamMap.find(STit.name());
    if (bitshift != m_TDTstreamMap.end()) {
       ATH_MSG_DEBUG("Inserting for " << STit.name());
       stream += 1ULL << bitshift->second;
    }
    else {
       ATH_MSG_WARNING("StreamTag " << STit.name() << " not found in StreamMap from TDT");
    }
  }
  
  globalTriggerTag.insert(TriggerAttributeNames[Trg::Stream], stream );

  /** BunchGroup **/
  unsigned int bg = 999; // Max physical value is 256 (8 bits for 8 bunch group)
  bg = (unsigned char) m_trigDec->getBGCode();
  globalTriggerTag.insert(TriggerAttributeNames[Trg::BunchGroup], bg );

  ATH_MSG_DEBUG("GlobalTriggerTagTool - execute() return success");

  return StatusCode::SUCCESS;

}

/** finialize - called once at the end */
StatusCode  GlobalTriggerTagTool::finalize() {
  ATH_MSG_DEBUG("in finalize()");
  return StatusCode::SUCCESS;
}

inline bool GlobalTriggerTagTool::isPassed(std::string name, unsigned int condition) {
  return m_trigDec->isPassed(name, condition);
}

inline bool GlobalTriggerTagTool::isPassedBits(std::string name, unsigned int condition) {
  return m_trigDec->isPassedBits(name) & condition;
}


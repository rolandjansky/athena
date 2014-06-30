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

/*
#include "EventInfo/TriggerInfo.h"
*/
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
     ccnameL1 = getChainCounter("L1_.*");
     ccnameL2 = getChainCounter("L2_.*");
     ccnameEF = getChainCounter("EF_.*");

     CollectionMetadataContainer* bitmaplist = 0;
     bool newlist(true);

     if (m_pMetaDataStore->contains<CollectionMetadataContainer>(this->name())) {
        newlist=false;
        ATH_MSG_DEBUG("Pre-existing CollectionMetadataContainer found");
        StatusCode status = m_pMetaDataStore->retrieve(bitmaplist,this->name());
        if (!status.isSuccess()) {
           ATH_MSG_ERROR("Could not retrieve CollectionMetadataContainer for " << this->name());
        }
     }
     else {
        bitmaplist = new CollectionMetadataContainer;
     }
     const EventIncident* evtinc = dynamic_cast<const EventIncident*>(&incident);
     if(evtinc==0) ATH_MSG_ERROR("Couldn't get EventIncident object => EventInfo not available");
     unsigned long run = evtinc->eventInfo().event_ID()->run_number();

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
     CollectionMetadata* newmap = new CollectionMetadata(bitmap); 
     bitmaplist->push_back(newmap);

     // if new list then record it in metadata store.
     if (newlist) {
        StatusCode sc = m_pMetaDataStore->record(bitmaplist,this->name());
        if (!sc.isSuccess()) {
           ATH_MSG_ERROR("Could not store stream bit map in Metadata store for " << this->name());
        }
        else {
           ATH_MSG_INFO("stream bit map copied to MetaDataStore");
        }
     }

  }
  else {
     ATH_MSG_DEBUG(incident.type() << " not handled");
  } 

}

std::map<int,std::string> GlobalTriggerTagTool::getChainCounter(std::string level) { 

  ATH_MSG_DEBUG("in getchaincounter ");

  // get the chains for each trigger level.
  const Trig::ChainGroup* m_level = m_trigDec->getChainGroup(level);
  std::vector<std::string> name = m_level->getListOfTriggers();
  
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
    if (i < 8) os << TriggerAttributeNames[Trg::LVL1] << "TBP" << std::dec << i;
    else if (i < 16) os << TriggerAttributeNames[Trg::LVL1] << "TAP" << std::dec << (i%8);
    else  os << TriggerAttributeNames[Trg::LVL1] << "TAV" << std::dec << (i%8);
    attrMap[ os.str() ] = AthenaAttributeType("unsigned int", TriggerAttributeUnitNames[Trg::LVL1], TriggerAttributeGroupNames[Trg::LVL1]);
    m_lv1Str.push_back( os.str() );
  }
  
  if ( max.size() > 1 ) maxNumber = max[1];
  for (int i=0; i<maxNumber; ++i) {  
    /** level2 */
    os.str("");
    os << TriggerAttributeNames[Trg::LVL2] << std::dec << i;
    attrMap[ os.str() ] = AthenaAttributeType("unsigned int", TriggerAttributeUnitNames[Trg::LVL2], TriggerAttributeGroupNames[Trg::LVL2]);
    m_lv2Str.push_back( os.str() );
  }

  if ( max.size() > 2 ) maxNumber = max[2];
  for (int i=0; i<maxNumber; ++i) { 
    /** EF */
    os.str("");
    os << TriggerAttributeNames[Trg::EF] << std::dec << i;
    attrMap[ os.str() ] = AthenaAttributeType("unsigned long long", TriggerAttributeUnitNames[Trg::EF], TriggerAttributeGroupNames[Trg::EF]);
    m_efStr.push_back( os.str() );

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
    ATH_MSG_DEBUG("Cannot get event info.");
    return StatusCode::SUCCESS;
  }
  globalTriggerTag.insert( m_lv1StrType[0], eventInfo->level1TriggerType() );


  /** Level 1 */
  unsigned int maxNumber = 0;
  if ( max.size()>0 ) maxNumber = max[0];
  std::vector<unsigned int> test;
  test.resize(maxNumber,0x0);

  // Fill the 3 sets of words for Level 1
  for( std::map<int, std::string>::iterator it=ccnameL1.begin(); it!=ccnameL1.end(); ++it) {
    unsigned int bit = 1u << ( (*it).first % 32 );
    unsigned int word = ((*it).first)/32;
    if ( word<maxNumber ) {
      if(this->isPassedBits(((*it).second), TrigDefs::L1_isPassedBeforePrescale)) test[word] |= bit;
      if(this->isPassedBits(((*it).second), TrigDefs::L1_isPassedAfterPrescale))  test[word+8] |= bit;
      if(this->isPassedBits(((*it).second), TrigDefs::L1_isPassedAfterVeto))      test[word+16] |= bit;
    } else {
      ATH_MSG_DEBUG("Word is not within the limit : " << word);
    }
  }

  for (unsigned int i=0; i<maxNumber; i++) {
    if ( i<ccnameL1.size() ) {
      globalTriggerTag.insert( m_lv1Str[i], test[i]);
    } else {
      globalTriggerTag.insert( m_lv1Str[i], 0.0 );
    }

  }


  /** Level 2 */
  if ( max.size()>1 )  maxNumber = max[1]; 
  test.clear(); test.resize(maxNumber,0x0);

  for( std::map<int, std::string>::iterator it=ccnameL2.begin(); it!=ccnameL2.end(); ++it) {
    unsigned int bit = 1u << ( (*it).first % 32 );
    unsigned int word = ((*it).first)/32;
    if ( word<maxNumber ) {
      //(RawAccepted && ! Prescaled && ! Resurrected) || PassedThrough
      bool passedPhysics = m_trigDec->isPassed((*it).second);
      bool passedPT      = this->isPassed((*it).second, TrigDefs::passedThrough);
      bool passedRes     = this->isPassedBits((*it).second, TrigDefs::L2_resurrected);

      if( passedPhysics || passedPT || passedRes ) test[word] |= bit;
    } else {
      ATH_MSG_DEBUG("Word is not within the limit : " << word);
    }
  }

  for (unsigned int i=0; i<maxNumber; i++) {
    if ( i<ccnameL2.size()) {
      globalTriggerTag.insert( m_lv2Str[i], test[i]);
    } else {
      globalTriggerTag.insert( m_lv2Str[i], 0.0 );
    }
  }
 
  /** Event Filter */
  if ( max.size()>2 ) maxNumber = max[2];
  std::vector<unsigned long long> testEF;
  testEF.resize(maxNumber,0x0L);

  for( std::map<int, std::string>::iterator it=ccnameEF.begin(); it!=ccnameEF.end(); ++it) {
    // take word number as chain counter integer divided by word length (32 bits)
    unsigned int word = (*it).first/32;
    // take raw bit in word as chain counter remaindered with word length (32 bits)
    unsigned long long bitR = 1ull << ( (*it).first % 32 );
    // take physics bit in word as chain counter remaindered with word length (32 bits) shifted by 32
    unsigned long long bitT = 1ull << (((*it).first % 32 )+32); 
    // check that word number is legal, then set decision bit if passed
    if ( word<maxNumber ) {
      bool passedPhysics = m_trigDec->isPassed((*it).second);
      bool passedPT      = this->isPassed((*it).second, TrigDefs::passedThrough);
      bool passedRes     = this->isPassedBits((*it).second, TrigDefs::EF_resurrected);

      bool passedInclusive = (passedPhysics || passedPT || passedRes );

      if( passedInclusive ) testEF[word] |= bitR;
      if( passedPhysics )   testEF[word] |= bitT;

    } else {
      ATH_MSG_DEBUG("Word is not within the limit : " << word);
    }
  }

  for (unsigned int i=0; i<maxNumber; i++) {
    if ( i<ccnameEF.size() ) globalTriggerTag.insert( m_efStr[i], testEF[i]);
    else globalTriggerTag.insert( m_efStr[i], 0.0 );
  }

  // Put stream decisions into a bit mask
  unsigned int stream = 0;
  //std::vector<TriggerInfo::StreamTag>::const_iterator STit  = eventInfo->streamTags().begin();
  //std::vector<TriggerInfo::StreamTag>::const_iterator STend = eventInfo->streamTags().end();
  //while (STit != STend) {

  for (const auto& STit :  eventInfo->streamTags()) {
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

/** destructor */
GlobalTriggerTagTool::~GlobalTriggerTagTool() {}

inline bool GlobalTriggerTagTool::isPassed(std::string name, unsigned int condition) {
  return m_trigDec->isPassed(name, condition);
}

inline bool GlobalTriggerTagTool::isPassedBits(std::string name, unsigned int condition) {
  return m_trigDec->isPassedBits(name) & condition;
}


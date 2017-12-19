/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "SCT_TdaqEnabledCondAlg.h"

#include "Identifier/IdentifierHash.h"
#include "SCT_Cabling/SCT_OnlineId.h"
#include "EventInfo/EventID.h"

#include "GaudiKernel/EventIDRange.h"

SCT_TdaqEnabledCondAlg::SCT_TdaqEnabledCondAlg(const std::string& name, ISvcLocator* pSvcLocator)
  : ::AthAlgorithm(name, pSvcLocator)
  , m_readKey{"/TDAQ/Resources/ATLAS/SCT/Robins"}
  , m_writeKey{"SCT_TdaqEnabledCondData", "SCT_TdaqEnabledCondData"}
  , m_condSvc{"CondSvc", name}
  , m_cablingSvc{"SCT_CablingSvc", name}
{
  declareProperty("ReadKey", m_readKey, "Key of input (raw) conditions folder");
  declareProperty("WriteKey", m_writeKey, "Key of output (derived) conditions folder");
  declareProperty("EventInfoKey", m_eventInfoKey=std::string{"ByteStreamEventInfo"}, "Key of non-xAOD EventInfo");
}

SCT_TdaqEnabledCondAlg::~SCT_TdaqEnabledCondAlg()
{
}

StatusCode SCT_TdaqEnabledCondAlg::initialize()
{
  ATH_MSG_DEBUG("initialize " << name());

  // CondSvc
  ATH_CHECK( m_condSvc.retrieve() );
  // SCT cabling service
  ATH_CHECK( m_cablingSvc.retrieve() );

  // Read Cond Handle
  ATH_CHECK( m_readKey.initialize() );

  // Write Cond Handle
  ATH_CHECK( m_writeKey.initialize() );
  // Register write handle
  if(m_condSvc->regHandle(this, m_writeKey).isFailure()) {
    ATH_MSG_ERROR("unable to register WriteCondHandle " << m_writeKey.fullKey() << " with CondSvc");
    return StatusCode::FAILURE;
  }

  // Read Handle
  ATH_CHECK( m_eventInfoKey.initialize() );

  return StatusCode::SUCCESS;
}

StatusCode SCT_TdaqEnabledCondAlg::execute()
{
  ATH_MSG_DEBUG("execute " << name());

  // Write Cond Handle
  SG::WriteCondHandle<SCT_TdaqEnabledCondData> writeHandle{m_writeKey};

  // Do we have a valid Write Cond Handle for current time?
  if(writeHandle.isValid()) {
    // in theory this should never be called in MT
    writeHandle.updateStore();
    ATH_MSG_DEBUG("CondHandle " << writeHandle.fullKey() << " is already valid."
                  << ". In theory this should not be called, but may happen"
                  << " if multiple concurrent events are being processed out of order."
                  << " Forcing update of Store contents");
    return StatusCode::SUCCESS; 
  }

  // Construct the output Cond Object and fill it in
  SCT_TdaqEnabledCondData* writeCdo{new SCT_TdaqEnabledCondData()};
  // clear structures before filling
  writeCdo->clear();

  // Define validity of the output cond obbject and record it
  EventIDRange rangeW;

  // check whether we expect valid data at this time
  if(unfilledRun()) {
    EventIDBase unfilledStart{0, 0, 0, 0}; // run 0, event 0, timestamp 0, timestamp_ns 0
    EventIDBase unfilledStop{s_earliestRunForFolder, 0, s_earliestTimeStampForFolder, 0}; // run 119253, event 0, timestamp 1245064619, timestamp_ns 0
    EventIDRange unfilledRange{unfilledStart, unfilledStop};
    rangeW = unfilledRange;
    writeCdo->setNoneBad(true);
    writeCdo->setFilled(true);
  } else {
    // Read Cond Handle 
    SG::ReadCondHandle<CondAttrListCollection> readHandle{m_readKey};
    const CondAttrListCollection* readCdo{*readHandle}; 
    if(readCdo==nullptr) {
      ATH_MSG_ERROR("Null pointer to the read conditions object");
      delete writeCdo;
      return StatusCode::FAILURE;
    }

    ATH_MSG_INFO("Size of CondAttrListCollection readCdo->size()= " << readCdo->size());

    CondAttrListCollection::const_iterator attrList{readCdo->begin()};
    CondAttrListCollection::const_iterator end{readCdo->end()};
    // CondAttrListCollection doesnt support C++11 type loops, no generic 'begin'
    for(; attrList!=end; ++attrList) {
      // A CondAttrListCollection is a map of ChanNum and AttributeList
      CondAttrListCollection::ChanNum channelNumber{attrList->first};
      CondAttrListCollection::AttributeList payload{attrList->second};
      std::string enabled{payload["class"].data<std::string>()};
      std::string chanName{readCdo->chanName(channelNumber)};
      unsigned int rodNumber{parseChannelName(chanName)};
      // range check on the rod channel number has been removed, since it refers both to existing channel names
      // which can be rods in slots 1-128 but also historical names which have since been removed
      if(SCT_OnlineId::rodIdInRange(rodNumber)) {
        if((not enabled.empty()) and (not writeCdo->setGoodRod(rodNumber))) {
          ATH_MSG_WARNING("Set insertion failed for rod "<<rodNumber);
        }
      } else {
        ATH_MSG_WARNING("Names in "<<m_readKey.key()<<" should be of the form ROL-SCT-BA-00-210000 this channel, number "<<channelNumber<<", is: "<<chanName);
      }
    }

    if(writeCdo->getGoodRods().size()>s_NRODS) {
      ATH_MSG_ERROR("The number of rods declared as good appears to be greater than the permissible number of rods ("<<s_NRODS<<")");
      writeCdo->setFilled(false);
      delete writeCdo;
      return StatusCode::FAILURE;
    }
    
    long unsigned int nBad{s_NRODS-writeCdo->getGoodRods().size()};
    std::string howMany{inWords(nBad)};
    ATH_MSG_DEBUG(howMany << " declared bad in the database.");
    
    // provide short-cut for no rods bad (hopefully the most common case)
    writeCdo->setNoneBad(nBad==0);
    if(writeCdo->isNoneBad()) {
      writeCdo->setFilled(true);
    } else {
      // now find the modules which belong to those rods...
      //      const int modulesPerRod(48);
      std::vector<IdentifierHash> tmpIdVec{0};
      tmpIdVec.reserve(s_modulesPerRod);
      for(const auto & thisRod : writeCdo->getGoodRods()) {
        tmpIdVec.clear();
        m_cablingSvc->getHashesForRod(tmpIdVec, thisRod);
        writeCdo->setGoodModules(tmpIdVec);
      }
      writeCdo->setFilled(true);
    } 

    if(!readHandle.range(rangeW)) {
      ATH_MSG_ERROR("Failed to retrieve validity range for " << readHandle.key());
      delete writeCdo;
      return StatusCode::FAILURE;
    }
  }

  if(writeHandle.record(rangeW, writeCdo).isFailure()) {
    ATH_MSG_ERROR("Could not record SCT_TdaqEnabledCondData " << writeHandle.key() 
                  << " with EventRange " << rangeW
                  << " into Conditions Store");
    return StatusCode::FAILURE;
  }
  ATH_MSG_INFO("recorded new CDO " << writeHandle.key() << " with range " << rangeW << " into Conditions Store");

  return StatusCode::SUCCESS;
}

StatusCode SCT_TdaqEnabledCondAlg::finalize()
{
  ATH_MSG_DEBUG("finalize " << name());
  return StatusCode::SUCCESS;
}

bool SCT_TdaqEnabledCondAlg::unfilledRun() const {
  SG::ReadHandle<EventInfo> event{m_eventInfoKey};
  if (event.isValid()) {
    const unsigned int runNumber{event->event_ID()->run_number()};
    const bool noDataExpected{runNumber < s_earliestRunForFolder};
    if (noDataExpected) ATH_MSG_INFO("This run occurred before the /TDAQ/EnabledResources/ATLAS/SCT/Robins folder was filled in COOL; assuming the SCT is all ok.");
    return noDataExpected;
  }
  ATH_MSG_WARNING("The event information could not be retrieved in this service");
  return false; //this means the service will attempt to access the folder and fill it, even though the run number is unknown
}

//parse a rod channel name to a rod number, names are of the format 'ROL-SCT-BA-00-210000'
//October 2014: names can now also be of format 'ROL-SCT-B-00-210100'
unsigned int SCT_TdaqEnabledCondAlg::parseChannelName(const std::string &chanNameString) const {
  unsigned int result{0};
  const long unsigned int length{chanNameString.size()};
  if (length < 19) return result; //very rough check on sanity of string, should use regex
  //get the last six numbers, these are in hex  
  std::istringstream iss{chanNameString.substr(length-6, 6)};
  iss.exceptions(std::ios_base::badbit|std::ios_base::failbit);
  try{
    iss>>std::hex>>result;
  } catch (std::ios_base::failure){ //bad conversion to int
    std::cerr<<"Bad conversion of last 6 digits of "<<chanNameString<<" to a hex number"<<std::endl;
    throw(std::ios_base::failure("stringToInt failure in SCT_TdaqEnabledSvc"));
  } 
  return result; 
}

std::string SCT_TdaqEnabledCondAlg::inWords(const unsigned int aNumber) const {
  switch (aNumber){
  case 0:
    return std::string("No SCT rods were");
    break;
  case 1:
    return std::string("One SCT rod was");
    break;
  default:
    return std::to_string(aNumber) + " SCT rods were"; //C++11
    break;
  }
}

const unsigned int SCT_TdaqEnabledCondAlg::s_NRODS{128};
const unsigned int SCT_TdaqEnabledCondAlg::s_modulesPerRod{48};
const unsigned int SCT_TdaqEnabledCondAlg::s_earliestRunForFolder{119253};
const unsigned int SCT_TdaqEnabledCondAlg::s_earliestTimeStampForFolder{1245064619}; // 20090615T111659 UTC for run 119253

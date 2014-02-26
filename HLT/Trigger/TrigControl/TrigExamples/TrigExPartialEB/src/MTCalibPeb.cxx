/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "TrigExPartialEB/MTCalibPeb.h"
#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/AlgFactory.h"
#include "GaudiKernel/Parsers.h"
#include "AthenaKernel/Timeout.h"
#include "TrigT1Result/RoIBResult.h"
#include "StoreGate/StoreGateSvc.h"
#include "StoreGate/DataHandle.h"
#include "ByteStreamCnvSvcBase/IROBDataProviderSvc.h"
#include "TrigSteeringEvent/HLTResult.h"
#include "TrigDataAccessMonitoring/ROBDataMonitor.h"
#include <algorithm>
#include <boost/tokenizer.hpp>

/////////////////////////////////////////////////////////////////////////////

MTCalibPeb::MTCalibPeb(const std::string& name, ISvcLocator* pSvcLocator) :
  Algorithm(name, pSvcLocator), 
  m_storeGateSvc( "StoreGateSvc", name ),
  m_robDataProviderSvc( "ROBDataProviderSvc", name ),
  m_max_chain_counter(0)
{
  // Declare the properties
  declareProperty("RetrieveLvl1",          m_retrieveLvl1=false);
  declareProperty("RetrieveROBs",          m_retrieveROBs=false);
  declareProperty("HLTInstance",           m_hltInstance="HLT");
  declareProperty("HLTResultName",         m_hltResultName="HLTResult");
  declareProperty("ROBDataMonitorCollectionSGName", m_ROBDataMonitorCollection_SG_Name="ROBDataMonitorCollection","Name of cost monitoring collection in SG");
  declareProperty("RobId",                 m_mapRobIdsProperty);
  declareProperty("TimeBetweenRobRet",     m_timeBetweenRobRetMicroSec=0);
  declareProperty("ConfiguredStreamTags",  m_listStreamTags);
  declareProperty("ConfiguredChainNumbers",m_configuredChainNumbers);
  declareProperty("RandomAcceptRate"      ,m_acceptRate=-1.);
  declareProperty("BurnTime"              ,m_burnTimeMicroSec=0);
  declareProperty("NumberOfBurnCycles"    ,m_burnTimeCycles=1);
}

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * 

StatusCode MTCalibPeb::initialize(){

  // Part 1: Get the messaging service, print where you are
  MsgStream log(msgSvc(), name());
  log << MSG::INFO << "initialize()" << endreq;

#ifdef ATLAS_GAUDI_V21
  SmartIF<IService> tmp_msgSvc(msgSvc());
  if(tmp_msgSvc.isValid()) {
    log << MSG::INFO << " Algorithm = " << name() << " is connected to Message Service = "
        << tmp_msgSvc->name() << endreq;
  }
#else
  Service* tmp_msgSvc = dynamic_cast<Service*> (msgSvc());
  if(tmp_msgSvc != 0) {
    log << MSG::INFO << " Algorithm = " << name() << " is connected to Message Service = "
        << tmp_msgSvc->name() << endreq;
  }
#endif

  // Print out the property values
  log << MSG::INFO << " HLT Instance                       = " << m_hltInstance << endreq;
  log << MSG::INFO << " HLT Result name                    = " << m_hltResultName << endreq;
  m_hltResultSGKey = m_hltResultName.value() + "_" + m_hltInstance.value();
  log << MSG::INFO << " HLT Result SG key                  = " << m_hltResultSGKey << endreq;
  log << MSG::INFO << " SG key for ROB monitoring collect. = " << m_ROBDataMonitorCollection_SG_Name << endreq;
  log << MSG::INFO << " RetrieveLvl1                       = " << m_retrieveLvl1 << endreq;
  log << MSG::INFO << " RetrieveROBs                       = " << m_retrieveROBs << endreq;
  log << MSG::INFO << " Time between ROB retrievals [usec] = " << m_timeBetweenRobRetMicroSec << endreq;
  log << MSG::INFO << " List of ROBs for retieval: Number of retrievals = " << m_mapRobIdsProperty.value().size() <<  endreq;
  for (MapStringVector_t::const_iterator it_map = m_mapRobIdsProperty.value().begin(); it_map != m_mapRobIdsProperty.value().end();++it_map) {
    log << MSG::INFO << " Label = " << it_map->first << " Number of ROBs = " << (it_map->second).size() << endreq;
    std::string retr;
    for (std::vector<int>::const_iterator it_vec = (it_map->second).begin(); it_vec != (it_map->second).end(); ++it_vec) {
      if (*it_vec < 0 ) {
	log << MSG::DEBUG << "         ROB = 0x" << MSG::hex << *it_vec << MSG::dec << " -> invalid ROB Id. Ignored." << endreq;
      } else {  
	if ( (it_map->first).find(":ADD:") != std::string::npos ) {
	  retr = std::string(" -> on prefetching list ");
	  m_mapRobIds[it_map->first].push_back(*it_vec); 
	} else if ( (it_map->first).find(":GET:") != std::string::npos )  {
	  retr = std::string(" -> retrieved from ROS ");
	  m_mapRobIds[it_map->first].push_back(*it_vec);
	} else if ( (it_map->first).find(":COL:") != std::string::npos ) {
	  retr = std::string(" -> build event ");
	  m_mapRobIds[it_map->first].push_back(*it_vec);
	}
	log << MSG::DEBUG << "         ROB = 0x" << MSG::hex << *it_vec << MSG::dec << retr << endreq;
      }
    }
  }

  log << MSG::INFO << " Stream Tags List                   = " << m_listStreamTags << endreq;
  if (m_listStreamTags.value().size() > 0) {
    m_configuredStreamTags.reserve( m_listStreamTags.value().size() );
    std::vector< std::vector<std::string> >::const_iterator it = m_listStreamTags.value().begin();
    while (it != m_listStreamTags.value().end()) {
      if ( (*it).size() < 3 ) {
	log << MSG::WARNING << " Invalid StreamTag specified:" << (*it)[0] << " Provide at least (name, type, obeys_LB)" << endreq;
      }
      if ( (*it).size() == 3 ) {
	m_configuredStreamTags.push_back(TriggerInfo::StreamTag( (*it)[0], (*it)[1], ( (*it)[2] == "true" ) )) ;
      }
      if ( (*it).size() == 4 ) {
	std::set<TriggerInfo::number_type> tmp_robs;
	streamTagRobDetHelper((*it)[3], tmp_robs);
	m_configuredStreamTags.push_back(TriggerInfo::StreamTag( (*it)[0], (*it)[1], ( (*it)[2] == "true" ), tmp_robs )) ;
      }
      if ( (*it).size() == 5 ) {
	std::set<TriggerInfo::number_type> tmp_robs;
	streamTagRobDetHelper((*it)[3], tmp_robs);
	std::set<TriggerInfo::number_type> tmp_dets;
	streamTagRobDetHelper((*it)[4], tmp_dets);
	m_configuredStreamTags.push_back(TriggerInfo::StreamTag( (*it)[0], (*it)[1], ( (*it)[2] == "true" ), tmp_robs , tmp_dets )) ;
      }
      ++it;
    }
  }
  log << MSG::INFO << " Configured StreamTags " << endreq; 
  log << MSG::INFO << " ---------------------" << printStreamTags(m_configuredStreamTags) << endreq;

  log << MSG::INFO << " Chain numbers                      = " << m_configuredChainNumbers << endreq;
  m_max_chain_counter = 0;
  if (m_configuredChainNumbers.value().size() > 0) {
    Uint32Array_t::const_iterator it = m_configuredChainNumbers.value().begin();
    while (it != m_configuredChainNumbers.value().end()) {
      if ( (*it) > m_max_chain_counter ) m_max_chain_counter = *it;
      ++it;
    }
  }

  log << MSG::INFO << " Max. chain number                  = " << m_max_chain_counter << endreq;
  log << MSG::INFO << " Random accept rate                 = " << m_acceptRate << endreq;
  log << MSG::INFO << " Burn time [usec]                   = " << m_burnTimeMicroSec << endreq;
  log << MSG::INFO << " Number of Burn time Cycles         = " << m_burnTimeCycles << endreq;

  // Part 3: Locate the StoreGateSvc
  StatusCode sc =  m_storeGateSvc.retrieve();
  if (!sc.isSuccess()) {
    log << MSG::ERROR << "Could not find StoreGateSvc" << endreq;
    return sc;
  } else {
    log << MSG::INFO << " Algorithm = " << name() << " is connected to StoreGate Service          = "
        << m_storeGateSvc->name() << endreq;
  }

  // Part 4: Locate the ROBDataProviderSvc
  sc = m_robDataProviderSvc.retrieve();
  if (!sc.isSuccess()) {
    log << MSG::ERROR << "Could not find ROBDataProviderSvc" << endreq;
    return sc;
  } else {
    IService* svc = dynamic_cast<IService*>(&*m_robDataProviderSvc);
    if(svc != 0 ) {
      log << MSG::INFO << " Algorithm = " << name() << " is connected to ROBDataProviderSvc Service = "
	  << svc->name() << endreq;
    }
  }

  // Setup the HLT ROB Data Provider Service when configured
  if ( &*m_robDataProviderSvc ) {
#ifdef ATLAS_GAUDI_V21
    m_hltROBDataProviderSvc = SmartIF<ITrigROBDataProviderSvc>( &*m_robDataProviderSvc );
#else
    m_hltROBDataProviderSvc = SmartIF<ITrigROBDataProviderSvc>( IID_ITrigROBDataProviderSvc, &*m_robDataProviderSvc );
#endif
    if (m_hltROBDataProviderSvc.isValid()) {
      log << MSG::INFO << " A ROBDataProviderSvc implementing the HLT interface ITrigROBDataProviderSvc was found."
		  << endreq;
    } else {
      log << MSG::INFO << " No ROBDataProviderSvc implementing the HLT interface ITrigROBDataProviderSvc was found."
		  << endreq;
    }
  }

  return StatusCode::SUCCESS;
}

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * 

StatusCode MTCalibPeb::execute() {

  // Get the messaging service, print where you are
  MsgStream log(msgSvc(), name());
  log << MSG::INFO << "execute()" << endreq;

  log << MSG::DEBUG << " StoreGate structure: Begin execute " << name() << "\n"
      << m_storeGateSvc->dump() << endreq;

  uint32_t lvl1ID(0);

  // get EventInfo
  const EventInfo* p_EventInfo(0);
  StatusCode sc = m_storeGateSvc->retrieve(p_EventInfo);
  if(sc.isFailure()){
    log << MSG::FATAL << "Can't get EventInfo object for update of the StreamTag and TriggerInfo" << endreq;
    return sc;
  }

  lvl1ID = p_EventInfo->event_ID()->event_number();

  // Get the Lvl1 Id from the RoIBResult
  const DataHandle<ROIB::RoIBResult> dobj;
  if (m_retrieveLvl1) {
    sc = m_storeGateSvc->retrieve(dobj,"RoIBResult") ;
    if ( sc.isFailure() ) {
      log << MSG::ERROR << "Could not find Lvl1Result" << endreq;
      return sc;
    } else {
      log << MSG::INFO << " ---> Lvl1Result ID " << dobj->cTPResult().header().L1ID() 
	  << " (From EventInfo = " << lvl1ID << " )"
	  << endreq;
    }
  }

  // Get the ROBs
  if (m_retrieveROBs) {
    // preload and retrieve ROBs
    for (MapStringUint32Array_t::const_iterator it_map = m_mapRobIds.begin(); it_map != m_mapRobIds.end(); ++it_map) {
      // check for time out
      if (Athena::Timeout::instance().reached()) {
	log << MSG::INFO << " Time out reached in ROB retrieval loop." << endreq;
	break;
      }

      log << MSG::INFO << " ===> ROB preloading/retrieval: Label = " << it_map->first << " <=== " << endreq;
      // preload ROBs in memory
      if ( (it_map->first).find(":ADD:") != std::string::npos ) {
	m_robDataProviderSvc->addROBData(it_map->second, name()+"-ADD");
      }


      // retrieve ROBs
      if ( (it_map->first).find(":GET:") != std::string::npos ) {
	std::vector<const OFFLINE_FRAGMENTS_NAMESPACE::ROBFragment*> robFragments;
	m_robDataProviderSvc->getROBData(it_map->second, robFragments, name()+"-GET") ;
	log << MSG::INFO << " ---> number of ROBs retrieved " << robFragments.size() << endreq;
	std::vector<const OFFLINE_FRAGMENTS_NAMESPACE::ROBFragment*>::const_iterator it = robFragments.begin();
	log << MSG::INFO << " ---> List of ROBs found: " << endreq;                                   
	while (it != robFragments.end()) {
	  log << MSG::INFO << " ---> ROB ID = 0x" << MSG::hex << (*it)->source_id() << MSG::dec
	      << " ROD ID = 0x" << MSG::hex << (*it)->rod_source_id() << MSG::dec 
	      << " Level-1 ID = " << (*it)->rod_lvl1_id() << endreq;
	  ++it;
	}
      }

      // do event building / get ROBs for complete event
      if ( (it_map->first).find(":COL:") != std::string::npos ) {
	int n_rbs(0);	
	if (m_hltROBDataProviderSvc.isValid()) {
	  n_rbs = m_hltROBDataProviderSvc->collectCompleteEventData(name()+"-COL");
	  log << MSG::INFO << " ---> Caller name for collectCompleteEventData = " 
	      << m_hltROBDataProviderSvc->getCallerName() << endreq;
	}
	log << MSG::INFO << " ---> number of ROBs retrieved " << n_rbs << endreq;
      }

      // burn some time between ROB retrievals
      // m_cpu_burner.burn(m_timeBetweenRobRetMicroSec);
      usleep(randomCPUBurn(m_timeBetweenRobRetMicroSec));
    }
  }

  // get the HLT result
  HLT::HLTResult* pHltResult(0);
  if ( !(m_storeGateSvc->transientContains<HLT::HLTResult>(m_hltResultSGKey)) ) {
    pHltResult = new HLT::HLTResult() ;
    if (pHltResult) {
      // Record it in StoreGate (object can be modified)
      sc = m_storeGateSvc->record(pHltResult, m_hltResultSGKey, true);
      if( !sc.isSuccess() ) {
	log << MSG::ERROR << "Error declaring L2 Result object in SG" << endreq;
      }
    } else {
      log << MSG::ERROR << "Error creating L2 Result object. Pointer = 0x" << MSG::hex << pHltResult << endreq;
    }
  } else {
    sc = m_storeGateSvc->retrieve(pHltResult,m_hltResultSGKey) ;
    if ( sc.isFailure() ) {
      log << MSG::ERROR << "Error retrieving L2 Result object. Pointer = 0x" << MSG::hex << pHltResult << endreq;
    }
  }

  // burn cpu time
  for (unsigned int itr=0; itr != (unsigned int) (m_burnTimeCycles.value()); ++itr) {
    if (Athena::Timeout::instance().reached()) {
      log << MSG::INFO << " Time out reached:CPU burn Loop # = " << itr << endreq;
      break;
    }
    unsigned int burntime = randomCPUBurn(m_burnTimeMicroSec);
    log << MSG::VERBOSE << " CPU burn Loop # = " << itr
	<< " Burn time [micro sec] = " << burntime << endreq;
    usleep(burntime);
  }

  // decide if to accept the event
  if (pHltResult) {
    pHltResult->setHLTStatus( 0 ) ;   
    pHltResult->setLvl1Id( lvl1ID );   
    pHltResult->setAccepted( randomAccept(m_acceptRate) );
    pHltResult->setPassThrough( false );
    if (m_hltInstance.value() == "L2") {
      pHltResult->setHLTLevel( HLT::L2 );
    } else if (m_hltInstance.value() == "EF") {
      pHltResult->setHLTLevel( HLT::EF );
    } else {
      pHltResult->setHLTLevel( HLT::HLT );
    }
    pHltResult->setCreatedOutsideHLT( false );

    if ( pHltResult->isAccepted() ) {
      // set the stream tag
      StreamTagVector_t vecStreamTags = p_EventInfo->trigger_info()->streamTags();
      for (StreamTagVector_t::const_iterator it = m_configuredStreamTags.begin();
	   it != m_configuredStreamTags.end(); ++it) {
	vecStreamTags.push_back( (*it) );
      }
      p_EventInfo->trigger_info()->setStreamTags(vecStreamTags);

      // set trigger info words
      unsigned int origSize(0);
      if (m_hltInstance.value() == "L2") {
	origSize = (p_EventInfo->trigger_info()->level2TriggerInfo()).size();
      } else {
	origSize = (p_EventInfo->trigger_info()->eventFilterInfo()).size();
      }

      unsigned int infoSize = int(std::floor( m_max_chain_counter/(sizeof(uint32_t)*8) + 1. ));
      Uint32Array_t vecTriggerTypeBits( std::max(infoSize, origSize) ) ;

      // copy old trigger words
      for (unsigned int index=0; index != origSize; ++index) {
	if (m_hltInstance.value() == "L2") {
	  vecTriggerTypeBits[index] = (p_EventInfo->trigger_info()->level2TriggerInfo())[index];
	} else {
	  vecTriggerTypeBits[index] = (p_EventInfo->trigger_info()->eventFilterInfo())[index];
	}
      }

      for (Uint32Array_t::const_iterator it = m_configuredChainNumbers.value().begin(); 
	   it != m_configuredChainNumbers.value().end(); ++it) { 
	setTriggerTypeBit( (*it), vecTriggerTypeBits); 
      }

      if (m_hltInstance.value() == "L2") {
	p_EventInfo->trigger_info()->setLevel2TriggerInfo(vecTriggerTypeBits);
      } else {
	p_EventInfo->trigger_info()->setEventFilterInfo(vecTriggerTypeBits);
      }
    } // end pHltResult->isAccepted()
  }

  log << MSG::DEBUG << " +-------------------+ " << endreq;
  log << MSG::DEBUG << " | Updated EventInfo | Level-1 ID = " << lvl1ID << endreq;
  log << MSG::DEBUG << " +-------------------+ " << endreq;
  log << MSG::DEBUG << " Complete EventID   = " << *(p_EventInfo->event_ID()) << endreq;
  log << MSG::DEBUG << "        EventType   = " << ((p_EventInfo->event_type())->typeToString()) << endreq;
  log << MSG::DEBUG << "      TriggerInfo   = " << *(p_EventInfo->trigger_info()) << endreq;

  log << MSG::DEBUG << " Dump of extended StreamTags " << endreq;
  log << MSG::DEBUG << " --------------------------- " 
      << printStreamTags(p_EventInfo->trigger_info()->streamTags())
      << endreq;

  std::vector<uint32_t> rawResult;
  bool serializeOk(false);
  if (pHltResult)  serializeOk = pHltResult->serialize(rawResult);
  log << MSG::DEBUG << " HLT Result payload: serializeOk = " << serializeOk << endreq; 
  unsigned int index(0);
  for (std::vector<uint32_t>::const_iterator it=rawResult.begin();it != rawResult.end(); ++it) {
    log << MSG::DEBUG << " data[" << std::setw(3) << index << "]" 
	<< " 0x" << std::setw(8) << MSG::hex << *it << MSG::dec << std::setw(12) << *it << endreq ;
    index++;
  }

  log << MSG::DEBUG << " StoreGate structure: End execute " << name() << "\n"
      << m_storeGateSvc->dump() << endreq;

  // Print ROB monitoring
  ROBDataMonitorCollection* p_robMonCollection(0);
  if ( m_storeGateSvc->transientContains<ROBDataMonitorCollection>(m_ROBDataMonitorCollection_SG_Name.value()) ) {
    if ( m_storeGateSvc->retrieve(p_robMonCollection).isFailure() ) {
      log << MSG::WARNING << " Retrieval of ROB Monitoring collection from StoreGate failed." << endreq;
      p_robMonCollection = 0;
    }
  }

  if ( p_robMonCollection ) {
    for (ROBDataMonitorCollection::iterator it=p_robMonCollection->begin();
	 it!=p_robMonCollection->end();++it) {
      std::ostringstream ost;
      ost << " --> ROB Mon Struct: " << (**it)  ;
      log << MSG::DEBUG << ost.str() << endreq;
    }
  }

  return StatusCode::SUCCESS;
}

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * 

StatusCode MTCalibPeb::finalize() {

  // Part 1: Get the messaging service, print where you are
  MsgStream log(msgSvc(), name());
  log << MSG::INFO << "finalize()" << endreq;

#ifdef ATLAS_GAUDI_V21
  m_hltROBDataProviderSvc.reset();
#endif

  return StatusCode::SUCCESS;
}

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * 

void MTCalibPeb::setTriggerTypeBit(unsigned int bit, std::vector<uint32_t>& bitarray ) {
  // find if bit is still in the range of already allocated trigger info if no allocate more
  const unsigned word_size_in_bits = sizeof(uint32_t)*8;
  // find a word to set the bit
  unsigned word  = bit/word_size_in_bits;
  unsigned shift = bit%word_size_in_bits;
  bitarray[word] |= 0x1 << shift;
}

bool MTCalibPeb::randomAccept(double rate) {
  bool decision = true;
  if (rate < 0.) {
    decision = false;
  } else if (rate > 1.) {
    decision = true;
  } else if ((double)std::rand()/double(RAND_MAX) > rate) {
    decision = false;
  }
  return decision;
}

unsigned int MTCalibPeb::randomCPUBurn(int burntime) {
  unsigned int randomBurnTime = 0;
  if (burntime <= 0) {
    randomBurnTime = -burntime;
  } else if (burntime > 0) {
    randomBurnTime = (unsigned int) (burntime * ((double)std::rand()/double(RAND_MAX))) ; 
  }
  return randomBurnTime;
}

void MTCalibPeb::streamTagRobDetHelper(std::string input_str, std::set<TriggerInfo::number_type>& output_set) {
  typedef boost::tokenizer<boost::char_separator<char> > _tokenizer_;
  boost::char_separator<char> _separator_(",[] ");
  _tokenizer_ tokens(input_str,_separator_);
  for (_tokenizer_::const_iterator tok_iter = tokens.begin();tok_iter != tokens.end();tok_iter++) {
    TriggerInfo::number_type tmp_int = strtoul((*tok_iter).c_str(),0,0);
    if (tmp_int != 0) output_set.insert(tmp_int);
  }
}

//=========================================================================
// output operators
//=========================================================================
std::string MTCalibPeb::printStreamTags(const std::vector<TriggerInfo::StreamTag>& rhs)
{
  std::ostringstream os;
  std::string prefix("\n  (");
  for (std::vector<TriggerInfo::StreamTag>::const_iterator it = rhs.begin(); it != rhs.end(); it++) {
    os << prefix << (*it).name() << "," << (*it).type() << "," << (*it).obeysLumiblock()
       << ",[Rob="  << (*it).robs().size() << ":" << (*it).robs()
       << "],[Det=" << (*it).dets().size() << ":" << (*it).dets()
       << "])";
    prefix=",\n  (";
  }
  return os.str();
}

std::ostream& operator<<(std::ostream& os, const std::set<TriggerInfo::number_type>& rhs)
{
  std::string prefix("0x");
  os << std::hex << std::setfill( '0' );
  for (std::set<uint32_t>::const_iterator it = rhs.begin(); it != rhs.end(); it++) {
    os << prefix << std::setw(6) << *it;
    prefix = ", 0x";
  }
  os << std::dec << std::setfill(' ');
  return os;
}

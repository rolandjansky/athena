/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "SourceCompAlg.h"
#include "RDBAccessSvc.h"
#include <sstream>
#include <fstream>

SourceCompAlg::SourceCompAlg(const std::string& name, ISvcLocator* pSvcLocator)
  : AthAlgorithm(name, pSvcLocator)
  , m_globalTag("")
{
  declareProperty("GlobalTag",m_globalTag);
}

SourceCompAlg::~SourceCompAlg()
{
}

StatusCode SourceCompAlg::initialize()
{
  msg(MSG::INFO) << " in initialize()" << endmsg;

  if(m_globalTag.empty()) {
    msg(MSG::FATAL) << "Global Tag is empty!" << endmsg;
    return StatusCode::FAILURE;
  }

  m_recoMaps.reserve(2);

  // Get RDBAccessSvc
  IRDBAccessSvc* rdbAccess = 0;
  StatusCode sc = service("RDBAccessSvc",rdbAccess);
  if(sc.isFailure()) {
    msg(MSG::FATAL) << "Unable to get RDBaccessSvc!" << endmsg;
    return StatusCode::FAILURE;
  }

  // Populate reco maps for the connection 0 and 1
  for(unsigned iConn=0; iConn<2; ++iConn) {
    NodeToRecordsetMap_ptr map(new NodeToRecordsetMap);

    std::ostringstream streamConnInd;
    streamConnInd << iConn;
    std::string connName = "Session" + streamConnInd.str();
    msg(MSG::INFO) << "Working with " << connName << endmsg;

    // Connect to the database
    if(!rdbAccess->connect(connName)) {
      msg(MSG::FATAL) << "Unable to get connection for " << connName << endmsg;
      return StatusCode::FAILURE;
    }

    // Check existence of the global tag in the database
    if(rdbAccess->getChildTag("ATLAS",m_globalTag,"ATLAS",connName)=="") {
      msg(MSG::FATAL) << "Unable to find " << m_globalTag << " in the connection " << connName << endmsg;
      return StatusCode::FAILURE;
    }

    // Get tag details, only needed for generation of tag cache
    RDBTagDetails atlasTagDetails = rdbAccess->getTagDetails(m_globalTag,connName);
    std::ostringstream tagDetailStream;
    tagDetailStream << atlasTagDetails << std::endl;
    if(msgLvl(MSG::DEBUG))
      msg(MSG::DEBUG) << "Global tag in connection " << connName << ": " << tagDetailStream.str() << endmsg;

    // Get the list of all leaf nodes
    RDBAccessSvc* rdbAccessConc = dynamic_cast<RDBAccessSvc*>(rdbAccess);
    if(rdbAccessConc) {
      std::vector<std::string> leafNodes;
      rdbAccessConc->getAllLeafNodes(leafNodes,connName);
      for(size_t ii=0; ii<leafNodes.size(); ++ii) {
	std::string nodeName = leafNodes[ii];
	int perc = ii*100/leafNodes.size();
	msg(MSG::INFO) << "\t ** Node: " << nodeName << "\t" << perc << "%" << endmsg;
	IRDBRecordset_ptr recPtr = rdbAccess->getRecordsetPtr(nodeName,m_globalTag,"ATLAS",connName);
	if(recPtr->size())
	  (*map)[nodeName] = recPtr;
      }
    }
    m_recoMaps.push_back(map);

    rdbAccess->disconnect(connName);
  }

  std::string logName("GeomDBDiff_"+m_globalTag+".log");
  std::fstream log;
  log.open(logName.c_str(),std::fstream::out);
  log << "*** Differences found for the tag " << m_globalTag << " ***" << std::endl << std::endl;

  // Compare recordsets in reco maps
  NodeToRecordsetMap_ptr map0 = m_recoMaps[0];
  NodeToRecordsetMap_ptr map1 = m_recoMaps[1];

  if(map0->size()!=map1->size()) 
    log << "Different number of leaf nodes in databases! " << map0->size() << " vs " << map1->size() << std::endl;
  
  log << std::endl << "Nodes existing in Session0 but missing in Session1:" << std::endl;
  NodeToRecordsetMap::const_iterator it = map0->begin();
  for(; it!=map0->end(); it++) {
    if(map1->find(it->first)==map1->end())
      log << "** " << it->first << std::endl;
  }
  
  log << std::endl << "Nodes existing in Session1 but missing in Session0:" << std::endl;
  it = map1->begin();
  for(; it!=map1->end(); it++) {
    if(map0->find(it->first)==map0->end())
      log << "** " << it->first << std::endl;
  }

  log << std::endl << "Differences for common nodes:" << std::endl;
  it = map0->begin();
  for(; it!=map0->end(); it++) {
    NodeToRecordsetMap::const_iterator it1 = map1->find(it->first);
    if(it1!=map1->end()) {
      IRDBRecordset_ptr recPtr0 = it->second;
      IRDBRecordset_ptr recPtr1 = it1->second;
      RDBRecordset* recConcrete0 = dynamic_cast<RDBRecordset*>(recPtr0.get());
      RDBRecordset* recConcrete1 = dynamic_cast<RDBRecordset*>(recPtr1.get());
      if(recConcrete0!=0
	 && recConcrete1!=0
	 && *recConcrete0!=*recConcrete1) {
	log <<std::endl << "** " << it->first << std::endl;
	recConcrete0->compare(*recConcrete1,log);
      }
    }
  }

  log.close();

  return StatusCode::SUCCESS;
}

StatusCode SourceCompAlg::execute()
{
  if(msgLvl(MSG::DEBUG))
    msg(MSG::DEBUG) <<" in execute()" <<endmsg;
  return StatusCode::SUCCESS;
}

StatusCode SourceCompAlg::finalize()
{
  if(msgLvl(MSG::DEBUG))
    msg(MSG::DEBUG) <<" in finalize()" <<endmsg;
  return StatusCode::SUCCESS;
}

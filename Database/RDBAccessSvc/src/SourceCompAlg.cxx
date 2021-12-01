/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#include "SourceCompAlg.h"
#include "RDBAccessSvc.h"
#include "CxxUtils/checker_macros.h"
#include "GaudiKernel/ServiceHandle.h"

typedef std::map<std::string,IRDBRecordset_ptr> NodeToRecordsetMap;

SourceCompAlg::SourceCompAlg(const std::string& name, ISvcLocator* pSvcLocator)
  : AthAlgorithm(name, pSvcLocator)
{
}

SourceCompAlg::~SourceCompAlg()
{
}

StatusCode SourceCompAlg::initialize()
{
  ATH_MSG_INFO(" in initialize()");

  // Get RDBAccessSvc
  ServiceHandle<IRDBAccessSvc> rdbAccessSvc("RDBAccessSvc",name());
  ATH_CHECK(rdbAccessSvc.retrieve());

  // Open connections to two databases
  for(const std::string& connName : m_connNames) {
    if(!rdbAccessSvc->connect(connName)) {
      ATH_MSG_FATAL("Failed to connect to " << connName);
      return StatusCode::FAILURE;
    }
  }

  RDBAccessSvc* rdbAccess = dynamic_cast<RDBAccessSvc*>(rdbAccessSvc.get());
  if(!rdbAccess) {
    ATH_MSG_FATAL("Unable to cast IRDBAccessSvc to RDBAccessSvc");
    return StatusCode::FAILURE;
  }

  // Open log file
  std::ofstream log;
  log.open("GeometryDBTagDiff.log",std::ofstream::out);

  std::vector<std::string> tagList;

  if(m_globalTag.value().empty()) {
    // Get the list of locked supported global tags
    tagList = getGlobalTags(rdbAccess,log);
  }
  else {
    tagList.push_back(m_globalTag.value());
  }

  if(!tagList.empty()) {
    // Loop over the tags and compare them
    compareGlobalTags(tagList,rdbAccess,log);
  }

  log.close();
  rdbAccess->disconnect(m_connNames[1]);
  rdbAccess->disconnect(m_connNames[0]);

  return StatusCode::SUCCESS;
}

StatusCode SourceCompAlg::execute()
{
  ATH_MSG_DEBUG(" in execute()");
  return StatusCode::SUCCESS;
}

StatusCode SourceCompAlg::finalize()
{
  ATH_MSG_DEBUG(" in finalize()");
  return StatusCode::SUCCESS;
}

std::vector<std::string> SourceCompAlg::getGlobalTags(RDBAccessSvc* rdbAccessSvc
						      , std::ofstream& log)
{
  std::vector<std::string> tagdiff, taglist1, taglist2;
  std::vector<char> leftright;
  taglist1 = rdbAccessSvc->getLockedSupportedTags(m_connNames[0]);
  taglist2 = rdbAccessSvc->getLockedSupportedTags(m_connNames[1]);
  tagdiff.resize(taglist1.size()+taglist2.size());
  auto it1 = taglist1.begin();
  auto it1last = taglist1.end();
  auto it2 = taglist2.begin();
  auto it2last = taglist2.end();
  auto itdiff = tagdiff.begin();

  while(true) {
    if(it1==it1last) {
      while(it2!=it2last) {
	leftright.push_back('>');
	*itdiff = *it2;
	++itdiff;
	++it2;
      }
      break;
    }
    if(it2==it2last) {
      while(it1!=it1last) {
	leftright.push_back('<');
	*itdiff = *it1;
	++itdiff;
	++it1;
      }
      break;
    }

    if (*it1<*it2) { 
      *itdiff=*it1; 
      ++itdiff; 
      ++it1; 
      leftright.push_back('<'); 
    }
    else if (*it2<*it1) { 
      *itdiff = *it2; 
      ++itdiff; 
      ++it2; 
      leftright.push_back('>'); 
    }
    else { 
      ++it1; 
      ++it2; 
    }
  }

  tagdiff.resize(itdiff-tagdiff.begin());
  if(!tagdiff.empty()) {
    log << "The databases contain different sets of supported locked global tags" << std::endl;
    auto leftrightIt = leftright.begin();
    for(auto tag : tagdiff ) {
      log << *leftrightIt << " " << tag << std::endl;
      ++leftrightIt;
    }
    tagdiff.clear();
    return tagdiff;
  }
  return taglist1;
}


void SourceCompAlg::compareGlobalTags(const std::vector<std::string>& globalTags
				      , RDBAccessSvc* rdbAccessSvc
				      , std::ofstream& log)
{
  ATH_MSG_INFO("compareGlobalTags()");
  for(const std::string& tag : globalTags) {
    ATH_MSG_INFO("Working on " << tag);
    std::vector<NodeToRecordsetMap> map;
    map.resize(2);
    size_t connInd(0);
    for(auto& mapel : map) {
      // Double-check the existence of the global tag in the database
      if(rdbAccessSvc->getChildTag("ATLAS",tag,"ATLAS",m_connNames[connInd])=="") {
	ATH_MSG_FATAL("Unable to find " << tag << " in the connection " << m_connNames[connInd]);
	return;
      }

      // Get tag details, only needed for generation of tag cache
      RDBTagDetails atlasTagDetails ATLAS_THREAD_SAFE;
      rdbAccessSvc->getTagDetails(atlasTagDetails,tag,m_connNames[connInd]);
      std::ostringstream tagDetailStream;
      tagDetailStream << atlasTagDetails << std::endl;
      ATH_MSG_DEBUG("Global tag in connection " << m_connNames[connInd] << ": " << tagDetailStream.str());

      // Get the list of all leaf nodes
      RDBAccessSvc* rdbAccessConc = dynamic_cast<RDBAccessSvc*>(rdbAccessSvc);
      if(rdbAccessConc) {
	std::vector<std::string> leafNodes;
	rdbAccessConc->getAllLeafNodes(leafNodes,m_connNames[connInd]);
	for(size_t ii=0; ii<leafNodes.size(); ++ii) {
	  std::string nodeName = leafNodes[ii];
	  int perc = ii*100/leafNodes.size();
	  ATH_MSG_DEBUG("\t ** Node: " << nodeName << "\t" << perc << "%");
	  std::string childTag = rdbAccessSvc->getChildTag(nodeName,tag,"ATLAS",m_connNames[connInd],true);
	  if(!childTag.empty()) {
	    IRDBRecordset_ptr recPtr = rdbAccessSvc->getRecordsetPtr(nodeName,childTag,"",m_connNames[connInd]);
	    if(recPtr->size()) {
	      mapel[nodeName] = recPtr;
	    }
	  }
	}
      }
      ++connInd;
    }

    if(map[0].size()!=map[1].size()) {
      log << std::endl << "TAG: " << tag << std::endl
	  << "Different number of leaf nodes in databases! " << map[0].size() << " vs " << map[1].size() << std::endl;
    }
  
    bool difFound{false};
    NodeToRecordsetMap::const_iterator it = map[0].begin();
    for(; it!=map[0].end(); ++it) {
      if(map[1].find(it->first)==map[1].end()) {
	if(!difFound) {
	  difFound = true;
	  log << std::endl << "TAG: " << tag << std::endl
	      << "Nodes existing in Session0 but missing in Session1:" << std::endl;
	}
	log << "** " << it->first << std::endl;
      }
    }
  
    difFound = false;
    it = map[1].begin();
    for(; it!=map[1].end(); ++it) {
      if(map[0].find(it->first)==map[0].end()) {
	if(!difFound) {
          difFound = true;
	  log << std::endl << "TAG: " << tag << std::endl
	      << "Nodes existing in Session1 but missing in Session0:" << std::endl;
	}
	log << "** " << it->first << std::endl;
      }
    }

    difFound = false;
    it = map[0].begin();
    for(; it!=map[0].end(); ++it) {
      NodeToRecordsetMap::const_iterator it1 = map[1].find(it->first);
      if(it1!=map[1].end()) {
	IRDBRecordset_ptr recPtr0 = it->second;
	IRDBRecordset_ptr recPtr1 = it1->second;
	RDBRecordset* recConcrete0 = dynamic_cast<RDBRecordset*>(recPtr0.get());
	RDBRecordset* recConcrete1 = dynamic_cast<RDBRecordset*>(recPtr1.get());
	if(recConcrete0!=0
	   && recConcrete1!=0
	   && *recConcrete0!=*recConcrete1) {
	  if(!difFound) {
	    difFound = true;
	    log << std::endl << "TAG: " << tag << std::endl
		<< "Differences for common nodes:" << std::endl;
	  }
	  log <<std::endl << "** " << it->first << std::endl;
	  recConcrete0->compare(*recConcrete1,log);
	}
      }
    } 
  }
}

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**
 * @file RDBAccessSvc.cxx
 *
 * @brief Implementation of RDBAccessSvc class
 *
 * @author Vakho Tsulaia <Vakhtang.Tsulaia@cern.ch>
 *
 * $Id: RDBAccessSvc.cxx,v 1.41 2008-10-13 12:24:09 tsulaia Exp $
 */

#include "RDBAccessSvc.h"
#include "RDBRecordset.h"
#include "RDBVersionAccessor.h"
#include "RDBQuery.h"

#include "RelationalAccess/RelationalServiceException.h"
#include "RelationalAccess/SchemaException.h"
#include "RelationalAccess/SessionException.h"
#include "RelationalAccess/IRelationalService.h"
#include "RelationalAccess/IRelationalDomain.h"
#include "RelationalAccess/ISessionProxy.h"
#include "RelationalAccess/ITransaction.h"
#include "RelationalAccess/SchemaException.h"
#include "RelationalAccess/ConnectionService.h"
#include "RelationalAccess/IConnectionService.h"
#include "RelationalAccess/AccessMode.h"
#include "RelationalAccess/ICursor.h"
#include "RelationalAccess/ITable.h"
#include "RelationalAccess/ISchema.h"
#include "RelationalAccess/IQuery.h"

#include "CoralBase/Exception.h"
#include "RelationalAccess/AuthenticationServiceException.h"

#include "PoolSvc/IPoolSvc.h"

RDBAccessSvc::RDBAccessSvc(const std::string& name, ISvcLocator* svc):
  AthService(name,svc),
  m_hostName(""),
  m_empty(this)
{
  declareProperty("HostName",m_hostName);
}

RDBAccessSvc::~RDBAccessSvc()
{
}

void RDBAccessSvc::eraseRecordset(const RDBRecordset* recordset)
{
  RecordsetsByConn::iterator first_map = m_recordsets.begin();
  RecordsetsByConn::iterator last_map = m_recordsets.end();

  // Navigate all recordset maps
  for(; first_map!=last_map; first_map++)
  {
    RecordsetMap* current_map = first_map->second;

    RecordsetMap::iterator first_rec = current_map->begin();
    RecordsetMap::iterator last_rec = current_map->end();

    // Navigate all recordsets within current map
    for(; first_rec!=last_rec; first_rec++)
    {
      if(first_rec->second==recordset)
      {
	current_map->erase(first_rec);
	return;
      }
    }
  }
}

bool RDBAccessSvc::connect(const std::string& connName)
{
  // Check if it is the first attempt to open a connection connName
  if(m_sessions.find(connName)==m_sessions.end())
  {
    if(msgLvl(MSG::DEBUG))
      msg(MSG::DEBUG) << " Trying to open the connection " << connName << " for the first time" << endreq;
    // Add initial entries for this connection to three maps
    // 1. Sessions
    // 2. open connections
    // 3. Recordset by connection
    m_sessions[connName] = 0;
    m_openConnections[connName] = 0;
    m_recordsets[connName] = new RecordsetMap();
    m_recordsetptrs[connName] = new RecordsetPtrMap();
  }

  // Use existing Connection Proxy if available
  if(m_openConnections[connName]++)
  {
    if(msgLvl(MSG::DEBUG))
      msg(MSG::DEBUG) << " Connection " << connName << " already open, sessions = " 
		      << m_openConnections[connName] << endreq;
    return true;
  }

  // Request new connection proxy from the Connection Service
  coral::ConnectionService conSvcH;
  coral::ISessionProxy *proxy = 0;
  try {
    proxy = conSvcH.connect(connName,coral::ReadOnly);
    if(msgLvl(MSG::DEBUG))
      msg(MSG::DEBUG) << " Proxy for connection "  << connName << " obtained" << endreq;
  }
  catch(std::exception& e) {
    msg(MSG::ERROR) <<"Exception caught: " << e.what() << endreq;
    m_openConnections[connName]--;
    return false;
  }

  m_sessions[connName] = proxy;

  return true;
}

bool RDBAccessSvc::disconnect(const std::string& connName)
{
  if(m_openConnections.find(connName)==m_openConnections.end()) {
    msg(MSG::ERROR) << "Wrong name for the connection: " << connName << endreq;
    return false;
  }

  if(m_openConnections[connName]>0) {
    m_openConnections[connName]--;
    if(msgLvl(MSG::DEBUG))
      msg(MSG::DEBUG) << "Connection " << connName
		      << " Sessions = " << m_openConnections[connName] << endreq;

    if(m_openConnections[connName]==0) {

      if(m_sessions.find(connName)!=m_sessions.end()) {
	delete m_sessions[connName];
	m_sessions[connName] = 0;
      }

      if(msgLvl(MSG::DEBUG))
	msg(MSG::DEBUG) << connName << " Disconnected!" << endreq;

      // clean up all shared recordsets for this connection
      if(m_recordsetptrs.find(connName)!=m_recordsetptrs.end())
	m_recordsetptrs[connName]->clear();
    }
  }
  return true;
}

bool RDBAccessSvc::shutdown(const std::string& connName)
{
  if(connName=="*Everything*") {
    for(const auto& ii : m_openConnections) {
      if(ii.second != 0) {
	msg(MSG::WARNING) << "Close everything: Connection: " << ii.first 
			  << " with reference count = " << ii.second << " will be closed." <<endreq;
	return shutdown_connection(ii.first);
      }
    }
    return true;
  }
  
  return shutdown_connection(connName);
}

bool RDBAccessSvc::shutdown_connection(const std::string& connName)
{
  if(m_openConnections.find(connName)==m_openConnections.end())
  {
    msg(MSG::ERROR) << "Wrong name for the connection: " << connName << endreq;
    return false;
  }

  m_openConnections[connName]=0;
  if(msgLvl(MSG::DEBUG))
    msg(MSG::DEBUG) << "Connection " << connName 
		    << " Sessions = " << m_openConnections[connName] << endreq;
  if(m_sessions.find(connName)!=m_sessions.end()) {
    delete m_sessions[connName];
    m_sessions[connName] = 0;
  }
  if(msgLvl(MSG::DEBUG))
    msg(MSG::DEBUG) << connName << " Disconnected!" << endreq;

  // clean up all shared recordsets for this connection
  if(m_recordsetptrs.find(connName)!=m_recordsetptrs.end())
    m_recordsetptrs[connName]->clear();

  return true;
}

const IRDBRecordset* RDBAccessSvc::getRecordset(const std::string& node,
						const std::string& tag,
						const std::string& tag2node,
						const std::string& connName)
{
  std::string key = node + "::" + tag;
  if(tag2node!="")
    key += ("::" + tag2node);

  RecordsetsByConn::const_iterator it_maps = m_recordsets.find(connName);
  if(it_maps==m_recordsets.end()) {
    if(msgLvl(MSG::DEBUG))
      msg(MSG::DEBUG) << "Wrong name for connection " << connName
		      << ". Unable to find recordset map. Returning empty recordset" << endreq;

    return &m_empty;
  }

  RecordsetMap* recordsets = it_maps->second;
  RecordsetMap::const_iterator it = recordsets->find(key);
  if(it != recordsets->end()) {
    const RDBRecordset* rec = it->second;
    return rec;
  }

  RDBRecordset* rec = new RDBRecordset(this);

  SessionMap::const_iterator it_sessions = m_sessions.find(connName);

  if(it_sessions != m_sessions.end()) {
    coral::ISessionProxy* session = it_sessions->second;

    if(session) {
      try {
	// Start new readonly transaction
	session->transaction().start(true);

	// Check lookup table first
	bool lookupFound = false;
	std::string lookupMapKey = tag + "::" + connName;
	GlobalTagLookupMap::const_iterator lookupmap = m_globalTagLookup.find(lookupMapKey);
	if(lookupmap!=m_globalTagLookup.end()) {
	  lookupFound = true;
	  TagNameIdByNode::const_iterator childtagdet = lookupmap->second->find(node);
	  if(childtagdet!=lookupmap->second->end()) {
	    rec->getData(session,node,childtagdet->second.first,childtagdet->second.second);
	  }
	  else {
	    if(msgLvl(MSG::DEBUG))
	      msg(MSG::DEBUG) << "Unable to find tag for the node " << node
			      << " in the cache of global tag " << tag << ". Returning empty recordset" << endreq;
	  }
	}

	if(!lookupFound) {
	  if(tag2node!="") {
	    RDBVersionAccessor versionAccessor(node,tag2node,tag,session,msg());
	    versionAccessor.getChildTagData();
	    rec->getData(session,versionAccessor.getNodeName(),versionAccessor.getTagName(),versionAccessor.getTagID());
	  }
	  else {
	    RDBVersionAccessor versionAccessor(node,node,tag,session,msg());
	    versionAccessor.getChildTagData();
	    rec->getData(session,versionAccessor.getNodeName(),versionAccessor.getTagName(),versionAccessor.getTagID());
	  }
	}
	
	// Finish the transaction
	session->transaction().commit();
      }
      catch(coral::SchemaException& se) {
	msg(MSG::ERROR) << "Schema Exception : " << se.what() << endreq;
      }
      catch(std::exception& e) {
	msg(MSG::ERROR) << e.what() << endreq;
      }
      catch(...) {
	msg(MSG::ERROR) << "Exception caught(...)" << endreq;
      }
    }
    else {
      msg(MSG::ERROR) << "Connection " << connName << " is not open." 
		      << " Returning empty recordset" << endreq;
    }
  }
  else
    msg(MSG::ERROR) << "Wrong name for connection " << connName
		    << ". Unable to find session. Returning empty recordset" << endreq;

  (*recordsets)[key] = rec;
  return rec;
}

IRDBRecordset_ptr RDBAccessSvc::getRecordsetPtr(const std::string& node,
						const std::string& tag,
						const std::string& tag2node,
						const std::string& connName)
{
  std::string key = node + "::" + tag;
  if(tag2node!="")
    key += ("::" + tag2node);

  if(msgLvl(MSG::DEBUG))
    msg(MSG::DEBUG) << "Getting RecordsetPtr with key " << key << endreq;

  RecordsetPtrsByConn::const_iterator it_maps = m_recordsetptrs.find(connName);
  if(it_maps==m_recordsetptrs.end()) {
    if(msgLvl(MSG::DEBUG))
      msg(MSG::DEBUG) << "Wrong name for connection " << connName
		      << ". Unable to find recordset map. Returning empty recordset" << endreq;

    return IRDBRecordset_ptr(new RDBRecordset(this));
  }

  RecordsetPtrMap* recordsets = it_maps->second;
  RecordsetPtrMap::const_iterator it = recordsets->find(key);
  if(it != recordsets->end()) {
    if(msgLvl(MSG::DEBUG))
      msg(MSG::DEBUG) << "Reusing existing recordset" << endreq;
    return it->second;
  }

  IRDBRecordset_ptr rec(new RDBRecordset(this));

  SessionMap::const_iterator it_sessions = m_sessions.find(connName);

  if(it_sessions != m_sessions.end()) {
    coral::ISessionProxy* session = it_sessions->second;

    if(session) {
      try {
	// Start new readonly transaction
	session->transaction().start(true);

	// Check lookup table first
	bool lookupFound = false;
	std::string lookupMapKey = tag + "::" + connName;
	GlobalTagLookupMap::const_iterator lookupmap = m_globalTagLookup.find(lookupMapKey);
	if(lookupmap!=m_globalTagLookup.end()) {
	  lookupFound = true;
	  TagNameIdByNode::const_iterator childtagdet = lookupmap->second->find(node);
	  RDBRecordset* recConcrete = dynamic_cast<RDBRecordset*>(rec.get());
	  if(recConcrete) {
	    if(childtagdet!=lookupmap->second->end()) {
	      recConcrete->getData(session,node,childtagdet->second.first,childtagdet->second.second);
	    }
	    else {
	      recConcrete->setNodeName(node);
	      if(msgLvl(MSG::DEBUG))
		msg(MSG::DEBUG) << "Unable to find tag for the node " << node
				<< " in the cache of global tag " << tag << ". Returning empty recordset" << endreq;
	    }
	  }
	}

	if(!lookupFound) {
	  if(tag2node!="") {
	    RDBVersionAccessor versionAccessor(node,tag2node,tag,session,msg());
	    versionAccessor.getChildTagData();
	    RDBRecordset* recConcrete = dynamic_cast<RDBRecordset*>(rec.get());
	    if(recConcrete)
	      recConcrete->getData(session,versionAccessor.getNodeName(),versionAccessor.getTagName(),versionAccessor.getTagID());
	  }
	  else {
	    RDBVersionAccessor versionAccessor(node,node,tag,session,msg());
	    versionAccessor.getChildTagData();
	    RDBRecordset* recConcrete = dynamic_cast<RDBRecordset*>(rec.get());
	    if(recConcrete)
	      recConcrete->getData(session,versionAccessor.getNodeName(),versionAccessor.getTagName(),versionAccessor.getTagID());
	  }
	}
	
	// Finish the transaction
	session->transaction().commit();
      }
      catch(coral::SchemaException& se) {
	msg(MSG::ERROR) << "Schema Exception : " << se.what() << endreq;
      }
      catch(std::exception& e) {
	msg(MSG::ERROR) << e.what() << endreq;
      }
      catch(...) {
	msg(MSG::ERROR) << "Exception caught(...)" << endreq;
      }
    }
    else {
      msg(MSG::ERROR) << "Connection " << connName << " is not open." 
	  << " Returning empty recordset" << endreq;
    }
  }
  else
    msg(MSG::ERROR) << "Wrong name for connection " << connName
	<< ". Unable to find session. Returning empty recordset" << endreq;

  (*recordsets)[key] = rec;
  return rec;
}

IRDBQuery* RDBAccessSvc::getQuery(const std::string& node,
				  const std::string& tag,
				  const std::string& tag2node,
				  const std::string& connName)
{
  IRDBQuery* query(0);

  SessionMap::const_iterator it_sessions = m_sessions.find(connName);
  if(it_sessions != m_sessions.end()) {
    coral::ISessionProxy* session = it_sessions->second;

    if(session) {
      try {
	// Check lookup table first
	std::string childTagId("");
	bool lookupFound = false;
	std::string lookupMapKey = tag + "::" + connName;
	GlobalTagLookupMap::const_iterator lookupmap = m_globalTagLookup.find(lookupMapKey);
	if(lookupmap!=m_globalTagLookup.end()) {
	  lookupFound = true;
	  TagNameIdByNode::const_iterator childtagdet = lookupmap->second->find(node);
	  if(childtagdet!=lookupmap->second->end()) {
	    childTagId = childtagdet->second.second;
	  }
	}

	if(!lookupFound) {
	  // Start new readonly transaction
	  session->transaction().start(true);

	  RDBVersionAccessor_ptr versionAccessor;
	  if(tag2node.empty())
	    versionAccessor = RDBVersionAccessor_ptr(new RDBVersionAccessor(node,node,tag,session,msg()));
	  else
	    versionAccessor = RDBVersionAccessor_ptr(new RDBVersionAccessor(node,tag2node,tag,session,msg()));

	  versionAccessor->getChildTagData();
	  childTagId = versionAccessor->getTagID();

	  // Finish the transaction
	  session->transaction().commit();
	}

	if(childTagId.empty()) {
	  if(msgLvl(MSG::DEBUG))
	    msg(MSG::DEBUG) << "Could not get the tag for " << node
			    << " node. Returning 0 pointer to IRDBQuery" << endreq;
	}
	else {
	  query = new RDBQuery(this,session,node,childTagId);
	}

      }
      catch(coral::SchemaException& se) {
	msg(MSG::ERROR) << "Schema Exception : " << se.what() << endreq;
      }
      catch(std::exception& e) {
	msg(MSG::ERROR) << e.what() << endreq;
      }
      catch(...) {
	msg(MSG::ERROR) << "Exception caught(...)" << endreq;
      }
    }
    else {
      msg(MSG::ERROR) << "Connection " << connName << " is not open." << endreq; 
    }
  }
  else
    msg(MSG::ERROR) << "Wrong name for connection " << connName
	<< ". Unable to find session." << endreq;

  return query;
}

std::string RDBAccessSvc::getChildTag(const std::string& childNode,
				      const std::string& parentTag,
				      const std::string& parentNode,
				      bool fetchData,
				      const std::string& connName)
{
  // The fetchData argument is not much of use and also
  // after introduction of smart pointers to recordsets this parameter can lead to some
  // extra data retrieval.
  //
  // Based on all above I'm going to make fetchData obsolete and keep it around only for backwards compatibility

  if(msgLvl(MSG::DEBUG))
    msg(MSG::DEBUG) << "getChildTag for " << childNode << " " << parentTag << " " << parentNode << " " << (fetchData?"Fetch":"Nofetch") << endreq;

  SessionMap::const_iterator it_sessions = m_sessions.find(connName);

  if(it_sessions != m_sessions.end()) {
    coral::ISessionProxy* session = it_sessions->second;

    if(session) {
      try {
	// Check lookup table first
	std::string lookupMapKey = parentTag + "::" + connName;
	GlobalTagLookupMap::const_iterator lookupmap = m_globalTagLookup.find(lookupMapKey);
	if(lookupmap!=m_globalTagLookup.end()) {
	  TagNameIdByNode::const_iterator childtagdet = lookupmap->second->find(childNode);
	  if(childtagdet!=lookupmap->second->end())
	    return childtagdet->second.first;
	  else
	    return std::string("");
	}

	// We don't have lookup table for given parent tag. Go into slow mode through Version Accessor
	// Start new readonly transaction
	session->transaction().start(true);

	RDBVersionAccessor versionAccessor(childNode,parentNode,parentTag,session,msg());
	versionAccessor.getChildTagData();
	      
	// Finish the transaction
	session->transaction().commit(); 

	return versionAccessor.getTagName();
      }
      catch(coral::SchemaException& se) {
	msg(MSG::ERROR) << "Schema Exception : " << se.what() << endreq;
      }
      catch(std::exception& e) {
	msg(MSG::ERROR) << e.what() << endreq;
      }
      catch(...) {
	msg(MSG::ERROR) << "Exception caught(...)" << endreq;
      }
    }
    else {
      msg(MSG::ERROR) << "Connection " << connName << " is not open." << endreq;
    }
  }
  else
    msg(MSG::ERROR) << "Wrong name for connection " << connName << endreq;

  return std::string("");
}
 
RDBTagDetails RDBAccessSvc::getTagDetails(const std::string& tag,
					  const std::string& connName)
{
  RDBTagDetails tagDetails;

  SessionMap::const_iterator it_sessions = m_sessions.find(connName);

  if(it_sessions != m_sessions.end()) {
    coral::ISessionProxy* session = it_sessions->second;

    if(session) {
      try {
	// Start new readonly transaction
	session->transaction().start(true);

	coral::ITable& tableTag2Node = session->nominalSchema().tableHandle("HVS_TAG2NODE");
	coral::IQuery *queryTag2Node = tableTag2Node.newQuery();
	queryTag2Node->addToOutputList("LOCKED");
	queryTag2Node->addToOutputList("SUPPORTED");
	queryTag2Node->setMemoryCacheSize(1);
	coral::AttributeList bindsTag2Node;
	bindsTag2Node.extend<std::string>("tagN");
	queryTag2Node->setCondition("TAG_NAME=:tagN", bindsTag2Node);
	bindsTag2Node[0].data<std::string>() = tag;

	coral::ICursor& cursorTag2Node = queryTag2Node->execute();
	if(cursorTag2Node.next()) {
	  tagDetails = cursorTag2Node.currentRow();
	}

	delete queryTag2Node;

	// Build lookup table for the given global tag if has not been built yet
	std::string lookupMapKey = tag + "::" + connName;
	if(m_globalTagLookup.find(lookupMapKey)==m_globalTagLookup.end()) {
	  // Get tag contents from the database
	  TagNameIdByNode* lookup = new TagNameIdByNode();

	  coral::ITable& tableRootTag2Child = session->nominalSchema().tableHandle("HVS_TAGCACHE");
	  coral::IQuery* queryRootTag2Child = tableRootTag2Child.newQuery();
	  queryRootTag2Child->addToOutputList("CHILDNODE");
	  queryRootTag2Child->addToOutputList("CHILDTAG");
	  queryRootTag2Child->addToOutputList("CHILDTAGID");
	  queryRootTag2Child->setMemoryCacheSize(1);
	  coral::AttributeList bindsRootTag2Child;
	  bindsRootTag2Child.extend<std::string>("tagN");
	  queryRootTag2Child->setCondition("ROOTTAG=:tagN", bindsRootTag2Child);
	  bindsRootTag2Child[0].data<std::string>() = tag;
	  queryRootTag2Child->addToOrderList("CHILDNODE");

	  coral::ICursor& cursorRootTag2Child = queryRootTag2Child->execute();
	  while(cursorRootTag2Child.next()) {
	    const coral::AttributeList& row = cursorRootTag2Child.currentRow();

	    // turn TAG_ID into srting
	    std::stringstream tagIdStream;
	    tagIdStream << row["CHILDTAGID"].data<long long>();
	    (*lookup)[row["CHILDNODE"].data<std::string>()]=std::make_pair(row["CHILDTAG"].data<std::string>(),tagIdStream.str());
	  }

	  delete queryRootTag2Child;

	  if(lookup->size()>0)
	    m_globalTagLookup[lookupMapKey]=lookup;
	  else
	    delete lookup;
	}
      }
      catch(coral::SchemaException& se) {
	msg(MSG::INFO) << "Schema Exception : " << se.what() << endreq;
      }
      catch(std::exception& e) {
	msg(MSG::ERROR) << e.what() << endreq;
      }
      catch(...) {
	msg(MSG::ERROR) << "Exception caught(...)" << endreq;
      }

      // Finish the transaction
      if(session->transaction().isActive())
	session->transaction().commit();
    }
    else {
      msg(MSG::ERROR) << "Connection " << connName << " is not open." << endreq;
    }
  }
  else {
    msg(MSG::ERROR) << "Wrong name for connection " << connName << endreq;
  }

  return tagDetails;
}

void RDBAccessSvc::getAllLeafNodes(std::vector<std::string>& list,
				   const std::string& connName)
{
  list.clear();
  SessionMap::const_iterator it_sessions = m_sessions.find(connName);

  if(it_sessions != m_sessions.end()) {
    coral::ISessionProxy* session = it_sessions->second;

    if(session) {
      try {
	// Start new readonly transaction
	session->transaction().start(true);

	coral::ITable& tableNode = session->nominalSchema().tableHandle("HVS_NODE");
	coral::IQuery *queryNode = tableNode.newQuery();
	queryNode->addToOutputList("NODE_NAME");
	queryNode->setMemoryCacheSize(1);
//	coral::AttributeList bindsNode;
//	bindsNode.extend<bool>("branchF");
	queryNode->setCondition("BRANCH_FLAG=0", coral::AttributeList());
	queryNode->addToOrderList("NODE_NAME");
//	bindsNode[0].data<bool>() = false;

	coral::ICursor& cursorNode = queryNode->execute();
	while(cursorNode.next()) {
	  list.push_back(cursorNode.currentRow()["NODE_NAME"].data<std::string>());
	}

	delete queryNode;
      }
      catch(coral::SchemaException& se) {
	msg(MSG::INFO) << "Schema Exception : " << se.what() << endreq;
      }
      catch(std::exception& e) {
	msg(MSG::ERROR) << e.what() << endreq;
      }
      catch(...) {
	msg(MSG::ERROR) << "Exception caught(...)" << endreq;
      }

      // Finish the transaction
      if(session->transaction().isActive())
	session->transaction().commit();
    }
    else {
      msg(MSG::ERROR) << "Connection " << connName << " is not open." << endreq;
    }
  }
  else {
    msg(MSG::ERROR) << "Wrong name for connection " << connName << endreq;
  }

}


StatusCode RDBAccessSvc::initialize()
{
  // Get IPoolSvc
  IPoolSvc* poolSvc;
  StatusCode result = service("PoolSvc", poolSvc);
  if(result.isFailure())
  {
    msg(MSG::ERROR) << "Could not find IPoolSvc" << endreq;
    return result;
  }

  if(msgLvl(MSG::DEBUG))
    msg(MSG::DEBUG) << "Successfully initialized" << endreq;

  return result;
}

StatusCode RDBAccessSvc::finalize()
{
  // Clean up all remaining recordsets

  // Deal with pointers to recordsets
  RecordsetsByConn::iterator first_map = m_recordsets.begin();
  RecordsetsByConn::iterator last_map = m_recordsets.end();

  // Navigate all recordset maps
  for(; first_map!=last_map; first_map++)
  {
    RecordsetMap* current_map = first_map->second;

    RecordsetMap::iterator first_rec = current_map->begin();
    RecordsetMap::iterator last_rec = current_map->end();

    for(; first_rec!=last_rec; first_rec++)
      delete first_rec->second;

    delete current_map;
  }

  // Deal with shared pointers
  RecordsetPtrsByConn::iterator first_ptr_map = m_recordsetptrs.begin();
  RecordsetPtrsByConn::iterator last_ptr_map = m_recordsetptrs.end();

  for(;first_ptr_map!=last_ptr_map;++first_ptr_map)
    delete first_ptr_map->second;

  m_recordsetptrs.clear();

  // Clear global tag lookup table
  GlobalTagLookupMap::iterator itbegin = m_globalTagLookup.begin();
  GlobalTagLookupMap::iterator itend = m_globalTagLookup.end();
  while(itbegin!=itend) {
    delete itbegin->second;
    itbegin++;
  }
  m_globalTagLookup.clear();

  // Shut down all connections

  SessionMap::iterator first_con = m_sessions.begin();
  SessionMap::iterator last_con = m_sessions.end();

  for(;first_con!=last_con; first_con++)
    shutdown(first_con->first);

  return StatusCode::SUCCESS;
}

StatusCode RDBAccessSvc::queryInterface(const InterfaceID& riid, void** ppvInterface)
{
  if (IID_IRDBAccessSvc == riid)
    *ppvInterface = (IRDBAccessSvc*)this;
  else 
    return AthService::queryInterface(riid, ppvInterface);

  addRef();
  return StatusCode::SUCCESS;
}


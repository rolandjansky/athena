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

#include "GaudiKernel/ServiceHandle.h"
#include "PoolSvc/IPoolSvc.h"

#include <thread>

RDBAccessSvc::RDBAccessSvc(const std::string& name, ISvcLocator* svc):
  AthService(name,svc)
{
}

RDBAccessSvc::~RDBAccessSvc()
{
}

bool RDBAccessSvc::connect(const std::string& connName)
{
  // Check if it is the first attempt to open a connection connName
  if(m_sessions.find(connName)==m_sessions.end()) {
    ATH_MSG_DEBUG(" Trying to open the connection " << connName << " for the first time");
    // Add initial entries for this connection to three maps
    // 1. Sessions
    // 2. open connections
    // 3. Recordset by connection
    m_sessions[connName] = 0;
    m_openConnections[connName] = 0;
    m_recordsetptrs[connName] = new RecordsetPtrMap();
  }

  // Use existing Connection Proxy if available
  if(m_openConnections[connName]++) {
    ATH_MSG_DEBUG(" Connection " << connName << " already open, sessions = " << m_openConnections[connName]);
    return true;
  }

  // Request new connection proxy from the Connection Service
  coral::ConnectionService conSvcH;
  coral::ISessionProxy *proxy = 0;
  try {
    proxy = conSvcH.connect(connName,coral::ReadOnly);
    ATH_MSG_DEBUG("Proxy for connection "  << connName << " obtained");
  }
  catch(std::exception& e) {
    ATH_MSG_ERROR("Exception caught: " << e.what());
    m_openConnections[connName]--;
    return false;
  }

  m_sessions[connName] = proxy;

  return true;
}

bool RDBAccessSvc::disconnect(const std::string& connName)
{
  if(m_openConnections.find(connName)==m_openConnections.end()) {
    ATH_MSG_ERROR("Wrong name for the connection: " << connName);
    return false;
  }

  if(m_openConnections[connName]>0) {
    m_openConnections[connName]--;
    
    ATH_MSG_DEBUG("Connection " << connName << " Sessions = " << m_openConnections[connName]);

    if(m_openConnections[connName]==0) {
      if(m_sessions.find(connName)!=m_sessions.end()) {
	delete m_sessions[connName];
	m_sessions[connName] = 0;
      }
      
      ATH_MSG_DEBUG(connName << " Disconnected!");

      // clean up all shared recordsets for this connection
      if(m_recordsetptrs.find(connName)!=m_recordsetptrs.end()) {
	m_recordsetptrs[connName]->clear();
      }
    }
  }
  return true;
}

bool RDBAccessSvc::shutdown(const std::string& connName)
{
  if(connName=="*Everything*") {
    for(const auto& ii : m_openConnections) {
      if(ii.second != 0) {
	ATH_MSG_WARNING("Close everything: Connection: " << ii.first << " with reference count = " << ii.second << " will be closed.");
	return shutdown_connection(ii.first);
      }
    }
    return true;
  }
  
  return shutdown_connection(connName);
}

bool RDBAccessSvc::shutdown_connection(const std::string& connName)
{
  if(m_openConnections.find(connName)==m_openConnections.end()) {
    ATH_MSG_ERROR("Wrong name for the connection: " << connName);
    return false;
  }

  m_openConnections[connName]=0;
  
  ATH_MSG_DEBUG("Connection " << connName << " Sessions = " << m_openConnections[connName]);
  if(m_sessions.find(connName)!=m_sessions.end()) {
    delete m_sessions[connName];
    m_sessions[connName] = 0;
  }
  
  ATH_MSG_DEBUG(connName << " Disconnected!");

  // clean up all shared recordsets for this connection
  if(m_recordsetptrs.find(connName)!=m_recordsetptrs.end()) {
    m_recordsetptrs[connName]->clear();
  }

  return true;
}

IRDBRecordset_ptr RDBAccessSvc::getRecordsetPtr(const std::string& node,
						const std::string& tag,
						const std::string& tag2node,
						const std::string& connName)
{
  std::string key = node + "::" + tag;
  if(tag2node!="")
    key += ("::" + tag2node);

  ATH_MSG_DEBUG("Getting RecordsetPtr with key " << key);

  std::lock_guard<std::mutex> guard(m_mutex);
  if(!connect(connName)) {
    ATH_MSG_ERROR("Unable to open connection " << connName << ". Returning empty recordset");
    return IRDBRecordset_ptr(new RDBRecordset(this));
  }

  RecordsetPtrMap* recordsets = m_recordsetptrs[connName];
  RecordsetPtrMap::const_iterator it = recordsets->find(key);
  if(it != recordsets->end()) {
    ATH_MSG_DEBUG("Reusing existing recordset");
    disconnect(connName);
    return it->second;
  }

  RDBRecordset* recConcrete = new RDBRecordset(this);
  IRDBRecordset_ptr rec(recConcrete);
  coral::ISessionProxy* session = m_sessions[connName];

  try {
    // Start new readonly transaction
    session->transaction().start(true);

    // Check lookup table first
    std::string lookupMapKey = tag + "::" + connName;
    GlobalTagLookupMap::const_iterator lookupmap = m_globalTagLookup.find(lookupMapKey);
    if(lookupmap!=m_globalTagLookup.end()) {
      TagNameIdByNode::const_iterator childtagdet = lookupmap->second->find(node);
      if(childtagdet!=lookupmap->second->end()) {
	recConcrete->getData(session,node,childtagdet->second.first,childtagdet->second.second);
      }
      else {
	recConcrete->setNodeName(node);
	ATH_MSG_DEBUG("Unable to find tag for the node " << node << " in the cache of global tag " << tag << ". Returning empty recordset");
      }
    }
    else {
      RDBVersionAccessor versionAccessor(node,(tag2node.empty()?node:tag2node),tag,session,msg());
      versionAccessor.getChildTagData();
      recConcrete->getData(session,versionAccessor.getNodeName(),versionAccessor.getTagName(),versionAccessor.getTagID());
    }
	
    // Finish the transaction
    session->transaction().commit();
  }
  catch(coral::SchemaException& se) {
    ATH_MSG_ERROR("Schema Exception : " << se.what());
  }
  catch(std::exception& e) {
    ATH_MSG_ERROR(e.what());
  }
  catch(...) {
    ATH_MSG_ERROR("Exception caught(...)");
  }

  (*recordsets)[key] = rec;
  disconnect(connName);
  return rec;
}

std::unique_ptr<IRDBQuery> RDBAccessSvc::getQuery(const std::string& node,
						  const std::string& tag,
						  const std::string& tag2node,
						  const std::string& connName)
{
  ATH_MSG_DEBUG("getQuery (" << node << "," << tag << "," << tag2node << "," << connName << ")");
  std::lock_guard<std::mutex> guard(m_mutex);

  std::unique_ptr<IRDBQuery> query;

  if(!connect(connName)) {
    ATH_MSG_ERROR("Unable to open connection " << connName << ". Returning nullptr to IRDBQuery");
    return query;
  }

  coral::ISessionProxy* session = m_sessions[connName];

  try {
    // Check lookup table first
    std::string childTagId("");
    std::string lookupMapKey = tag + "::" + connName;
    GlobalTagLookupMap::const_iterator lookupmap = m_globalTagLookup.find(lookupMapKey);
    if(lookupmap!=m_globalTagLookup.end()) {
      TagNameIdByNode::const_iterator childtagdet = lookupmap->second->find(node);
      if(childtagdet!=lookupmap->second->end()) {
	childTagId = childtagdet->second.second;
      }
    }
    else {
      // Start new readonly transaction
      session->transaction().start(true);

      RDBVersionAccessor versionAccessor{node,(tag2node.empty()?node:tag2node),tag,session,msg()};
      versionAccessor.getChildTagData();
      childTagId = versionAccessor.getTagID();
      
      // Finish the transaction
      session->transaction().commit();
    }

    if(childTagId.empty()) {
      ATH_MSG_WARNING("Could not get the tag for " << node << " node. Returning 0 pointer to IRDBQuery");
    }
    else {
      query = std::unique_ptr<IRDBQuery>(new RDBQuery(this,session,node,childTagId,connName));
    }
  }
  catch(coral::SchemaException& se) {
    ATH_MSG_ERROR("Schema Exception : " << se.what());
  }
  catch(std::exception& e) {
    ATH_MSG_ERROR(e.what());
  }
  catch(...) {
    ATH_MSG_ERROR("Exception caught(...)");
  }

  disconnect(connName);
  return query;
}

std::string RDBAccessSvc::getChildTag(const std::string& childNode,
				      const std::string& parentTag,
				      const std::string& parentNode,
				      const std::string& connName)
{
  ATH_MSG_DEBUG("getChildTag for " << childNode << " " << parentTag << " " << parentNode);
  std::lock_guard<std::mutex> guard(m_mutex);

  // Check lookup table first
  std::string lookupMapKey = parentTag + "::" + connName;
  GlobalTagLookupMap::const_iterator lookupmap = m_globalTagLookup.find(lookupMapKey);
  if(lookupmap!=m_globalTagLookup.end()) {
    TagNameIdByNode::const_iterator childtagdet = lookupmap->second->find(childNode);
    if(childtagdet!=lookupmap->second->end()) {
      return childtagdet->second.first;
    }
    else {
      return std::string("");
    }
  }

  if(!connect(connName)) {
    ATH_MSG_ERROR("Unable to open connection " << connName << ". Returning empty string");
    return std::string("");
  }

  std::string childTag("");
  try {
    // We don't have lookup table for given parent tag. Go into slow mode through Version Accessor
    // Start new readonly transaction
    coral::ISessionProxy* session = m_sessions[connName];
    session->transaction().start(true);

    RDBVersionAccessor versionAccessor(childNode,parentNode,parentTag,session,msg());
    versionAccessor.getChildTagData();
	      
    // Finish the transaction
    session->transaction().commit(); 

    childTag = versionAccessor.getTagName();
  }
  catch(coral::SchemaException& se) {
    ATH_MSG_ERROR("Schema Exception : " << se.what());
  }
  catch(std::exception& e) {
    ATH_MSG_ERROR(e.what());
  }
  catch(...) {
    ATH_MSG_ERROR("Exception caught(...)");
  }

  disconnect(connName);

  return childTag;
}
 
RDBTagDetails RDBAccessSvc::getTagDetails(const std::string& tag,
					  const std::string& connName)
{
  ATH_MSG_DEBUG("getTagDetails for tag: " << tag);
  std::lock_guard<std::mutex> guard(m_mutex);

  RDBTagDetails tagDetails;

  if(!connect(connName)) {
    ATH_MSG_ERROR("Failed to open connection " << connName);
    return tagDetails;
  }

  coral::ISessionProxy* session = m_sessions[connName];
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
	(*lookup)[row["CHILDNODE"].data<std::string>()]=std::make_pair(row["CHILDTAG"].data<std::string>(),std::to_string(row["CHILDTAGID"].data<long long>()));
      }

      delete queryRootTag2Child;
	  
      if(lookup->size()>0) {
	m_globalTagLookup[lookupMapKey]=lookup;
      }
      else {
	delete lookup;
      }
    }
    // Finish the transaction
    if(session->transaction().isActive()) {
      session->transaction().commit();
    }
  }
  catch(coral::SchemaException& se) {
    ATH_MSG_INFO("Schema Exception : " << se.what());
  }
  catch(std::exception& e) {
    ATH_MSG_ERROR(e.what());
  }
  catch(...) {
    ATH_MSG_ERROR("Exception caught(...)");
  }
  
  disconnect(connName);
  return tagDetails;
}

void RDBAccessSvc::getAllLeafNodes(std::vector<std::string>& list,
				   const std::string& connName)
{
  list.clear();
  if(!connect(connName)) {
    ATH_MSG_ERROR("Failed to open Connection " << connName);
    return;
  }

  coral::ISessionProxy* session = m_sessions[connName];
  try {
    // Start new readonly transaction
    session->transaction().start(true);

    coral::ITable& tableNode = session->nominalSchema().tableHandle("HVS_NODE");
    coral::IQuery *queryNode = tableNode.newQuery();
    queryNode->addToOutputList("NODE_NAME");
    queryNode->setMemoryCacheSize(1);
    queryNode->setCondition("BRANCH_FLAG=0", coral::AttributeList());
    queryNode->addToOrderList("NODE_NAME");
    
    coral::ICursor& cursorNode = queryNode->execute();
    while(cursorNode.next()) {
      list.push_back(cursorNode.currentRow()["NODE_NAME"].data<std::string>());
    }
    
    delete queryNode;

    // Finish the transaction
    if(session->transaction().isActive()) {
      session->transaction().commit();
    }
  }
  catch(coral::SchemaException& se) {
    ATH_MSG_INFO("Schema Exception : " << se.what());
  }
  catch(std::exception& e) {
    ATH_MSG_ERROR(e.what());
  }
  catch(...) {
    ATH_MSG_ERROR("Exception caught(...)");
  }

  disconnect(connName);
}


StatusCode RDBAccessSvc::initialize()
{
  ServiceHandle<IPoolSvc> poolSvc("PoolSvc",name());
  ATH_CHECK(poolSvc.retrieve());
  return StatusCode::SUCCESS;
}

StatusCode RDBAccessSvc::finalize()
{
  // Clean up all remaining recordsets
  RecordsetPtrsByConn::iterator first_ptr_map = m_recordsetptrs.begin();
  RecordsetPtrsByConn::iterator last_ptr_map = m_recordsetptrs.end();
  for(;first_ptr_map!=last_ptr_map;++first_ptr_map) {
    delete first_ptr_map->second;
  }
  m_recordsetptrs.clear();

  // Clear global tag lookup table
  GlobalTagLookupMap::iterator itbegin = m_globalTagLookup.begin();
  GlobalTagLookupMap::iterator itend = m_globalTagLookup.end();
  for(;itbegin!=itend;itbegin++) {
    delete itbegin->second;
  }
  m_globalTagLookup.clear();

  // Shut down all connections
  SessionMap::iterator first_con = m_sessions.begin();
  SessionMap::iterator last_con = m_sessions.end();
  for(;first_con!=last_con; first_con++) {
    shutdown(first_con->first);
  }

  return StatusCode::SUCCESS;
}

StatusCode RDBAccessSvc::queryInterface(const InterfaceID& riid, void** ppvInterface)
{
  if (IID_IRDBAccessSvc == riid) {
    *ppvInterface = (IRDBAccessSvc*)this;
  }
  else { 
    return AthService::queryInterface(riid, ppvInterface);
  }

  addRef();
  return StatusCode::SUCCESS;
}

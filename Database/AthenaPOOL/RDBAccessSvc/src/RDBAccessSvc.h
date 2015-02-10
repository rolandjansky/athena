/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**
 * @file RDBAccessSvc.h
 *
 * @brief Definition of RDBAccessSvc class
 *
 * @author Vakho Tsulaia <Vakhtang.Tsulaia@cern.ch>
 *
 * $Id: RDBAccessSvc.h,v 1.17 2008-10-13 12:24:09 tsulaia Exp $
 */

#ifndef RDBACCESSSVC_RDBACCESSSVC_H
#define RDBACCESSSVC_RDBACCESSSVC_H

#include "RDBAccessSvc/IRDBAccessSvc.h"
#include "RDBRecordset.h"

#include "GaudiKernel/Property.h"
#include "AthenaBaseComps/AthService.h"

#include <string>
#include <map>

class ISvcLocator;
class RDBRecordset;

namespace coral 
{
  class ISessionProxy;
  class IRelationalService;
}

template <class TYPE> class SvcFactory;


// Recordsets of single connection
typedef std::map<std::string, RDBRecordset*, std::less<std::string> > RecordsetMap;
typedef std::map<std::string, IRDBRecordset_ptr> RecordsetPtrMap;

// Pointers to recordset maps by connection name
typedef std::map<std::string, RecordsetMap*, std::less<std::string> > RecordsetsByConn;
typedef std::map<std::string, RecordsetPtrMap*> RecordsetPtrsByConn;

// Session map
typedef std::map<std::string, coral::ISessionProxy*, std::less<std::string> > SessionMap;

// Lookup table for global tag contents quick access
typedef std::pair<std::string, std::string> TagNameId;
typedef std::map<std::string, TagNameId> TagNameIdByNode;
typedef std::map<std::string, TagNameIdByNode*> GlobalTagLookupMap; // Key - <Global_Tag_Name>::<Connection>

/**
 * @class RDBAccessSvc
 *
 * @brief RDBAccessSvc is the implementation of IRDBAccessSvc interface
 *
 */

class RDBAccessSvc : public AthService, virtual public IRDBAccessSvc 
{
  friend class RDBRecordset;

 public:

  /// Retrieve interface ID
  static const InterfaceID& interfaceID() { return IID_IRDBAccessSvc; }

  /// Connect to the relational DB.
  /// If this method is called for already open connection the connection
  /// counter is incremented.
  /// @return success/failure
  virtual bool connect(const std::string& connName);

  /// If the counnection counter==1 closes the connection.
  /// Decrements the connection counter value otherwise.
  /// @return success/failure
  virtual bool disconnect(const std::string& connName);

 /// Closes the connection regardless of the counter value.
  /// @return success/failure
  virtual bool shutdown(const std::string& connName);

  /// Provides access to the Recordset object containing HVS-tagged data.
  /// @param node [IN] name of the leaf HVS node
  /// @param tag [IN] tag of the HVS node specified by node parameter if tag2node is omitted,
  /// tag of the HVS branch node specified by tag2node otherwise
  /// @param tag2node [IN] some parent of the HVS leaf node specified by node parameter
  /// @return pointer to the recordset object
  virtual const IRDBRecordset* getRecordset(const std::string& node,
					    const std::string& tag,
					    const std::string& tag2node,
					    const std::string& connName);

  virtual IRDBRecordset_ptr getRecordsetPtr(const std::string& node,
					    const std::string& tag,
					    const std::string& tag2node="",
					    const std::string& connName = "ATLASDD");

  /// Gets the tag name for the node by giving its parent node tag
  /// @param childNode [IN] name of the child node
  /// @param parentTag [IN] name of the parent tag
  /// @param parentNode [IN] name of the parent node
  /// @param fetchData [IN] if true fetch the corresponding data
  /// this parameter has no sence if child is the branch node
  virtual std::string getChildTag(const std::string& childNode,
				  const std::string& parentTag,
				  const std::string& parentNode,
				  bool fetchData,
				  const std::string& connName);

  virtual IRDBQuery* getQuery(const std::string& node,
			      const std::string& tag,
			      const std::string& tag2node,
			      const std::string& connName);

  virtual RDBTagDetails getTagDetails(const std::string& tag,
				      const std::string& connName = "ATLASDD");

  void getAllLeafNodes(std::vector<std::string>& list,
		       const std::string& connName = "ATLASDD");

  inline MsgStream& msgStream() { return msg(); }

  //Implementation of standard methots

  /// Initialize
  virtual StatusCode initialize();

  /// Finalize
  virtual StatusCode finalize();

  /// Query Interface
  virtual StatusCode queryInterface( const InterfaceID& riid, void** ppvInterface );

 protected:

  friend class SvcFactory<RDBAccessSvc>;

  /// Standard Service Constructor
  RDBAccessSvc(const std::string& name, ISvcLocator* svc);

  /// Standard Service Destructor
  virtual ~RDBAccessSvc();

  /// Cleaning up the recordset entry
  void eraseRecordset(const RDBRecordset* recordset);

private:
  SessionMap m_sessions;
  std::map<std::string, unsigned int, std::less<std::string> > m_openConnections;

  std::string   m_hostName;  // To be removed as soon as two remainig client scripts have been cleaned up

  RecordsetsByConn m_recordsets;
  RecordsetPtrsByConn m_recordsetptrs;
  
  RDBRecordset m_empty;

  GlobalTagLookupMap m_globalTagLookup;

  bool shutdown_connection(const std::string& connName);
};

#endif 

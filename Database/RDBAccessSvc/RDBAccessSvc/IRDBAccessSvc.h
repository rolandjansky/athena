/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**
 * @file IRDBAccessSvc.h
 *
 * @brief Definition of the abstract IRDBAccessSvc interface
 *
 * @author Vakho Tsulaia <Vakhtang.Tsulaia@cern.ch>
 *
 * $Id: IRDBAccessSvc.h,v 1.8 2006-11-28 22:39:26 tsulaia Exp $
 */

#ifndef RDBACCESSSVC_IRDBACCESSSVC_H
#define RDBACCESSSVC_IRDBACCESSSVC_H

#include "GaudiKernel/IInterface.h"
#include <string>
#include <memory>

#include "CoralBase/AttributeList.h"
#include "CoralBase/Attribute.h"

class IRDBRecordset;
class IRDBQuery;

typedef std::shared_ptr<IRDBRecordset> IRDBRecordset_ptr;

typedef coral::AttributeList RDBTagDetails;

// Declaration of the interface ID ( interface id, major version, minor version)
static const InterfaceID IID_IRDBAccessSvc(1012, 1 , 0);

/**
 * @class IRDBAccessSvc
 *
 * @brief IRDBAccessSvc is an abstract interface to the athena service
 * that provides the following functionalities:
 *  1. Connect/disconnect from the relational database
 *  2. Access the HVS-tagged data in the database through IRDBRecordset interfaces
 *
 */

class IRDBAccessSvc : virtual public IInterface 
{
  // Special friends who need to call connect()/disconnect() methods
  friend class GeoModelSvc;
  friend class SourceCompAlg;

 public:

  /// Retrieve interface ID
  static const InterfaceID& interfaceID() { return IID_IRDBAccessSvc; }

  /// Provides access to the Recordset object containing HVS-tagged data.
  /// @param node [IN] name of the leaf HVS node
  /// @param tag [IN] tag of the HVS node specified by node parameter if tag2node is omitted,
  /// tag of the HVS branch node specified by tag2node otherwise
  /// @param tag2node [IN] some parent of the HVS leaf node specified by node parameter
  /// @return pointer to the recordset object
  virtual IRDBRecordset_ptr getRecordsetPtr(const std::string& node,
					    const std::string& tag,
					    const std::string& tag2node="",
					    const std::string& connName = "ATLASDD") = 0;

  /// Gets the tag name for the node by giving its parent node tag
  /// @param childNode [IN] name of the child node
  /// @param parentTag [IN] name of the parent tag
  /// @param parentNode [IN] name of the parent node
  /// @param fetchData [IN] if true fetch the corresponding data
  /// this parameter has no sence if child is the branch node
  virtual std::string getChildTag(const std::string& childNode,
				  const std::string& parentTag,
				  const std::string& parentNode,
				  const std::string& connName = "ATLASDD") = 0;


  // 
  // ------------------------ WARNING! ----------------------------
  // --- Following method should be used for special cases only ---
  // --------------------------------------------------------------
  //
  // 1. After calling this function call IRDBQuery::execute() in order
  //    to fetch data from database
  // 2. Navigate the cursor, get the data and store it into custom
  //    structures. 
  //        * The Cursor is NOT BIDIRECTIONAL
  //

  virtual std::unique_ptr<IRDBQuery> getQuery(const std::string& node,
					      const std::string& tag,
					      const std::string& tag2node="",
					      const std::string& connName = "ATLASDD") = 0;

  /// Returns AttributeList with tag details
  /// Attributes in the list: Locked (bool), Supported (bool)
  /// @param tag [IN] name of the tag
  /// @return attribute list with tag details
  virtual RDBTagDetails getTagDetails(const std::string& tag,
				      const std::string& connName = "ATLASDD") = 0;

 protected:
  /// Connect to the relational DB.
  /// If this method is called for already open connection the connection
  /// counter is incremented.
  /// @return success/failure
  virtual bool connect(const std::string& connName = "ATLASDD") = 0;

  /// If the counnection counter==1 closes the connection.
  /// Decrements the connection counter value otherwise.
  /// @return success/failure
  virtual bool disconnect(const std::string& connName = "ATLASDD") = 0;

  /// Closes the connection regardless of the counter value.
  /// @return success/failure
  virtual bool shutdown(const std::string& connName = "ATLASDD") = 0;

};

#endif 




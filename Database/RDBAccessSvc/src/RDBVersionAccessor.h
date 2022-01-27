/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

/**
 * @file RDBVersionAccessor.h
 *
 * @brief Definition of RDBVersionAccessor class
 *
 * @author Vakho Tsulaia <Vakhtang.Tsulaia@cern.ch>
 *
 * $Id: RDBVersionAccessor.h,v 1.11 2006-05-11 22:34:39 tsulaia Exp $
 */

#ifndef RDBACCESSSVC_RDBVERSIONACCESSOR_H
#define RDBACCESSSVC_RDBVERSIONACCESSOR_H

#include <string>
#include "GaudiKernel/MsgStream.h"

namespace coral
{
  class ISessionProxy;
  class AttributeList;
}

class MsgStream;
/**
 * @class RDBVersionAccessor
 *
 * @brief RDBVersionAccessor is a helper class navigating HVS
 * tree and getting child node tag by the tag of one of its parents
 * 
 */

class RDBVersionAccessor
{
 public:
  /// Constructor
  /// @param childNode [IN] child node name
  /// @param parentNode [IN] parent node name
  /// @param parentTag [IN] parent tag name
  /// @param session [IN] active relational session  
  RDBVersionAccessor(const std::string& childNode
		     , const std::string& parentNode
		     , const std::string& parentTag
		     , coral::ISessionProxy* session
		     , MsgStream& stream);
  
  /// Constructs SQL query and retrieves child tag ID and Name from DB
  void getChildTagData();

  /// @return node name
  inline std::string getNodeName() const {return m_childNode;}


  /// @return tag name for the child node 
  inline std::string getTagName() const {return m_tagName;}

  /// @return stringified tag ID
  inline std::string getTagID() const {return m_tagID;}

  /// @return branch flag for the child node
  /// returns false if the data has not been fetched yet
  inline bool isLeaf() const {return m_isChildLeaf;}
 private:
  /// Default constructor
  RDBVersionAccessor();

  // Convert attribute value to std::string
  std::string attribute2String(const coral::AttributeList& attList
			       , std::string fieldName);

  coral::ISessionProxy* m_session;

  std::string m_childNode;
  std::string m_parentNode;
  std::string m_parentTag;

  std::string m_tagName;
  std::string m_tagID;
  bool m_isChildLeaf;

  MsgStream& m_msgStream;
};

#endif

/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

/**
 * @file RDBRecordset.cxx
 *
 * @brief Implementation of RDBRecordset class
 *
 * @author Vakho Tsulaia <Vakhtang.Tsulaia@cern.ch>
 *
 * $Id: RDBRecordset.cxx,v 1.27 2006-11-28 22:39:26 tsulaia Exp $
 */

#include "RDBAccessSvc.h"
#include "RDBRecordset.h"
#include "RDBRecord.h"

#include "RelationalAccess/ISessionProxy.h"
#include "RelationalAccess/ICursor.h"
#include "RelationalAccess/ITableDescription.h"
#include "RelationalAccess/IQuery.h"
#include "RelationalAccess/ISchema.h"
#include "RelationalAccess/ITable.h"
#include "RelationalAccess/IColumn.h"
#include "RelationalAccess/SchemaException.h"

#include "CxxUtils/checker_macros.h"

#include "CoralBase/Attribute.h"
#include "CoralBase/AttributeList.h"

#include "GaudiKernel/MsgStream.h"

#include <stdexcept>
#include <iostream>
#include <set>

RDBRecordset::RDBRecordset(RDBAccessSvc* accessSvc)
  : m_tableName("")
  , m_tagName("")
  , m_accessSvc(accessSvc)
{
}

RDBRecordset::~RDBRecordset()
{
  for(auto record : m_records) {
    delete record;
  }
  m_records.clear();
}

void RDBRecordset::getData(coral::ISessionProxy* session
			   , const std::string& nodeName
			   , const std::string& tagName
			   , const std::string& tagId)
{
  if(m_accessSvc->msg().level() <= MSG::DEBUG) {
    m_accessSvc->msg() << MSG::DEBUG << "Recordset get data " << nodeName << ", " 
		       << tagName << ", " 
		       << tagId << endmsg;
  }

  if(session==0) {
    m_accessSvc->msg() << MSG::ERROR << "RT: No connection to database!" << endmsg;
    return;
  }

  try 
  {
    m_tableName = nodeName;

    if(tagId.empty()) {
      if(m_accessSvc->msg().level() <= MSG::DEBUG)
	m_accessSvc->msg() << MSG::DEBUG << "RT: Could not get the tag for " << m_tableName 
			   << " node. Returning empty recordset" << endmsg;
      return;
    }

    m_tagName = tagName;

    // change table name to upper case
    std::string upperName = m_tableName;
    for (char& ch : upperName) {
      ch = std::toupper (static_cast<unsigned int>(ch));
    }

    coral::IQuery* queryStructure = session->nominalSchema().newQuery();

    // Compose the list of output fields for queryStructure
    const coral::ITableDescription& dataTableDesc = session->nominalSchema().tableHandle(upperName + "_DATA").description();
    for(int i=0; i<dataTableDesc.numberOfColumns(); i++) {
      queryStructure->addToOutputList(upperName + "_DATA." + dataTableDesc.columnDescription(i).name());
    }

    // Create table list for query >> data and data2vers tables
    queryStructure->addToTableList(upperName + "_DATA");
    queryStructure->addToTableList(upperName + "_DATA2TAG");

    coral::AttributeList bindsData ATLAS_THREAD_SAFE;
    bindsData.extend<std::string>("tagID");

    // Compose condition for structure query
    std::string queryStructCondition = upperName +"_DATA2TAG." + upperName + "_TAG_ID =:tagID";
    queryStructCondition += " AND " + upperName +"_DATA." + upperName + "_DATA_ID=" + upperName + "_DATA2TAG." + upperName + "_DATA_ID";

    queryStructure->setCondition(queryStructCondition , bindsData);
    bindsData[0].data<std::string>() = tagId;

    queryStructure->addToOrderList(upperName + "_DATA." + upperName + "_DATA_ID");
    queryStructure->setMemoryCacheSize(1);

    coral::ICursor& cursorStructure = queryStructure->execute();

    // Process Query results
    while(cursorStructure.next()) {
      const coral::AttributeList& row = cursorStructure.currentRow();
      m_records.push_back(new RDBRecord(row,upperName + "_DATA"));
    }

    delete queryStructure;
  }
  catch(coral::SchemaException& se) {
     m_accessSvc->msg() << MSG::ERROR<< "RT: Schema Exception : " << se.what() << endmsg;
  }
  catch(std::exception& e) {
    m_accessSvc->msg() << MSG::ERROR << e.what() << endmsg;
  }
  catch(...) {
    m_accessSvc->msg() << MSG::ERROR << "RT: Exception caught(...)" << endmsg;
  }

}

unsigned int RDBRecordset::size() const 
{
  return m_records.size();
}

std::string RDBRecordset::nodeName() const
{
  return m_tableName;
}

std::string RDBRecordset::tagName() const
{
  return m_tagName;
}

const IRDBRecord* RDBRecordset::operator[](unsigned int index) const 
{
  return m_records[index];
}
IRDBRecordset::const_iterator RDBRecordset::begin() const
{
  return m_records.begin();
}

IRDBRecordset::const_iterator RDBRecordset::end() const
{
  return m_records.end();
}

bool RDBRecordset::operator!=(const RDBRecordset& rhs) const
{
  if(m_records.size()!=rhs.m_records.size()) return true;

  for(size_t i=0; i<m_records.size(); ++i) {
    RDBRecord* rec1 = dynamic_cast<RDBRecord*>(m_records[i]);
    RDBRecord* rec2 = dynamic_cast<RDBRecord*>(rhs.m_records[i]);
    if(rec1!=0
       && rec2!=0
       && *rec1!=*rec2) return true;
  }

  return false;
}

void RDBRecordset::compare(const RDBRecordset& rec, std::ostream& os) const
{
  if(m_tableName!=rec.m_tableName) {
    os << "Recordsets correspond to different nodes" << std::endl;
    return;
  }

  if(m_tagName!=rec.m_tagName) {
    os << "Recordsets correspond to different tags" << std::endl;
    return;
  }

  std::set<size_t> uniq0, uniq1;
  std::vector<long> dataId0, dataId1;

  // change table name to upper case
  std::string upperName = m_tableName;
  for (char& ch : upperName) {
    ch = std::toupper (static_cast<unsigned int>(ch));
  }

  // Collect data_id's for both recordsets
  for(size_t i=0; i<m_records.size(); ++i) {
    dataId0.push_back(m_records[i]->getLong(upperName+"_DATA_ID"));
  }

  for(size_t i=0; i<rec.m_records.size(); ++i) {
    dataId1.push_back(rec.m_records[i]->getLong(upperName+"_DATA_ID"));
  }

  // print out possible diffs of recordsets with same data id's
  // collect indices of records with data_id's unique to rec0 (this)
  bool diffFound = false;
  for(size_t i=0; i<dataId0.size(); ++i) {
    long id0 = dataId0[i];
    bool found = false;
    size_t i1(0);
    for(size_t ii=0; ii<dataId1.size(); ++ii) {
      if(id0==dataId1[ii]) {
	found = true;
	i1 = ii;
	break;
      }
    }
    if(found) {
      RDBRecord* record0 = dynamic_cast<RDBRecord*>(m_records[i]);
      RDBRecord* record1 = dynamic_cast<RDBRecord*>(rec.m_records[i1]);
      if(record0!=0
	 && record1!=0
	 && *record0!=*record1) {
	if(!diffFound) {
	  os << "Records with the same data id and different contents:" << std::endl;
	  diffFound = true;
	}
	os << "< " << *record0 << std::endl; 
	os << "> " << *record1 << std::endl; 
	os << "--" << std::endl;
      }
    }
    else
      uniq0.insert(i);
  }

  // collect indices of records with data_id's unique to rec1
  for(size_t i=0; i<dataId1.size(); ++i) {
    long id1 = dataId1[i];
    bool found = false;
    for(size_t ii=0; ii<dataId0.size(); ++ii) {
      if(id1==dataId0[ii]) {
	found = true;
	break;
      }
    }
    if(!found) {
      uniq1.insert(i);
    }
  }

  // print out results
  if(uniq0.size()>0) {
    os << "Records with the following data ids are present in recordset 0 and missing in recordset 1: " << std::endl;
    std::set<size_t>::const_iterator it = uniq0.begin();
    for(; it!=uniq0.end(); ++it) {
      os << m_records[*it]->getLong(upperName+"_DATA_ID") << " ";
    }
    os << std::endl;
  }
  if(uniq1.size()>0) {
    os << "Records with the following data ids are present in recordset 1 and missing in recordset 0: " << std::endl;
    std::set<size_t>::const_iterator it = uniq1.begin();
    for(; it!=uniq1.end(); ++it) {
      os << rec.m_records[*it]->getLong(upperName+"_DATA_ID") << " ";
    }
    os << std::endl;
  }

}

void RDBRecordset::setNodeName(const std::string& nodeName)
{
  if(m_tableName.empty()) m_tableName = nodeName; 
}

/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

// CoraCoolSequence.cxx
// implementation for CoraCoolSequence class
// Richard Hawkings, started 24/10/06

#include "CoralBase/AttributeList.h"
#include "CoralBase/Attribute.h"
#include "RelationalAccess/ISessionProxy.h"
#include "RelationalAccess/ITransaction.h"
#include "RelationalAccess/ITable.h"
#include "RelationalAccess/TableDescription.h"
#include "RelationalAccess/ITableDataEditor.h"
#include "RelationalAccess/ISchema.h"
#include "RelationalAccess/IQuery.h"
#include "RelationalAccess/ICursor.h"
#include "RelationalAccess/SchemaException.h"
#include "CoraCool/CoraCoolException.h"

#include "CoraCoolSequence.h"
#include "CxxUtils/checker_macros.h"
#include <unistd.h>

CoraCoolSequence::CoraCoolSequence(const std::string& dbname, 
				   const std::string& seqname,
				   coral::ISessionProxy* proxy, 
				   bool create) :
  m_dbname(dbname), m_seqname(seqname), m_proxy(proxy), m_table(0) {

  // derive key table name from CoraCool database name
  const std::string keytblname=m_dbname+"_CORACOOLKEYS";
  // check for existance of key table
  try {
    m_table=&(m_proxy->nominalSchema().tableHandle(keytblname));
  }
  catch (coral::SchemaException& e) {
    // try to create the table if requested
    if (create) {
      coral::TableDescription tdesc(keytblname);
      tdesc.setName(keytblname);
      tdesc.insertColumn("SEQNAME","string",63,false);
      tdesc.insertColumn("SEQVAL","int");
      tdesc.setPrimaryKey("SEQNAME");
      m_table=&(m_proxy->nominalSchema().createTable(tdesc));
      // pause for thought
      m_proxy->transaction().commit();
      sleep(1);
      m_proxy->transaction().start(false);
      m_table=&(m_proxy->nominalSchema().tableHandle(keytblname));
    }
  }
  // throw exception here? FIXME
  if (m_table==0) throw CoraCoolException("Cannot create "+keytblname+" table",
					"CoraCoolSequence::CoraCoolSequence");
  // if not creating, finish here in any case - have pointer to table
  if (!create) return;

  int ifk;
  if (!querySeq(ifk,false)) {
    // update table with sequence starting at 0
    coral::AttributeList data ATLAS_THREAD_SAFE; // Not shared, ok
    data.extend<std::string>("SEQNAME");
    data.extend<int>("SEQVAL");
    data[0].data<std::string>()=m_seqname;
    data[1].data<int>()=0;
    coral::ITableDataEditor& editor=m_table->dataEditor();
    editor.insertRow(data);
  }
}

int CoraCoolSequence::fetch(const int inc) {
  int key=0;
  // query sequence setting lock on row
  if (querySeq(key,true)) {
    // got result, now update rows
    coral::AttributeList bindvar ATLAS_THREAD_SAFE; // Not shared, ok
    bindvar.extend<std::string>("SKEY");
    bindvar[0].data<std::string>()=m_seqname;
    bindvar.extend<int>("SINC");
    bindvar[1].data<int>()=inc;
    coral::ITableDataEditor& editor=m_table->dataEditor();
    int rowsupdated=editor.updateRows("SEQVAL=SEQVAL+:SINC",
				      "SEQNAME=:SKEY",bindvar);
    if (rowsupdated!=1) throw CoraCoolException(
	 "Unexpected number of rows locked in keytable",
	 "CoraCoolSequence::CoraCoolSequence");
  } else {
    throw ("Problem generating next key value for "+m_seqname,
	   "CoraCoolSequence::fetch");
  }
  return key;
}

bool CoraCoolSequence::querySeq(int& keyval,bool update,bool gettable) {
 if (gettable) {
    const std::string keytblname=m_dbname+"_CORACOOLKEYS";
    m_table=&(m_proxy->nominalSchema().tableHandle(keytblname));
  }
   coral::IQuery* query=m_table->newQuery();
  coral::AttributeList bindvar ATLAS_THREAD_SAFE; // Not shared, ok
  bindvar.extend<std::string>("SKEY");
  bindvar[0].data<std::string>()=m_seqname;
  query->setCondition("SEQNAME=:SKEY",bindvar);
  query->setRowCacheSize(2);
  query->defineOutputType("SEQVAL","int");
  if (update) query->setForUpdate();
  coral::ICursor& cursor=query->execute();
  bool res=true;
  if (!cursor.next()) {
    res=false;
  } else {
    const coral::AttributeList& res=cursor.currentRow();
    keyval=res["SEQVAL"].data<int>();
  }
  if (cursor.next()) {
    res=false;
  }
  delete query;
  return res;
}

bool CoraCoolSequence::dropSeq() {
  coral::AttributeList bindvar ATLAS_THREAD_SAFE; // Not shared, ok
  bindvar.extend<std::string>("SKEY");
  bindvar[0].data<std::string>()=m_seqname;
  coral::ITableDataEditor& editor=m_table->dataEditor();
  long rows=editor.deleteRows("SEQNAME=:SKEY",bindvar);
  return (rows==1);
}

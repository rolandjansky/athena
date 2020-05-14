/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#include "MuonCalibDbOperations/CalibHeadOperations.h"
#include "MuonCalibDbOperations/CalibDbConnection.h"
#include "RelationalAccess/IRelationalService.h"
#include "RelationalAccess/IRelationalDomain.h"
#include "RelationalAccess/ITransaction.h"
#include "RelationalAccess/IQuery.h"
#include "RelationalAccess/ISchema.h"
#include "RelationalAccess/ICursor.h"
#include "RelationalAccess/ITable.h"
#include "RelationalAccess/IAuthenticationService.h"
#include "RelationalAccess/IAuthenticationCredentials.h"
#include "RelationalAccess/SchemaException.h"
#include "CoralBase/AttributeList.h"
#include "CoralBase/Attribute.h"
#include "CoralBase/AttributeSpecification.h"
#include "CoralKernel/Context.h"
#include "GaudiKernel/MsgStream.h"
#include "AthenaKernel/getMessageSvc.h"

#include "iostream"

namespace MuonCalib {

CalibHeadOperations::CalibHeadOperations(CalibDbConnection & db_conn) : m_meta_connection(&db_conn) {
}
	
int CalibHeadOperations::GetLatestHeadId() const {
  try {
    m_meta_connection->OpenTransaction();
    coral::IQuery * query=m_meta_connection->GetQuery();
    query->setRowCacheSize(1);
    query->addToTableList("MDT_HEAD");
    query->addToOutputList("max(HEAD_ID)", "max_head_id");
    coral::ICursor& cursor = query->execute();
    if(!cursor.next()) {
      MsgStream log(Athena::getMessageSvc(),"CalibHeadOperations");
      log<<MSG::WARNING<<"Query for head_id failed!"<<endmsg;
      return -1;
    }
    const coral::AttributeList & al= cursor.currentRow();
    return static_cast<int>(al["max_head_id"].data<double>());
  }
  catch( coral::SchemaException& e ) {
    MsgStream log(Athena::getMessageSvc(),"CalibHeadOperations");
    log<<MSG::WARNING<<"Schema exception : " << e.what()<<endmsg;
    return -1;
  }
}
	
bool CalibHeadOperations::GetHeadInfo(int &head_id, int & lowrun, int &uprun, int &lowtime, int &uptime) {
  if(head_id<0)	{
    head_id=GetLatestHeadId();
  }
  if(head_id<0)	{
    MsgStream log(Athena::getMessageSvc(),"CalibHeadOperations");
    log<<MSG::WARNING<<"CalibHeadOperations::GetHeadInfo: Cannot get latest head id"<<endmsg;
    return -1;
  }
  try {
    m_meta_connection->OpenTransaction();
    coral::IQuery * query=m_meta_connection->GetQuery();
    query->addToTableList("MDT_HEAD");
    query->addToOutputList("LOWRUN");
    query->addToOutputList("UPRUN");
    query->addToOutputList("LOWTIME");
    query->addToOutputList("UPTIME");
    std::string condition = "HEAD_ID=:hid";
    coral::AttributeList conditionData;
    conditionData.extend<int>("hid");
    conditionData["hid"].data<int>()=head_id;
    query->setCondition( condition, conditionData );
    coral::ICursor& cursor = query->execute();
    if(!cursor.next()) {
      MsgStream log(Athena::getMessageSvc(),"CalibHeadOperations");
      log<<MSG::WARNING<<"No information about head_id="<<head_id<<" found!"<<endmsg;
      return false;
    }
    const coral::AttributeList & al= cursor.currentRow();
    lowrun=al["LOWRUN"].data<int>();
    uprun=al["UPRUN"].data<int>();
    lowtime=al["LOWTIME"].data<int>();
    uptime=al["UPTIME"].data<int>();
    return true;
  }
  catch( coral::SchemaException& e ) {
    MsgStream log(Athena::getMessageSvc(),"CalibHeadOperations");
    log<<MSG::WARNING<<"Schema exception : " << e.what()<<endmsg;
    return false;
  }	
	
}

CalibDbConnection * CalibHeadOperations::GetDataConnection(int head_id, bool write, const std::string & writer_connection, const std::string & writer_user, const std::string & writer_password) {
  if (head_id<0) {
    head_id = GetLatestHeadId();
		}
  if (head_id<0)
    return nullptr;
  try {
    m_meta_connection->OpenTransaction();
    coral::IQuery * query=m_meta_connection->GetQuery();
    query->setRowCacheSize(1);
    query->addToTableList("MDT_HEAD");
    query->addToTableList("MDT_DATA_SCHEMA");
    query->addToOutputList("MDT_DATA_SCHEMA.SCHEMA_NAME", "SCHEMA");
    query->addToOutputList("MDT_DATA_SCHEMA.WRITER_ACCOUNT", "WRITER_ACCOUNT");
    query->addToOutputList("MDT_DATA_SCHEMA.ACTIVE", "ACTIVE");
    query->addToOutputList("MDT_DATA_SCHEMA.ARCHIVED", "ARCHIVED");
    query->addToOutputList( "MDT_DATA_SCHEMA.ARCHIVE_CONNECTION_STRING", "ARCHIVE_CONNECTION_STRING");
    std::string condition = "MDT_HEAD.DATA_SCHEMA=MDT_DATA_SCHEMA.SCHEMA_NAME and MDT_HEAD.HEAD_ID=:hid";
    coral::AttributeList conditionData;
    conditionData.extend<int>("hid");
    conditionData["hid"].data<int>()=head_id;
    query->setCondition( condition, conditionData );
    coral::ICursor& cursor = query->execute();
    if(!cursor.next()) {
      MsgStream log(Athena::getMessageSvc(),"CalibHeadOperations");
      log<<MSG::WARNING<<"No information about head_id="<<head_id<<" found!"<<endmsg;
      return nullptr;
    }
    const coral::AttributeList & al= cursor.currentRow();
    if(write && !al["ACTIVE"].data<bool>()) {
      MsgStream log(Athena::getMessageSvc(),"CalibHeadOperations");
      log<<MSG::WARNING<<"Can only write to the active schema"<<endmsg;
      return nullptr;
    }
    if(al["ARCHIVED"].data<bool>()) {
      return new CalibDbConnection(al["ARCHIVE_CONNECTION_STRING"].data<std::string>(), al["SCHEMA"].data<std::string>());
    }
    if(!write) {
      CalibDbConnection *conn=new CalibDbConnection(m_meta_connection->GetConnectionString(), al["SCHEMA"].data<std::string>());
      std::string user, passwd;
      m_meta_connection->GetLogin(user, passwd);
      conn->SetLogin(user, passwd);
      return conn;
    }
    CalibDbConnection *ret= new CalibDbConnection(writer_connection, al["SCHEMA"].data<std::string>());
    ret->SetTargetUser(al["WRITER_ACCOUNT"].data<std::string>());
    ret->SetLogin(writer_user, writer_password);
    return ret;
  }
  catch( coral::SchemaException& e ) {
    MsgStream log(Athena::getMessageSvc(),"CalibHeadOperations");
    log<<MSG::WARNING<<"Schema exception : " << e.what()<<endmsg;
    return nullptr;
  }	
}
	
}// namespace MuonCalib 

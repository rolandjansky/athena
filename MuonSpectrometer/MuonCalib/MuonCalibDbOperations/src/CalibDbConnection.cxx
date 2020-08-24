/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

//this
#include "MuonCalibDbOperations/CalibDbConnection.h"

#include "RelationalAccess/IRelationalService.h"
#include "RelationalAccess/IConnectionService.h"
#include "RelationalAccess/ISessionProxy.h"
#include "RelationalAccess/IRelationalDomain.h"
#include "RelationalAccess/ITransaction.h"
#include "RelationalAccess/IQuery.h"
#include "RelationalAccess/ISchema.h"
#include "RelationalAccess/ICursor.h"
#include "RelationalAccess/ITable.h"
#include "RelationalAccess/ITableDataEditor.h"
#include "RelationalAccess/IAuthenticationService.h"
#include "RelationalAccess/IAuthenticationCredentials.h"
#include "RelationalAccess/SchemaException.h"
#include "CoralKernel/Context.h"
#include "MuonCalibStandAloneBase/RegionSelectorBase.h"
#include "MuonCalibStandAloneBase/RegionSelectionSvc.h"
#include "GaudiKernel/MsgStream.h"
#include "AthenaKernel/getMessageSvc.h"

#include <stdexcept>
#include "iostream"
#include <TString.h> // for Form

namespace MuonCalib{

/////////////////
// Constructor //
/////////////////

CalibDbConnection::CalibDbConnection(const std::string& ConnectionString, const std::string& WorkingSchema): m_connection_string(ConnectionString), m_working_schema(WorkingSchema), m_comp_loaded(false), m_context( &coral::Context::instance() ), m_session(nullptr), m_transaction(false) {
  coral::IHandle<coral::IConnectionService> lookSvcH = m_context->query<coral::IConnectionService>();
  if (!lookSvcH.isValid()) {
    m_context->loadComponent( "CORAL/Services/ConnectionService" );
    lookSvcH = m_context->query<coral::IConnectionService>();
  }
  if (!lookSvcH.isValid()) {
    return;
  }
  m_context->loadComponent( "CORAL/Services/XMLAuthenticationService" );
  m_context->loadComponent( "CORAL/Services/RelationalService" );
  m_comp_loaded=true;
  m_session=nullptr;
}


CalibDbConnection::~CalibDbConnection() {
  for(std::set<coral::IQuery*>::iterator it=m_queries.begin(); it!=m_queries.end(); it++) {
    if (*it) { delete (*it); }
    m_queries.clear();
  }
}
	
////////////////////
// OpenConnection //
////////////////////

bool CalibDbConnection::OpenConnection() {
  if(!m_comp_loaded) { return false; }
  if (m_session) { return true; }
  
  try {
    // Load CORAL connection service
    coral::IHandle<coral::IConnectionService> lookSvcH = m_context->query<coral::IConnectionService>();
    m_context->loadComponent( "CORAL/Services/ConnectionService" );
    lookSvcH = m_context->query<coral::IConnectionService>();
    
    if (!lookSvcH.isValid()) {
      throw std::runtime_error(Form("File: %s, Line: %d\nCalibDbConnection::OpenConnection() - Could not locate the connection service!", __FILE__, __LINE__));
    }
    // connection to CORAL
    MsgStream log(Athena::getMessageSvc(),"CalibDbConnection");
    log<<MSG::INFO<<"CalibDbConnection::OpenConnection: "<<m_connection_string<<" Schema: "<<m_working_schema<<endmsg;
    m_session = lookSvcH->connect( m_connection_string, coral::Update );
    return true;
  }
  catch( coral::SchemaException& e ) {
    MsgStream log(Athena::getMessageSvc(),"CalibDbConnection");
    log<<MSG::WARNING<<"Schema exception : " << e.what()<<endmsg;
    m_session=nullptr;
    return false;
  }		
}
	
/////////////////////	
// OpenTransaction //
/////////////////////
void CalibDbConnection::OpenTransaction() {
  if(!m_transaction) {
    m_session->transaction().start();
    m_transaction=true;
  }
}

////////////
// Commit //
////////////
void CalibDbConnection::Commit() {
  if(m_transaction) {
    m_session->transaction().commit();
    m_transaction=false;
  }
}

//////////////
// Rollback //
//////////////	
void CalibDbConnection::Rollback() {
  if(m_transaction) {
    m_session->transaction().rollback();
    m_transaction=false;
  }		
}	

////////////////////
// GetTableEditor //
////////////////////
coral::ITableDataEditor& CalibDbConnection::GetTableEditor(const std::string & table_name) {
  return m_session->schema(  m_working_schema ).tableHandle(table_name).dataEditor();
}
	
//////////////
// GetQuery //
//////////////
coral::IQuery* CalibDbConnection::GetQuery() {
  if(!m_session) {
    MsgStream log(Athena::getMessageSvc(),"CalibDbConnection");
    log<<MSG::WARNING<<"No Session open!"<<endmsg;
    return nullptr;
  }
  coral::ISchema& workingSchema = m_session->schema(  m_working_schema );
  coral::IQuery* query = workingSchema.newQuery();
  m_queries.insert(query);
  return query;
}

//////////////
// GetQuery //
//////////////
void CalibDbConnection::DestroyQuery(coral::IQuery *query) {
  std::set<coral::IQuery*>::iterator it=m_queries.find(query);
  if(it==m_queries.end()) {
    return;
  }
  m_queries.erase(it);
  delete query;
}
	
coral::IRelationalDomain& CalibDbConnection::domain( const std::string& connectionString ) {
  coral::IHandle<coral::IRelationalService> relationalService= m_context->query<coral::IRelationalService>();
  if ( !relationalService.isValid() ) {
    throw std::runtime_error(Form("File: %s, Line: %d\nCalibDbConnection::domain() - Could not locate the relational service!", __FILE__, __LINE__));
  }

  coral::IHandle<coral::IAuthenticationService> authenticationService= m_context->query<coral::IAuthenticationService>();
  if ( !authenticationService.isValid() ) {
    throw std::runtime_error(Form("File: %s, Line: %d\nCalibDbConnection::domain() - Could not locate the authentication service!", __FILE__, __LINE__));
  }
  if(m_username=="") {
    const coral::IAuthenticationCredentials& credentials = authenticationService->credentials( connectionString );
    m_username = credentials.valueForItem( "user" );
    m_password = credentials.valueForItem( "password" );
  }
  if(m_target_user!="" && m_target_user!=m_username) {
    m_username = m_username + "[" + m_target_user + "]";
  }
  return relationalService->domainForConnection( connectionString );
}

}//namespace MuonCalib

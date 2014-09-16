/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//this
#include "MuonCalibDbOperations/CalibDbConnection.h"

//coral
#include "RelationalAccess/IRelationalService.h"
#include "RelationalAccess/IConnection.h"
#include "RelationalAccess/IConnectionService.h"
#include "RelationalAccess/ISession.h"
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
#include "CoralBase/AttributeList.h"
#include "CoralBase/Attribute.h"
#include "CoralBase/AttributeSpecification.h"
#include "CoralKernel/Context.h"


// c- c++
#include <stdexcept>
#include "iostream"

namespace MuonCalib{

/////////////////
// Constructor //
/////////////////

CalibDbConnection::CalibDbConnection(const std::string& ConnectionString, const std::string& WorkingSchema): m_connection_string(ConnectionString), m_working_schema(WorkingSchema), m_comp_loaded(false), m_context( &coral::Context::instance() ), m_connection(NULL), m_session(NULL), m_transaction(false)
	{
	std::cout<<"X"<<std::endl;
	coral::IHandle<coral::IConnectionService> lookSvcH = m_context->query<coral::IConnectionService>();
	std::cout<<"XX"<<std::endl;
	if (!lookSvcH.isValid()) 
		{
		std::cout<<"XX"<<std::endl;
		m_context->loadComponent( "CORAL/Services/ConnectionService" );
		lookSvcH = m_context->query<coral::IConnectionService>();
		std::cout<<"XX"<<std::endl;
		}
	if (!lookSvcH.isValid()) 
		{
		return;
		}
	std::cout<<"Y"<<std::endl;
	m_context->loadComponent( "CORAL/Services/XMLAuthenticationService" );
	std::cout<<"YY"<<std::endl;
	m_context->loadComponent( "CORAL/Services/RelationalService" );
	std::cout<<"YYY"<<std::endl;
	m_comp_loaded=true;
	m_connection=NULL;
	}


CalibDbConnection::~CalibDbConnection()
	{
	CloseConnection();
	}
	
	
////////////////////
// OpenConnection //
////////////////////

bool CalibDbConnection::OpenConnection()
	{
	if(!m_comp_loaded)
		{
		return false;
		}
	if (m_connection!=NULL)
		{
		return true;
		}
	try {
	coral::IRelationalDomain &domain = this->domain( m_connection_string); 
	std::pair<std::string,std::string> cstr(domain.decodeUserConnectionString(m_connection_string));
	m_connection=domain.newConnection(cstr.first);
	m_connection->connect(); 
	m_session=m_connection->newSession(cstr.second);
	m_session->startUserSession(m_username,m_password);
	return true;
	}
	catch( coral::SchemaException& e )
		{
		std::cerr << "Schema exception : " << e.what() << std::endl;
		m_connection=NULL;
		return false;
		}		
	}
	
	
/////////////////////	
// CloseConnection //
/////////////////////

void CalibDbConnection::CloseConnection(bool commit)
  	{
	for(std::set<coral::IQuery*>::iterator it=m_queries.begin(); it!=m_queries.end(); it++)
		{
		if(*it)
			delete (*it);
		}	
	m_queries.clear();
	if(m_session != NULL)
		{
		if(commit)
			Commit();
		else
			Rollback();
		m_session->endUserSession();
		}
	if (m_connection !=NULL)
		m_connection->disconnect();
	m_session=NULL;
	m_connection=NULL;
	}
	
	
/////////////////////	
// OpenTransaction //
/////////////////////

void CalibDbConnection :: OpenTransaction()
	{
	if(!m_transaction)
		{
		m_session->transaction().start();
		m_transaction=true;
		}
	}


////////////
// Commit //
////////////

void CalibDbConnection :: Commit()
	{
	if(m_transaction)
		{
		m_session->transaction().commit();
		m_transaction=false;
		}
	}


//////////////
// Rollback //
//////////////
	
void CalibDbConnection :: Rollback()
	{
	if(m_transaction)
		{
		m_session->transaction().rollback();
		m_transaction=false;
		}		
	}	


////////////////////
// GetTableEditor //
////////////////////

coral::ITableDataEditor& CalibDbConnection :: GetTableEditor(const std::string & table_name)
	{
	return m_session->schema(  m_working_schema ).tableHandle(table_name).dataEditor();
	}
	
	
//////////////
// GetQuery //
//////////////

coral::IQuery* CalibDbConnection :: GetQuery()
	{
	if(m_session==NULL)
		{
		std::cerr<<"No Session open!"<<std::endl;
		return NULL;
		}
	std::cout<<m_connection_string<<" "<<m_working_schema<<std::endl;
	coral::ISchema& workingSchema = m_session->schema(  m_working_schema );
	coral::IQuery* query = workingSchema.newQuery();
	m_queries.insert(query);
	return query;
	}

//////////////
// GetQuery //
//////////////

void CalibDbConnection :: DestroyQuery(coral::IQuery *query)
	{
	std::set<coral::IQuery*> :: iterator it=m_queries.find(query);
	if(it==m_queries.end())
		{
		return;
		}
	m_queries.erase(it);
	delete query;
	}
	
coral::IRelationalDomain&
CalibDbConnection::domain( const std::string& connectionString )
	{
	coral::IHandle<coral::IRelationalService> relationalService= m_context->query<coral::IRelationalService>();
	if ( !relationalService.isValid() ) 
		{
		throw std::runtime_error( "Could not locate the relational service" );
		}

	coral::IHandle<coral::IAuthenticationService> authenticationService= m_context->query<coral::IAuthenticationService>();
	if ( !authenticationService.isValid() ) 
		{
		throw std::runtime_error( "Could not locate the authentication service" );
		}
	if(m_username=="")
		{
		const coral::IAuthenticationCredentials& credentials = authenticationService->credentials( connectionString );
		m_username = credentials.valueForItem( "user" );
		m_password = credentials.valueForItem( "password" );
		}
	if(m_target_user!="" && m_target_user!=m_username)
		{
		m_username = m_username + "[" + m_target_user + "]";
		}
	return relationalService->domainForConnection( connectionString );
	}


}//namespace MuonCalib

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//*****************************************************************************
//
// $Id: CoralDB.cxx,v 1.70 2008-11-18 16:28:44 hirsch Exp $
//
// CoralDB is an implementation of the pixel conditions and connectivity database
// based on CORAL.
//
// Written in August 2006 by Juerg Beringer.
//
//*****************************************************************************



#include "CoralDB/CoralDB.h"
#include "CoralDB/Connection.h"
#include "CoralDB/Alias.h"

#include "CoralKernel/Context.h"
#include "CoralBase/Exception.h"
#include "CoralBase/Attribute.h"
#include "CoralBase/AttributeList.h"
#include "CoralBase/AttributeSpecification.h"
#include "CoralBase/Blob.h"
#include "RelationalAccess/IConnectionService.h"
#include "RelationalAccess/IConnectionServiceConfiguration.h"
#include "RelationalAccess/ISessionProxy.h"
#include "RelationalAccess/ITransaction.h"
#include "RelationalAccess/ITableDataEditor.h"
#include "RelationalAccess/ITable.h"
#include "RelationalAccess/ISchema.h"
#include "RelationalAccess/IBulkOperation.h"
#include "RelationalAccess/TableDescription.h"
#include "RelationalAccess/IQuery.h"
#include "RelationalAccess/ICursor.h"
// #include "RelationalAccess/IMonitoring.h"
#include "RelationalAccess/IOperationWithQuery.h"
#include "RelationalAccess/IViewFactory.h"
#include "RelationalAccess/ITablePrivilegeManager.h"

#include <vector>
#include <memory>
#include <iostream>
// #include <iomanip>

//#define CORALDB_DEBUG_TRANSACTIONS
#ifdef CORALDB_DEBUG_TRANSACTIONS
#define CORALDB_TRACE_TRANS_CALL(f) do { cerr<<"CORALDB_TRACE_TRANSACTIONS: "; f; } while(0)
#else
#define CORALDB_TRACE_TRANS_CALL(f)
#endif

#define ASSERT_UNLOCKED(tagType) do { \
    if(!this->m_cacheEnabled) { \
        this->m_locked_##tagType = tagType##Locked(this->m_tags.tagType());\
    } \
    if(this->m_locked_##tagType) { \
        std::ostringstream os; \
        os<<"Attempt to modify locked tag: " #tagType " = \"" + m_tags.tagType()+  "\", file " __FILE__ ", line "<<__LINE__; \
        throw TagLocked(os.str());\
    } \
} while(0)


namespace CoralDB {
#if 0
} // this is for better indentation in Emacs
#endif

using namespace std;

const string CoralDB::TABLENAME_PREFIX = "CORALDB2_";


InvalidCompoundTag::InvalidCompoundTag(const std::string& prefix, const CompoundTag& tags) 
  : std::runtime_error(prefix + ": idTag is not specified, but some other tag is: "
		       + tags.toString()
		       )
{}

ostream& operator<<(ostream& os, const TagStatus ts) {
  return os<<ts.tag()<<(ts.locked() ? "*" : " ");
}


vector<string> makeStringVector(const string& s1, const string& s2) {
  vector<string> res(1,s1);
  res.push_back(s2);
  return res;
}

vector<string> makeStringVector(const string& s1, const string& s2, const string& s3) {
  vector<string> tmp = makeStringVector(s1,s2);
  tmp.push_back(s3);
  return tmp;
}

vector<string> makeStringVector(const string& s1, const string& s2, const string& s3, const string& s4) {
  vector<string> tmp = makeStringVector(s1,s2,s3);
  tmp.push_back(s4);
  return tmp;
}

vector<string> makeStringVector(const string& s1, const string& s2, const string& s3, const string& s4, const string& s5) {
  vector<string> tmp = makeStringVector(s1,s2,s3,s4);
  tmp.push_back(s5);
  return tmp;
}

vector<string> makeStringVector(const string& s1, const string& s2, const string& s3, const string& s4, const string& s5, const string& s6) {
  vector<string> tmp = makeStringVector(s1,s2,s3,s4,s5);
  tmp.push_back(s6);
  return tmp;
}


// Escapes underscores and the escape char itself
string escapePattern(const string& in, char esc) {
  string out;
  string::size_type length = in.length();
  out.reserve(2*length);
  for(string::size_type i = 0; i<length; ++i) {
    if( (in[i] == '_') || (in[i] == esc) ) {
      out.push_back(esc);
    }
    out.push_back(in[i]);
  }
  return out;
}


CoralDB::CoralDB(const string& connection, coral::AccessMode accessMode, coral::MsgLevel outputLevel, bool autoCommit)
  : m_connection(connection)
  , m_session(0)
  , m_autocommit(autoCommit)
  , m_cacheEnabled(true)
  , m_locked_objectDictionaryTag(false)
  , m_locked_connectivityTag(false)
  , m_locked_dataTag(false)
  , m_locked_aliasTag(false)
{
  CORALDB_TRACE_TRANS_CALL(cerr<<"CoralDB::CoralDB[this="<<this<<"]("<<connection
			   <<","<<accessMode<<","<<outputLevel<<","<<autoCommit<<")"<<endl);
  
  setMsgLevel(outputLevel);

  // Instantiate connection service
  m_context = &coral::Context::instance();

  // Load CORAL connection service
  coral::IHandle<coral::IConnectionService> lookSvcH = m_context->query<coral::IConnectionService>();
  if (!lookSvcH.isValid()) {
      m_context->loadComponent( "CORAL/Services/ConnectionService" );
      lookSvcH = m_context->query<coral::IConnectionService>();
  }
  if (!lookSvcH.isValid()) {
      throw std::runtime_error( "Could not locate the connection service" );
  }
  m_session = lookSvcH->connect( m_connection, accessMode );

  // For Oracle, it seems we could also use something like (see RCDCoralUtils: CoralConnectionExample.cpp
  // m_session = theConnectionService->connect( m_connection, m_username, m_password, accessMode );

  // Maintain an open transaction in the manual commit mode.
  if(!m_autocommit) {
    m_session->transaction().start(READONLY);
  }
}



void CoralDB::setMsgLevel(coral::MsgLevel outputLevel) {
    coral::MessageStream::setMsgVerbosity(outputLevel);
}


void CoralDB::transactionStart() {
  CORALDB_TRACE_TRANS_CALL(cerr<<"CoralDB::transactionStart[this="<<this<<"]()"<<endl);
  if(!m_autocommit) {
    // Close the default read-only transaction, but let Coral to catch attempts to nest update transactions.
    if(m_session->transaction().isActive() && m_session->transaction().isReadOnly()) {
      m_session->transaction().rollback();
    }
    m_session->transaction().start(UPDATE);
  }
}

void CoralDB::transactionCommit() {
  CORALDB_TRACE_TRANS_CALL(cerr<<"CoralDB::transactionCommit[this="<<this<<"]()"<<endl);
  if(!m_autocommit) {
    m_session->transaction().commit();
    m_session->transaction().start(READONLY);
  }
}

void CoralDB::transactionRollBack() {
  CORALDB_TRACE_TRANS_CALL(cerr<<"CoralDB::transactionRollBack[this="<<this<<"]()"<<endl);
  if(!m_autocommit) {
    m_session->transaction().rollback();
    m_session->transaction().start(READONLY);
  }
}

// All explicitly started transactions are UPDATE.
// Read-only transactions are transparent to the user.
bool CoralDB::transactionIsActive() const {
  return m_session->transaction().isActive() && !m_session->transaction().isReadOnly();
}


void CoralDB::updateCache() const {
  ReadOnlyTransactionGuard tg(this);
  observeTagLocks();
  tg.commit();
}


CoralDB::~CoralDB() {
  CORALDB_TRACE_TRANS_CALL(cerr<<"CoralDB::~CoralDB[this="<<this<<"]()"<<endl);
  // Delete the session proxy (the connection will be not dropped)
  delete m_session;
  // delete m_context; ????
}

void CoralDB::createTagListTable(const string& tablename) {
  coral::TableDescription tableDescr;
  tableDescr.setName(tablename);
  tableDescr.insertColumn("IDTAG",coral::AttributeSpecification::typeNameForType<std::string>(), /*size = */ MAX_TAG_LENGTH, /*fixed_size=*/ false);
  tableDescr.insertColumn("TAG",coral::AttributeSpecification::typeNameForType<std::string>(), /*size = */ MAX_TAG_LENGTH, /*fixed_size=*/ false);
  tableDescr.insertColumn("LOCKED",coral::AttributeSpecification::typeNameForType<bool>());
  tableDescr.setPrimaryKey(makeStringVector("IDTAG", "TAG"));
  tableDescr.setNotNullConstraint("IDTAG");
  tableDescr.setNotNullConstraint("TAG");
  tableDescr.setNotNullConstraint("LOCKED");
  tableDescr.createForeignKey(tablename+"_FK", "IDTAG", TABLENAME_PREFIX+"OBJECTDICT_TAGS", "IDTAG");
  tableDescr.createIndex(tablename+"_FKI", "IDTAG");

  m_session->nominalSchema().createTable( tableDescr );
  m_session->nominalSchema().tableHandle( tableDescr.name() ).privilegeManager().grantToPublic(coral::ITablePrivilegeManager::Select);
}


void CoralDB::createSchema() {
  UpdateTransactionGuard tg(this);

  const vector<string> ID_IDTAG(makeStringVector("ID", "IDTAG") );
  const vector<string> IDTAG_TAG(makeStringVector("IDTAG", "TAG"));

  // Create object dictionary taglist
  {
    coral::TableDescription tableDescr;
    tableDescr.setName(TABLENAME_PREFIX+"OBJECTDICT_TAGS");
    tableDescr.insertColumn("IDTAG",coral::AttributeSpecification::typeNameForType<std::string>(), /*size = */ MAX_TAG_LENGTH, /*fixed_size=*/ false);
    tableDescr.insertColumn("LOCKED",coral::AttributeSpecification::typeNameForType<bool>());
    tableDescr.setPrimaryKey("IDTAG");
    // explicit NOT NULL constraints are needed *in addition* to the primary key, at least for SQLite
    tableDescr.setNotNullConstraint("IDTAG");
    tableDescr.setNotNullConstraint("LOCKED");
    m_session->nominalSchema().createTable( tableDescr );
    m_session->nominalSchema().tableHandle( tableDescr.name() ).privilegeManager().grantToPublic(coral::ITablePrivilegeManager::Select);
  }

  // Create other tag list tables
  createTagListTable(TABLENAME_PREFIX+"CONNECTIONS_TAGS");
  createTagListTable(TABLENAME_PREFIX+"CLOBS_TAGS");
  createTagListTable(TABLENAME_PREFIX+"ALIASES_TAGS");

  // Create object dictionary table
  {
    coral::TableDescription objdictDescr;
    objdictDescr.setName(TABLENAME_PREFIX+"OBJECTDICT");
    objdictDescr.insertColumn("ID",coral::AttributeSpecification::typeNameForType<std::string>(), MAX_ID_LENGTH, false);
    objdictDescr.insertColumn("IDTAG",coral::AttributeSpecification::typeNameForType<std::string>(), MAX_TAG_LENGTH, false);
    objdictDescr.insertColumn("TYPE",coral::AttributeSpecification::typeNameForType<std::string>(), MAX_OBJECTTYPE_LENGTH, false);
    objdictDescr.setPrimaryKey(ID_IDTAG);
    // explicit NOT NULL constraints are needed *in addition* to the primary key, at least for SQLite
    objdictDescr.setNotNullConstraint("ID");
    objdictDescr.setNotNullConstraint("IDTAG");
    objdictDescr.setNotNullConstraint("TYPE");

    objdictDescr.createForeignKey(TABLENAME_PREFIX+"OD_IDTAG_FK", "IDTAG", TABLENAME_PREFIX+"OBJECTDICT_TAGS", "IDTAG");
    objdictDescr.createIndex(TABLENAME_PREFIX+"OD_IDTAG_FKI", "IDTAG");

    m_session->nominalSchema().createTable( objdictDescr );
    m_session->nominalSchema().tableHandle( objdictDescr.name() ).privilegeManager().grantToPublic(coral::ITablePrivilegeManager::Select);
  }
  
  // Create master list table
  {
    coral::TableDescription masterDescr;
    masterDescr.setName(TABLENAME_PREFIX+"MASTERLIST");
    masterDescr.insertColumn("TAG",coral::AttributeSpecification::typeNameForType<std::string>(), /*size = */ MAX_TAG_LENGTH, /*fixed_size=*/ false);
    masterDescr.insertColumn("ID",coral::AttributeSpecification::typeNameForType<std::string>(), /*size=*/ MAX_TAG_LENGTH, /*fixed_size*/ false);
    masterDescr.insertColumn("IDTAG",coral::AttributeSpecification::typeNameForType<std::string>(), /*size=*/ MAX_ID_LENGTH, /*fixed_size*/ false);
    masterDescr.setPrimaryKey(IDTAG_TAG);
    // explicit NOT NULL constraints are needed *in addition* to the primary key, at least for SQLite
    masterDescr.setNotNullConstraint("TAG");
    masterDescr.setNotNullConstraint("ID");
    masterDescr.setNotNullConstraint("IDTAG");
    masterDescr.createForeignKey(TABLENAME_PREFIX+"ML_ID_FK", ID_IDTAG, TABLENAME_PREFIX+"OBJECTDICT", ID_IDTAG);
    masterDescr.createIndex(TABLENAME_PREFIX+"ML_ID_FKI", ID_IDTAG);
    masterDescr.createForeignKey(TABLENAME_PREFIX+"ML_TAG_FK", IDTAG_TAG, TABLENAME_PREFIX+"CONNECTIONS_TAGS", IDTAG_TAG);
    //don't repeat the primary key: masterDescr.createIndex(TABLENAME_PREFIX+"ML_TAG_FKI", IDTAG_TAG);
    m_session->nominalSchema().createTable( masterDescr );
    m_session->nominalSchema().tableHandle( masterDescr.name() ).privilegeManager().grantToPublic(coral::ITablePrivilegeManager::Select);
  }

  //unused: // FIXME: unused code
  //unused: // Create data dictionary table.
  //unused: {
  //unused:   coral::TableDescription datadictDescr;
  //unused:   datadictDescr.setName(TABLENAME_PREFIX+"DATADICT");
  //unused:   datadictDescr.insertColumn("OBJECTTYPE",coral::AttributeSpecification::typeNameForType<std::string>(), MAX_OBJECTTYPE_LENGTH, false);
  //unused:   datadictDescr.insertColumn("DATATYPE",coral::AttributeSpecification::typeNameForType<std::string>(), MAX_DATATYPE_LENGTH, false);
  //unused:   datadictDescr.insertColumn("TAGTYPE",coral::AttributeSpecification::typeNameForType<std::string>(), MAX_TAGTYPE_LENGTH, false);
  //unused:   datadictDescr.insertColumn("TABLENAME",coral::AttributeSpecification::typeNameForType<std::string>(), MAX_TABLENAME_LENGTH, false);
  //unused:   datadictDescr.setPrimaryKey(OBJECTTYPE_DATATYPE);
  //unused:   m_session->nominalSchema().createTable( datadictDescr );
  //unused:   m_session->nominalSchema().tableHandle( datadictDescr.name() ).privilegeManager().grantToPublic(coral::ITablePrivilegeManager::Select);
  //unused: }

  // Create connection table
  {
    coral::TableDescription connDescr;
    connDescr.setName(TABLENAME_PREFIX+"CONNECTIONS");
    connDescr.insertColumn("ID",coral::AttributeSpecification::typeNameForType<std::string>(), MAX_ID_LENGTH, false);
    connDescr.insertColumn("IDTAG",coral::AttributeSpecification::typeNameForType<std::string>(), MAX_TAG_LENGTH, false);
    connDescr.insertColumn("TAG",coral::AttributeSpecification::typeNameForType<string>(), MAX_ID_LENGTH, false);
    connDescr.insertColumn("SLOT",coral::AttributeSpecification::typeNameForType<std::string>(), MAX_SLOT_LENGTH, false);
    connDescr.insertColumn("TOID",coral::AttributeSpecification::typeNameForType<std::string>(), MAX_ID_LENGTH, false);
    connDescr.insertColumn("TOSLOT",coral::AttributeSpecification::typeNameForType<std::string>(), MAX_SLOT_LENGTH, false);
    connDescr.setPrimaryKey(makeStringVector("ID", "IDTAG", "TAG", "SLOT"));
    // TOID and TOSLOT must also be unique. Can impose constraint in the current scheme without revision sharing.
    connDescr.setUniqueConstraint(makeStringVector("TOID", "IDTAG", "TAG", "TOSLOT"), TABLENAME_PREFIX+"CONNECTIONS_UC1");
    connDescr.setNotNullConstraint("TOID");
    connDescr.setNotNullConstraint("TOSLOT");
    // explicit NOT NULL constraints are needed *in addition* to the primary key, at least for SQLite
    connDescr.setNotNullConstraint("ID");
    connDescr.setNotNullConstraint("IDTAG");
    connDescr.setNotNullConstraint("TAG");
    connDescr.setNotNullConstraint("SLOT");
    connDescr.createForeignKey(TABLENAME_PREFIX+"CT_FROMIDTAG_FK", ID_IDTAG, TABLENAME_PREFIX+"OBJECTDICT", ID_IDTAG);
    connDescr.createIndex(TABLENAME_PREFIX+"CT_FROMIDTAG_FKI", ID_IDTAG);
    connDescr.createForeignKey(TABLENAME_PREFIX+"CT_TOID_FK", makeStringVector("TOID", "IDTAG"), TABLENAME_PREFIX+"OBJECTDICT", ID_IDTAG);
    connDescr.createIndex(TABLENAME_PREFIX+"CT_TOID_FKI", makeStringVector("TOID", "IDTAG"));
    connDescr.createForeignKey(TABLENAME_PREFIX+"CT_TAG_FK", IDTAG_TAG, TABLENAME_PREFIX+"CONNECTIONS_TAGS", IDTAG_TAG);
    connDescr.createIndex(TABLENAME_PREFIX+"CT_TAG_FKI", IDTAG_TAG);
    m_session->nominalSchema().createTable( connDescr );
    m_session->nominalSchema().tableHandle( connDescr.name() ).privilegeManager().grantToPublic(coral::ITablePrivilegeManager::Select);
  }
  
  // Create aliases table
  {
    coral::TableDescription aliasesDescr;
    aliasesDescr.setName(TABLENAME_PREFIX+"ALIASES");
    aliasesDescr.insertColumn("ALIAS",coral::AttributeSpecification::typeNameForType<std::string>(), MAX_ALIAS_LENGTH, false);
    aliasesDescr.insertColumn("TAG",coral::AttributeSpecification::typeNameForType<std::string>(), MAX_ALIAS_LENGTH, false);
    aliasesDescr.insertColumn("CONVENTION",coral::AttributeSpecification::typeNameForType<std::string>(), MAX_CONVENTION_LENGTH, false);
    aliasesDescr.insertColumn("ID",coral::AttributeSpecification::typeNameForType<std::string>(), MAX_ID_LENGTH, false);
    aliasesDescr.insertColumn("IDTAG",coral::AttributeSpecification::typeNameForType<std::string>(), MAX_TAG_LENGTH, false);
    aliasesDescr.setPrimaryKey(makeStringVector("ALIAS", "TAG", "IDTAG", "CONVENTION"));
    // Should we allow more than one alias for a given ID in the _same_ CONVENTION?
    // If so, remove findAlias method since it doesn't make sense anymore
    aliasesDescr.setUniqueConstraint(makeStringVector("ID", "IDTAG", "TAG", "CONVENTION"), TABLENAME_PREFIX+"ALIASES_UC1");
    // explicit NOT NULL constraints are needed *in addition* to the primary key, at least for SQLite
    aliasesDescr.setNotNullConstraint("ALIAS");
    aliasesDescr.setNotNullConstraint("TAG");
    aliasesDescr.setNotNullConstraint("CONVENTION");
    aliasesDescr.setNotNullConstraint("ID");
    aliasesDescr.setNotNullConstraint("IDTAG");
    aliasesDescr.createForeignKey(TABLENAME_PREFIX+"AT_TAG_FK", IDTAG_TAG, TABLENAME_PREFIX+"ALIASES_TAGS", IDTAG_TAG);
    aliasesDescr.createIndex(TABLENAME_PREFIX+"AT_TAG_FKI", IDTAG_TAG);
    aliasesDescr.createForeignKey(TABLENAME_PREFIX+"AT_IDTAG_FK", ID_IDTAG, TABLENAME_PREFIX+"OBJECTDICT", ID_IDTAG);
    aliasesDescr.createIndex(TABLENAME_PREFIX+"AT_IDTAG_FKI", ID_IDTAG);
    m_session->nominalSchema().createTable( aliasesDescr );
    m_session->nominalSchema().tableHandle( aliasesDescr.name() ).privilegeManager().grantToPublic(coral::ITablePrivilegeManager::Select);
   }

  // Create CLOB payload table
  {
    coral::TableDescription clobDescr;
    clobDescr.setName(TABLENAME_PREFIX+"CLOBS");
    clobDescr.insertColumn("ID",coral::AttributeSpecification::typeNameForType<std::string>(), MAX_ID_LENGTH, false);
    clobDescr.insertColumn("IDTAG",coral::AttributeSpecification::typeNameForType<std::string>(), MAX_TAG_LENGTH, false);
    clobDescr.insertColumn("REV",coral::AttributeSpecification::typeNameForType<int>());
    clobDescr.insertColumn("FIELD",coral::AttributeSpecification::typeNameForType<std::string>(), MAX_FIELDNAME_LENGTH, false);
    // Hmm, this is not really a CLOB!  CLOB in Oracle may be almost 4GB big, while string is limited to 4000 bytes.  But there is no CLOB in Coral...
    clobDescr.insertColumn("DATA",coral::AttributeSpecification::typeNameForType<std::string>() /* , Coral uses max possible size by default */ );
    clobDescr.setPrimaryKey(makeStringVector("REV", "ID", "IDTAG", "FIELD"));
    // explicit NOT NULL constraints are needed *in addition* to the primary key, at least for SQLite
    clobDescr.setNotNullConstraint("ID");
    clobDescr.setNotNullConstraint("IDTAG");
    clobDescr.setNotNullConstraint("REV");
    clobDescr.setNotNullConstraint("FIELD");
    clobDescr.createForeignKey(TABLENAME_PREFIX+"CLOBS_IDTAG_FK", ID_IDTAG, TABLENAME_PREFIX+"OBJECTDICT", ID_IDTAG);
    clobDescr.createIndex(TABLENAME_PREFIX+"CLOBS_IDTAG_FKI", ID_IDTAG);
    m_session->nominalSchema().createTable( clobDescr );
    m_session->nominalSchema().tableHandle( clobDescr.name() ).privilegeManager().grantToPublic(coral::ITablePrivilegeManager::Select);
  }

  //unused:  // Create BLOB payload table
  //unused:  coral::TableDescription blobDescr;
  //unused:  blobDescr.setName(TABLENAME_PREFIX+"BLOBS");
  //unused:  blobDescr.insertColumn("ID",coral::AttributeSpecification::typeNameForType<std::string>(), MAX_ID_LENGTH, false);
  //unused:  blobDescr.insertColumn("REV",coral::AttributeSpecification::typeNameForType<int>());
  //unused:  blobDescr.insertColumn("FIELD",coral::AttributeSpecification::typeNameForType<std::string>(), MAX_FIELDNAME_LENGTH, false);
  //unused:  blobDescr.insertColumn("DATA",coral::AttributeSpecification::typeNameForType<coral::Blob>());
  //unused:  blobDescr.setPrimaryKey(REV_ID_FIELD);
  //unused:  // explicit NOT NULL constraints are needed *in addition* to the primary key, at least for SQLite
  //unused:  blobDescr.setNotNullConstraint("ID");
  //unused:  blobDescr.setNotNullConstraint("REV");
  //unused:  blobDescr.setNotNullConstraint("FIELD");
  //unused:  m_session->nominalSchema().createTable( blobDescr );

  // Create revision table for payloads.
  // It should contain en entry for each ID and each TAG.
  {
    coral::TableDescription revDescr;
    revDescr.setName(TABLENAME_PREFIX+"CLOBS_REVS");
    revDescr.insertColumn("ID",coral::AttributeSpecification::typeNameForType<std::string>(), MAX_ID_LENGTH, false);
    revDescr.insertColumn("IDTAG",coral::AttributeSpecification::typeNameForType<std::string>(), MAX_TAG_LENGTH, false);
    revDescr.insertColumn("TAG",coral::AttributeSpecification::typeNameForType<std::string>(), MAX_TAG_LENGTH, false);
    revDescr.insertColumn("REV",coral::AttributeSpecification::typeNameForType<int>());
    revDescr.setPrimaryKey(makeStringVector("ID", "IDTAG", "TAG"));
    revDescr.setNotNullConstraint("REV");
    // explicit NOT NULL constraints are needed *in addition* to the primary key, at least for SQLite
    revDescr.setNotNullConstraint("ID");
    revDescr.setNotNullConstraint("IDTAG");
    revDescr.setNotNullConstraint("TAG");
    revDescr.createForeignKey(TABLENAME_PREFIX+"PT_IDTAG_FK", ID_IDTAG, TABLENAME_PREFIX+"OBJECTDICT", ID_IDTAG);
    revDescr.createIndex(TABLENAME_PREFIX+"PT_IDTAG_FKI", ID_IDTAG);
    revDescr.createForeignKey(TABLENAME_PREFIX+"PT_TAG_FK", IDTAG_TAG, TABLENAME_PREFIX+"CLOBS_TAGS", IDTAG_TAG);
    revDescr.createIndex(TABLENAME_PREFIX+"PT_TAG_FKI", IDTAG_TAG);
    m_session->nominalSchema().createTable( revDescr );
    m_session->nominalSchema().tableHandle( revDescr.name() ).privilegeManager().grantToPublic(coral::ITablePrivilegeManager::Select);
  }

  {
    coral::TableDescription tableDescr;
    tableDescr.setName(TABLENAME_PREFIX+"TAG_HISTORY");
    tableDescr.insertColumn("IOV_BEGIN",coral::AttributeSpecification::typeNameForType<time_t>());

    tableDescr.insertColumn("IDTAG",coral::AttributeSpecification::typeNameForType<std::string>(), /*size = */ MAX_TAG_LENGTH, /*fixed_size=*/ false);
    tableDescr.insertColumn("CONNECTIVITY_TAG",coral::AttributeSpecification::typeNameForType<std::string>(), /*size = */ MAX_TAG_LENGTH, /*fixed_size=*/ false);
    tableDescr.insertColumn("DATA_TAG",coral::AttributeSpecification::typeNameForType<std::string>(), /*size = */ MAX_TAG_LENGTH, /*fixed_size=*/ false);
    tableDescr.insertColumn("ALIAS_TAG",coral::AttributeSpecification::typeNameForType<std::string>(), /*size = */ MAX_TAG_LENGTH, /*fixed_size=*/ false);

    tableDescr.setNotNullConstraint("IOV_BEGIN");
    tableDescr.setNotNullConstraint("IDTAG");
    tableDescr.setNotNullConstraint("CONNECTIVITY_TAG");
    tableDescr.setNotNullConstraint("DATA_TAG");
    tableDescr.setNotNullConstraint("ALIAS_TAG");

    tableDescr.createForeignKey(TABLENAME_PREFIX+"CUR_OT_FK", "IDTAG", TABLENAME_PREFIX+"OBJECTDICT_TAGS", "IDTAG");
    tableDescr.createIndex(TABLENAME_PREFIX+"CUR_OT_FKI", "IDTAG");
    tableDescr.createForeignKey(TABLENAME_PREFIX+"CUR_CT_FK", makeStringVector("IDTAG", "CONNECTIVITY_TAG"), TABLENAME_PREFIX+"CONNECTIONS_TAGS", IDTAG_TAG);
    tableDescr.createIndex(TABLENAME_PREFIX+"CUR_CT_FKI", makeStringVector("IDTAG", "CONNECTIVITY_TAG"));
    tableDescr.createForeignKey(TABLENAME_PREFIX+"CUR_DT_FK", makeStringVector("IDTAG", "DATA_TAG"), TABLENAME_PREFIX+"CLOBS_TAGS", IDTAG_TAG);
    tableDescr.createIndex(TABLENAME_PREFIX+"CUR_DT_FKI", makeStringVector("IDTAG", "DATA_TAG"));
    tableDescr.createForeignKey(TABLENAME_PREFIX+"CUR_AT_FK", makeStringVector("IDTAG", "ALIAS_TAG"), TABLENAME_PREFIX+"ALIASES_TAGS", IDTAG_TAG);
    tableDescr.createIndex(TABLENAME_PREFIX+"CUR_AT_FKI", makeStringVector("IDTAG", "ALIAS_TAG"));

    m_session->nominalSchema().createTable( tableDescr );
    m_session->nominalSchema().tableHandle( tableDescr.name() ).privilegeManager().grantToPublic(coral::ITablePrivilegeManager::Select);
  }

  tg.commit();
}


void CoralDB::dropSchema() {
  UpdateTransactionGuard tg(this);
  
  // Delete  leftovers from previous versions of CoralDB
  m_session->nominalSchema().dropIfExistsTable(TABLENAME_PREFIX+"REVISIONTABLE");
  m_session->nominalSchema().dropIfExistsTable(TABLENAME_PREFIX+"IDMAP");
  m_session->nominalSchema().dropIfExistsTable(TABLENAME_PREFIX+"CONNECTIVITY_TAGS");
  m_session->nominalSchema().dropIfExistsTable(TABLENAME_PREFIX+"PAYLOAD_TAGS");

  // Tables are dropped in the reverse creation order to make
  // sure foreign key dependencies are not violated.
  m_session->nominalSchema().dropIfExistsTable(TABLENAME_PREFIX+"TAG_HISTORY");
  m_session->nominalSchema().dropIfExistsTable(TABLENAME_PREFIX+"CLOBTABLE_TAGS"); // leftovers
  m_session->nominalSchema().dropIfExistsTable(TABLENAME_PREFIX+"CLOBS_REVS");
  m_session->nominalSchema().dropIfExistsTable(TABLENAME_PREFIX+"BLOBTABLE"); // leftovers
  m_session->nominalSchema().dropIfExistsTable(TABLENAME_PREFIX+"BLOBS");
  m_session->nominalSchema().dropIfExistsTable(TABLENAME_PREFIX+"CLOBTABLE"); // leftovers
  m_session->nominalSchema().dropIfExistsTable(TABLENAME_PREFIX+"CLOBS");
  m_session->nominalSchema().dropIfExistsTable(TABLENAME_PREFIX+"ALIASTABLE"); // leftovers
  m_session->nominalSchema().dropIfExistsTable(TABLENAME_PREFIX+"ALIASES");
  m_session->nominalSchema().dropIfExistsTable(TABLENAME_PREFIX+"CONNECTIONTABLE"); // leftovers
  m_session->nominalSchema().dropIfExistsTable(TABLENAME_PREFIX+"CONNECTIONS");
  m_session->nominalSchema().dropIfExistsTable(TABLENAME_PREFIX+"DATADICTIONARY"); // leftovers
  m_session->nominalSchema().dropIfExistsTable(TABLENAME_PREFIX+"DATADICT");
  m_session->nominalSchema().dropIfExistsTable(TABLENAME_PREFIX+"MASTERLIST");
  m_session->nominalSchema().dropIfExistsTable(TABLENAME_PREFIX+"OBJECTDICTIONARY"); // leftovers
  m_session->nominalSchema().dropIfExistsTable(TABLENAME_PREFIX+"OBJECTDICT");
  m_session->nominalSchema().dropIfExistsTable(TABLENAME_PREFIX+"ALIASTABLE_TAGS"); // leftovers
  m_session->nominalSchema().dropIfExistsTable(TABLENAME_PREFIX+"ALIASES_TAGS");
  m_session->nominalSchema().dropIfExistsTable(TABLENAME_PREFIX+"CLOBTABLE_TAGS"); // leftovers
  m_session->nominalSchema().dropIfExistsTable(TABLENAME_PREFIX+"CLOBS_TAGS");
  m_session->nominalSchema().dropIfExistsTable(TABLENAME_PREFIX+"CONNECTIONS_TAGS");
  m_session->nominalSchema().dropIfExistsTable(TABLENAME_PREFIX+"OBJECTDICT_TAGS");

  tg.commit();
}


void CoralDB::insertIdTagListTableRow(const std::string& newIdTag) {
  const std::string tablename = TABLENAME_PREFIX+"OBJECTDICT_TAGS";
  coral::ITableDataEditor& editor = m_session->nominalSchema().tableHandle(tablename).dataEditor();
  coral::AttributeList objectBuf;
  objectBuf.extend<string>("IDTAG");
  objectBuf.extend<bool>("LOCKED");
  objectBuf["IDTAG"].data<string>() = newIdTag;
  objectBuf["LOCKED"].data<bool>() = false;
  editor.insertRow(objectBuf);
}


void CoralDB::insertTagListTableRow(const string& tablename, const string& newTag, const string& newIdTag) {
  if(newIdTag.empty()) {
    throw runtime_error("Error: insertTagListTableRow() called with emtpy newIdTag");
  }

  if(newTag.empty()) {
    throw runtime_error("insertTagListTableRow() called with emtpy newTag");
  }

  coral::ITableDataEditor& editor = m_session->nominalSchema().tableHandle(tablename).dataEditor();
  coral::AttributeList objectBuf;
  objectBuf.extend<string>("IDTAG");
  objectBuf.extend<string>("TAG");
  objectBuf.extend<bool>("LOCKED");
  objectBuf["IDTAG"].data<string>() = newIdTag;
  objectBuf["TAG"].data<string>() = newTag;
  objectBuf["LOCKED"].data<bool>() = false;
  editor.insertRow(objectBuf);
}


int CoralDB::highestRevision(const std::string& tagsTableName) const {
  std::unique_ptr<coral::IQuery> query(m_session->nominalSchema().tableHandle(tagsTableName).newQuery());
  query->addToOutputList("COALESCE(MAX(REV),-1)", "RESULT");
  query->defineOutputType("RESULT", "int");
  coral::ICursor& c = query->execute();
  return  c.next() ? c.currentRow()[0].data<int>() : -1;
}


// All IDs will be set to newRevision in newTag
void CoralDB::copyRevisions(const string& newIdTag, const string& newTag, int newRevision) {
  const string tagsTableName =   TABLENAME_PREFIX+"CLOBS_REVS";

  std::ostringstream os;
  os<<newRevision;
  string newRevisionString = os.str();

  coral::ITableDataEditor& editor = m_session->nominalSchema().tableHandle(tagsTableName).dataEditor();
  std::unique_ptr<coral::IOperationWithQuery> operation(editor.insertWithQuery());
  coral::IQueryDefinition& subquery = operation->query();
  subquery.addToTableList(tagsTableName);
  subquery.addToOutputList("ID");
  subquery.addToOutputList("'"+newIdTag+"'");
  subquery.addToOutputList("'"+newTag+"'");
  subquery.addToOutputList(newRevisionString); // simply "REV" here if willing to share data

  coral::AttributeList data;
  data.extend<string>("IDTAG");
  data.extend<string>("TAG");
  data["IDTAG"].data<string>() = m_tags.objectDictionaryTag();
  data["TAG"].data<string>() = m_tags.dataTag();
  // copy all IDs for oldTag to newTag with newRev
  subquery.setCondition("IDTAG = :IDTAG AND TAG = :TAG", data);
  operation->execute();
}


void CoralDB::copyConnectivityTag(const string& newtag, const string& tmpIdTag) {
  const string newIdTag = tmpIdTag.empty() ? m_tags.objectDictionaryTag() : tmpIdTag;

  UpdateTransactionGuard tg(this);

  insertTagListTableRow(TABLENAME_PREFIX+"CONNECTIONS_TAGS", newtag, newIdTag );
  
  { // copy MASTERLIST    
    coral::ITableDataEditor& editor = m_session->nominalSchema().tableHandle(TABLENAME_PREFIX+"MASTERLIST").dataEditor();
    std::unique_ptr<coral::IOperationWithQuery> operation(editor.insertWithQuery());
    coral::IQueryDefinition& subquery = operation->query();
    subquery.addToTableList(TABLENAME_PREFIX+"MASTERLIST");
    
    subquery.addToOutputList("'"+newtag+"'");
    subquery.addToOutputList("ID");
    subquery.addToOutputList("'"+newIdTag+"'");
    
    coral::AttributeList data;
    data.extend<string>("IDTAG");
    data.extend<string>("TAG");
    data["IDTAG"].data<string>() = m_tags.objectDictionaryTag();
    data["TAG"].data<string>() = m_tags.connectivityTag();
    // copy all rows for the current tag into new tag
    subquery.setCondition("IDTAG = :IDTAG AND TAG = :TAG", data);
    operation->execute();
  }

  { // Copy CONNECTIONS
    coral::ITableDataEditor& editor = m_session->nominalSchema().tableHandle(TABLENAME_PREFIX+"CONNECTIONS").dataEditor();
    std::unique_ptr<coral::IOperationWithQuery> operation(editor.insertWithQuery());
    coral::IQueryDefinition& subquery = operation->query();
    subquery.addToTableList(TABLENAME_PREFIX+"CONNECTIONS");

    subquery.addToOutputList("ID");
    subquery.addToOutputList("'"+newIdTag+"'");
    subquery.addToOutputList("'"+newtag+"'"); // Connectivity TAG
    subquery.addToOutputList("SLOT");
    subquery.addToOutputList("TOID");
    subquery.addToOutputList("TOSLOT");
    
    coral::AttributeList data;
    data.extend<string>("IDTAG");
    data.extend<string>("TAG");
    data["IDTAG"].data<string>() = m_tags.objectDictionaryTag();
    data["TAG"].data<string>() = m_tags.connectivityTag();
    // copy all rows for the current tag into new tag
    subquery.setCondition("IDTAG = :IDTAG AND TAG = :TAG", data);
    operation->execute();
  }

  tg.commit();
}


void CoralDB::copyDataTag(const string& newtag, const string& tmpIdTag) {
  const string newIdTag = tmpIdTag.empty() ? m_tags.objectDictionaryTag() : tmpIdTag;
  
  UpdateTransactionGuard tg(this);
  
  insertTagListTableRow(TABLENAME_PREFIX+"CLOBS_TAGS", newtag, newIdTag );

  // Force the same new revision for all IDs.  When tags are not shared, it is the case anyway.
  int newRevision = 1 + highestRevision(TABLENAME_PREFIX+"CLOBS_REVS");
    
  // Copy data to not worry about sharing of CLOBS among different tags
  // This block will not be needed if data are shared among revisions.
  if(1) {
    std::ostringstream os;
    os<<newRevision;
    string newRevisionString = os.str();

    coral::ITableDataEditor& editor = m_session->nominalSchema().tableHandle(TABLENAME_PREFIX+"CLOBS").dataEditor();
    std::unique_ptr<coral::IOperationWithQuery> operation(editor.insertWithQuery());
    coral::IQueryDefinition& subquery = operation->query();
    subquery.addToTableList(TABLENAME_PREFIX+"CLOBS", "C");
    subquery.addToTableList(TABLENAME_PREFIX+"CLOBS_REVS", "T");
    subquery.addToOutputList("C.ID");
    subquery.addToOutputList("'"+newIdTag+"'");
    subquery.addToOutputList("'"+newRevisionString+"'");
    subquery.addToOutputList("FIELD");
    subquery.addToOutputList("DATA");
      
    coral::AttributeList data;
    data.extend<string>("IDTAG");
    data.extend<string>("TAG");
    data["IDTAG"].data<string>() = m_tags.objectDictionaryTag();
    data["TAG"].data<string>() = m_tags.dataTag();
    // select all data rows for all IDs for the current tag
    subquery.setCondition("T.IDTAG = :IDTAG AND T.TAG = :TAG AND T.REV = C.REV AND T.ID = C.ID", data);
    operation->execute();
  }

  copyRevisions(newIdTag, newtag, newRevision);
  tg.commit();
}


void CoralDB::copyAliasTag(const string& newtag, const string& tmpIdTag) {
  const string newIdTag = tmpIdTag.empty() ? m_tags.objectDictionaryTag() : tmpIdTag;

  UpdateTransactionGuard tg(this);

  insertTagListTableRow(TABLENAME_PREFIX+"ALIASES_TAGS", newtag, newIdTag );

  coral::ITableDataEditor& editor = m_session->nominalSchema().tableHandle(TABLENAME_PREFIX+"ALIASES").dataEditor();
  std::unique_ptr<coral::IOperationWithQuery> operation(editor.insertWithQuery());
  coral::IQueryDefinition& subquery = operation->query();
  subquery.addToTableList(TABLENAME_PREFIX+"ALIASES");

  subquery.addToOutputList("ALIAS");
  subquery.addToOutputList("'"+newtag+"'");
  subquery.addToOutputList("CONVENTION");
  subquery.addToOutputList("ID");
  subquery.addToOutputList("'"+newIdTag+"'");
    
  coral::AttributeList data;
  data.extend<string>("IDTAG");
  data.extend<string>("TAG");
  data["IDTAG"].data<string>() = m_tags.objectDictionaryTag();
  data["TAG"].data<string>() = m_tags.aliasTag();
  // copy all rows for the current tag into new tag
  subquery.setCondition("IDTAG = :IDTAG AND TAG = :TAG", data);
  operation->execute();

  tg.commit();
}

long CoralDB::copyIdTagRows(const string& tableTo, const string& tableFrom, const string& srcIdTag, const vector<string>& columns) 
{
  coral::ITableDataEditor& editor = m_session->nominalSchema().tableHandle(tableTo).dataEditor();
  std::unique_ptr<coral::IOperationWithQuery> operation(editor.insertWithQuery());
  coral::IQueryDefinition& subquery = operation->query();
  subquery.addToTableList(tableFrom);
  
  for(vector<string>::const_iterator i=columns.begin(); i!=columns.end(); i++) {
    subquery.addToOutputList(*i);
  }
  
  coral::AttributeList data;
  data.extend<string>("IDTAG");
  data["IDTAG"].data<string>() = srcIdTag;
  subquery.setCondition("IDTAG = :IDTAG", data);
  return operation->execute();
}


// Copy just ObjectDict
void CoralDB::copyObjectDictionaryTag(const string& newIdTag) {
  UpdateTransactionGuard tg(this);

  insertIdTagListTableRow(newIdTag);

  copyIdTagRows(TABLENAME_PREFIX+"OBJECTDICT",
		TABLENAME_PREFIX+"OBJECTDICT",
		m_tags.objectDictionaryTag(),
		makeStringVector("ID", "'"+newIdTag+"'", "TYPE") );


  tg.commit();
}


void CoralDB::copyMissingObjects(const string& newIdTag, const CompoundTag &from) {
  UpdateTransactionGuard tg(this);

  //----------------------------------------------------------------
  // INSERT INTO OBJECTDICT ...
  coral::ITableDataEditor& editor = m_session->nominalSchema().tableHandle(TABLENAME_PREFIX+"OBJECTDICT").dataEditor();
  std::unique_ptr<coral::IOperationWithQuery> operation(editor.insertWithQuery());
  coral::IQueryDefinition& topquery = operation->query();

  //----------------------------------------------------------------
  // ... SELECT * FROM ( big union query ) ...

  coral::IQueryDefinition& topleft = topquery.defineSubQuery("topleft");
  topquery.addToTableList("topleft");
  topquery.addToOutputList("ID");
  topquery.addToOutputList("'"+newIdTag+"'");
  topquery.addToOutputList("TYPE");

  std::vector<coral::AttributeList> vdata;
  fillUsedIdsQuery(topleft, vdata, from);
  
  //----------------------------------------------------------------
  // ... EXCEPT SELECT * FROM OBJECTDICT WHERE IDTAG = newIdTag;
  coral::IQueryDefinition& topright = topquery.applySetOperation(coral::IQueryDefinition::Minus);

  topright.addToTableList(TABLENAME_PREFIX+"OBJECTDICT");
  topright.addToOutputList("*");

  coral::AttributeList trdata;
  trdata.extend<string>("IDTAG");
  trdata["IDTAG"].data<string>() = newIdTag;

  topright.setCondition("IDTAG = :IDTAG", trdata);
  //----------------------------------------------------------------

  operation->execute();

  tg.commit();
}


void CoralDB::fillUsedIdsQuery(coral::IQueryDefinition& queryToFill,
			       std::vector<coral::AttributeList> & data,
			       const CompoundTag& tags)
  const
{
  if(!tags.isValid()) {
    throw InvalidCompoundTag("fillBigUnionQuery()", tags);
  }
  if(tags.empty()) {
    throw EmptyTag("fillBigUnionQuery(): arg CompoundTag should not be empty.");
  }
  
  // Get a subset of IDs according to the requested tags. 
  
  coral::IQueryDefinition *current_query(0);
  
  if(!tags.connectivityTag().empty()) {
    
    coral::IQueryDefinition& cnquery = current_query ? 
      current_query->applySetOperation(coral::IQueryDefinition::Union) : 
      queryToFill;
    
    current_query = &cnquery;
    
    cnquery.addToTableList(TABLENAME_PREFIX+"OBJECTDICT", "CNOD");
    cnquery.addToTableList(TABLENAME_PREFIX+"CONNECTIONS", "CN");
    cnquery.addToOutputList("CNOD.ID", "ID");
    cnquery.addToOutputList("CNOD.TYPE", "TYPE");
    
    data.push_back(coral::AttributeList());
    data.back().extend<string>("UIQIDTAG");
    data.back()["UIQIDTAG"].data<string>() = tags.objectDictionaryTag();
    data.back().extend<string>("CTAG");
    data.back()["CTAG"].data<string>() = tags.connectivityTag();

    cnquery.setCondition("CNOD.IDTAG = CN.IDTAG AND ( CNOD.ID = CN.ID OR CNOD.ID = CN.TOID )"
			 " AND CN.IDTAG = :UIQIDTAG AND CN.TAG = :CTAG"
			 , data.back() );

    if(1) {
      coral::IQueryDefinition& mlquery = current_query->applySetOperation(coral::IQueryDefinition::Union);
      current_query = &mlquery;
	
      mlquery.addToTableList(TABLENAME_PREFIX+"OBJECTDICT", "MLOD");
      mlquery.addToTableList(TABLENAME_PREFIX+"MASTERLIST", "ML");
      mlquery.addToOutputList("MLOD.ID");
      mlquery.addToOutputList("MLOD.TYPE");

      data.push_back(coral::AttributeList());
      data.back().extend<string>("UIQIDTAG");
      data.back()["UIQIDTAG"].data<string>() = tags.objectDictionaryTag();
      data.back().extend<string>("CTAG");
      data.back()["CTAG"].data<string>() = tags.connectivityTag();

      mlquery.setCondition("MLOD.IDTAG = ML.IDTAG AND MLOD.ID = ML.ID AND ML.IDTAG = :UIQIDTAG AND ML.TAG = :CTAG", data.back());
    }
  }

  if(!tags.aliasTag().empty()) {
    coral::IQueryDefinition& alquery = current_query ? 
      current_query->applySetOperation(coral::IQueryDefinition::Union) : 
      queryToFill;

    current_query = &alquery;

    alquery.addToTableList(TABLENAME_PREFIX+"OBJECTDICT", "ALOD");
    alquery.addToTableList(TABLENAME_PREFIX+"ALIASES", "AL");
    alquery.addToOutputList("ALOD.ID", "ID");
    alquery.addToOutputList("ALOD.TYPE", "TYPE");

    data.push_back(coral::AttributeList());
    data.back().extend<string>("UIQIDTAG");
    data.back()["UIQIDTAG"].data<string>() = tags.objectDictionaryTag();
    data.back().extend<string>("ATAG");
    data.back()["ATAG"].data<string>() = tags.aliasTag();

    alquery.setCondition("ALOD.IDTAG = AL.IDTAG AND ALOD.ID = AL.ID AND AL.IDTAG = :UIQIDTAG AND AL.TAG = :ATAG", data.back());
  }

  if(!tags.dataTag().empty()) {
    coral::IQueryDefinition& daquery = current_query ? 
      current_query->applySetOperation(coral::IQueryDefinition::Union) : 
      queryToFill;
    
    current_query = &daquery;
    daquery.addToTableList(TABLENAME_PREFIX+"OBJECTDICT", "DAOD");
    daquery.addToTableList(TABLENAME_PREFIX+"CLOBS_REVS", "DA");
    daquery.addToOutputList("DAOD.ID", "ID");
    daquery.addToOutputList("DAOD.TYPE", "TYPE");

    data.push_back(coral::AttributeList());
    data.back().extend<string>("UIQIDTAG");
    data.back()["UIQIDTAG"].data<string>() = tags.objectDictionaryTag();
    data.back().extend<string>("DTAG");
    data.back()["DTAG"].data<string>() = tags.dataTag();
    daquery.setCondition("DAOD.IDTAG = DA.IDTAG AND DAOD.ID = DA.ID AND DA.IDTAG = :UIQIDTAG AND DA.TAG = :DTAG", data.back());
  }

}


// Copy everything in current IDTAG to the new one
void CoralDB::copyAllForObjectDictionaryTag(const string& newIdTag) {
  UpdateTransactionGuard tg(this);

  insertIdTagListTableRow(newIdTag);

  //----------------------------------------------------------------
  // Handle ObjectDict

  copyIdTagRows(TABLENAME_PREFIX+"OBJECTDICT",
		TABLENAME_PREFIX+"OBJECTDICT",
		m_tags.objectDictionaryTag(),
		makeStringVector("ID", "'"+newIdTag+"'", "TYPE") );


  //----------------------------------------------------------------
  // Handle connectivity
  
  copyIdTagRows(TABLENAME_PREFIX+"CONNECTIONS_TAGS",
		TABLENAME_PREFIX+"CONNECTIONS_TAGS",
		m_tags.objectDictionaryTag(),
		makeStringVector("'"+newIdTag+"'", "TAG", "0") );

  copyIdTagRows(TABLENAME_PREFIX+"MASTERLIST",
		TABLENAME_PREFIX+"MASTERLIST",
		m_tags.objectDictionaryTag(),
		makeStringVector("TAG", "ID", "'"+newIdTag+"'") );

  copyIdTagRows(TABLENAME_PREFIX+"CONNECTIONS",
		TABLENAME_PREFIX+"CONNECTIONS",
		m_tags.objectDictionaryTag(),
		makeStringVector("ID", "'"+newIdTag+"'", "TAG", "SLOT", "TOID", "TOSLOT") );

  //----------------------------------------------------------------
  // CLOBS

  copyIdTagRows(TABLENAME_PREFIX+"CLOBS_TAGS",
		TABLENAME_PREFIX+"CLOBS_TAGS",
		m_tags.objectDictionaryTag(),
		makeStringVector("'"+newIdTag+"'", "TAG", "0"/*FALSE*/) );
		
  // copy clob data
  copyIdTagRows(TABLENAME_PREFIX+"CLOBS",
		TABLENAME_PREFIX+"CLOBS",
		m_tags.objectDictionaryTag(),
		makeStringVector("ID", "'"+newIdTag+"'", "REV", "FIELD", "DATA") );


  // Copy revisions
  copyIdTagRows(TABLENAME_PREFIX+"CLOBS_REVS",
		TABLENAME_PREFIX+"CLOBS_REVS",
		m_tags.objectDictionaryTag(),
		makeStringVector("ID", "'"+newIdTag+"'", "TAG", "REV") );

  //----------------------------------------------------------------
  // ALIASES

  copyIdTagRows(TABLENAME_PREFIX+"ALIASES_TAGS",
		TABLENAME_PREFIX+"ALIASES_TAGS",
		m_tags.objectDictionaryTag(),
		makeStringVector("'"+newIdTag+"'", "TAG", "0") );

  copyIdTagRows(TABLENAME_PREFIX+"ALIASES",
		TABLENAME_PREFIX+"ALIASES",
		m_tags.objectDictionaryTag(),
		makeStringVector("ALIAS", "TAG", "CONVENTION", "ID", "'"+newIdTag+"'") );

  //----------------------------------------------------------------

  tg.commit();
}


bool CoralDB::deleteConnectivityTag(const string& tag, const string& tmpIdTag) {
  const string idTag = tmpIdTag.empty() ? m_tags.objectDictionaryTag() : tmpIdTag;
  
  UpdateTransactionGuard tg(this);

  bool isLocked = findEntry<bool>(TABLENAME_PREFIX+"CONNECTIONS_TAGS", "IDTAG", idTag, "TAG", tag, "LOCKED");
  if(isLocked) {
    throw TagLocked("deleteConnectivityTag(): attempt to modify locked tag \""+tag+"\"");
  }

  unsigned nHist = findEntry<unsigned>(TABLENAME_PREFIX+"TAG_HISTORY", "IDTAG", idTag, "CONNECTIVITY_TAG", tag, "COUNT(*)");
  if(nHist > 0) {
    throw std::runtime_error("deleteConnectivityTag(): can not delete tag \""+tag+"\", it is referenced from the history table.");
  }

  deleteRows(TABLENAME_PREFIX+"MASTERLIST", "IDTAG", idTag, "TAG", tag);
  deleteRows(TABLENAME_PREFIX+"CONNECTIONS", "IDTAG", idTag, "TAG", tag);
  long nRows = deleteRows(TABLENAME_PREFIX+"CONNECTIONS_TAGS", "IDTAG", idTag, "TAG", tag);
  tg.commit();
  return nRows;
}

bool CoralDB::deleteDataTag(const string& tag, const string& tmpIdTag) {
  const string idTag = tmpIdTag.empty() ? m_tags.objectDictionaryTag() : tmpIdTag;

  UpdateTransactionGuard tg(this);
  bool isLocked = findEntry<bool>(TABLENAME_PREFIX+"CLOBS_TAGS", "IDTAG", idTag, "TAG", tag, "LOCKED");
  if(isLocked) {
    throw TagLocked("deleteDataTag(): attempt to modify locked tag \""+tag+"\"");
  }

  unsigned nHist = findEntry<unsigned>(TABLENAME_PREFIX+"TAG_HISTORY", "IDTAG", idTag, "DATA_TAG", tag, "COUNT(*)");
  if(nHist > 0) {
    throw std::runtime_error("deleteDataTag(): can not delete tag \""+tag+"\", it is referenced from the history table.");
  }

  // At present we copy CLOBS so that every CLOB in a tag has the same REV number.
  // findEntry() checks that this is the case.
  // We also need to have a single REV for deletion, because Coral does not yet have 
  // a way to "DELETE FROM table WHERE ... IN (SELECT ...);
  try {
    int rev = findEntry<int>(TABLENAME_PREFIX+"CLOBS_REVS", "TAG", tag, "IDTAG", idTag, "DISTINCT(REV)");
    deleteRows(TABLENAME_PREFIX+"CLOBS", "IDTAG", idTag, "REV", rev);
    deleteRows(TABLENAME_PREFIX+"CLOBS_REVS", "IDTAG", idTag, "TAG", tag);
  }
  catch(const RowNotFound&) {
    // OK, this dataTag did not have any fields => no REV found by findEntry().
    // Only need to delete from CLOBS_TAGS below.
  }

  long nRows = deleteRows(TABLENAME_PREFIX+"CLOBS_TAGS", "IDTAG", idTag, "TAG", tag);
  tg.commit();
  return nRows;
}


bool CoralDB::deleteAliasTag(const string& tag, const string& tmpIdTag) {
  const string idTag = tmpIdTag.empty() ? m_tags.objectDictionaryTag() : tmpIdTag;
  UpdateTransactionGuard tg(this);

  bool isLocked = findEntry<bool>(TABLENAME_PREFIX+"ALIASES_TAGS", "IDTAG", idTag, "TAG", tag, "LOCKED");
  if(isLocked) {
    throw TagLocked("deleteAliasTag(): attempt to modify locked tag \""+tag+"\"");
  }

  unsigned nHist = findEntry<unsigned>(TABLENAME_PREFIX+"TAG_HISTORY", "IDTAG", idTag, "ALIAS_TAG", tag, "COUNT(*)");
  if(nHist > 0) {
    throw std::runtime_error("deleteAliasTag(): can not delete tag \""+tag+"\", it is referenced from the history table.");
  }

  deleteRows(TABLENAME_PREFIX+"ALIASES", "IDTAG", idTag, "TAG", tag);
  long nRows = deleteRows(TABLENAME_PREFIX+"ALIASES_TAGS", "IDTAG", idTag, "TAG", tag);
  tg.commit();
  return nRows;
}


bool CoralDB::deleteAllForObjectDictionaryTag(const string& idTag) {
  UpdateTransactionGuard tg(this);

  unsigned nHist = findEntry<unsigned>(TABLENAME_PREFIX+"TAG_HISTORY", "IDTAG", idTag, "COUNT(*)");
  if(nHist > 0) {
    throw std::runtime_error("deleteAllForObjectDictionaryTag(): can not delete idTag \""+idTag+"\", it is referenced from the history table.");
  }

  if(findEntry<bool>(TABLENAME_PREFIX+"OBJECTDICT_TAGS", "IDTAG", idTag, "LOCKED")) {
    throw TagLocked("deleteAllForObjectDictionaryTag(): attempt to modify locked tag \""+idTag+"\"");
  }

  if(checkForLockedTags(TABLENAME_PREFIX+"CONNECTIONS_TAGS", idTag)) {
    throw TagLocked("deleteAllForObjectDictionaryTag(): a connectivity tag is locked for objectDictionaryTag \""+idTag+"\"");
  }
  
  if(checkForLockedTags(TABLENAME_PREFIX+"CLOBS_TAGS", idTag)) {
    throw TagLocked("deleteAllForObjectDictionaryTag(): a data tag is locked for objectDictionaryTag \""+idTag+"\"");
  }
  
  if(checkForLockedTags(TABLENAME_PREFIX+"ALIASES_TAGS", idTag)) {
    throw TagLocked("deleteAllForObjectDictionaryTag(): an alias tag is locked for objectDictionaryTag \""+idTag+"\"");
  }
  
  // All related connectivity records - don't forget the MasterList
  deleteRows(TABLENAME_PREFIX+"MASTERLIST", "IDTAG", idTag);
  deleteRows(TABLENAME_PREFIX+"CONNECTIONS", "IDTAG", idTag);

  // All related data records
  deleteRows(TABLENAME_PREFIX+"CLOBS", "IDTAG", idTag);
  deleteRows(TABLENAME_PREFIX+"CLOBS_REVS", "IDTAG", idTag);
  
  // All related aliases
  deleteRows(TABLENAME_PREFIX+"ALIASES", "IDTAG", idTag);

  // All subordinate tags
  deleteRows(TABLENAME_PREFIX+"CONNECTIONS_TAGS", "IDTAG", idTag);
  deleteRows(TABLENAME_PREFIX+"CLOBS_TAGS", "IDTAG", idTag);
  deleteRows(TABLENAME_PREFIX+"ALIASES_TAGS", "IDTAG", idTag);

  // All the objects
  deleteRows(TABLENAME_PREFIX + "OBJECTDICT", "IDTAG", idTag);

  // IDTAG itself
  long nRows = deleteRows(TABLENAME_PREFIX + "OBJECTDICT_TAGS", "IDTAG", idTag);

  tg.commit();
  return nRows;
}


void CoralDB::lockObjectDictionaryTag(const string& idTag) {
  UpdateTransactionGuard tg(this);

  long res = updateRows(TABLENAME_PREFIX+"OBJECTDICT_TAGS", "IDTAG", idTag, "LOCKED", true);
  if(res != 1) {
    ostringstream os;
    os<<"lockObjectDictionaryTag(\""<<idTag<<"\"): no such tag?  Number of updated rows = "<<res;
    throw runtime_error(os.str() );
  }

  if(idTag == m_tags.objectDictionaryTag()) {
    m_locked_objectDictionaryTag = true;
  }
  tg.commit();
}


void CoralDB::lockConnectivityTag(const string& tag, const string& tmpIdTag) {
  const string idTag = tmpIdTag.empty() ? m_tags.objectDictionaryTag() : tmpIdTag;
  UpdateTransactionGuard tg(this);
  
  if(!findEntry<bool>(TABLENAME_PREFIX+"OBJECTDICT_TAGS", "IDTAG", idTag, "LOCKED")) {
    throw IdTagUnlocked("lockConnectivityTag(): IDTAG \""+idTag+"\" must be locked before locking its connectivity tag \""+tag+"\"");
  }

  updateRows(TABLENAME_PREFIX+"CONNECTIONS_TAGS", "IDTAG", idTag, "TAG", tag, "LOCKED", true);
  if((idTag == m_tags.objectDictionaryTag()) && (tag == m_tags.connectivityTag())) {
    m_locked_connectivityTag = true;
  }
  tg.commit();
}


void CoralDB::lockDataTag(const string& tag, const string& tmpIdTag) {
  const string idTag = tmpIdTag.empty() ? m_tags.objectDictionaryTag() : tmpIdTag;
  UpdateTransactionGuard tg(this);

  if(!findEntry<bool>(TABLENAME_PREFIX+"OBJECTDICT_TAGS", "IDTAG", idTag, "LOCKED")) {
    throw IdTagUnlocked("lockDataTag(): IDTAG \""+idTag+"\" must be locked before locking its data tag \""+tag+"\"");
  }

  updateRows(TABLENAME_PREFIX+"CLOBS_TAGS", "IDTAG", idTag, "TAG", tag, "LOCKED", true);
  if((idTag == m_tags.objectDictionaryTag()) && (tag == m_tags.dataTag())) {
    m_locked_dataTag = true;
  }
  tg.commit();
}


void CoralDB::lockAliasTag(const string& tag, const string& tmpIdTag) {
  const string idTag = tmpIdTag.empty() ? m_tags.objectDictionaryTag() : tmpIdTag;
  UpdateTransactionGuard tg(this);

  if(!findEntry<bool>(TABLENAME_PREFIX+"OBJECTDICT_TAGS", "IDTAG", idTag, "LOCKED")) {
    throw IdTagUnlocked("lockAliasTag(): IDTAG \""+idTag+"\" must be locked before locking its alias tag \""+tag+"\"");
  }

  updateRows(TABLENAME_PREFIX+"ALIASES_TAGS", "IDTAG", idTag, "TAG", tag, "LOCKED", true);
  if((idTag == m_tags.objectDictionaryTag()) && (tag == m_tags.aliasTag())) {
    m_locked_aliasTag = true;
  }
  tg.commit();
}


void CoralDB::lockCompoundTag(const CompoundTag& tags) {
  if(!tags.isValid()) {
    throw InvalidCompoundTag("lockCompoundTag()", tags);
  }
  
  UpdateTransactionGuard tg(this);

  updateRows(TABLENAME_PREFIX+"OBJECTDICT_TAGS", "IDTAG", tags.objectDictionaryTag(), "LOCKED", true);
  updateRows(TABLENAME_PREFIX+"CONNECTIONS_TAGS", "IDTAG", tags.objectDictionaryTag(), "TAG", tags.connectivityTag(), "LOCKED", true);
  updateRows(TABLENAME_PREFIX+"CLOBS_TAGS", "IDTAG", tags.objectDictionaryTag(), "TAG", tags.dataTag(), "LOCKED", true);
  updateRows(TABLENAME_PREFIX+"ALIASES_TAGS", "IDTAG", tags.objectDictionaryTag(), "TAG", tags.aliasTag(), "LOCKED", true);

  observeTagLocks();

  tg.commit();
}


void CoralDB::lockAllForObjectDictionaryTag(const string& idtag) {
  UpdateTransactionGuard tg(this);

  long res = updateRows(TABLENAME_PREFIX+"OBJECTDICT_TAGS", "IDTAG", idtag, "LOCKED", true);
  if(res != 1) {
    ostringstream os;
    os<<"lockAllForObjectDictionaryTag(\""<<idtag<<"\"): no such tag?  Number of updated rows = "<<res;
    throw runtime_error(os.str() );
  }
  
  updateRows(TABLENAME_PREFIX+"CONNECTIONS_TAGS", "IDTAG", idtag, "LOCKED", true);
  updateRows(TABLENAME_PREFIX+"CLOBS_TAGS", "IDTAG", idtag, "LOCKED", true);
  updateRows(TABLENAME_PREFIX+"ALIASES_TAGS", "IDTAG", idtag, "LOCKED", true);

  observeTagLocks();

  tg.commit();
}


bool CoralDB::objectDictionaryTagLocked(const string& idTag) const {
  return findEntry<bool>(TABLENAME_PREFIX+"OBJECTDICT_TAGS", "IDTAG", idTag, "LOCKED");
}

bool CoralDB::connectivityTagLocked(const string& tag, const string& tmpIdTag) const {
  const string idTag(tmpIdTag.empty() ? m_tags.objectDictionaryTag() : tmpIdTag);
  return findEntry<bool>(TABLENAME_PREFIX+"CONNECTIONS_TAGS", "IDTAG", idTag, "TAG", tag, "LOCKED");
}

bool CoralDB::dataTagLocked(const string& tag, const string& tmpIdTag) const {
  const string idTag(tmpIdTag.empty() ? m_tags.objectDictionaryTag() : tmpIdTag);
  return findEntry<bool>(TABLENAME_PREFIX+"CLOBS_TAGS", "IDTAG", idTag, "TAG",tag, "LOCKED");
}

bool CoralDB::aliasTagLocked(const string& tag, const string& tmpIdTag) const {
  const string idTag(tmpIdTag.empty() ? m_tags.objectDictionaryTag() : tmpIdTag);
  return findEntry<bool>(TABLENAME_PREFIX+"ALIASES_TAGS", "IDTAG", idTag, "TAG", tag, "LOCKED");
}


void CoralDB::setCompoundTag(const CompoundTag& existingTag) {
  if(!existingTag.isValid()) {
    throw InvalidCompoundTag("setCompoundTag()", existingTag);
  }

  ReadOnlyTransactionGuard tg(this);
  m_tags = existingTag;
  observeTagLocks();
  tg.commit();
}


void CoralDB::setObjectDictionaryTag(const string& existingIdTag) {
  if(existingIdTag.empty()) {
    throw runtime_error("setObjectDictionaryTag(): idTag should not be empty.");
  }
  
  if(existingIdTag != m_tags.objectDictionaryTag()) {
    // connectivity, alias and data tags in the new IDTAG are not
    // related in any way to tags in the old IDTAG.  Instead of
    // keeping values of tags that are likely wrong (and may not
    // even be defined in the new IDTAG), clean them all.
    CompoundTag t;
    t.setObjectDictionaryTag(existingIdTag);
    setCompoundTag(t);
  }
}


void CoralDB::setConnectivityTag(const string& existingTag) {
  ReadOnlyTransactionGuard tg(this);

  try {
    m_locked_connectivityTag = findEntry<bool>(TABLENAME_PREFIX+"CONNECTIONS_TAGS", "IDTAG", m_tags.objectDictionaryTag(), "TAG", existingTag, "LOCKED");
  }
  catch(RowNotFound& e) {
    throw RowNotFound(string(e.what()) + "\nError: Connectivity tag \""+existingTag+"\" does not exist for idTag=\""+m_tags.objectDictionaryTag()+"\"");
  }

  m_tags.setConnectivityTag(existingTag);

  tg.commit();
}


void CoralDB::setDataTag(const string& existingTag) {
  ReadOnlyTransactionGuard tg(this);

  try {
    m_locked_dataTag = findEntry<bool>(TABLENAME_PREFIX+"CLOBS_TAGS", "IDTAG", m_tags.objectDictionaryTag(), "TAG", existingTag, "LOCKED");  
  }
  catch(RowNotFound& e) {
    throw RowNotFound(string(e.what()) + "\nError: Data tag \""+existingTag+"\" does not exist for idTag=\""+m_tags.objectDictionaryTag()+"\"");
  }

  m_tags.setDataTag(existingTag);

  tg.commit();
}

void CoralDB::setAliasTag(const string& existingTag) {
  ReadOnlyTransactionGuard tg(this);

  try {
    m_locked_aliasTag = findEntry<bool>(TABLENAME_PREFIX+"ALIASES_TAGS", "IDTAG", m_tags.objectDictionaryTag(), "TAG", existingTag, "LOCKED");  
  }
  catch(RowNotFound& e) {
    throw RowNotFound(string(e.what()) + "\nError: Alias tag \""+existingTag+"\" does not exist for idTag=\""+m_tags.objectDictionaryTag()+"\"");
  }

  m_tags.setAliasTag(existingTag);

  tg.commit();
}


void CoralDB::getTagList(const string& tablename, TagList& output, const string& idTag) const {
  output.clear();
  std::unique_ptr<coral::IQuery> query(m_session->nominalSchema().tableHandle(tablename).newQuery());
  query->addToOutputList("TAG");
  query->addToOutputList("LOCKED");

  coral::AttributeList data;
  data.extend<string>("IDTAG");
  data["IDTAG"].data<string>() = (idTag.empty() ? m_tags.objectDictionaryTag() : idTag);

  query->setCondition("IDTAG = :IDTAG", data);

  query->addToOrderList("TAG");
  
  coral::ICursor& c = query->execute();
  while (c.next()) {
    output.push_back(TagStatus(c.currentRow()["TAG"].data<string>(), c.currentRow()["LOCKED"].data<bool>() ));
  }
}

void CoralDB::getExistingObjectDictionaryTags(IdTagList& output) const {
  ReadOnlyTransactionGuard tg(this);
  output.clear();
  std::unique_ptr<coral::IQuery> query(m_session->nominalSchema().tableHandle(TABLENAME_PREFIX+"OBJECTDICT_TAGS").newQuery());
  query->addToOutputList("IDTAG");
  query->addToOutputList("LOCKED");

  query->addToOrderList("IDTAG");

  coral::ICursor& c = query->execute();
  while (c.next()) {
    output.push_back(TagStatus(c.currentRow()["IDTAG"].data<string>(), c.currentRow()["LOCKED"].data<bool>()));
  }
  tg.commit();
}

void CoralDB::getExistingConnectivityTags(TagList& output, const string& idTag) const {
  ReadOnlyTransactionGuard tg(this);
  getTagList(TABLENAME_PREFIX+"CONNECTIONS_TAGS", output, idTag);
  tg.commit();
}

void CoralDB::getExistingDataTags(TagList& output, const string& idTag) const {
  ReadOnlyTransactionGuard tg(this);
  getTagList(TABLENAME_PREFIX+"CLOBS_TAGS", output, idTag);
  tg.commit();
}

void CoralDB::getExistingAliasTags(TagList& output, const string& idTag) const {
  ReadOnlyTransactionGuard tg(this);
  getTagList(TABLENAME_PREFIX+"ALIASES_TAGS", output, idTag);
  tg.commit();
}

void CoralDB::makeNewObjectDictionaryTag(const string& tag) {
  UpdateTransactionGuard tg(this);
  insertIdTagListTableRow(tag);
  tg.commit();
}

void CoralDB::makeNewConnectivityTag(const string& tag, const string& tmpIdTag) {
  UpdateTransactionGuard tg(this);
  const string newIdTag = tmpIdTag.empty() ? m_tags.objectDictionaryTag() : tmpIdTag;
  insertTagListTableRow(TABLENAME_PREFIX+"CONNECTIONS_TAGS", tag, newIdTag );
  tg.commit();
}

void CoralDB::makeNewDataTag(const string& tag, const string& tmpIdTag) {
  UpdateTransactionGuard tg(this);
  const string newIdTag = tmpIdTag.empty() ? m_tags.objectDictionaryTag() : tmpIdTag;
  insertTagListTableRow(TABLENAME_PREFIX+"CLOBS_TAGS", tag, newIdTag );
  tg.commit();
}

void CoralDB::makeNewAliasTag(const string& tag, const string& tmpIdTag) {
  UpdateTransactionGuard tg(this);
  const string newIdTag = tmpIdTag.empty() ? m_tags.objectDictionaryTag() : tmpIdTag;
  insertTagListTableRow(TABLENAME_PREFIX+"ALIASES_TAGS", tag, newIdTag );
  tg.commit();
}

void CoralDB::makeNewCompoundTag(const CompoundTag& tag) {
  UpdateTransactionGuard tg(this);
  insertIdTagListTableRow(tag.objectDictionaryTag());
  insertTagListTableRow(TABLENAME_PREFIX+"CONNECTIONS_TAGS", tag.connectivityTag(), tag.objectDictionaryTag() );
  insertTagListTableRow(TABLENAME_PREFIX+"CLOBS_TAGS", tag.dataTag(), tag.objectDictionaryTag() );
  insertTagListTableRow(TABLENAME_PREFIX+"ALIASES_TAGS", tag.aliasTag(), tag.objectDictionaryTag() );
  tg.commit();
}


void CoralDB::setHistoricTag(const CompoundTag& tag, time_t timeArg) {
  UpdateTransactionGuard tg(this);
  time_t startIOV =  timeArg ? timeArg : time(0);

  coral::ITableDataEditor& editor = m_session->nominalSchema().tableHandle(TABLENAME_PREFIX+"TAG_HISTORY").dataEditor();
  coral::AttributeList objectBuf;
  objectBuf.extend<time_t>("IOV_BEGIN");
  objectBuf.extend<string>("IDTAG");
  objectBuf.extend<string>("CONNECTIVITY_TAG");
  objectBuf.extend<string>("DATA_TAG");
  objectBuf.extend<string>("ALIAS_TAG");
  objectBuf["IOV_BEGIN"].data<time_t>() = startIOV;
  objectBuf["IDTAG"].data<string>() = tag.objectDictionaryTag();
  objectBuf["CONNECTIVITY_TAG"].data<string>() = tag.connectivityTag();
  objectBuf["DATA_TAG"].data<string>() = tag.dataTag();
  objectBuf["ALIAS_TAG"].data<string>() = tag.aliasTag();
  editor.insertRow(objectBuf);
  
  tg.commit();
}

CompoundTag CoralDB::getHistoricTag(time_t timeArg) const {
  ReadOnlyTransactionGuard tg(this);
  time_t requestedTime = timeArg ? timeArg : time(0);
  
  std::unique_ptr<coral::IQuery> query(m_session->nominalSchema().tableHandle(TABLENAME_PREFIX+"TAG_HISTORY").newQuery());
  query->addToOutputList("IDTAG");
  query->addToOutputList("CONNECTIVITY_TAG");
  query->addToOutputList("DATA_TAG");
  query->addToOutputList("ALIAS_TAG");

  coral::AttributeList data;
  data.extend<time_t>("REQUESTED_TIME");
  data["REQUESTED_TIME"].data<time_t>() = requestedTime;

  query->setCondition("IOV_BEGIN <= :REQUESTED_TIME",data);
  query->addToOrderList("IOV_BEGIN DESC");
  query->limitReturnedRows(1);

  vector<string> objectList;
  coral::ICursor& c = query->execute();
  if(!c.next()) {
    throw runtime_error("Coral::getHistoricTag(): no tags found");
  }
  
  CompoundTag result(c.currentRow()["IDTAG"].data<string>(), 
		     c.currentRow()["CONNECTIVITY_TAG"].data<string>(), 
		     c.currentRow()["DATA_TAG"].data<string>(),
		     c.currentRow()["ALIAS_TAG"].data<string>()  );
  
  if(c.next()) {
    throw runtime_error("CoralDB::getHistoricTag(): got multiple rows!");
  }
  
  tg.commit();
  return result;
}


void CoralDB::getHistoryTable(HistoryTable& result) {
  ReadOnlyTransactionGuard tg(this);
  //coral::TimeStamp requestedTime = (timeArg == coral::TimeStamp())  ? coral::TimeStamp(seal::Time::current()) : timeArg;
  
  std::unique_ptr<coral::IQuery> query(m_session->nominalSchema().tableHandle(TABLENAME_PREFIX+"TAG_HISTORY").newQuery());
  query->addToOutputList("IOV_BEGIN");
  query->addToOutputList("IDTAG");
  query->addToOutputList("CONNECTIVITY_TAG");
  query->addToOutputList("DATA_TAG");
  query->addToOutputList("ALIAS_TAG");

  query->addToOrderList("IOV_BEGIN");

  query->defineOutputType("IOV_BEGIN", coral::AttributeSpecification::typeNameForType<time_t>());

  vector<string> objectList;
  coral::ICursor& c = query->execute();
  while(c.next()) {
    result.push_back(make_pair(c.currentRow()["IOV_BEGIN"].data<time_t>(),
			       CompoundTag(c.currentRow()["IDTAG"].data<string>(), 
					   c.currentRow()["CONNECTIVITY_TAG"].data<string>(), 
					   c.currentRow()["DATA_TAG"].data<string>(),
					   c.currentRow()["ALIAS_TAG"].data<string>()  )
			       )
		     );
  }
  
  tg.commit();
}


bool CoralDB::deleteHistoryEntry(time_t exactTimeStamp) {
  UpdateTransactionGuard tg(this);

  coral::ITableDataEditor& editor = m_session->nominalSchema().tableHandle(TABLENAME_PREFIX+"TAG_HISTORY").dataEditor();
  coral::AttributeList objectBuf;
  objectBuf.extend<time_t>("IOV_BEGIN");
  objectBuf["IOV_BEGIN"].data<time_t>() = exactTimeStamp;
  long nrows = editor.deleteRows("IOV_BEGIN = :IOV_BEGIN", objectBuf);
  
  tg.commit();

  return (nrows > 0);
}



void CoralDB::insertMaster(const string& id) {
  ASSERT_UNLOCKED(connectivityTag);
  UpdateTransactionGuard tg(this);
  coral::ITableDataEditor& editor = m_session->nominalSchema().tableHandle(TABLENAME_PREFIX+"MASTERLIST").dataEditor();
  coral::AttributeList objectBuf;
  objectBuf.extend<string>("TAG");
  objectBuf.extend<string>("ID");
  objectBuf.extend<string>("IDTAG");
  objectBuf["TAG"].data<string>() = m_tags.connectivityTag();
  objectBuf["ID"].data<string>() = id;
  objectBuf["IDTAG"].data<string>() = m_tags.objectDictionaryTag();
  editor.insertRow(objectBuf);
  tg.commit();
}



bool CoralDB::deleteMaster(const string& id) {
  ASSERT_UNLOCKED(connectivityTag);
  UpdateTransactionGuard tg(this);
  coral::ITableDataEditor& editor = m_session->nominalSchema().tableHandle(TABLENAME_PREFIX+"MASTERLIST").dataEditor();
  coral::AttributeList data;
  data.extend<string>("tag");
  data.extend<string>("id");
  data.extend<string>("idtag");
  data["tag"].data<string>() = m_tags.connectivityTag();
  data["id"].data<string>() = id;
  data["idtag"].data<string>() = m_tags.objectDictionaryTag();
  long nChangedRows = editor.deleteRows("TAG = :tag AND ID = :id AND IDTAG = :idtag", data);
  tg.commit();
  return nChangedRows>0;
}



vector<string> CoralDB::masterList() const {
  ReadOnlyTransactionGuard tg(this);
  std::unique_ptr<coral::IQuery> query(m_session->nominalSchema().tableHandle(TABLENAME_PREFIX+"MASTERLIST").newQuery());
  query->addToOutputList("ID");
  coral::AttributeList data;
  data.extend<string>("idtag");
  data.extend<string>("tag");
  query->setCondition("IDTAG = :idtag AND TAG = :tag",data);
  data["idtag"].data<string>() = m_tags.objectDictionaryTag();
  data["tag"].data<string>() = m_tags.connectivityTag();

  vector<string> objectList;
  coral::ICursor& c = query->execute();
  while (c.next()) {
    objectList.push_back(c.currentRow()["ID"].data<string>());
  }

  tg.commit();
  return objectList;
}


bool CoralDB::addObject(const string& id, const string& type) {
  bool didInsert = false;
  std::unique_ptr<coral::IQuery> query(m_session->nominalSchema().tableHandle(TABLENAME_PREFIX+"OBJECTDICT").newQuery());
  query->addToOutputList("TYPE");
  coral::AttributeList data;
  data.extend<string>("ID");
  data.extend<string>("IDTAG");
  query->setCondition("ID = :ID AND IDTAG = :IDTAG",data);
  data["ID"].data<string>() = id;
  data["IDTAG"].data<string>() = m_tags.objectDictionaryTag();

  coral::ICursor& c = query->execute();
  if (c.next()) {
    string existingType = c.currentRow()["TYPE"].data<string>();
    if (existingType != type)
      throw runtime_error("Dictionary already contains object "+id+" with type "+existingType+" instead of "+type);
    // else the correct entry is already there and there's nothing to do
  } else {
    coral::ITableDataEditor& editor = m_session->nominalSchema().tableHandle(TABLENAME_PREFIX+"OBJECTDICT").dataEditor();
    coral::AttributeList objectBuf;
    objectBuf.extend<string>("ID");
    objectBuf.extend<string>("IDTAG");
    objectBuf.extend<string>("TYPE");
    objectBuf["ID"].data<string>() = id;
    objectBuf["IDTAG"].data<string>() = m_tags.objectDictionaryTag();
    objectBuf["TYPE"].data<string>() = type;
    editor.insertRow(objectBuf);
    didInsert = true;
  }

  return didInsert;
}


bool CoralDB::checkAndInsertObject(const string& id, const string& type) {
  ASSERT_UNLOCKED(objectDictionaryTag);
  UpdateTransactionGuard tg(this);
  bool didInsert = addObject(id,type);
  tg.commit();
  return didInsert;
}


unsigned CoralDB::insertBulkObjects(istream& data, bool debug) {
  unsigned nEntries = 0;
  string colData[2];
  int rowCacheSize = debug ? 1 : 20;
  
  UpdateTransactionGuard tg(this);
  
  coral::ITableDataEditor& editor = m_session->nominalSchema().tableHandle(TABLENAME_PREFIX+"OBJECTDICT").dataEditor();
  coral::AttributeList connectionBuf;
  connectionBuf.extend<string>("ID");
  connectionBuf.extend<string>("IDTAG");
  connectionBuf.extend<string>("TYPE");
  coral::IBulkOperation* bulkInserter = editor.bulkInsert(connectionBuf,rowCacheSize);
  
  connectionBuf["IDTAG"].data<string>() = m_tags.objectDictionaryTag();

  // Tab separated file
  while (parseLine(data,2,2,colData,"\t","\t",0,debug)>0) {
    
    connectionBuf["ID"].data<string>() = colData[0];
    connectionBuf["TYPE"].data<string>() = colData[1];
    
    bulkInserter->processNextIteration();
    nEntries++;
  }
  bulkInserter->flush();
  delete bulkInserter;
  
  tg.commit();
  return nEntries;
}

bool CoralDB::deleteObject(const string& id) {
  ASSERT_UNLOCKED(objectDictionaryTag);
  UpdateTransactionGuard tg(this);
  // First delete revisions for the object from all *_REVS tables
  // *_REVS tables are special, because they may have rows for an ID
  // even after all fields for the ID are deleted.
  deleteRows(TABLENAME_PREFIX+"CLOBS_REVS", "ID", id, "IDTAG", m_tags.objectDictionaryTag() );
  long nChangedRows = deleteRows(TABLENAME_PREFIX+"OBJECTDICT", "ID", id, "IDTAG", m_tags.objectDictionaryTag() );
  tg.commit();
  return nChangedRows>0;
}


bool CoralDB::updateObject(const string& id, const string& type) {
  ASSERT_UNLOCKED(objectDictionaryTag);
  UpdateTransactionGuard tg(this);
  coral::ITableDataEditor& editor = m_session->nominalSchema().tableHandle(TABLENAME_PREFIX+"OBJECTDICT").dataEditor();
  coral::AttributeList data;
  data.extend<string>("ID");
  data.extend<string>("IDTAG");
  data.extend<string>("TYPE");
  data["ID"].data<string>() = id;
  data["IDTAG"].data<string>() = m_tags.objectDictionaryTag();
  data["TYPE"].data<string>() = type;
  long nChangedRows = editor.updateRows("TYPE = :TYPE", "ID = :ID AND IDTAG = :IDTAG", data);
  tg.commit();
  return nChangedRows>0;
}



string CoralDB::objectType(const string& id) const {
  ReadOnlyTransactionGuard tg(this);
  string result = findEntry<string>(TABLENAME_PREFIX+"OBJECTDICT","ID",id,"IDTAG",m_tags.objectDictionaryTag(),"TYPE");
  tg.commit();
  return result;
}



bool CoralDB::renameID(const string& newId, const string& oldId) {
  ASSERT_UNLOCKED(objectDictionaryTag);
  
  UpdateTransactionGuard tg(this);
  
  // Simple UPDATE here
  //
  // bool res = updateRows(TABLENAME_PREFIX+"OBJECTDICT", "IDTAG", m_tags.objectDictionaryTag(), "ID", oldId, "ID", newId);
  //
  // does not work on Oracle because of foreign key constraints, even
  // though all other tables are consistently updated in the same
  // transaction.  It's not clear how to defer the check using
  // Coral+Oracle.  So instead we do:
  //
  //       copy oldId ==> newId in objdict
  //       update all other tables
  //       delete oldId from objdict.
  
  //----------------------------------------------------------------
  // copy oldId ==> newId
    
  coral::ITableDataEditor& editor = m_session->nominalSchema().tableHandle(TABLENAME_PREFIX+"OBJECTDICT").dataEditor();
  std::unique_ptr<coral::IOperationWithQuery> operation(editor.insertWithQuery());
  coral::IQueryDefinition& subquery = operation->query();
  subquery.addToTableList(TABLENAME_PREFIX+"OBJECTDICT");
  subquery.addToOutputList(":NEWID");
  subquery.addToOutputList("IDTAG");
  subquery.addToOutputList("TYPE");

  coral::AttributeList data;
  data.extend<string>("IDTAG");
  data["IDTAG"].data<string>() = m_tags.objectDictionaryTag();
  data.extend<string>("OLDID");
  data["OLDID"].data<string>() = oldId;
  data.extend<string>("NEWID");
  data["NEWID"].data<string>() = newId;

  subquery.setCondition("ID = :OLDID AND IDTAG = :IDTAG", data);
  operation->execute();

  //----------------------------------------------------------------
  // update all other tables

  updateRows(TABLENAME_PREFIX+"CONNECTIONS", "IDTAG", m_tags.objectDictionaryTag(), "ID", oldId, /*update:*/ "ID", newId);
  updateRows(TABLENAME_PREFIX+"CONNECTIONS", "IDTAG", m_tags.objectDictionaryTag(), "TOID", oldId, /*update:*/ "TOID", newId);

  updateRows(TABLENAME_PREFIX+"MASTERLIST", "IDTAG", m_tags.objectDictionaryTag(), "ID", oldId, /*update:*/ "ID", newId);

  updateRows(TABLENAME_PREFIX+"ALIASES", "IDTAG", m_tags.objectDictionaryTag(), "ID", oldId, /*update:*/ "ID", newId);

  updateRows(TABLENAME_PREFIX+"CLOBS", "IDTAG", m_tags.objectDictionaryTag(), "ID", oldId, /*update:*/ "ID", newId);
  updateRows(TABLENAME_PREFIX+"CLOBS_REVS", "IDTAG", m_tags.objectDictionaryTag(), "ID", oldId, /*update:*/ "ID", newId);


  
  //----------------------------------------------------------------
  // delete oldId

  long res = deleteRows(TABLENAME_PREFIX+"OBJECTDICT", "IDTAG", m_tags.objectDictionaryTag(), "ID", oldId);

  //----------------------------------------------------------------
  
  tg.commit();
  return res;
}


long CoralDB::renameType(const string& newType, const string& oldType) {
  ASSERT_UNLOCKED(objectDictionaryTag);
  UpdateTransactionGuard tg(this);
  
  long res = updateRows(TABLENAME_PREFIX+"OBJECTDICT", 
			
			// conditions
			"IDTAG", m_tags.objectDictionaryTag(), "TYPE", oldType,
			
			// update
			"TYPE", newType);

  tg.commit();
  return res;
}



void CoralDB::getObjectDictionary(ObjectDictionaryMap& objDict, const CompoundTag& tags) const {
  if(!tags.isValid()) {
    throw InvalidCompoundTag("getObjectDictionary()", tags);
  }
  if(tags.empty()) {
    throw EmptyTag("getObjectDictionary(): arg CompoundTag should not be empty.");
  }

  objDict.clear();
  ReadOnlyTransactionGuard tg(this);

  std::unique_ptr<coral::IQuery> query(m_session->nominalSchema().newQuery());

  std::vector<coral::AttributeList> data;

  if(tags.fullIdTag()) {   //----------------------------------------------------------------
    // Get all IDs in the tag.
    query->addToTableList(TABLENAME_PREFIX+"OBJECTDICT", "OD");

    query->addToOutputList("OD.ID", "ID");
    query->addToOutputList("OD.TYPE", "TYPE");
    
    data.push_back(coral::AttributeList());
    data.back().extend<string>("IDTAG");
    data.back()["IDTAG"].data<string>() = tags.objectDictionaryTag();
    query->setCondition("OD.IDTAG = :IDTAG", data.back());
  }
  else {   //----------------------------------------------------------------
    // Get a subset of IDs according to the requested tags. 
    fillUsedIdsQuery(*query, data, tags);
  }
  
  //----------------------------------------------------------------

  // Do we need to apply the DISTINCT operator to the result?  Looks like this is not needed
  // because UNION already does that.  There is also UNION ALL in SQL but not in CORAL.

  coral::ICursor& c = query->execute();
  while (c.next()) {
    //std::cerr<<"Got row: "; c.currentRow().toOutputStream(std::cerr); std::cerr<<std::endl;
    objDict.insert(make_pair(c.currentRow()["ID"].data<string>(),c.currentRow()["TYPE"].data<string>()));
  }

  tg.commit();
}


//unused: void CoralDB::insertDataType(const string& objectType, const string& dataType, const string& tagType, const string& tableName) {
//unused:   UpdateTransactionGuard tg(this);
//unused:   coral::ITableDataEditor& editor = m_session->nominalSchema().tableHandle(TABLENAME_PREFIX+"DATADICT").dataEditor();
//unused:   coral::AttributeList objectBuf;
//unused:   objectBuf.extend<string>("OBJECTTYPE");
//unused:   objectBuf.extend<string>("DATATYPE");
//unused:   objectBuf.extend<string>("TAGTYPE");
//unused:   objectBuf.extend<string>("TABLENAME");
//unused:   objectBuf["OBJECTTYPE"].data<string>() = objectType;
//unused:   objectBuf["DATATYPE"].data<string>() = dataType;
//unused:   objectBuf["TAGTYPE"].data<string>() = tagType;
//unused:   objectBuf["TABLENAME"].data<string>() = tableName;
//unused:   editor.insertRow(objectBuf);
//unused:   tg.commit();
//unused: }


unsigned CoralDB::insertBulkConnections(istream& data, BulkConnectionsFileFormat f, bool debug) {
  ASSERT_UNLOCKED(connectivityTag);

  unsigned nEntries = 0;
  string colData[6];
  string line;
  int rowCacheSize = debug ? 1 : 20;

  UpdateTransactionGuard tg(this);

  coral::ITableDataEditor& editor = m_session->nominalSchema().tableHandle(TABLENAME_PREFIX+"CONNECTIONS").dataEditor();
  coral::AttributeList connectionBuf;
  connectionBuf.extend<string>("ID");
  connectionBuf.extend<string>("IDTAG");
  connectionBuf.extend<string>("TAG");
  connectionBuf.extend<string>("SLOT");
  connectionBuf.extend<string>("TOID");
  connectionBuf.extend<string>("TOSLOT");
  coral::IBulkOperation* bulkInserter = editor.bulkInsert(connectionBuf,rowCacheSize);

  connectionBuf["IDTAG"].data<string>() = m_tags.objectDictionaryTag();
  connectionBuf["TAG"].data<string>() = m_tags.connectivityTag();

  // For older, CSV files:
  // while (parseLine(data,6,6,colData,",,,,,",",\"",1,debug)>0) {
  // For newer files with tab separated columns:
  switch(f) {
  case SIX_COL: 
  case SIX_COL_REVERSED:

  while (parseLine(data,6,6,colData,"\t\t\t\t\t","\t",0,debug)>0) {

    // Better use bulk INSERT-SELECT with IBulkOperationWithQuery here?
    // (see also IOperationWithQuery)
    addObject(colData[0],colData[1]);
    addObject(colData[3],colData[4]);

    if( f == SIX_COL_REVERSED ) {
      connectionBuf["ID"].data<string>() = colData[3];
      connectionBuf["SLOT"].data<string>() = colData[5];
      connectionBuf["TOID"].data<string>() = colData[0];
      connectionBuf["TOSLOT"].data<string>() = colData[2];
    } else {
      connectionBuf["ID"].data<string>() = colData[0];
      connectionBuf["SLOT"].data<string>() = colData[2];
      connectionBuf["TOID"].data<string>() = colData[3];
      connectionBuf["TOSLOT"].data<string>() = colData[5];
    }
    // cerr << colData[0] << " / " << colData[2] << endl;
    bulkInserter->processNextIteration();
    nEntries++;
  }
  break;
  case FOUR_COL:
    while (parseLine(data,4,4,colData,"\t\t\t","\t",0,debug)>0) {
      connectionBuf["ID"].data<string>() = colData[0];
      connectionBuf["SLOT"].data<string>() = colData[1];
      connectionBuf["TOID"].data<string>() = colData[2];
      connectionBuf["TOSLOT"].data<string>() = colData[3];
      bulkInserter->processNextIteration();
      nEntries++;
    }
    
    break;
  default:
    throw runtime_error("insertBulkConnection(): unknown value for BulkConnectionsFileFormat.");
  }

  bulkInserter->flush();
  delete bulkInserter;

  tg.commit();
  return nEntries;
  //cout << "*** " << nEntries << " entries loaded into connection table" << endl;
}


void CoralDB::insertConnection(const string& id, const string& slot, const string& toId, const string& toSlot) {
  ASSERT_UNLOCKED(connectivityTag);
  UpdateTransactionGuard tg(this);
  coral::ITableDataEditor& editor = m_session->nominalSchema().tableHandle(TABLENAME_PREFIX+"CONNECTIONS").dataEditor();
  coral::AttributeList data;
  data.extend<string>("ID");
  data.extend<string>("IDTAG");
  data.extend<string>("TAG");
  data.extend<string>("SLOT");
  data.extend<string>("TOID");
  data.extend<string>("TOSLOT");
  data["ID"].data<string>() = id;
  data["IDTAG"].data<string>() = m_tags.objectDictionaryTag();
  data["TAG"].data<string>() = m_tags.connectivityTag();
  data["SLOT"].data<string>() = slot;
  data["TOID"].data<string>() = toId;
  data["TOSLOT"].data<string>() = toSlot;
  editor.insertRow(data);
  tg.commit();
}


bool CoralDB::deleteConnection(const string& id, const string& slot, bool outConnection) {
  ASSERT_UNLOCKED(connectivityTag);
  UpdateTransactionGuard tg(this);
  coral::ITableDataEditor& editor = m_session->nominalSchema().tableHandle(TABLENAME_PREFIX+"CONNECTIONS").dataEditor();
  coral::AttributeList data;
  data.extend<string>("ID");
  data.extend<string>("IDTAG");
  data.extend<string>("TAG");
  data.extend<string>("SLOT");
  data["ID"].data<string>() = id;
  data["IDTAG"].data<string>() = m_tags.objectDictionaryTag();
  data["TAG"].data<string>() = m_tags.connectivityTag();
  data["SLOT"].data<string>() = slot;
  long nChangedRows;
  if (outConnection)
    nChangedRows = editor.deleteRows("ID = :ID AND IDTAG = :IDTAG AND TAG = :TAG AND SLOT = :SLOT", data);
  else
    nChangedRows = editor.deleteRows("TOID = :ID AND IDTAG = :IDTAG AND TAG = :TAG AND TOSLOT = :SLOT", data);
  tg.commit();
  return nChangedRows>0;
}



bool CoralDB::updateConnection(const string& id, const string& slot, const string& toId, const string& toSlot, bool outConnection) {
  ASSERT_UNLOCKED(connectivityTag);
  UpdateTransactionGuard tg(this);
  coral::ITableDataEditor& editor = m_session->nominalSchema().tableHandle(TABLENAME_PREFIX+"CONNECTIONS").dataEditor();
  coral::AttributeList data;
  data.extend<string>("TOID");
  data.extend<string>("TOSLOT");
  data.extend<string>("TAG");
  data.extend<string>("ID");
  data.extend<string>("IDTAG");
  data.extend<string>("SLOT");
  data["ID"].data<string>() = id;
  data["IDTAG"].data<string>() = m_tags.objectDictionaryTag();
  data["TAG"].data<string>() = m_tags.connectivityTag();
  data["SLOT"].data<string>() = slot;
  data["TOID"].data<string>() = toId;
  data["TOSLOT"].data<string>() = toSlot;
  long nChangedRows;
  if (outConnection)
    nChangedRows = editor.updateRows("TOID = :TOID, TOSLOT = :TOSLOT", "TAG = :TAG AND ID = :ID AND IDTAG = :IDTAG AND SLOT = :SLOT", data);
  else
    nChangedRows = editor.updateRows("ID = :ID, SLOT = :SLOT", "TAG = :TAG AND TOID = :TOID AND IDTAG = :IDTAG AND TOSLOT = :TOSLOT", data);
  tg.commit();
  return nChangedRows>0;
}


vector<Connection> CoralDB::findAllConnections(const string& id, bool getOutConnections) const {
  ReadOnlyTransactionGuard tg(this);

  coral::IQuery* query = m_session->nominalSchema().tableHandle(TABLENAME_PREFIX+"CONNECTIONS").newQuery();
  query->addToOutputList("ID");
  query->addToOutputList("SLOT");
  query->addToOutputList("TOID");
  query->addToOutputList("TOSLOT");
  coral::AttributeList data;
  data.extend<string>("ID");
  data.extend<string>("IDTAG");
  data.extend<string>("TAG");
  if (getOutConnections)
    query->setCondition("ID = :ID AND IDTAG = :IDTAG AND TAG = :TAG",data);
  else
    query->setCondition("TOID = :ID AND IDTAG = :IDTAG AND TAG = :TAG",data);
  data["ID"].data<string>() = id;
  data["IDTAG"].data<string>() = m_tags.objectDictionaryTag();
  data["TAG"].data<string>() = m_tags.connectivityTag();

  vector<Connection> connections;
  coral::ICursor& c = query->execute();
  while (c.next()) {
    Connection conn(c.currentRow()["ID"].data<string>(),
			   c.currentRow()["SLOT"].data<string>(),
			   c.currentRow()["TOID"].data<string>(),
			   c.currentRow()["TOSLOT"].data<string>());
    connections.push_back(conn);
  }
  delete query;
  tg.commit();
  return connections;
}


vector<Connection> CoralDB::findConnections(const string& id, const string& slotPattern, bool getOutConnections) const {
  ReadOnlyTransactionGuard tg(this);

  coral::IQuery* query = m_session->nominalSchema().tableHandle(TABLENAME_PREFIX+"CONNECTIONS").newQuery();
  query->addToOutputList("ID");
  query->addToOutputList("SLOT");
  query->addToOutputList("TOID");
  query->addToOutputList("TOSLOT");
  coral::AttributeList data;
  data.extend<string>("ID");
  data.extend<string>("IDTAG");
  data.extend<string>("TAG");
  data.extend<string>("SLOT");

  data["ID"].data<string>() = id;
  data["IDTAG"].data<string>() = m_tags.objectDictionaryTag();
  data["TAG"].data<string>() = m_tags.connectivityTag();
  data["SLOT"].data<string>() = escapePattern(slotPattern, '|');

  if (getOutConnections) {
    query->setCondition("ID = :ID AND IDTAG = :IDTAG AND TAG = :TAG AND SLOT LIKE :SLOT ESCAPE '|'",data);
  }
  else {
    query->setCondition("TOID = :ID  AND IDTAG = :IDTAG AND TAG = :TAG AND TOSLOT LIKE :SLOT ESCAPE '|'",data);
  }

  vector<Connection> res;
  coral::ICursor& c = query->execute();
  while (c.next()) {
    res.push_back(Connection(c.currentRow()["ID"].data<string>(),
			     c.currentRow()["SLOT"].data<string>(),
			     c.currentRow()["TOID"].data<string>(),
			     c.currentRow()["TOSLOT"].data<string>()
			     )
		  );
  }
  delete query;
  tg.commit();
  return res;
}


vector<Connection> CoralDB::findConnectionsByType(const string& id, const string& typePattern, bool getOutConnections) const {
  ReadOnlyTransactionGuard tg(this);

  coral::IQuery* query = m_session->nominalSchema().newQuery();
  query->addToTableList(TABLENAME_PREFIX+"CONNECTIONS", "C");
  query->addToTableList(TABLENAME_PREFIX+"OBJECTDICT", "D");

  query->addToOutputList("C.ID");
  query->addToOutputList("C.SLOT");
  query->addToOutputList("C.TOID");
  query->addToOutputList("C.TOSLOT");
  coral::AttributeList data;
  data.extend<string>("ID");
  data.extend<string>("TYPE");
  data.extend<string>("IDTAG");
  data.extend<string>("TAG");

  data["ID"].data<string>() = id;
  data["TYPE"].data<string>() = escapePattern(typePattern, '|');
  data["IDTAG"].data<string>() = m_tags.objectDictionaryTag();
  data["TAG"].data<string>() = m_tags.connectivityTag();

  std::string commonCond = "D.TYPE LIKE :TYPE ESCAPE '|' AND C.IDTAG = :IDTAG AND C.TAG = :TAG AND C.IDTAG = D.IDTAG";

  if (getOutConnections) {
    query->setCondition("C.ID   = :ID AND C.TOID = D.ID AND "+commonCond, data);
  }
  else {
    query->setCondition("C.TOID = :ID AND C.ID   = D.ID AND "+commonCond, data);
  }

  vector<Connection> connections;
  coral::ICursor& c = query->execute();
  while (c.next()) {
    Connection conn(c.currentRow()["C.ID"].data<string>(),
		    c.currentRow()["C.SLOT"].data<string>(),
		    c.currentRow()["C.TOID"].data<string>(),
		    c.currentRow()["C.TOSLOT"].data<string>());
    connections.push_back(conn);
  }
  delete query;
  tg.commit();
  return connections;
}


vector<Connection> CoralDB::findConnectionsFromSlot(const string& slotPattern, const string& toId) const {
  ReadOnlyTransactionGuard tg(this);

  coral::IQuery* query = m_session->nominalSchema().tableHandle(TABLENAME_PREFIX+"CONNECTIONS").newQuery();

  query->addToOutputList("ID");
  query->addToOutputList("SLOT");
  query->addToOutputList("TOID");
  query->addToOutputList("TOSLOT");
  coral::AttributeList data;
  data.extend<string>("SLOT");
  data.extend<string>("TOID");
  data.extend<string>("IDTAG");
  data.extend<string>("TAG");

  data["SLOT"].data<string>() = escapePattern(slotPattern, '|');
  data["TOID"].data<string>() = toId;
  data["IDTAG"].data<string>() = m_tags.objectDictionaryTag();
  data["TAG"].data<string>() = m_tags.connectivityTag();

  query->setCondition("SLOT LIKE :SLOT ESCAPE '|' AND TOID = :TOID AND  IDTAG = :IDTAG AND TAG = :TAG", data);

  vector<Connection> connections;
  coral::ICursor& c = query->execute();
  while (c.next()) {
    Connection conn(c.currentRow()["ID"].data<string>(),
		    c.currentRow()["SLOT"].data<string>(),
		    c.currentRow()["TOID"].data<string>(),
		    c.currentRow()["TOSLOT"].data<string>());
    connections.push_back(conn);
  }
  delete query;
  tg.commit();
  return connections;
}



vector<Connection> CoralDB::findConnectionsToSlot(const string& id, const string& toSlotPattern) const {
  ReadOnlyTransactionGuard tg(this);

  coral::IQuery* query = m_session->nominalSchema().tableHandle(TABLENAME_PREFIX+"CONNECTIONS").newQuery();

  query->addToOutputList("ID");
  query->addToOutputList("SLOT");
  query->addToOutputList("TOID");
  query->addToOutputList("TOSLOT");
  coral::AttributeList data;
  data.extend<string>("ID");
  data.extend<string>("TOSLOT");
  data.extend<string>("IDTAG");
  data.extend<string>("TAG");

  data["ID"].data<string>() = id;
  data["TOSLOT"].data<string>() = escapePattern(toSlotPattern, '|');
  data["IDTAG"].data<string>() = m_tags.objectDictionaryTag();
  data["TAG"].data<string>() = m_tags.connectivityTag();

  query->setCondition("ID = :ID AND TOSLOT LIKE :TOSLOT ESCAPE '|' AND  IDTAG = :IDTAG AND TAG = :TAG", data);

  vector<Connection> connections;
  coral::ICursor& c = query->execute();
  while (c.next()) {
    Connection conn(c.currentRow()["ID"].data<string>(),
		    c.currentRow()["SLOT"].data<string>(),
		    c.currentRow()["TOID"].data<string>(),
		    c.currentRow()["TOSLOT"].data<string>());
    connections.push_back(conn);
  }
  delete query;
  tg.commit();
  return connections;
}


void CoralDB::getConnectionTable(ConnectionTableMap& connTable) const {
  connTable.clear();
  ReadOnlyTransactionGuard tg(this);
  std::unique_ptr<coral::IQuery> query(m_session->nominalSchema().tableHandle(TABLENAME_PREFIX+"CONNECTIONS").newQuery());
  query->addToOutputList("ID");
  query->addToOutputList("SLOT");
  query->addToOutputList("TOID");
  query->addToOutputList("TOSLOT");

  coral::AttributeList data;
  data.extend<string>("IDTAG");
  data.extend<string>("TAG");
  data["IDTAG"].data<string>() = m_tags.objectDictionaryTag();
  data["TAG"].data<string>() = m_tags.connectivityTag();
  query->setCondition("IDTAG = :IDTAG AND TAG = :TAG",data);

  coral::ICursor& c = query->execute();
  
  while (c.next()) {
    connTable.push_back(Connection(c.currentRow()["ID"].data<string>(),
				   c.currentRow()["SLOT"].data<string>(),
				   c.currentRow()["TOID"].data<string>(),
				   c.currentRow()["TOSLOT"].data<string>()));
  }

  tg.commit();
}

void CoralDB::getConnectionTableForType(const string& type, bool getOutConnections, ConnectionTableMap& connTable) const {
  connTable.clear();
  ReadOnlyTransactionGuard tg(this);
  std::unique_ptr<coral::IQuery> query(m_session->nominalSchema().newQuery());
  query->addToTableList(TABLENAME_PREFIX + "CONNECTIONS", "C");
  query->addToTableList(TABLENAME_PREFIX + "OBJECTDICT", "D");

  query->addToOutputList("C.ID");
  query->addToOutputList("C.SLOT");
  query->addToOutputList("C.TOID");
  query->addToOutputList("C.TOSLOT");

  coral::AttributeList data;
  data.extend<string>("IDTAG");
  data.extend<string>("TAG");
  data.extend<string>("TYPE");
  data["IDTAG"].data<string>() = m_tags.objectDictionaryTag();
  data["TAG"].data<string>() = m_tags.connectivityTag();
  data["TYPE"].data<string>() = type;
  if (getOutConnections)
    query->setCondition("C.TOID = D.ID AND D.TYPE = :TYPE AND C.IDTAG = :IDTAG AND C.TAG = :TAG AND C.IDTAG = D.IDTAG"
			,data);
  else
    query->setCondition("C.ID = D.ID AND D.TYPE = :TYPE AND C.IDTAG = :IDTAG AND C.TAG = :TAG AND C.IDTAG = D.IDTAG"
			,data);

  coral::ICursor& c = query->execute();
  
  while (c.next()) {
    connTable.push_back(Connection(c.currentRow()["C.ID"].data<string>(),
				   c.currentRow()["C.SLOT"].data<string>(),
				   c.currentRow()["C.TOID"].data<string>(),
				   c.currentRow()["C.TOSLOT"].data<string>()));
  }

  tg.commit();
}


unsigned CoralDB::insertBulkAliases(istream& data, bool genericFile, bool debug, bool sloppy) {
  ASSERT_UNLOCKED(aliasTag);

  unsigned nEntries = 0;
  string colData[9];
  string line;
  int rowCacheSize = debug ? 1 : 20;

  ObjectDictionaryMap idmap;
  if(sloppy) {
    getObjectDictionary(idmap, CompoundTag::OnlyIdTag(m_tags.objectDictionaryTag())); // this is a public method - must be called outside of our transaction5D.
  }


  UpdateTransactionGuard tg(this);

  coral::ITableDataEditor& idmapEditor = m_session->nominalSchema().tableHandle(TABLENAME_PREFIX+"ALIASES").dataEditor();
  coral::AttributeList buf;
  buf.extend<string>("ALIAS");
  buf.extend<string>("TAG");
  buf.extend<string>("CONVENTION");
  buf.extend<string>("ID");
  buf.extend<string>("IDTAG");
  coral::IBulkOperation* bulkInserter = idmapEditor.bulkInsert(buf,rowCacheSize);

  buf["TAG"].data<string>() = m_tags.aliasTag();
  buf["IDTAG"].data<string>() = m_tags.objectDictionaryTag();

  if (genericFile) {
    // This is for reading a "generic" aliases definition file with 3 tab-separated
    // columns containing rows of the form "ALIAS\tCONVENTION\tID"
    while (parseLine(data,3,3,colData,"\t\t","\t",0,debug)>0) {
      string id = colData[2];
      if(sloppy && (idmap.find(id) == idmap.end()) ) {
	cout<<"insertBulkAliases(): Sloppily skipping ID="<<id
	    <<" in IDTAG="<<m_tags.objectDictionaryTag()
	    <<", aliasTag="<<m_tags.aliasTag()<<endl;
      }
      else {
	buf["ALIAS"].data<string>() = colData[0];
	buf["CONVENTION"].data<string>() = colData[1];
	buf["ID"].data<string>() = id;  // ID is geographical id
	bulkInserter->processNextIteration();
	nEntries++;
      }
    }

  } else {
    // The code below is currently designed for reading Attilio's NamingConversions.txt file
    while (parseLine(data,9,9,colData,"","\t",0,debug)>0) {
      string id = colData[2];  // ID is geographical id
      if(sloppy && (idmap.find(id) == idmap.end()) ) {
	cout<<"insertBulkAliases(): Sloppily skipping ID="<<id
	    <<" in IDTAG="<<m_tags.objectDictionaryTag()
	    <<", aliasTag="<<m_tags.aliasTag()<<endl;
      }
      else {
	buf["ID"].data<string>() = id;
	
	// Offline id string
	buf["CONVENTION"].data<string>() = "OFFLINEID";
	buf["ALIAS"].data<string>() = colData[0];
	bulkInserter->processNextIteration();
	nEntries++;

	// Hash id
	buf["CONVENTION"].data<string>() = "HASHID";
	buf["ALIAS"].data<string>() = colData[1];
	bulkInserter->processNextIteration();
	nEntries++;

	// DCS id
	buf["CONVENTION"].data<string>() = "DCSID";
	buf["ALIAS"].data<string>() = colData[3];
	bulkInserter->processNextIteration();
	nEntries++;

	// PDB serial numbers
	buf["CONVENTION"].data<string>() = "PDBSERIAL";
	buf["ALIAS"].data<string>() = colData[4];
	bulkInserter->processNextIteration();
	nEntries++;
      }
    }
  }
  bulkInserter->flush();
  delete bulkInserter;

  tg.commit();
  //cout << "*** Aliases for " << nEntries << " IDs loaded into aliases table" << endl;
  return nEntries;
}



void CoralDB::insertAlias(const Alias& a) {
  insertAlias(a.alias(), a.convention(), a.id() );
}


void CoralDB::insertAlias(const string& alias, const string& convention, const string& id) {
  ASSERT_UNLOCKED(aliasTag);
  UpdateTransactionGuard tg(this);
  coral::ITableDataEditor& editor = m_session->nominalSchema().tableHandle(TABLENAME_PREFIX+"ALIASES").dataEditor();
  coral::AttributeList data;
  data.extend<string>("ALIAS");
  data.extend<string>("CONVENTION");
  data.extend<string>("TAG");
  data.extend<string>("ID");
  data.extend<string>("IDTAG");
  data["ALIAS"].data<string>() = alias;
  data["TAG"].data<string>() = m_tags.aliasTag();
  data["CONVENTION"].data<string>() = convention;
  data["ID"].data<string>() = id;
  data["IDTAG"].data<string>() = m_tags.objectDictionaryTag();
  editor.insertRow(data);
  tg.commit();
}


bool CoralDB::deleteAlias(const Alias& a) {
  ASSERT_UNLOCKED(aliasTag);
  UpdateTransactionGuard tg(this);
  
  std::map<string,string> keys;
  
  keys["TAG"]   = m_tags.aliasTag();
  keys["IDTAG"] = m_tags.objectDictionaryTag();
  keys["ID"]    = a.id();
  keys["CONVENTION"] = a.convention();
  keys["ALIAS"] = a.alias();

  long res = deleteRows(TABLENAME_PREFIX + "ALIASES", keys);
  tg.commit();
  return res;
}

bool CoralDB::deleteAlias(const string& id, const string& convention) {
  ASSERT_UNLOCKED(aliasTag);
  UpdateTransactionGuard tg(this);
  
  std::map<string,string> keys;
  
  keys["TAG"]   = m_tags.aliasTag();
  keys["IDTAG"] = m_tags.objectDictionaryTag();
  keys["ID"]    = id;
  keys["CONVENTION"] = convention;

  long res = deleteRows(TABLENAME_PREFIX + "ALIASES", keys);
  tg.commit();
  return res;
}


vector<Alias> CoralDB::findAliases(const string& id) const {
  ReadOnlyTransactionGuard tg(this);
  std::unique_ptr<coral::IQuery> query(m_session->nominalSchema().tableHandle(TABLENAME_PREFIX+"ALIASES").newQuery());
  query->addToOutputList("ALIAS");
  query->addToOutputList("CONVENTION");
  coral::AttributeList data;
  data.extend<string>("ID");
  data.extend<string>("IDTAG");
  data.extend<string>("TAG");
  query->setCondition("ID = :ID and IDTAG = :IDTAG and TAG = :TAG",data);
  data["ID"].data<string>() = id;
  data["IDTAG"].data<string>() = m_tags.objectDictionaryTag();
  data["TAG"].data<string>() = m_tags.aliasTag();

  vector<Alias> aliases;
  coral::ICursor& c = query->execute();
  while (c.next()) {
    aliases.push_back(Alias(c.currentRow()["ALIAS"].data<string>(), 
			    c.currentRow()["CONVENTION"].data<string>(), 
			    id
			    ));
  }

  tg.commit();
  return aliases;
}



string CoralDB::findAlias(const string& id, const string& convention) const {
  ReadOnlyTransactionGuard tg(this);
  std::map<string,string> keys;
  keys["ID"] = id;
  keys["IDTAG"] = m_tags.objectDictionaryTag();
  keys["TAG"] = m_tags.aliasTag();
  keys["CONVENTION"] = convention;
  string result = findEntry<string>(TABLENAME_PREFIX+"ALIASES", keys, "ALIAS");
  tg.commit();
  return result;
}


string CoralDB::findId(const string& alias, const string& convention) const {
  ReadOnlyTransactionGuard tg(this);
  std::map<string,string> keys;
  keys["ALIAS"] = alias;
  keys["TAG"] = m_tags.aliasTag();
  keys["CONVENTION"] = convention;
  keys["IDTAG"] = m_tags.objectDictionaryTag();
  string result = findEntry<string>(TABLENAME_PREFIX+"ALIASES", keys, "ID");
  tg.commit();
  return result;
}


void CoralDB::getAliasesTable(AliasesTable& res) const {
  ReadOnlyTransactionGuard tg(this);
  std::unique_ptr<coral::IQuery> query(m_session->nominalSchema().tableHandle(TABLENAME_PREFIX+"ALIASES").newQuery());
  query->addToOutputList("ALIAS");
  query->addToOrderList("ALIAS");
  query->addToOutputList("CONVENTION");
  query->addToOrderList("CONVENTION");
  query->addToOutputList("ID");
  query->addToOrderList("ID");

  coral::AttributeList data;
  data.extend<string>("TAG");
  data.extend<string>("IDTAG");
  data["TAG"].data<string>() = m_tags.aliasTag();
  data["IDTAG"].data<string>() = m_tags.objectDictionaryTag();
  query->setCondition("TAG = :TAG AND IDTAG = :IDTAG",data);

  res.clear();
  coral::ICursor& c = query->execute();
  while (c.next()) {
    res.push_back(Alias(c.currentRow()["ALIAS"].data<string>(), 
			c.currentRow()["CONVENTION"].data<string>(), 
			c.currentRow()["ID"].data<string>()
			));
  }

  tg.commit();
}

void CoralDB::addNewRevision(const string& id, const string& tag, const string& idTag, const string& tagsTableName) {

  // First try to insert the new ID, assuming a REV is already defined for this (IDTAG,TAG)
  coral::ITableDataEditor& editor = m_session->nominalSchema().tableHandle(tagsTableName).dataEditor();
  std::unique_ptr<coral::IOperationWithQuery> newidop(editor.insertWithQuery());
  coral::IQueryDefinition& newidq = newidop->query();
  newidq.addToTableList(tagsTableName);
  newidq.addToOutputList("'"+id+"'");
  newidq.addToOutputList("'"+idTag+"'");
  newidq.addToOutputList("'"+tag+"'");
  newidq.addToOutputList("REV");
  newidq.limitReturnedRows(1); // all REVs are supposed to be the same, no sorting necessary
    
  coral::AttributeList data;
  data.extend<string>("IDTAG");
  data.extend<string>("TAG");
  data["IDTAG"].data<string>() = idTag;
  data["TAG"].data<string>() = tag;
  newidq.setCondition("IDTAG = :IDTAG AND TAG = :TAG", data);
  long numRows  = newidop->execute();
  
  if(numRows>1) {
    std::ostringstream os;
    os<<"CoralDB::addNewRevision(ID="<<id<<",TAG="<<tag<<",IDTAG="<<idTag<<",tagsTableName="<<tagsTableName
      <<"): internal error adding ID to existing TAG: numRows=="<<numRows;
    throw runtime_error(os.str());
  }

  if(!numRows) { // Need to define new REV number, this (IDTAG,TAG) has not been seen
    // First try to insert the new ID, assuming a REV is already defined for this (IDTAG,TAG)

    std::unique_ptr<coral::IOperationWithQuery> newtagop(editor.insertWithQuery());
    coral::IQueryDefinition& newtagq = newtagop->query();
    newtagq.addToTableList(tagsTableName);
    newtagq.addToOutputList("'"+id+"'");
    newtagq.addToOutputList("'"+idTag+"'");
    newtagq.addToOutputList("'"+tag+"'");
    newtagq.addToOutputList("COALESCE(1+MAX(REV),0)");
    
    numRows = newtagop->execute();
    
    if(numRows!=1) {
      std::ostringstream os;
      os<<"CoralDB::addNewtagision(ID="<<id<<",TAG="<<tag<<",IDTAG="<<idTag<<",tagsTableName="<<tagsTableName
	<<"): internal error defining REV for a new TAG: numRows=="<<numRows;
      throw runtime_error(os.str());
    }
  }
}


// If the current REV is not shared, there is never a need to make a new
// revision.  Use REV that is already defined for the ID in the current tag.
//
// insert into CLOBS select ID,REV,'field','data' from CLOBS_REVS where ID='id' AND TAG='dataTag';
//
bool CoralDB::insertClobTableRow(const string& id, const string& field, const string& clob) {
  coral::ITableDataEditor& editor = m_session->nominalSchema().tableHandle(TABLENAME_PREFIX+"CLOBS").dataEditor();
  std::unique_ptr<coral::IOperationWithQuery> operation(editor.insertWithQuery());
  coral::IQueryDefinition& subquery = operation->query();
  subquery.addToTableList(TABLENAME_PREFIX+"CLOBS_REVS");
  subquery.addToOutputList("ID");
  subquery.addToOutputList("IDTAG");
  subquery.addToOutputList("REV");
  subquery.addToOutputList("'"+field+"'");
  // FIXME: need to properly quote the clob
  // FIXME: this copies potentially large data - is there a way to optimize?
  subquery.addToOutputList("'"+clob+"'");
  coral::AttributeList data;
  data.extend<string>("ID");
  data.extend<string>("IDTAG");
  data.extend<string>("TAG");
  data["ID"].data<string>() = id;
  data["IDTAG"].data<string>() = m_tags.objectDictionaryTag();
  data["TAG"].data<string>() = m_tags.dataTag();
  subquery.setCondition("ID= :ID AND IDTAG = :IDTAG AND TAG = :TAG", data);
  return operation->execute();
}

void CoralDB::insertCLOB(const string& id, const string& field, const string& clob) {
  ASSERT_UNLOCKED(dataTag);
  UpdateTransactionGuard tg(this);

  if(!insertClobTableRow(id,field,clob)) {

    // There is no entry in CLOBS_REVS for the current tag.
    // Need to insert one, than insertClobTableRow() must work.
    addNewRevision(id, m_tags.dataTag(), m_tags.objectDictionaryTag(), TABLENAME_PREFIX+"CLOBS_REVS");

    if(!insertClobTableRow(id,field,clob)) {
      throw runtime_error("CoralDB::insertCLOB("+id+","+field+","+clob+"): "
			  +"no rows inserted.  Data tag = "+m_tags.dataTag()
			  +", objectDictionaryTag = "+m_tags.objectDictionaryTag() );
    }
  }

  tg.commit();
}


// Need to 
//
// delete from CLOBS where ID='id' AND FIELD='field' AND REV IN (select REV from CLOBS_REVS where ID='id' and TAG='tag');
//
// but there seems to be no way to build such a query with Coral.
//
// Instead need to find out REV in a separate query to use it for DELETE condition.
bool CoralDB::deleteCLOB(const string& id, const string& field) {
  ASSERT_UNLOCKED(dataTag);
  UpdateTransactionGuard tg(this);

  //FIXME: can a newer Coral do the deletion in a single query?
  std::map<string,string> keys;
  keys["ID"] = id;
  keys["TAG"] = m_tags.dataTag();
  keys["IDTAG"] = m_tags.objectDictionaryTag();
  int rev = findEntry<int>(TABLENAME_PREFIX+"CLOBS_REVS", keys, "REV");

  coral::ITableDataEditor& editor = m_session->nominalSchema().tableHandle(TABLENAME_PREFIX+"CLOBS").dataEditor();
  coral::AttributeList data;
  data.extend<int>("REV");
  data.extend<string>("ID");
  data.extend<string>("IDTAG");
  data.extend<string>("FIELD");
  data["REV"].data<int>() = rev;
  data["ID"].data<string>() = id;
  data["IDTAG"].data<string>() = m_tags.objectDictionaryTag();
  data["FIELD"].data<string>() = field;
  long nChangedRows = editor.deleteRows("REV = :REV AND ID = :ID AND IDTAG = :IDTAG AND FIELD = :FIELD", data);
  tg.commit();
  return nChangedRows>0;
}


bool CoralDB::updateCLOB(const string& id, const string& field, const string& clob) {
  ASSERT_UNLOCKED(dataTag);
  UpdateTransactionGuard tg(this);

  //FIXME: can a newer Coral do the deletion in a single query?
  std::map<string,string> keys;
  keys["ID"] = id;
  keys["TAG"] = m_tags.dataTag();
  keys["IDTAG"] = m_tags.objectDictionaryTag();
  int rev = findEntry<int>(TABLENAME_PREFIX+"CLOBS_REVS", keys, "REV");

  coral::ITableDataEditor& editor = m_session->nominalSchema().tableHandle(TABLENAME_PREFIX+"CLOBS").dataEditor();
  coral::AttributeList data;
  data.extend<string>("DATA");
  data.extend<int>("REV");
  data.extend<string>("ID");
  data.extend<string>("IDTAG");
  data.extend<string>("FIELD");
  data["DATA"].data<string>() = clob;
  data["REV"].data<int>() = rev;
  data["ID"].data<string>() = id;
  data["IDTAG"].data<string>() = m_tags.objectDictionaryTag();
  data["FIELD"].data<string>() = field;

  long nChangedRows = editor.updateRows("DATA = :DATA", "REV = :REV AND ID = :ID AND IDTAG = :IDTAG AND FIELD = :FIELD", data);

  tg.commit();
  return nChangedRows>0;
}


string CoralDB::findCLOB(const string& id, const string& field) const {
  ReadOnlyTransactionGuard tg(this);
  std::unique_ptr<coral::IQuery> query(m_session->nominalSchema().newQuery());
  query->addToTableList(TABLENAME_PREFIX + "CLOBS", "C");
  query->addToTableList(TABLENAME_PREFIX + "CLOBS_REVS", "T");
  query->addToOutputList("DATA");

  coral::AttributeList data;
  data.extend<string>("TAG");
  data.extend<string>("ID");
  data.extend<string>("IDTAG");
  data.extend<string>("FIELD");
  data["TAG"].data<string>() = m_tags.dataTag();
  data["ID"].data<string>() = id;
  data["IDTAG"].data<string>() = m_tags.objectDictionaryTag();
  data["FIELD"].data<string>() = field;

  //  query->limitReturnedRows(1); // optimization?

  query->setCondition("C.ID=T.ID AND C.IDTAG = T.IDTAG AND C.REV=T.REV AND TAG=:TAG AND T.ID = :ID AND T.IDTAG = :IDTAG AND FIELD = :FIELD", data);

  Connection conn;
  coral::ICursor& c = query->execute();
  if(!c.next()) {
    throw runtime_error("CoralDB::findCLOB(): no FIELD="+field+" for ID="+id+" in data tag="+m_tags.dataTag()
			+", objectDictionaryTag="+m_tags.objectDictionaryTag() );
  }
  // FIXME: may need to unquote the clob
  // FIXME: avoid string copying with AttributeList
  string result = c.currentRow()[0].data<string>();  
  
  // Have to call c.next() to close the cursor. Check on its status as well.
  if(c.next()) {
    throw runtime_error("CoralDB::findCLOB(): query returned mutliple rows for FIELD="+field+" for ID="+id+" in data tag="+m_tags.dataTag());
  }

  tg.commit();
  return result;
}

void CoralDB::findCLOBs(const string& id, ClobDataContainer& result) const {
  ReadOnlyTransactionGuard tg(this);
  std::unique_ptr<coral::IQuery> query(m_session->nominalSchema().newQuery());
  query->addToTableList(TABLENAME_PREFIX + "CLOBS", "C");
  query->addToTableList(TABLENAME_PREFIX + "CLOBS_REVS", "T");
  query->addToOutputList("FIELD");
  query->addToOutputList("DATA");

  coral::AttributeList data;
  data.extend<string>("TAG");
  data.extend<string>("ID");
  data.extend<string>("IDTAG");
  data["TAG"].data<string>() = m_tags.dataTag();
  data["ID"].data<string>() = id;
  data["IDTAG"].data<string>() = m_tags.objectDictionaryTag();

  query->setCondition("C.ID=T.ID AND C.IDTAG = T.IDTAG AND C.REV=T.REV AND TAG=:TAG AND T.ID = :ID AND T.IDTAG = :IDTAG", data);

  Connection conn;
  // FIX ME: what is the maximum number of fields? Here it returns up to 1800 fields per object
  query->setRowCacheSize(1800); // to accommodate all modules
  coral::ICursor& c = query->execute();
  int count = 0;
  while (c.next()) {
    result.push_back(make_pair(c.currentRow()["FIELD"].data<string>(), c.currentRow()["DATA"].data<string>()));
    count ++;
  }

  if(count == 0){
    throw runtime_error("CoralDB::findCLOBs(): no FIELD for ID="+id+" in data tag="+m_tags.dataTag()
			+", objectDictionaryTag="+m_tags.objectDictionaryTag() );
  }
  tg.commit();
}

void CoralDB::getIdClobbedForType(const string& objType,ObjectDictionaryMap& result) const {
  result.clear();
  ReadOnlyTransactionGuard tg(this);

  std::unique_ptr<coral::IQuery> query(m_session->nominalSchema().newQuery());
  query->addToTableList(TABLENAME_PREFIX + "OBJECTDICT", "D");
  query->addToTableList(TABLENAME_PREFIX + "CLOBS", "C");
  query->addToTableList(TABLENAME_PREFIX + "CLOBS_REVS", "T");

  query->addToOutputList("C.ID", "ID");
  query->addToOutputList("TYPE");

  coral::AttributeList data;
  data.extend<string>("IDTAG");
  data.extend<string>("TAG");
  data.extend<string>("TYPE");
  data["IDTAG"].data<string>() = m_tags.objectDictionaryTag();
  data["TAG"].data<string>() = m_tags.dataTag();
  data["TYPE"].data<string>() = objType;

  query->setCondition("C.ID=T.ID AND C.IDTAG = T.IDTAG AND C.REV=T.REV AND C.IDTAG = :IDTAG AND TAG=:TAG"" AND C.ID=D.ID AND D.IDTAG = :IDTAG AND D.TYPE =:TYPE", data);

  // FIXME: Optimization: set number of prefetched rows? Is the default 1 row?
  query->setRowCacheSize(1800); // to accommodate all modules
  coral::ICursor& c = query->execute();
  while (c.next()) {
    result.insert(make_pair(c.currentRow()["ID"].data<string>(), c.currentRow()["TYPE"].data<string>()));
  }
  tg.commit();
}

void CoralDB::getClobNames(ClobNameContainer& result) const {
  result.clear();
  ReadOnlyTransactionGuard tg(this);

  std::unique_ptr<coral::IQuery> query(m_session->nominalSchema().newQuery());
  query->addToTableList(TABLENAME_PREFIX + "CLOBS", "C");
  query->addToTableList(TABLENAME_PREFIX + "CLOBS_REVS", "T");
  query->addToOutputList("C.ID", "ID");
  query->addToOutputList("FIELD");

  coral::AttributeList data;
  data.extend<string>("IDTAG");
  data.extend<string>("TAG");
  data["IDTAG"].data<string>() = m_tags.objectDictionaryTag();
  data["TAG"].data<string>() = m_tags.dataTag();

  query->setCondition("C.ID=T.ID AND C.IDTAG = T.IDTAG AND C.REV=T.REV AND C.IDTAG = :IDTAG AND TAG=:TAG", data);

  // FIXME: Optimization: set number of prefetched rows? Is the default 1 row?
  coral::ICursor& c = query->execute();
  while (c.next()) {
    result.insert(make_pair(c.currentRow()["ID"].data<string>(), c.currentRow()["FIELD"].data<string>()));
  }
  tg.commit();
}


void CoralDB::getNamedClobsForType(const string& objType, const string& field, ClobDataContainer& result) const {
  result.clear();
  ReadOnlyTransactionGuard tg(this);

  std::unique_ptr<coral::IQuery> query(m_session->nominalSchema().newQuery());
  query->addToTableList(TABLENAME_PREFIX + "OBJECTDICT", "D");
  query->addToTableList(TABLENAME_PREFIX + "CLOBS", "C");
  query->addToTableList(TABLENAME_PREFIX + "CLOBS_REVS", "T");

  query->addToOutputList("C.ID", "ID");
  query->addToOutputList("DATA");

  coral::AttributeList data;
  data.extend<string>("IDTAG");
  data.extend<string>("TAG");
  data.extend<string>("TYPE");
  data.extend<string>("FIELD");
  data["IDTAG"].data<string>() = m_tags.objectDictionaryTag();
  data["TAG"].data<string>() = m_tags.dataTag();
  data["TYPE"].data<string>() = objType;
  data["FIELD"].data<string>() = field;

  query->setCondition("C.ID=T.ID AND C.IDTAG = T.IDTAG AND C.REV=T.REV AND C.IDTAG = :IDTAG AND TAG=:TAG"
		      " AND C.ID=D.ID AND D.IDTAG = :IDTAG AND D.TYPE = :TYPE AND C.FIELD = :FIELD"
		      , data);

  // FIXME: Optimization: set number of prefetched rows? Is the default 1 row?
  query->setRowCacheSize(1800); // to accommodate all modules
  coral::ICursor& c = query->execute();
  while (c.next()) {
    result.push_back(make_pair(c.currentRow()["ID"].data<string>(), c.currentRow()["DATA"].data<string>()));
  }
  tg.commit();
}


void  CoralDB::insertBLOB(const string& /*id*/, const string& /*field*/, const coral::Blob& /*blob*/) {
  throw runtime_error("CoralDB: BLOB operations not implemented with versioning");
}



bool  CoralDB::deleteBLOB(const string& /*id*/, const string& /*field*/) {
  throw runtime_error("CoralDB: BLOB operations not implemented with versioning");
}



bool  CoralDB::updateBLOB(const string& /*id*/, const string& /*field*/, const coral::Blob& /*blob*/) {
  throw runtime_error("CoralDB: BLOB operations not implemented with versioning");
}


void CoralDB::observeTagLocks() const {
  m_locked_objectDictionaryTag = m_locked_connectivityTag = m_locked_dataTag = m_locked_aliasTag = true;

  if(! m_tags.objectDictionaryTag().empty() ) {
    try {
      m_locked_objectDictionaryTag = findEntry<bool>(TABLENAME_PREFIX+"OBJECTDICT_TAGS", "IDTAG", m_tags.objectDictionaryTag(), "LOCKED");
    }
    catch(RowNotFound& e) {
      throw RowNotFound(string(e.what()) + "\nError: IDTAG "+m_tags.objectDictionaryTag()+" does not exist" );
    }

    if(!m_tags.connectivityTag().empty()) {
      try {
	m_locked_connectivityTag = findEntry<bool>(TABLENAME_PREFIX+"CONNECTIONS_TAGS", "IDTAG", m_tags.objectDictionaryTag(), "TAG", m_tags.connectivityTag(), "LOCKED");
      }
      catch(RowNotFound& e) {
	throw RowNotFound(string(e.what()) + "\nError: Connectivity tag "+m_tags.connectivityTag()+" does not exist for idTag="+m_tags.objectDictionaryTag() );
      }
    }

    if(!m_tags.dataTag().empty()) {
      try {
	m_locked_dataTag = findEntry<bool>(TABLENAME_PREFIX+"CLOBS_TAGS", "IDTAG", m_tags.objectDictionaryTag(), "TAG", m_tags.dataTag(), "LOCKED");
      }
      catch(RowNotFound& e) {
        throw RowNotFound(string(e.what()) + "\nError: Data tag "+m_tags.dataTag()+" does not exist for idTag="+m_tags.objectDictionaryTag() );
      }
      
    }

    if(!m_tags.aliasTag().empty()) {
      try {
	m_locked_aliasTag = findEntry<bool>(TABLENAME_PREFIX+"ALIASES_TAGS", "IDTAG", m_tags.objectDictionaryTag(), "TAG", m_tags.aliasTag(), "LOCKED");
      }
      catch(RowNotFound& e) {
        throw RowNotFound(string(e.what()) + "\nError: Alias tag "+m_tags.aliasTag()+" does not exist for idTag="+m_tags.objectDictionaryTag() );
      }

    }
  }
}


template<class T> T CoralDB::findEntry(const string& tableName, const string& keyField, const string& keyValue, const string& resultField) 
  const
{
  std::unique_ptr<coral::IQuery> query(m_session->nominalSchema().tableHandle(tableName).newQuery());
  query->addToOutputList(resultField);
  coral::AttributeList data;
  data.template extend<string>(keyField);
  query->setCondition(keyField+" = :"+keyField,data);
  data[keyField].template data<string>() = keyValue;
  query->defineOutputType(resultField, coral::AttributeSpecification::typeNameForType<T>());
  coral::ICursor& c = query->execute();
  if(!c.next()) {
    throw RowNotFound("CoralDB::findEntry(tableName="+tableName+", keyField="+keyField
		      +", keyValue="+keyValue+", resultField="+resultField+")"
		      +": no rows returned by the query");
  }
  T result(c.currentRow()[resultField].template data<T>());
  if(c.next()) {
    throw runtime_error("CoralDB::findEntry(tableName="+tableName+", keyField="+keyField
                        +", keyValue="+keyValue+", resultField="+resultField+")"
                        +": multiple rows returned by the query");
  }
  return result;
}



template<class T> T CoralDB::findEntry(const string& tableName, const string& keyField1, const string& keyValue1,
				       const string& keyField2, const string& keyValue2, const string& resultField)
  const
{
  std::unique_ptr<coral::IQuery> query(m_session->nominalSchema().tableHandle(tableName).newQuery());
  query->addToOutputList(resultField);
  coral::AttributeList data;
  data.template extend<string>(keyField1);
  data.template extend<string>(keyField2);
  query->setCondition(keyField1+" = :"+keyField1+" AND "+keyField2+" = :"+keyField2,data);
  data[keyField1].template data<string>() = keyValue1;
  data[keyField2].template data<string>() = keyValue2;
  query->defineOutputType(resultField, coral::AttributeSpecification::typeNameForType<T>());
  coral::ICursor& c = query->execute();
  if(!c.next()) {
    throw RowNotFound("CoralDB::findEntry(tableName="+tableName
		      +", keyField1="+keyField1+", keyValue1="+keyValue1
		      +", keyField2="+keyField2+", keyValue2="+keyValue2
		      +", resultField="+resultField+")"
		      +": no rows returned by the query");
  }
  T result(c.currentRow()[resultField].template data<T>());
  if(c.next()) {
    throw runtime_error("CoralDB::findEntry(tableName="+tableName
			+", keyField1="+keyField1+", keyValue1="+keyValue1
			+", keyField2="+keyField2+", keyValue2="+keyValue2
			+", resultField="+resultField+")"
                        +": multiple rows returned by the query");
  }
  return result;
}

// KeyContainer can be e.g. a map<string,string>, or a vector<pair<string,string> >
template<class T, class KeyContainer> T CoralDB::findEntry(const string& tableName, const KeyContainer& keys, const string& resultField)
  const
{
  std::unique_ptr<coral::IQuery> query(m_session->nominalSchema().tableHandle(tableName).newQuery());
  query->addToOutputList(resultField);
  coral::AttributeList data;
  string conditions;
  for(typename KeyContainer::const_iterator i = keys.begin(); i != keys.end(); i++) {
    data.template extend<string>(i->first); 
    data[i->first].template data<string>() = i->second;
    conditions += (i->first+" = :"+i->first+" AND ");
  }
  // trim the last " AND ";
  if(!conditions.empty()) {
    string::size_type length = conditions.length();
    conditions.erase(length-5);
  }
  query->setCondition(conditions,data);
  query->limitReturnedRows(1);
  query->defineOutputType(resultField, coral::AttributeSpecification::typeNameForType<T>());
  coral::ICursor& c = query->execute();
  if(!c.next()) {
    //FIXME: show the content of KeyContainer
    throw RowNotFound("CoralDB::findEntry(tableName="+tableName+", KeyContainer& , resultField="+resultField+")"
		      +": no rows returned by the query");
  }
  T result(c.currentRow()[resultField].template data<T>());
  if(c.next()) {
    throw runtime_error("CoralDB::findEntry(tableName="+tableName+", KeyContainer& , resultField="+resultField+")"
			+": multiple rows returned by the query");
  }
  return result;
}


template<class T> long CoralDB::deleteRows(const string& tableName, const string& keyField, const T& keyValue) {
  coral::ITableDataEditor& editor = m_session->nominalSchema().tableHandle(tableName).dataEditor();
  coral::AttributeList data;
  data.template extend<T>(keyField);
  data[keyField].template data<T>() = keyValue;
  return editor.deleteRows(keyField+" = :"+keyField, data);
}


template<class T1, class T2> long CoralDB::deleteRows(const string& tableName, const string& keyField1, const T1& keyValue1, const string& keyField2, const T2& keyValue2) {
  coral::ITableDataEditor& editor = m_session->nominalSchema().tableHandle(tableName).dataEditor();
  coral::AttributeList data;
  data.template extend<T1>(keyField1);
  data.template extend<T2>(keyField2);
  data[keyField1].template data<T1>() = keyValue1;
  data[keyField2].template data<T2>() = keyValue2;
  return editor.deleteRows(keyField1+" = :"+keyField1+" AND "+keyField2+" = :"+keyField2, data);
}


// KeyContainer can be e.g. a map<string,string>, or a vector<pair<string,string> >
template<class KeyContainer> long CoralDB::deleteRows(const string& tableName, const KeyContainer& keys) {
  coral::ITableDataEditor& editor = m_session->nominalSchema().tableHandle(tableName).dataEditor();
  coral::AttributeList data;

  string conditions;
  for(typename KeyContainer::const_iterator i = keys.begin(); i != keys.end(); i++) {
    data.template extend<string>(i->first); 
    data[i->first].template data<string>() = i->second;
    conditions += (i->first+" = :"+i->first+" AND ");
  }
  // trim the last " AND ";
  if(!conditions.empty()) {
    string::size_type length = conditions.length();
    conditions.erase(length-5);
  }

  return editor.deleteRows(conditions, data);
}


template<class T, class R>
long CoralDB::updateRows(const string& tableName, const string& keyField, const T& keyValue, 
			 const string& fieldToUpdate, const R& newValue)
{
  coral::ITableDataEditor& editor = m_session->nominalSchema().tableHandle(tableName).dataEditor();
  coral::AttributeList data;
  data.template extend<R>(fieldToUpdate);
  data[fieldToUpdate].template data<R>() = newValue;
  data.template extend<T>(keyField);
  data[keyField].template data<T>() = keyValue;
  return editor.updateRows(fieldToUpdate+" = :"+fieldToUpdate, keyField+" = :"+keyField, data);
}


template<class T1, class T2, class R>
long CoralDB::updateRows(const string& tableName, const string& keyField1, const T1& keyValue1,
			 const string& keyField2, const T2& keyValue2,
			 const string& fieldToUpdate, const R& newValue)
{
  coral::ITableDataEditor& editor = m_session->nominalSchema().tableHandle(tableName).dataEditor();
  coral::AttributeList data;
  // To allow a condition on fieldToUpdate need to have different names for its two attributes:
  // one for the condition, another for the new value.
  data.template extend<R>("NEW_"+fieldToUpdate);
  data["NEW_"+fieldToUpdate].template data<R>() = newValue;
  data.template extend<T1>(keyField1);
  data[keyField1].template data<T1>() = keyValue1;
  data.template extend<T2>(keyField2);
  data[keyField2].template data<T2>() = keyValue2;
  return editor.updateRows(fieldToUpdate+" = :NEW_"+fieldToUpdate,
			   keyField1+" = :"+keyField1+" AND "+keyField2+" = :"+keyField2, data);
}


bool CoralDB::checkForLockedTags(const string& tableName, const string& idTag) const {
  std::unique_ptr<coral::IQuery> query(m_session->nominalSchema().tableHandle(tableName).newQuery());
  query->addToOutputList("LOCKED");
  coral::AttributeList data;
  data.extend<string>("IDTAG");
  data["IDTAG"].data<string>() = idTag;
  data.extend<bool>("LOCKED");
  data["LOCKED"].data<bool>() = true;
  query->setCondition("IDTAG = :IDTAG AND LOCKED = :LOCKED",data);
  query->limitReturnedRows(1);
  coral::ICursor& c = query->execute();
  return c.next();
}


int CoralDB::parseLine(istream& data, int maxCols, int minCols, string colData[], const string& skip,
		       const string& delims, string::size_type startColumn, bool debug) const {
  for (int i=0; i<maxCols; i++) colData[i] = "";
  int iCol = 0;
  string line;
  while (getline(data,line)) {
    if (debug) cerr << line << endl;
    if (line[0] == '#') continue;  // comment line
    if (line.empty()) continue;
    if (line == skip) continue;
    string::size_type colStart, colEnd;
    colStart = line.find_first_not_of(delims);
    if (colStart != startColumn) {
      cerr << "*** WARNING: Skipping ill-formatted line: " << line << endl;
      continue;
    }
    while ((colStart!=string::npos) && (iCol<maxCols)) {
      colEnd = line.find_first_of(delims,colStart);
      colData[iCol] = line.substr(colStart,colEnd-colStart);
      colStart = line.find_first_not_of(delims,colEnd);
      iCol++;
    }
    if (iCol >= minCols)
      break;  // Ok, got one of the desired lines so we stop for now and return data to caller
    cerr << "*** ERROR: Input line has only " << iCol << " fields (expected > " << minCols << " fields): "
	 << line << endl;
    for (int i=0; i<maxCols; i++) colData[i] = "";
    iCol = 0;

  }
  return iCol;
}


CoralDB::ReadOnlyTransactionGuard::ReadOnlyTransactionGuard(const CoralDB *db) 
  : m_db(db)
{
  CORALDB_TRACE_TRANS_CALL(cerr<<"CoralDB::ReadOnlyTransactionGuard::ReadOnlyTransactionGuard[this="<<this<<"]()"<<endl);
  if(m_db->m_autocommit) {
    if(m_db->m_session->transaction().isActive()) { // the previous transaction was not closed. Perhaps an exception?
      m_db->m_session->transaction().rollback(); // we should not commmit the leftovers
    }
    m_db->m_session->transaction().start(READONLY);
  }
  else {
    // Expect to have an open transaction.  If not treat the RO case like autocommit.
    if(m_db->m_session->transaction().isActive()) {
      // do nothing
    }
    else {
      // In the autocommit mode we should always have an open transaction, either ReadOnly or update. 
      // Should not get there.  Except may be due to a broken DB connection?
      m_db->m_session->transaction().start(READONLY);
    }
  }    
}


void CoralDB::ReadOnlyTransactionGuard::commit() {
  CORALDB_TRACE_TRANS_CALL(cerr<<"CoralDB::ReadOnlyTransactionGuard::commit[this="<<this<<"]()"<<endl);
  if(m_db->m_autocommit) {
    m_db->m_session->transaction().commit();
  }
}


CoralDB::UpdateTransactionGuard::UpdateTransactionGuard(CoralDB *db) 
  : m_db(db)
{
  CORALDB_TRACE_TRANS_CALL(cerr<<"CoralDB::UpdateTransactionGuard::UpdateTransactionGuard[this="<<this<<"]()"<<endl);
  if(m_db->m_autocommit) {
    if(m_db->m_session->transaction().isActive()) { // the previous transaction was not closed. Perhaps an exception?
      m_db->m_session->transaction().rollback(); // we should not commmit the leftovers
    }
    m_db->m_session->transaction().start(UPDATE);
  }
  else {
    // Must have an open transaction for updates.    
    if(m_db->m_session->transaction().isActive() && !m_db->m_session->transaction().isReadOnly() ) {
      // do nothing
    }
    else {
      throw runtime_error("CoralDB::UpdateTransactionGuard(): in the manual commit mode UPDATE transactions must be started explicitly");
    }
  }
}

void CoralDB::UpdateTransactionGuard::commit() {
  CORALDB_TRACE_TRANS_CALL(cerr<<"CoralDB::UpdateTransactionGuard::commit[this="<<this<<"]()"<<endl);
  if(m_db->m_autocommit) {
    m_db->m_session->transaction().commit();
  }
}

#if 0
{ // this is to make the number of braces even
#endif
} // namespace CoralDB

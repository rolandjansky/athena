/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

// CoraCoolDatabase.cxx
// implematation for CoraCoolDatabase
// Richard Hawkings, started 10/06

#include "CoralBase/AttributeSpecification.h"
#include "CoralBase/AttributeList.h"
#include "CoralBase/Attribute.h"
#include "CoralBase/Exception.h"
#include "RelationalAccess/IConnectionService.h"
#include "RelationalAccess/ISessionProxy.h"
#include "RelationalAccess/ITable.h"
#include "RelationalAccess/TableDescription.h"
#include "RelationalAccess/ITableDataEditor.h"
#include "RelationalAccess/ITransaction.h"
#include "RelationalAccess/ISchema.h"
#include "RelationalAccess/IQuery.h"
#include "RelationalAccess/ICursor.h"
#include "RelationalAccess/SchemaException.h"

#include "CoolKernel/IFolder.h"
#include "CoolKernel/IFieldSpecification.h"
#include "CoolKernel/FolderSpecification.h"

#include "CoraCool/CoraCoolFolder.h"
#include "CoraCoolSequence.h"
#include "CoraCool/CoraCoolException.h"

#include "CoraCool/CoraCoolDatabase.h"

#include "CxxUtils/checker_macros.h"

// constructor with external COOL database reference
CoraCoolDatabase::CoraCoolDatabase(const std::string dbconn,
                                   cool::IDatabasePtr cooldb,
                                   coral::IConnectionService& coralsvc,
                                   const bool readonly) :
  m_cooldb(cooldb), 
  m_connected(false),
  m_readonly(readonly),
  m_coralsvc(coralsvc),
  m_proxy(0),
  m_log("CoraCool")
{
  if (!extractCoralConStr(dbconn)) {
    m_log << coral::Error << "Cannot derive CORAL connection string from: "
	  << dbconn << coral::MessageStream::endmsg;
    throw CoraCoolException("Bad connection string",
			    "CoraCoolDatabase::CoraCoolDatabase");
  }
  m_log << coral::Debug << "CORAL dbconnection string is: " << m_dbconn 
	<< coral::MessageStream::endmsg;
}

bool CoraCoolDatabase::connect() {
  if (!m_connected) {
    coral::AccessMode access=m_readonly ? coral::ReadOnly : coral::Update;
    try {
      m_proxy=m_coralsvc.connect(m_dbconn,access);
      m_connected=true;
    }
    catch (std::exception& e) {
      m_log << coral::Error << "Exception caught: " << e.what() <<
        coral::MessageStream::endmsg;
      m_connected=false;
    }
  }
  return m_connected;
}

bool CoraCoolDatabase::disconnect() {
  if (m_connected) {
    delete m_proxy;
    m_connected=false;
  }
  return m_connected;
}

bool CoraCoolDatabase::extractCoralConStr(const std::string& coolstr) {
  // extract CORAL database string from COOL one
  bool dbok=false;
  // first check for initial colon - if so, technology-specific string
  std::string::size_type c1=coolstr.find(":");
  if (c1!=std::string::npos) {
    std::string techno,server,schema,user,passwd;
    techno=coolstr.substr(0,c1);
    std::string::size_type c2;
    c2=coolstr.find(";");
    if (c2==std::string::npos) c2=coolstr.size();
    server=coolstr.substr(c1+3,c2-c1-3);
    c1=coolstr.find("schema=");
    if (c1!=std::string::npos) {
      c2=coolstr.find(";",c1+7);
      if (c2==std::string::npos) c2=coolstr.size();
      schema=coolstr.substr(c1+7,c2-c1-7);
    }
    c1=coolstr.find("dbname=");
    if (c1!=std::string::npos) {
      c2=coolstr.find(";",c1+7);
      if (c2==std::string::npos) c2=coolstr.size();
      m_dbname=coolstr.substr(c1+7,c2-c1-7);
    }
    // construct the connection string
    if (techno=="oracle" || techno=="mysql" || techno=="frontier") {
      if (!server.empty() && !schema.empty()) {
        m_dbconn=techno+"://"+server+"/"+schema;
	dbok=true;
      }
    } else if (techno=="sqlite") {
      if (!schema.empty()) {
	m_dbconn="sqlite_file:"+schema;
	dbok=true;
      }
    }
  } else {
    c1=coolstr.find("/");
    if (c1!=std::string::npos) {
      m_dbconn=coolstr.substr(0,c1);
      m_dbname=coolstr.substr(c1+1);
      dbok=true;
    }
  }
  if (m_dbname.empty()) dbok=false;
  m_log << coral::Debug << "Extracted CORAL connection string: " << m_dbconn 
   << " and database name: " << m_dbname << " with status " << dbok << 
    coral::MessageStream::endmsg;
  return dbok;
}

std::string CoraCoolDatabase::encodeAttrSpec(
	 const cool::IRecordSpecification& spec) {
  std::string result="";
  unsigned int n=spec.size();
  for (unsigned int i=0;i<n;++i) {
    const cool::IFieldSpecification& field=spec[i];
    result+=field.name()+":"+field.storageType().name();
    if (i<n-1) result+=",";
  }
  return result;
}

bool CoraCoolDatabase::storeSpec(const std::string& tablename,
				 const std::string& spec) {
  m_log << coral::Debug << "Store AttrList specification for table " <<
    tablename << " : " << spec <<coral::MessageStream::endmsg;
  // first check for existance of table
  const std::string attrtable=m_dbname+"_CORACOOLATTR";
  coral::ITable* table;
  try {
    table=&(m_proxy->nominalSchema().tableHandle(attrtable));
  }
  catch (coral::SchemaException& e) {
    m_log << coral::Debug << "Creating table " << attrtable << coral::MessageStream::endmsg;
    coral::TableDescription tdesc(attrtable);
    tdesc.setName(attrtable);
    tdesc.insertColumn("NAME","string",31,false);
    tdesc.insertColumn("ATTRSPEC","string",4000,false);
    tdesc.setPrimaryKey("NAME");
    table=&(m_proxy->nominalSchema().createTable(tdesc));
    // pause for thought
    m_proxy->transaction().commit();
    sleep(1);
    m_proxy->transaction().start(false);
    table=&(m_proxy->nominalSchema().tableHandle(attrtable));
  }
  if (table==0) {
    m_log << coral::Error << "No pointer to " << attrtable << coral::MessageStream::endmsg;
    return false;
  }
  // now update table with specification
  try {
    coral::AttributeList data ATLAS_THREAD_SAFE; // Not shared, ok
    data.extend<std::string>("NAME");
    data.extend<std::string>("ATTRSPEC");
    data[0].data<std::string>()=tablename;
    data[1].data<std::string>()=spec;
    coral::ITableDataEditor& editor=table->dataEditor();
    editor.insertRow(data);
  }
  catch (coral::SchemaException&e) {
    m_log << coral::Error << "Could not insert " << tablename << " into " 
			    << attrtable << " exception: " << e.what() <<
      coral::MessageStream::endmsg;
    return false;
  }
  return true;
}

CoraCoolFolderPtr CoraCoolDatabase::createFolder(const std::string& coolpath,
     const std::string& coraltable,
     const cool::IRecordSpecification& fkspec,
     const cool::IRecordSpecification& payloadspec,
     const std::string coralfk,
     const std::string coralpk,
     const std::string description,
     const cool::FolderVersioning::Mode mode,
     const bool createParents) {

  // check input parameters
  if (coraltable.size()>18) throw CoraCoolException(
	"CORAL tablename too long (max 18 chars)",
	"CoraCoolDatabase::createFolder");
  // encode the folder description string
  // first remove any existing coracool string
  std::string::size_type p1,p2;
  std::string newdesc=description;
  p1=description.find("<coracool>");
  p2=description.find("</coracool>");
  if (p1!=std::string::npos && p2!=std::string::npos) {
    newdesc=description.substr(0,p1)+description.substr(p2+11);
  }
  // COOL foreign key column is name of COOL payload attribute
  newdesc=newdesc+"<coracool>"+coraltable+":"+
    fkspec[0].name()+":"+
    coralfk+":"+coralpk+"</coracool>";
  m_log << coral::Debug << "Created new description: " << newdesc << 
    coral::MessageStream::endmsg;

  // check for rows in key generation table, create if needed
  // start transaction here
  connect();
  m_proxy->transaction().start(false);
  CoraCoolSequence seqfk(m_dbname,m_dbname+"_"+coraltable+"_FK",m_proxy,true);
  if (coralfk!=coralpk)
   CoraCoolSequence seqpk(m_dbname,m_dbname+"_"+coraltable+"_PK",m_proxy,true);

  // endcode and store the attributelist specification in CORACOOLATTR table 
  if (!storeSpec(m_dbname+"_"+coraltable,
		 encodeAttrSpec(payloadspec)))
    throw CoraCoolException("Cannot store payload specification",
			    "CoraCoolDatabase::createFolder");

  // create CORAL table
  try {
    m_log << coral::Debug << "Generate CORAL table:" << coraltable << 
      coral::MessageStream::endmsg;
    coral::TableDescription tdesc("CoraCoolPayload");
    tdesc.setName(m_dbname+"_"+coraltable);
    for (unsigned int i=0;i<payloadspec.size();++i) {
      const cool::IFieldSpecification& field=payloadspec[i];
      int maxsize=field.storageType().maxSize();
      if (field.storageType()==cool::StorageType::Blob64k) maxsize=0;
      const std::string typen=coral::AttributeSpecification::typeNameForId(
	      field.storageType().cppType());
      m_log << coral::Debug << "Insert column " << field.name() << 
	" of type " << typen << std::endl;
      tdesc.insertColumn(field.name(),typen,maxsize,false);
    }
    // declare primary key
    tdesc.setPrimaryKey(coralpk);
    // create separate index on FK if different
    if (coralfk!=coralpk) 
      tdesc.createIndex(m_dbname+"_"+coraltable+"F",coralfk,false);

    m_proxy->nominalSchema().createTable(tdesc);
    m_proxy->transaction().commit();
    m_log << coral::Debug << "Created coral table " << coral::MessageStream::endmsg;

    // disconnect/reconnect to avoid connection problems
    disconnect();
    sleep(1);
    connect();
  }
  catch (std::exception& e) {
    // problem in table creation
    throw CoraCoolException(e.what(),"CoraCoolDatabase::createFolder");
  }

  // create corresponding COOL folder
  // this has to go after CORAL manipulations otherwise get problems
  // with schema consistency
  // disconnect/reconnect to COOL first in order to reset COOL
  // avoiding problems due to the reset database connection
  m_log << coral::Debug << "Disconnect/reconnect COOL connection" << 
      coral::MessageStream::endmsg;
  m_cooldb->closeDatabase();
  m_cooldb->openDatabase();
  m_log << coral::Debug << "Create COOL folder" << coolpath << coral::MessageStream::endmsg;

  m_cooldb->createFolder(coolpath,
       cool::FolderSpecification(mode,fkspec),newdesc,createParents);
  m_log << coral::Debug << "Created COOL folder" << coral::MessageStream::endmsg;

  // all structures created,
  // generate return value by instantiating new folder
  CoraCoolFolderPtr folder(new CoraCoolFolder(coolpath,
  			   m_proxy, m_cooldb,this,m_log));
  return folder;
}


CoraCoolFolderPtr CoraCoolDatabase::getFolder(const std::string& coolfolder) {
  connect();
  CoraCoolFolderPtr folder(new CoraCoolFolder(
     coolfolder,m_proxy,m_cooldb,this,m_log));
  return folder;
}

bool CoraCoolDatabase::existsFolder(const std::string& coolfolder) {
  // first check if the COOL folder exists
  if (!m_cooldb->existsFolder(coolfolder)) {
    return false;
  } else {
    // if it does, check the folder description finds a coracool tag
    // note this does not check the CORACOOL table actually exists
    cool::IFolderPtr folder=m_cooldb->getFolder(coolfolder);
    std::string tablename,keycolcool,fkeycolcoral,pkeycolcoral;
    return parseFolderDescription(folder->description(),tablename,
				  keycolcool,fkeycolcoral,pkeycolcoral);
  }
}

bool CoraCoolDatabase::parseFolderDescription(const std::string folderdesc,
					      std::string& tablename,
					      std::string& keycolcool,
					      std::string& fkeycolcoral,
					      std::string& pkeycolcoral) {
  // parse the folder description string to extract CORAL table and FKs
  // format <coracool>Table:COOL_key:CORAL_fkey:CORAL_pkey</coracool>
  // if CORAL_pkey is missing, assumed to be same as CORAL_fkey
  // return false for parsing problem or missing <coracool> specification
  // otherwise set return arguments

  // find <coracool> part of the description
  std::string::size_type p1,p2,c1,c2,c3;
  p1=folderdesc.find("<coracool>");
  p2=folderdesc.find("</coracool>");
  if (p1==std::string::npos || p2==std::string::npos) return false;
  // find the two : separating the values
  c1=folderdesc.find(':',p1+10);
  if (c1==std::string::npos) return false;
  c2=folderdesc.find(':',c1+1);
  if (c2==std::string::npos) return false;

  tablename=m_dbname+"_"+folderdesc.substr(p1+10,c1-p1-10);
  keycolcool=folderdesc.substr(c1+1,c2-c1-1);
  fkeycolcoral=folderdesc.substr(c2+1,p2-c2-1);
  // check for third colon to specify separate primary key
  c3=folderdesc.find(':',c2+1);
  if (c3!=std::string::npos) {
    fkeycolcoral=folderdesc.substr(c2+1,c3-c2-1);
    pkeycolcoral=folderdesc.substr(c3+1,p2-c3-1);
  } else {
    fkeycolcoral=folderdesc.substr(c2+1,p2-c2-1);
    pkeycolcoral=fkeycolcoral;
  }
  return true;
}

bool CoraCoolDatabase::deleteFolder(const std::string& coolfolder) {
  m_log << coral::Debug << "Deleting folder " << coolfolder << coral::MessageStream::endmsg;
  std::string desc;
  // get COOL folder and description
  try {
    cool::IFolderPtr folder=m_cooldb->getFolder(coolfolder);
    desc=folder->description();
    // delete the COOL folder
    m_cooldb->dropNode(coolfolder);
  }
  catch (cool::Exception& e) {
    throw CoraCoolException(e.what(),"CoraCoolDatabase::deleteFolder");
  }
  // extract the CORAL folder name
  std::string tablename,keycolcool,fkeycolcoral,pkeycolcoral;
  if (parseFolderDescription(desc,tablename,keycolcool,
			     fkeycolcoral,pkeycolcoral)) {
    m_log << coral::Debug << "Drop CORAL payload table " << tablename
			    << coral::MessageStream::endmsg;
    // drop the CORAL table
    connect();
    m_proxy->transaction().start(false);
    m_proxy->nominalSchema().dropIfExistsTable(tablename);
    // remove primary/foreign key rows from sequence table
    CoraCoolSequence seqfk(m_dbname,tablename+"_FK",m_proxy);
    seqfk.dropSeq();
    if (fkeycolcoral!=pkeycolcoral) {
      CoraCoolSequence seqpk(m_dbname,tablename+"_PK",m_proxy);
      seqpk.dropSeq();
    }
    // remove the row from the CORACOOLATTR table
    coral::AttributeList bindvar ATLAS_THREAD_SAFE; // Not shared, ok
    bindvar.extend<std::string>("SNAME");
    bindvar[0].data<std::string>()=tablename;
    coral::ITable& table=m_proxy->nominalSchema().tableHandle(
				       m_dbname+"_CORACOOLATTR");
    long rows=table.dataEditor().deleteRows("NAME=:SNAME",bindvar);
    if (rows!=1) m_log << coral::Error << "Unexpected number of rows "
       << rows << " with key " << tablename << 
       " deleted from CORACOOLATTR table" << coral::MessageStream::endmsg;
    m_proxy->transaction().commit();
    // pause for thought
    disconnect();
    sleep(1);
    connect();
  }
  return true;
}

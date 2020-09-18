/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

// CoraCoolFolder.cxx
// implementation for CoraCoolFolder
// Richard Hawkings, started 10/2006

#include "RelationalAccess/ISessionProxy.h"
#include "RelationalAccess/ITransaction.h"
#include "RelationalAccess/ISchema.h"
#include "RelationalAccess/ITable.h"
#include "RelationalAccess/ITableDescription.h"
#include "RelationalAccess/ITableDataEditor.h"
#include "RelationalAccess/IBulkOperation.h"
#include "RelationalAccess/IColumn.h"
#include "RelationalAccess/IQuery.h"
#include "RelationalAccess/ICursor.h"
#include "RelationalAccess/SchemaException.h"
#include "CoralBase/Attribute.h"
#include "CoralBase/Exception.h"
#include "CoolKernel/ValidityKey.h"
#include "CoolKernel/IDatabase.h"
#include "CoolKernel/IFolder.h"
#include "CoolKernel/IObject.h"
#include "CoolKernel/Record.h"
#include "CoolKernel/StorageType.h"
#include "CoraCool/CoraCoolObject.h"
#include "CoraCool/CoraCoolObjectIter.h"
#include "CoraCoolSequence.h"
#include "CoraCool/CoraCoolDatabase.h"
#include "CoraCool/CoraCoolException.h"

#include "CoraCool/CoraCoolFolder.h"
#include "CxxUtils/checker_macros.h"

// size of attributelist buffer for write operations
#define WB_SIZE 256

CoraCoolFolder::CoraCoolFolder(const std::string& coolfolder, 
             coral::ISessionProxy* proxy, cool::IDatabasePtr cooldb,
             CoraCoolDatabase* coradb, coral::MessageStream& log)
        :
        m_foldername(coolfolder),
        m_proxy(proxy),m_cooldb(cooldb),m_coradb(coradb),
        m_log(log),
	m_bulkactive(false),m_payloadbuf(0),m_bulki(0),m_seqpk(0),m_seqfk(0),
	m_nextpk(0),m_usedpk(0),m_nextfk(0),m_usedfk(0)
{
  m_dbname=m_coradb->dbname();

  // get information from COOL
  m_log << coral::Debug << "Initialize CoraCoolFolder " << coolfolder << 
    " in database instance " << m_dbname << coral::MessageStream::endmsg;

  std::string folderdesc;
  try {
    m_coolfolder=m_cooldb->getFolder(m_foldername);
    folderdesc=m_coolfolder->description();
  }
  catch (cool::Exception& e) {
    throw CoraCoolException(e.what(),"CoraCoolFolder::CoraCoolFolder");
  }
  m_log << coral::Debug << "COOL Folder description is: " << folderdesc 
			  << coral::MessageStream::endmsg;

  // set up linking information between COOL and CORAL folders
  if (m_coradb->parseFolderDescription(folderdesc,m_tablename,m_keycolcool,
		       m_keycolcoral,m_pkeycolcoral)) {
    m_pkey=(m_keycolcoral!=m_pkeycolcoral);
  } else {
    throw CoraCoolException("incorrect <coracool> description string ",
				  "CoraCoolFolder::CoraCoolFolder");
  }

  // obtain information about CORAL folder
  try {
    m_proxy->transaction().start(true);
    coral::ISchema& schema=m_proxy->nominalSchema();
    m_table=&schema.tableHandle(m_tablename);
    m_tabledesc=&(m_table->description());
    // find type of foreign key
    // lookup attribute specification for query outputs
    if (!decodeAttrSpec()) throw CoraCoolException(
      "Problem extracting attribute specification",
      "CoraCoolFolder::CoraCoolFolder");
    m_proxy->transaction().commit();
  }
  catch (coral::SessionException& e) {
    throw CoraCoolException("Problem accessing CORAL table "+m_tablename,
				  "CoraCoolFolder::CoraCoolFolder");
  }
}

namespace {
int delPayload ATLAS_NOT_THREAD_SAFE (coral::AttributeList* buf)
{
  delete buf;
  return 0;
}
}
CoraCoolFolder::~CoraCoolFolder() {
  // Suppress thread-safety checker warning.
  // Ok here since the specification is not shared.
  // FIXME: Attributes on lambdas don't work with gcc 8.3.
  //        Does work with gcc 10.
  //auto delPayload = [&] ATLAS_NOT_THREAD_SAFE () { delete m_payloadbuf; return 0; };
  int dum [[maybe_unused]] ATLAS_THREAD_SAFE = delPayload(m_payloadbuf);

  if (m_seqpk) delete m_seqpk;
  if (m_seqfk) delete m_seqfk;
}

const cool::IRecordSpecification& CoraCoolFolder::fkSpecification() 
 const 
{ return m_coolfolder->payloadSpecification(); }

const cool::RecordSpecification 
 CoraCoolFolder::payloadSpecification()  const {
  // have to assemble a RecordSpecification and return the specification
  // as ITableDescription does not allow to get this directly
  cool::RecordSpecification atrspec;
  for (AttrItr itr=m_attrvec.begin();itr!=m_attrvec.end();++itr)
    atrspec.extend(itr->first,nameToCoolType(itr->second));
  return atrspec;
}

coral::AttributeList CoraCoolFolder::emptyAttrList ATLAS_NOT_THREAD_SAFE () const {
  // assemble and return an attributelist for use in constructing payloads
  coral::AttributeList atrlist ATLAS_THREAD_SAFE;
  for (AttrItr itr=m_attrvec.begin();itr!=m_attrvec.end();++itr)
    atrlist.extend(itr->first,
     cool::StorageType::storageType(nameToCoolType(itr->second)).cppType());
  return atrlist;
}

int CoraCoolFolder::storeObject ATLAS_NOT_THREAD_SAFE (const cool::ValidityKey& since, 
		   const cool::ValidityKey until,
		   const_iterator begin,
		   const_iterator end,
		   const cool::ChannelId& channelId,
		   const std::string& tagName,
		   const bool userTagOnly) {
  // store objects into CORAL table, and reference in COOL table
  // generate new FK and PK for CORAL data as needed

  int fkey=0;
  if (m_bulkactive) {
    fkey=m_nextfk++;
    ++m_usedfk;
    bulkInsert(begin,end,fkey,true);
  } else {
    // non-bulk insert
    // first count number of CORAL rows to be inserted
    unsigned int nrow=0;
    for (const_iterator payitr=begin;payitr!=end;++payitr) ++nrow;
    m_log << coral::Debug << "Storing " << nrow << " rows in CoraCool folder " 
      << m_foldername << " channel " << channelId << 
      coral::MessageStream::endmsg;

    // generate keys - start transaction here
    int pkey;
    try {
      m_proxy->transaction().start(false);
      CoraCoolSequence seqfk(m_dbname,m_tablename+"_FK",m_proxy,false);
      fkey=seqfk.fetch();
      if (m_pkey) {
        CoraCoolSequence seqfk(m_dbname,m_tablename+"_PK",m_proxy,false);
        pkey=seqfk.fetch(nrow);
      } else {
        pkey=0; // set to 0 to satisfy compiler
      }
      // commit key transaction, releasing lock on keytable
      m_proxy->transaction().commit();
    }
    catch (coral::Exception& e) {
      throw CoraCoolException(e.what(),"CoraCoolFolder::storeObject(key)");
    }

    // insert rows into CORAL, using bulk operation
    try {
      m_proxy->transaction().start(false);
      coral::ISchema& schema=m_proxy->nominalSchema();
      m_table=&schema.tableHandle(m_tablename);
      coral::ITableDataEditor& editor=m_table->dataEditor();
      coral::AttributeList data=*begin;
      coral::IBulkOperation* bulki=editor.bulkInsert(data,WB_SIZE);

      for (const_iterator payitr=begin;payitr!=end;++payitr) {
        // copy data from iterator
        data.fastCopyData(*payitr);
        // update the keys
        setAttrKey(data[m_keycolcoral],fkey);
        if (m_pkey) setAttrKey(data[m_pkeycolcoral],pkey++);
        // store data into CORAL table
        bulki->processNextIteration();
      }
      bulki->flush();
      m_proxy->transaction().commit();
    }
    catch (coral::Exception& e) {
      throw CoraCoolException(e.what(),"CoraCoolFolder::storeObject(payload)");
    }

    // now store data in COOL, with correct foreign key (bulk/non bulk)
    // in bulk case, the COOL operation is already going via storage buffer
  }
  cool::Record coolfk(m_coolfolder->payloadSpecification());
  setFieldKey(coolfk[m_keycolcool],fkey);
  try {
    m_coolfolder->storeObject(since,until,coolfk,channelId,tagName,
			      userTagOnly);
  }
  catch (cool::Exception &e) {
    throw CoraCoolException(e.what(),"CoraCoolFolder::storeObject");
  }
  // return FK value, in case user wants to use it in a reference object call
  return fkey;
}

// Actually ok, but marked as not thread-safe due to the copy
// of the (non-shared) AttributeList.
bool CoraCoolFolder::setupStorageBuffer ATLAS_NOT_THREAD_SAFE() {
  // flush any existing data first to get clean state
  if (m_bulkactive) flushStorageBuffer();
  if (m_payloadbuf==0) {
    // actions done if this is the first time setupStorageBuffer done for
    // this folder
    // create AttributeList as bulk buffer
    m_payloadbuf=new coral::AttributeList(emptyAttrList());
    // setup bulk storage for the COOL folder
    m_coolfolder->setupStorageBuffer();
  }
  // setup bulk operation buffer - transaction begins here
  m_proxy->transaction().start(false);
  coral::ISchema& schema=m_proxy->nominalSchema();
  m_table=&schema.tableHandle(m_tablename);
  coral::ITableDataEditor& editor=m_table->dataEditor();
  m_bulki=editor.bulkInsert(*m_payloadbuf,WB_SIZE);
  // prepare the key sequences
  if (m_pkey) {
    if (m_seqpk==0) 
      m_seqpk=new CoraCoolSequence(m_dbname,m_tablename+"_PK",m_proxy,false);
    m_seqpk->querySeq(m_nextpk,true,true);
    m_usedpk=0;
  }
  if (m_seqfk==0) 
    m_seqfk=new CoraCoolSequence(m_dbname,m_tablename+"_FK",m_proxy,false);
  m_seqfk->querySeq(m_nextfk,true,true);
  m_usedfk=0;
  m_bulkactive=true;
  return true;
}

void CoraCoolFolder::flushStorageBuffer() {
  if (!m_bulkactive) return;
  // execute bulk store
  try {
    m_bulki->flush();
  // update the keys
    if (m_usedpk>0) m_seqpk->fetch(m_usedpk);
    if (m_usedfk>0) m_seqfk->fetch(m_usedfk);
    // commit transaction for payload table and keys
    m_proxy->transaction().commit();
  }
  catch (coral::Exception& e) {
    throw CoraCoolException(e.what(),"CoraCoolFolder::flushStorageBuffer");
  }
  m_coolfolder->flushStorageBuffer();
  m_bulkactive=false;
}

void CoraCoolFolder::referenceObject(const cool::ValidityKey& since, 
		       const cool::ValidityKey& until,
		       const coral::Attribute& fkey,
		       const cool::ChannelId& channelId,
		       const std::string& tagName,
		       const bool userTagOnly) {
  // add reference to existing payload object
  // call COOL API, just checking fkey has correct name
  if (fkey.specification().name()==m_keycolcool) {
    cool::Record fkeylist(m_coolfolder->payloadSpecification());
    setFieldAttr(fkeylist[m_keycolcool],fkey);
    m_coolfolder->storeObject(since,until,fkeylist,channelId,tagName,
    			      userTagOnly);
  } else {
    throw CoraCoolException("Foreign key name should be "+m_keycolcool,
        "CoraCool::Folder::referenceObject");
  }
}

void CoraCoolFolder::referenceObject(const cool::ValidityKey& since, 
		       const cool::ValidityKey& until,
		       const int ifkey,
		       const cool::ChannelId& channelId,
		       const std::string& tagName,
		       const bool userTagOnly) {
  // add reference to existing payload object, supplying key as int
  cool::Record fkeylist(m_coolfolder->payloadSpecification());
  setFieldKey(fkeylist[m_keycolcool],ifkey);
  m_coolfolder->storeObject(since,until,fkeylist,channelId,tagName,
			    userTagOnly);
}

void CoraCoolFolder::addPayload ATLAS_NOT_THREAD_SAFE (const_iterator begin, const_iterator end) { // Copies AttributeList, giving shared specification.
  // add payload to the CORAL table, using existing FKs in data
  // but regenerating primary keys

  // only makes sense to use this method for folders with separate pkeys
  if (!m_pkey) throw CoraCoolException("Folder has no primary key",
				       "CoralCoolFolder::addPayload");
  if (m_bulkactive) {
    // via bulk operation buffer
    bulkInsert(begin,end,0,false);
  } else {
    // first count number of CORAL rows to be inserted
    unsigned int nrow=0;
    for (const_iterator payitr=begin;payitr!=end;++payitr) ++nrow;
    m_log << coral::Debug << "Adding " << nrow << " into CoraCool folder " 
      << m_foldername << coral::MessageStream::endmsg;
    // generate keys - start transaction here
    int pkey;
    try {
      m_proxy->transaction().start(false);
      CoraCoolSequence seqfk(m_dbname,m_tablename+"_PK",m_proxy,false);
      pkey=seqfk.fetch(nrow);
      // commit key transaction, releasing lock on keytable
      m_proxy->transaction().commit();
    }
    catch (coral::Exception& e) {
      throw CoraCoolException(e.what(),"CoraCoolFolder::addPayload(key)");
    }
    // insert rows into CORAL
    try {
      m_proxy->transaction().start(false);
      coral::ISchema& schema=m_proxy->nominalSchema();
      m_table=&schema.tableHandle(m_tablename);
      coral::ITableDataEditor& editor=m_table->dataEditor();

      for (const_iterator payitr=begin;payitr!=end;++payitr) {
        // store data, updating pkey
        coral::AttributeList data=*payitr;
        setAttrKey(data[m_pkeycolcoral],pkey++);
        editor.insertRow(data);
      }
      m_proxy->transaction().commit();
    }
    catch (coral::Exception& e) {
      throw CoraCoolException(e.what(),"CoraCoolFolder::addPayload(payload)");
    }
  }
}

bool CoraCoolFolder::setAttrKey(coral::Attribute& attr,
				const int keyval) {
  // set the value of the key in the attribute, regardless of type
  const std::string typen=attr.specification().typeName();
  if (typen=="int") {
    attr.data<int>()=keyval;
  } else if (typen=="unsigned int") {
    attr.data<unsigned int>()=keyval;
  } else if (typen=="long long") {
    attr.data<long long>()=keyval;
  } else if (typen=="unsigned long long") {
    attr.data<unsigned long long>()=keyval;
  } else {
    // throw exception here?
    m_log << coral::Error << "Unrecognised key type " << typen << 
    " in CoraCoolFolder::setAttrKey" << coral::MessageStream::endmsg;
    return false;
  }
  return true;
}

bool CoraCoolFolder::setFieldKey(cool::IField& attr,
				const int keyval) {
  // set the value of the key in the Record, regardless of type
  const cool::StorageType& typen=attr.specification().storageType();
  if (typen==cool::StorageType::Int32) {
    attr.setValue<int>(keyval);
  } else if (typen==cool::StorageType::UInt32) {
    attr.setValue<unsigned int>(keyval);
  } else if (typen==cool::StorageType::Int64) {
    attr.setValue<long long>(keyval);
  } else if (typen==cool::StorageType::UInt63) {
    attr.setValue<unsigned long long>(keyval);
  } else {
    // throw exception here?
    m_log << coral::Error << "Unrecognised key type " << typen.name() << 
      " in CoraCoolFolder::setFieldKey" << coral::MessageStream::endmsg;
    return false;
  }
  return true;
}

bool CoraCoolFolder::setFieldAttr(cool::IField& attr,
				  const coral::Attribute& keyval) {
  // set the value of the key in the Record, from input Attribute
  const cool::StorageType& typen=attr.specification().storageType();
  if (typen==cool::StorageType::Int32) {
    attr.setValue<int>(keyval.data<int>());
  } else if (typen==cool::StorageType::UInt32) {
    attr.setValue<unsigned int>(keyval.data<unsigned int>());
  } else if (typen==cool::StorageType::Int64) {
    attr.setValue<long long>(keyval.data<long long>());
  } else if (typen==cool::StorageType::UInt63) {
    attr.setValue<unsigned long long>(keyval.data<unsigned long long>());
  } else {
    // throw exception here?
    m_log << coral::Error << "Unrecognised key type " << typen.name() << 
      " in CoraCoolFolder::setFieldAttr" << coral::MessageStream::endmsg;
    return false;
  }
  return true;
}


int CoraCoolFolder::getAttrKey(const coral::Attribute& attr) {
  const std::string typen=attr.specification().typeName();
  if (typen=="int") {
    return attr.data<int>();
  } else if (typen=="unsigned int") {
    return static_cast<int>(attr.data<unsigned int>());
  } else if (typen=="long long") {
    return static_cast<int>(attr.data<long long>());
  } else if (typen=="unsigned long long") {
    return static_cast<int>(attr.data<unsigned long long>());
  } else {
    m_log << coral::Error << "Unrecognised key type " << typen << 
    " in CoraCoolFolder::getAttrKey" << coral::MessageStream::endmsg;
    return 0;
  }
}

bool CoraCoolFolder::decodeAttrSpec() {
  // read information from the CORACOOLATTR table
  bool bres=false;
  coral::IQuery* query=0;
  try {
    coral::ITable& table=m_proxy->nominalSchema().tableHandle(
				       m_dbname+"_CORACOOLATTR");
    query=table.newQuery();
    coral::AttributeList bindvar ATLAS_THREAD_SAFE; // Not shared, ok
    bindvar.extend<std::string>("SNAME");
    bindvar[0].data<std::string>()=m_tablename;
    query->setCondition("NAME=:SNAME",bindvar);
    query->setRowCacheSize(1);
    query->defineOutputType("NAME","string");
    coral::ICursor& cursor=query->execute();
    if (cursor.next()) {
      const coral::AttributeList& res=cursor.currentRow();
      std::string spec=res["ATTRSPEC"].data<std::string>();
      // decode specification
      std::string::size_type iofs1,iofs2,iofs3;
      iofs1=0;
      bool last=false;
      while (!last) {
        iofs2=spec.find(":",iofs1);
        iofs3=spec.find(",",iofs2);
        if (iofs3==std::string::npos) {
          iofs3=spec.size();
          last=true;
        }
        m_attrvec.push_back(std::pair<std::string,std::string> (
         spec.substr(iofs1,iofs2-iofs1),spec.substr(iofs2+1,iofs3-iofs2-1)));
        if (!last) iofs1=iofs3+1;
      }
      bres=true;
    } else {
      m_log << coral::Error << "No data obtained for CORACOOLATTR" << 
	coral::MessageStream::endmsg;
    }
  }
  catch (coral::Exception &e) {
    m_log << coral::Error << "Exception reading CORACOOLATTR table: "
			    << e.what() << coral::MessageStream::endmsg;
  }
  delete query;
  return bres;
}

cool::StorageType::TypeId CoraCoolFolder::nameToCoolType(
			      const std::string& coolName) const {
  if (coolName=="Int16") return cool::StorageType::Int16;
  if (coolName=="UInt16") return cool::StorageType::UInt16;
  if (coolName=="Int32") return cool::StorageType::Int32;
  if (coolName=="UInt32") return cool::StorageType::UInt32;
  if (coolName=="UInt63") return cool::StorageType::UInt63;
  if (coolName=="Int64") return cool::StorageType::Int64;
  if (coolName=="Float") return cool::StorageType::Float;
  if (coolName=="Double") return cool::StorageType::Double;
  if (coolName=="String255") return cool::StorageType::String255;
  if (coolName=="String4k") return cool::StorageType::String4k;
  if (coolName=="String64k") return cool::StorageType::String64k;
  if (coolName=="String16M") return cool::StorageType::String16M;
  if (coolName=="Blob64k") return cool::StorageType::Blob64k;

  throw CoraCoolException("Undefined type: "+coolName,
    "CoraCoolFolder::nameToCoolType");
}

void CoraCoolFolder::setOutputSpec(coral::IQuery* query) {
  for (AttrItr itr=m_attrvec.begin();itr!=m_attrvec.end();++itr)
    query->defineOutputType(itr->first,
      coral::AttributeSpecification::typeNameForId(
      cool::StorageType::storageType(nameToCoolType(itr->second)).cppType()));
}

void CoraCoolFolder::bulkInsert(const_iterator begin,const_iterator end,
				const int fkey,bool updatefk) {
  // do bulk insertion into CORAL table buffer
  // if updateFK=true, update rows with given FK value
  try {
    for (const_iterator payitr=begin;payitr!=end;++payitr) {
      // copy data into buffer bound to bulk insert
      m_payloadbuf->fastCopyData(*payitr);
      // update the keys
      if (updatefk) setAttrKey((*m_payloadbuf)[m_keycolcoral],fkey);
      if (m_pkey) {
	setAttrKey((*m_payloadbuf)[m_pkeycolcoral],m_nextpk++);
	++m_usedpk;
      }
      // add to bulk operation
      m_bulki->processNextIteration();
    }
  }
  catch (coral::Exception& e) {
    throw CoraCoolException(e.what(),"CoraCoolFolder::bulkInsert");
  }
}

CoraCoolObjectPtr CoraCoolFolder::findObject(
   const cool::ValidityKey& pointInTime, const cool::ChannelId& channelId, 
   const std::string& tagName) {
  // first query cool to get foreign key
  m_log << coral::Debug << "CoraCoolFolder query time " << pointInTime << 
    " channel " << channelId << " tag " << tagName << coral::MessageStream::endmsg;
  cool::IObjectPtr obj=m_coolfolder->findObject(pointInTime,channelId,tagName);
  // create result object with IOV from COOL
  CoraCoolObjectPtr result(new CoraCoolObject(obj->since(),obj->until(),
					      obj->channelId()));
  // now query CORAL for payload
  try {
    m_proxy->transaction().start(true);
    coral::IQuery* query=m_table->newQuery();
    std::string where=m_keycolcoral+"=:"+m_keycolcool;
    const coral::AttributeList& fkeybind=obj->payload().attributeList();
    query->setCondition(where,fkeybind);
    coral::ICursor& cursor=query->execute();
    while (cursor.next()) {
      const coral::AttributeList& res=cursor.currentRow();
      result->add(res);
    }
    delete query;
    m_proxy->transaction().commit();
  }
  catch (coral::Exception& e) {
    throw CoraCoolException(e.what(),"CoraCoolFolder::findObject");
  }
  return result;
}

CoraCoolObjectIterPtr CoraCoolFolder::browseObjects(
       const cool::ValidityKey& pointInTime,
       const cool::ChannelSelection& channels,
       const std::string& tagName) {
  return browseObjects(pointInTime,pointInTime,channels,tagName);
}

CoraCoolObjectIterPtr CoraCoolFolder::browseObjects(
       const cool::ValidityKey& since,
       const cool::ValidityKey& until,
       const cool::ChannelSelection& channels,
       const std::string& tagName)
{
  // first initialise COOL query
  cool::IObjectIteratorPtr coolitr=m_coolfolder->browseObjects(since,until,
	channels,tagName);
  CoraCoolObjectIterPtr itr(new CoraCoolObjectIter(this,coolitr));
  return itr;
}

void CoraCoolFolder::setPrefetchAll(bool prefetchAll) {
  // pass setting onto COOL - no special actions for CoraCool yet
  m_coolfolder->setPrefetchAll(prefetchAll);
}

coral::ITable* CoraCoolFolder::table() const { 
  return &(m_proxy->nominalSchema().tableHandle(m_tablename));
}

/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#ifndef CORACOOL_CORACOOLFOLDER_H
#define CORACOOL_CORACOOLFOLDER_H
// CoraCoolFolder.h - interface to a COOL folder with data stored in
// a separate CORAL payload table
// Richard Hawkings, started 10/2006

#include <vector>
#include <string>
#include "CoralBase/MessageStream.h"
#include "CoralBase/AttributeList.h"
#include "CoralBase/Attribute.h"
#include "CoolKernel/ValidityKey.h"
#include "CoolKernel/ChannelSelection.h"
#include "CoolKernel/RecordSpecification.h"
#include "CoolKernel/IField.h"
#include "CoolKernel/pointers.h"
#include "CoraCool/CoraCoolTypes.h"

#include "CxxUtils/checker_macros.h"


class CoraCoolSequence;

namespace coral {
  class ISessionProxy;
  class ITable;
  class ITableDescription;
  class IColumn;
  class IQuery;
  class IBulkOperation;
}

class CoraCoolFolder {
  // allow the object iterator to access internals
 friend class CoraCoolObjectIter;
 public:
  // iterators for storing objects
  typedef std::vector<coral::AttributeList>::const_iterator const_iterator;

  // constructor - do not use this directly, use CoraCoolDatabase::getFolder
  CoraCoolFolder(const std::string& coolfolder,
                 coral::ISessionProxy* proxy, cool::IDatabasePtr cooldb,
		 CoraCoolDatabase* coradb, coral::MessageStream& log);

  // destructor
  ~CoraCoolFolder();

  // Disallow, since we manage resources.
  CoraCoolFolder (const CoraCoolFolder&) = delete;
  CoraCoolFolder& operator= (const CoraCoolFolder&) = delete;

  // properties of the folder
  std::string coralTableName() const;
  const std::string& coralFKey() const;
  const std::string& coralPKey() const;
  const cool::IRecordSpecification& fkSpecification() const;
  const cool::RecordSpecification payloadSpecification() const;
  // Actually ok, but need to declare it ATLAS_NOT_THREAD_SAFE to avoid
  // warnings related to the return-by-value.
  coral::AttributeList emptyAttrList ATLAS_NOT_THREAD_SAFE() const;

  // allow access to the underlying COOL folder
  cool::IFolderPtr coolFolder();

  // storing data - all these can throw CoraCool and Cool exceptions
  // store a vector of coral::AtributeLists (identified by begin/end iterators)
  // and store the reference in COOL, with IOV (since/until), channel and tag
  // the primary and foreign key values of the AttributeLists will be ignored
  // as the keys will be allocated internally by the API
  // return the value of the FK in case it is required later
  int storeObject ATLAS_NOT_THREAD_SAFE
                  (const cool::ValidityKey& since, 
		   const cool::ValidityKey until,
		   const_iterator begin,
		   const_iterator end,
		   const cool::ChannelId& channelId=0,
		   const std::string tagName="",
		   const bool userTagOnly=false);

  // setup storage buffer for bulk insertion via repeated storeObject calls
  bool setupStorageBuffer ATLAS_NOT_THREAD_SAFE();
  // flush the storage buffer (execute bulk insertion of objects)
  // unlike in COOL, flush deactivates the buffer - use setupStorageBuffer
  // again after flush if you want to carry on
  void flushStorageBuffer();

  // add a reference in COOL to an existing stored payload object identified
  // by its foreign key (as an Attribute - normally extracted from the
  // AttributeList), specifying the COOL IOV, channel and tag
  void referenceObject(const cool::ValidityKey& since, 
		       const cool::ValidityKey& until,
		       const coral::Attribute& fkey,
		       const cool::ChannelId& channelId=0,
		       const std::string tagName="",
		       const bool userTagOnly=false);

  // add a reference to COOL to an existing stored payload object
  // same as above, but taking the FK as a plain int
  void referenceObject(const cool::ValidityKey& since, 
		       const cool::ValidityKey& until,
		       const int ifkey,
		       const cool::ChannelId& channelId=0,
		       const std::string tagName="",
		       const bool userTagOnly=false);

  // add more payload AttributeLists to the CORAL payload table
  // the foreign key column is used to determine which existing COOL entries
  // will reference this data
  // the primary key column will be ignored and values allocated by the API
  void addPayload ATLAS_NOT_THREAD_SAFE (const_iterator begin, const_iterator end);

  // accessing data - all these can throw CoraCool and Cool exceptions
  // find the one object valid at a given time/channel and tag
  CoraCoolObjectPtr findObject(const cool::ValidityKey& pointInTime,
			       const cool::ChannelId& channelId=0, 
			       const std::string tagName="");

  // return an iterator to a set of objects identified by a point in time
  // and a channel specification (and optionally a tag)
  CoraCoolObjectIterPtr browseObjects(
       const cool::ValidityKey& pointInTime,
       const cool::ChannelSelection& channels,
       const std::string& tagName="");

  // return an interator to a set of objects identified by a range in 
  // time and a channel specification (and optionally a tag)
  CoraCoolObjectIterPtr browseObjects(
       const cool::ValidityKey& since=cool::ValidityKeyMin,
       const cool::ValidityKey& until=cool::ValidityKeyMax,
       const cool::ChannelSelection& channels=cool::ChannelSelection(0),
       const std::string& tagName="");

  // setup folder preFetch status (passed on to COOL)
  void setPrefetchAll(const bool prefetchAll);

  // Helper functions to set and return a FK/PK Attribute as an int
  // First oner eturns False if it fails
  // keys can be int, unsigned int, long long / unsigned long long
  bool setAttrKey(coral::Attribute& attr,const int keyval);
  bool setFieldKey(cool::IField& attr,const int keyval);
  bool setFieldAttr(cool::IField& attr,const coral::Attribute& keyval);
  int getAttrKey(const coral::Attribute& attr);

 private:
  // accessors for friend class CoraCoolObjectIter
  coral::ISessionProxy* proxy();
  const coral::ISessionProxy* proxy() const;
  coral::ITable* table() const;
  void setOutputSpec(coral::IQuery* query);

  bool decodeAttrSpec();
  cool::StorageType::TypeId nameToCoolType(const std::string& coolName) const;
  void bulkInsert(const_iterator begin,const_iterator end, 
		  const int fkey,bool updatefk);

  std::string m_foldername;      // name of the COOL folder
  std::string m_dbname;          // name of the COOL database instance
  coral::ISessionProxy* m_proxy; // database session proxy
  cool::IDatabasePtr m_cooldb;   // pointer to the COOL database
  CoraCoolDatabase* m_coradb;    // pointer to the parent CoraCool database
  coral::MessageStream& m_log;

  std::string m_tablename;    // name of the CORAL table for the payload
  std::string m_keycolcoral;  // name of the foreign key in CORAL table
  std::string m_keycolcool;   // name of the foreign key in COOL table
  std::string m_pkeycolcoral; // name of the primary key in CORAL table
  bool m_pkey; // table has separate primary key (not reusing foreign key)
  cool::IFolderPtr m_coolfolder; // pointer to the COOL folder
  coral::ITable* m_table;        // pointer to the payload table
  const coral::ITableDescription* m_tabledesc;  // and its description
  // vector of pairs of column name/type
  typedef std::vector<std::pair<std::string,std::string> > AttrVec;
  typedef AttrVec::const_iterator AttrItr;
  AttrVec m_attrvec;
  // variables for the bulk operations
  bool m_bulkactive;
  coral::AttributeList* m_payloadbuf;
  coral::IBulkOperation* m_bulki;
  CoraCoolSequence* m_seqpk;
  CoraCoolSequence* m_seqfk;
  int m_nextpk;
  int m_usedpk;
  int m_nextfk;
  int m_usedfk;
};

inline std::string CoraCoolFolder::coralTableName() const 
{ return m_tablename.substr(m_dbname.size()+1); }
  
inline const std::string& CoraCoolFolder::coralFKey() const
{ return m_keycolcoral; }

inline const std::string& CoraCoolFolder::coralPKey() const 
{ return m_pkeycolcoral; }

inline cool::IFolderPtr CoraCoolFolder::coolFolder()
{ return m_coolfolder; }

inline coral::ISessionProxy* CoraCoolFolder::proxy() {return m_proxy; }
inline const coral::ISessionProxy* CoraCoolFolder::proxy() const {return m_proxy; }

// inline coral::ITable* CoraCoolFolder::table() const { return m_table; }

#endif // CORACOOL_CORACOOLFOLDER_H

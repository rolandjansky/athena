/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//*****************************************************************************
//
// $Id: CoralDB.h,v 1.51 2008-11-18 16:28:43 hirsch Exp $
//
// CoralDB is an implementation of the pixel connectivity database based on CORAL.
//
// Written in August 2006 by Juerg Beringer.
//
//*****************************************************************************

// TODO: THINK ABOUT ERROR HANDLING

#ifndef CORALDB_H
#define CORALDB_H

#include "RelationalAccess/AccessMode.h"
#include "CoralBase/MessageStream.h"

#include <string>
#include <vector>
#include <map>
#include <set>
#include <stdexcept>
#include <ostream>
#include <ctime>

#include "CompoundTag.h"

namespace coral {
  class Context;
  class ISessionProxy;
  class IQueryDefinition;
  class AttributeList;
  class Blob;
}

namespace CoralDB {
#if 0
} 
#endif

using namespace std;

class Connection;
class Alias;

typedef map<string,string> ObjectDictionaryMap;
typedef vector<Connection> ConnectionTableMap;
typedef set<pair<string,string> > ClobNameContainer; // {ID, field}
typedef std::vector<Alias> AliasesTable;
typedef vector<pair<time_t,CompoundTag> > HistoryTable;
typedef vector<pair<string,string> > ClobDataContainer;



class TagStatus: private pair<string,bool>  // TAG, locked
{ public: TagStatus(const string& s, bool b) : pair<string,bool>(s,b) {} 
 string tag() const { return first; }
 bool locked() const { return second; }
};

ostream& operator<<(ostream& os, const TagStatus ts);

typedef vector<TagStatus> IdTagList;
typedef vector<TagStatus> TagList;



// Exception classes
class RowNotFound : public std::runtime_error { public: RowNotFound(const std::string& msg) : std::runtime_error(msg) {} };
class TagLocked : public std::runtime_error { public: TagLocked(const std::string& msg) : std::runtime_error(msg) {} };
class IdTagUnlocked : public std::runtime_error { public: IdTagUnlocked(const std::string& msg) : std::runtime_error(msg) {} };
class EmptyTag : public std::runtime_error { public: EmptyTag(const std::string& msg) : std::runtime_error(msg) {} };

class InvalidCompoundTag : public std::runtime_error { public: InvalidCompoundTag(const std::string& prefix, const CompoundTag& tags); };

// Interface to the database
class CoralDB {
 public:

   static const bool AUTOCOMMIT = true;
   static const bool MANUAL_COMMIT = false;  

   CoralDB(const string& connection, coral::AccessMode accessMode, coral::MsgLevel outputLevel, bool autoCommit);
   
   void setMsgLevel(coral::MsgLevel outputLevel);

   // Explicit transaction handling.  All of the void methods are no-ops in the auto-commit mode.
   // In the manual commit mode, one must explicitly start and commit update transactions.
   // There is no need (and no way) to explicitly start a read-only transaction.
   void transactionStart();
   void transactionCommit();
   void transactionRollBack();
   bool transactionIsActive() const;
   bool isAutoCommitMode() const { return m_autocommit; }

   // Explicit control of caching.
   // Only the lock status of the current tags is cached at the moment.
   // Stale cache may allow one to modify a tag that has been locked by another client.
   bool getCacheUse() const { return m_cacheEnabled; }
   void setCacheUse(bool state) { m_cacheEnabled = state; }
   void updateCache() const; // read from the database and update cached values

   virtual ~CoralDB();

   // Methods related to schema creation
   void createSchema();
   void dropSchema();

   // Methods dealing with tags
   CompoundTag compoundTag() const { return m_tags; } // returns the current set of tags
   void setCompoundTag(const CompoundTag& existingTag); // makes the given tags current
   void setObjectDictionaryTag(const string& existingTag);
   void setConnectivityTag(const string& existingTag);
   void setDataTag(const string& existingTag);
   void setAliasTag(const string& existingTag);

   // Return sorted lists.  The default idTag=="" means use the current idTag.
   void getExistingObjectDictionaryTags(IdTagList& output) const;
   void getExistingConnectivityTags(TagList& output, const string& idTag="") const;
   void getExistingDataTags(TagList& output, const string& idTag="") const;
   void getExistingAliasTags(TagList& output, const string& idTag="") const;
   
   // Define a new empty tag.
   void makeNewObjectDictionaryTag(const string& newtag);
   void makeNewConnectivityTag(const string& newtag, const string& idTag="");
   void makeNewDataTag(const string& newtag, const string& idTag="");
   void makeNewAliasTag(const string& newtag, const string& idTag="");
   void makeNewCompoundTag(const CompoundTag& newtags); // a shorthand for the above functions.

   // These methods define a new tag and copy the content of the current tag into it.
   // The new tag is associated to the objectDictionaryTag newIdTag, which must already be defined.
   // If newIdTag is empty (the default), current objectDictionaryTag is used.
   void copyConnectivityTag(const string& newtag, const string& newIdTag="");
   void copyDataTag(const string& newtag, const string& newIdTag="");
   void copyAliasTag(const string& newtag, const string& newIdTag="");

   // Copies just the object distionary.  The newly created newIdTag will not have any associated 
   // connectivity/clobs/aliases
   void copyObjectDictionaryTag(const string& newIdTag);

   // Copies object dictionary entries for objects mentioned in any of
   // the "from" tags to newIdTag, skipping those (ID,TYPE) pairs that
   // already exist in newIdTag.  If only idTag is defined in the
   // "from" tags, adds to newIdTag all pairs that are defined in
   // "from's" idTag.
   void copyMissingObjects(const string& newIdTag, const CompoundTag &from);

   // Copy all connectivity, data, and alias tags related to the current objectDictionaryTag,
   // keeping their names, to the new objectDictionaryTag.
   void copyAllForObjectDictionaryTag(const string& newIdTag);

   // Delete all data for the given tag.
   // If newIdTag is empty (the default), current objectDictionaryTag is used.
   bool deleteConnectivityTag(const string& tag, const string& idTag="");
   bool deleteDataTag(const string& tag, const string& idTag="");
   bool deleteAliasTag(const string& tag, const string& idTag="");

   // this deletes all related connetivity,data,and alias tags.
   bool deleteAllForObjectDictionaryTag(const string& idtag);

   // Tag locking
   void lockObjectDictionaryTag(const string& tag); // locks just OD tag, not the related tags
   void lockConnectivityTag(const string& tag, const string& idTag="");
   void lockDataTag(const string& tag, const string& idTag="");
   void lockAliasTag(const string& tag, const string& idTag="");
   void lockCompoundTag(const CompoundTag& tags); // a shorthand for the above functions.
   void lockAllForObjectDictionaryTag(const string& idTag); // locks the OD tag and all the "nested" tags

   // These methods ignore the cached values and read the database on each call.
   bool objectDictionaryTagLocked(const string& tag) const;
   bool connectivityTagLocked(const string& tag, const string& idTag="") const;
   bool dataTagLocked(const string& tag, const string& idTag="") const;
   bool aliasTagLocked(const string& tag, const string& idTag="") const;

   // Methods for the history table. Default time is current time on the client system (not on the server).
   void setHistoricTag(const CompoundTag& tag, time_t startIOV = 0);
   CompoundTag getHistoricTag(time_t when = 0 ) const;
   void getHistoryTable(HistoryTable& result);
   bool deleteHistoryEntry(time_t exactTimeStamp);

   // Methods dealing with the master table
   void insertMaster(const string& id);
   bool deleteMaster(const string& id);
   vector<string> masterList() const;

   // Methods dealing with the object dictionary
   bool checkAndInsertObject(const string& id, const string& type);
   unsigned  insertBulkObjects(istream& data, bool debug=false); // returns the number of objects inserted
   bool deleteObject(const string& id);
   bool updateObject(const string& id, const string& type);
   string objectType(const string& id) const;

   // Changes oldId to newId (for the current idTag) in all tables.
   // Returns true on successful update, false if oldId does not exists.
   bool renameID(const string& newId, const string& oldId);

   // Changes oldType to newType in the object dictionary for the current idTag.
   // Returns the number of updated objects in the dictionary.
   long renameType(const string& newType, const string& oldType);

   // If only idTag is defined in tags, get the complete dictionary
   // for this idTag.  If any other tag is also set, get the union of
   // sets of IDs mentioned in connectivity, data and alias tags.
   void getObjectDictionary(ObjectDictionaryMap& objDict, const CompoundTag& tags) const;

   //unused: // Methods dealing with the data dictionary
   //unused: void insertDataType(const string& objectType, const string& dataType, const string& tagType, const string& tableName);

   // Methods dealing with connections
   enum BulkConnectionsFileFormat { SIX_COL, SIX_COL_REVERSED, FOUR_COL};
   unsigned insertBulkConnections(istream& data, BulkConnectionsFileFormat f, bool debug = false);
   void insertConnection(const string& id, const string& slot, const string& toId, const string& toSlot);
   bool deleteConnection(const string& id, const string& slot, bool outConnection = true);
   bool updateConnection(const string& id, const string& slot, const string& toId, const string& toSlot, bool outConnection = true);
   vector<Connection> findAllConnections(const string& id, bool getOutConnections = true) const;

   // In CoralDB patterns the only special character is '%'. It matches any string of zero or more characters (just like SQL '%')
   // The escape character is '|'.
   vector<Connection> findConnections(const string& id, const string& slotPattern, bool getOutConnections = true) const;
   vector<Connection> findConnectionsByType(const string& id, const string& typePattern, bool getOutConnections=true) const;
   vector<Connection> findConnectionsFromSlot(const string& slotPattern, const string& toId) const;
   vector<Connection> findConnectionsToSlot(const string& id, const string& toSlotPattern) const;

   void getConnectionTable(ConnectionTableMap& connTable) const;
   void getConnectionTableForType(const string& type, bool getOutConnections, ConnectionTableMap& connTable) const;

   // Methods dealing with aliases
   // Sloppy means skip unknown IDs read from the stream instead of aborting.
   unsigned insertBulkAliases(istream& data, bool genericFile = true, bool debug = false, bool sloppy = false);
   void insertAlias(const Alias& a);
   void insertAlias(const string& alias, const string& convention, const string& id);
   bool deleteAlias(const Alias& a); // over-constrained
   bool deleteAlias(const string& id, const string& convention);
   vector<Alias> findAliases(const string& id) const;
   string findAlias(const string& id, const string& convention) const;
   string findId(const string& alias, const string& convention) const;
   void getAliasesTable(AliasesTable& aliases) const; // returns a sorted table

   
   // Methods dealing with configuration data
   void insertCLOB(const string& id, const string& field, const string& clob);
   bool deleteCLOB(const string& id, const string& field);
   bool updateCLOB(const string& id, const string& field, const string& clob);
   string findCLOB(const string& id, const string& field) const;
   void findCLOBs(const string& id, ClobDataContainer& result) const; //find the clobs of a specific id
   void getIdClobbedForType(const string& objType, ObjectDictionaryMap& result) const;
   void getClobNames(ClobNameContainer& names) const;
   void getNamedClobsForType(const string& objType, const string& field, ClobDataContainer& result) const;
   
   void insertBLOB(const string& id, const string& field, const coral::Blob& blob);
   bool deleteBLOB(const string& id, const string& field);
   bool updateBLOB(const string& id, const string& field, const coral::Blob& blob);
   coral::Blob findBLOB(const string& id, const string& field) const;


 private:
   // Note: private methods must NOT execute any transaction control statements.
   // Private methods are only called buy the public CoralDB methods, and the latter are responsible for transaction control.
   void createTagListTable(const string& tablename);
   bool addObject(const string& id, const string& type);
   template<class T> T findEntry(const string& tableName, const string& keyField, const string& keyValue, const string& resultField) const;
   template<class T> T findEntry(const string& tableName, const string& keyField1, const string& keyValue1,
				 const string& keyField2, const string& keyValue2, const string& resultField) const;
   template<class T, class KeyContainer> T findEntry(const string& tableName, const KeyContainer& keys, const string& resultField) const;

   template<class T> long deleteRows(const string& tableName, const string& keyField, const T& keyValue);
   template<class T1, class T2> long deleteRows(const string& tableName, const string& keyField1, const T1& keyValue1, const string& keyField2, const T2& keyValue2);
   // KeyContainer can be e.g. a map<string,string>, or a vector<pair<string,string> >
   template<class KeyContainer> long deleteRows(const string& tableName, const KeyContainer& keys);
   
   template<class T, class R> long updateRows(const string& tableName, const string& keyField, const T& keyValue, const string& fieldToUpdate, const R& newValue);
   template<class T1, class T2, class R> long updateRows(const string& tableName, const string& keyField1, const T1& keyValue1, 
							 const string& keyField2, const T2& keyValue2, const string& fieldToUpdate, const R& newValue);


   long copyIdTagRows(const string& tableTo, const string& tableFrom, const string& srcIdTag, const vector<string>& columns);

   // read status of current tags from DB to memory
   void observeTagLocks() const;

   // returns true iff there is a locked tag for the IDTAG in the given table
   bool checkForLockedTags(const string& tableName, const string& idTag) const;

   int parseLine(istream& data, int maxCols, int minCols, string colData[], const string& skip = "",
		 const string& delims = ",\"", string::size_type startColumn = 1, bool debug = false) const;

   void addNewRevision(const string& id, const string& tag, const string& idTag, const string& tagsTableName);
   int highestRevision(const string& tagsTableName) const;
   void copyRevisions(const string& newIdTag, const string& newTag, int newRevision);
   bool insertClobTableRow(const string& id, const string& field, const string& clob);
   void insertIdTagListTableRow(const string& newIdTag);
   void insertTagListTableRow(const string& tablename, const string& newTag, const string& newIdTag);\
   void getTagList(const string& tablename, TagList& output, const string& idTag) const;

   void fillUsedIdsQuery(coral::IQueryDefinition& queryToFill,
			 std::vector<coral::AttributeList>& dataToFill,
			 const CompoundTag& tags) const;
   
   string m_connection;
   coral::Context* m_context;
   mutable coral::ISessionProxy* m_session;

   CompoundTag m_tags;
   bool m_autocommit;
   bool m_cacheEnabled;

   // The names of the following data members are formed from access method names in CompoundTag.
   // A macro in CoralDB.cpp depends on this naming
   mutable bool m_locked_objectDictionaryTag;
   mutable bool m_locked_connectivityTag;
   mutable bool m_locked_dataTag;
   mutable bool m_locked_aliasTag;
   
   // Maximum length of the strings we are going to use.
   // If we don't specify this, Coral-Oracle declares string columns as VARCHAR2(4000),
   // and then can't digest uniquiness constraints involving more than one such column.
  
   static const int MAX_ID_LENGTH = 100;
   static const int MAX_ALIAS_LENGTH = 100;
   static const int MAX_CONVENTION_LENGTH = 100;
   static const int MAX_SLOT_LENGTH = 100;
   static const int MAX_FIELDNAME_LENGTH = 100;
   static const int MAX_TAG_LENGTH = 100;
   static const int MAX_OBJECTTYPE_LENGTH = 100;
   
   static const string TABLENAME_PREFIX;


 private:
   // mnemonic for Coral's ITransaction::start(bool) arg
   static const bool READONLY=true, UPDATE=false;
   
   class ReadOnlyTransactionGuard {
     const CoralDB* m_db;
   public:
     ReadOnlyTransactionGuard(const CoralDB* db);
     void commit();
   };

   class UpdateTransactionGuard {
     CoralDB* m_db;
   public:
     UpdateTransactionGuard(CoralDB* db);
     void commit();
   };
};

#if 0
{
#endif
} // namespace CoralDB


#endif

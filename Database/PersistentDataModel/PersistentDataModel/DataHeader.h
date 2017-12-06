/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef PERSISTENTDATAMODEL_DATAHEADER_H
#define PERSISTENTDATAMODEL_DATAHEADER_H

/** @file DataHeader.h
 *  @brief This file contains the class definition for the DataHeader and DataHeaderElement classes.
 *  @author Peter van Gemmeren <gemmeren@anl.gov>
 **/

#include "GaudiKernel/ClassID.h"

#include "PersistentDataModel/Token.h"

#include <string>
#include <set>
#include <vector>

// Forward declarations
class IOpaqueAddress;
namespace SG {
   class TransientAddress;
   class DataProxy;
}
namespace coral {
   class AttributeList;
}
class IStringPool;

/** @class DataHeaderElement
 *  @brief This class provides a persistent form for the TransientAddress.
 **/
class DataHeaderElement {
public: // Constructor and Destructor
   /// Default Constructor
   DataHeaderElement();
   /// Copy Constructor
   DataHeaderElement(const DataHeaderElement& rhs);

   /// Constructor
   /// @param sgAddress [IN] pointer to TransientAddress for which a DataHeaderElement is created.
   /// @param pTag [IN] string used as key element in DataHeader (SG key for DataObjects).
   DataHeaderElement(const SG::TransientAddress* sgAddress, IOpaqueAddress* tokAddress, const std::string& pTag);

   /// Constructor
   /// @param proxy [IN] pointer to DataProxy for which a DataHeaderElement is created.
   /// @param pTag [IN] string used as key element in DataHeader (SG key for DataObjects).
   DataHeaderElement(const SG::DataProxy* proxy, IOpaqueAddress* tokAddress, const std::string& pTag);

   /// Constructor
   /// @param classID [IN] Primary ClassID of the DataObject for which a DataHeaderElement is created.
   /// @param key [IN] SG Key of the DataObject for which a DataHeaderElement is created.
   /// @param token [IN] Token of the DataObject for which a DataHeaderElement is created.
   DataHeaderElement(const CLID classID, const std::string& key, const Token* token);
   /// Destructor
   virtual ~DataHeaderElement();

public: // Non-static members
   /// Assignment Operator
   DataHeaderElement& operator=(const DataHeaderElement& rhs);
 
   /// @return primary ClassID.
   CLID getPrimaryClassID() const;
   /// @return a set of all ClassIDs (primary and symlinked).
   const std::set<CLID> getClassIDs() const;
   /// @return StoreGate key string.
   const std::string& getKey() const;
   /// @return StoreGate alias string set.
   const std::set<std::string>& getAlias() const;
   /// @return token by pointer (and give away ownership).
   const Token* getToken() const;
   /// @return StorageType needed to read the DataObject (depends on technology).
   long getStorageType() const;
   /// @return the list of hash codes.
   ///         In 1-1 correspondence with the CLID set.
   const std::vector<unsigned int>& getHashes() const;
   /// @return a pointer to the TransientAddress of the DataObject.
   SG::TransientAddress* getAddress(unsigned long contextId = 0) const;
   /// @return a pointer to the TransientAddress of the DataObject, with new transient key.
   SG::TransientAddress* getAddress(const std::string& key,
	   unsigned long contextId = 0) const;
   /// Add new entry to hash map
   void addHash(IStringPool* pool);

private:
   friend class DataHeaderElementCnv_p3;
   friend class DataHeaderElementCnv_p4;
   friend class DataHeaderElementCnv_p5;
   friend class DataHeaderElementCnv_p6;

   /// Internal constructor.
   DataHeaderElement(CLID clid,
                     const std::string& name,
                     const std::vector<CLID>& tClids,
                     std::set<std::string>&& alias,
                     IOpaqueAddress* tadAddress,
                     IOpaqueAddress* tokAddress, const std::string& pTag);

   /// primary ClassID.
   CLID m_pClid;
   /// set of unsigned long to store ClassID's for symlinked container.
   std::set<CLID> m_clids;
   /// string with StoreGate key.
   std::string m_key;
   /// set of StoreGate alias string.
   std::set<std::string> m_alias;
   /// pointer to transient address
   const Token* m_token;
   bool m_ownToken;
   /// hash table for ElementLink host container keys.
   std::vector<unsigned int> m_hashes;
};

/** @class DataHeader
 *  @brief This class provides the layout for summary information stored for data written to POOL.
 **/
class  DataHeader {
public:
   enum statusFlag { Output, Primary, Input, Manual, Other };

public: // Constructor and Destructor
   /// Default Constructor
   DataHeader();
   /// Copy Constructor
   DataHeader(const DataHeader& rhs);
   /// Destructor
   virtual ~DataHeader();

public: // Non-static members
   /// Assignment Operator
   DataHeader& operator=(const DataHeader& rhs);
 
   /// Set StatusFlag enum for DataHeader.
   void setStatus(statusFlag status) const;
   /// Check StatusFlag enum for DataHeader.
   /// @param status [IN] StatusFlag to be checked for.
   bool checkStatus(statusFlag status) const;
   /// Check whether StatusFlag is "Input".
   bool isInput() const;

   /// Set ProcessTag for DataHeader.
   /// @param processTag [IN] string containing PorcessTag.
   void setProcessTag(const std::string& processTag) const;
   /// @return the string containing PorcessTag for the DataHeader.
   const std::string& getProcessTag() const;

   /// @return the number of elements in the "DataObject" vector.
   int size() const;
   /// @return a const_iterator at the begin() of the "DataObject" vector.
   std::vector<DataHeaderElement>::const_iterator begin() const;
   /// @return a const_iterator at the end() of the "DataObject" vector.
   std::vector<DataHeaderElement>::const_iterator end() const;
   /// @return the "DataObject" vector.
   const std::vector<DataHeaderElement>& elements() const;

   /// Insert a new element into the "DataObject" vector.
   /// @param sgAddress [IN] pointer to the TransientAddress of the DataObject.
   /// @param pTag [IN] string to overwrite key meber of the DataHeaderElement
   void insert(const SG::TransientAddress* sgAddress, IOpaqueAddress* tokAddress = 0, const std::string& pTag = "");
   /// Insert a new element into the "DataObject" vector.
   /// @param proxy [IN] pointer to the DataProxy for the DataObject.
   /// @param pTag [IN] string to overwrite key meber of the DataHeaderElement
   void insert(const SG::DataProxy* proxy, IOpaqueAddress* tokAddress = 0, const std::string& pTag = "");
   /// Insert a new element into the "DataObject" vector.
   /// @param dhe [IN] reference to the DataHeaderElement to be inserted.
   void insert(const DataHeaderElement& dhe);

   /// @return the number of elements in the "Provenance" vector.
   int sizeProvenance() const;
   /// @return a const_iterator at the begin() of the "Provenance" vector.
   std::vector<DataHeaderElement>::const_iterator beginProvenance() const;
   /// @return a const_iterator at the end() of the "Provenance" vector.
   std::vector<DataHeaderElement>::const_iterator endProvenance() const;

   /// Insert a new element into the "Provenance" vector.
   /// @param dhe [IN] reference to the DataHeaderElement to be inserted.
   void insertProvenance(const DataHeaderElement& dhe);

   /// Add new entry to hash map
   void addHash(IStringPool* pool);

   void setAttributeList(const coral::AttributeList* attrList);
   const coral::AttributeList* getAttributeList();
   void setEvtRefTokenStr(const std::string& tokenStr);
   const std::string& getEvtRefTokenStr();

private:
   friend class DataHeaderCnv_p3;
   friend class DataHeaderCnv_p4;
   friend class DataHeaderCnv_p5;
   friend class DataHeaderCnv_p6;

   /// vector with DataHeaderElement to "DataObject".
   std::vector<DataHeaderElement> m_dataHeader;
   /// vector with DataHeaderElement to upstream DataHeader ("Provenance").
   std::vector<DataHeaderElement> m_inputDataHeader;
   /// StatusFlag (i.e.: Output, Primary, Input, Manual, Other).
   mutable statusFlag m_status;
   /// ProcessTag (e.g.: "StreamESD", "StreamAOD" ...).
   mutable std::string m_processTag;
   /// Optional pointer to AttributeList to enable writing of TAG Attributes to Payload files.
   const coral::AttributeList* m_attrList;
   /// Optional Token String for Event Reference to enable writing of TAG to Payload files.
   std::string m_evtRefTokenStr;
};

#include "AthenaKernel/CLASS_DEF.h"
CLASS_DEF(DataHeader, 222376821, 1)
#endif

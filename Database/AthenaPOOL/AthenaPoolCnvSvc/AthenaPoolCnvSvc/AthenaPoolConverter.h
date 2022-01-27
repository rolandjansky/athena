/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#ifndef ATHENAPOOLCNVSVC_ATHENAPOOLCONVERTER_H
#define ATHENAPOOLCNVSVC_ATHENAPOOLCONVERTER_H

/** @file AthenaPoolConverter.h
 *  @brief This file contains the class definition for the AthenaPoolConverter class.
 *  @author Peter van Gemmeren <gemmeren@anl.gov>
 **/

#include "GaudiKernel/Converter.h"
#include "GaudiKernel/ServiceHandle.h"
#include "AthenaBaseComps/AthMessaging.h"
#include "DataModelRoot/RootType.h"
#include "AthenaPoolCnvSvc/IAthenaPoolCleanUp.h"
#include "PoolSvc/IPoolSvc.h"

#include <string>
#include <map>
#include <mutex>

class IOpaqueAddress;
class DataObject;
class StatusCode;
class IAthenaPoolCnvSvc;
class Guid;
class Placement;
class Token;

/// Abstract factory to create the converter
template <class TYPE> class CnvFactory;

/** @class AthenaPoolConverter
 *  @brief This class provides the abstract converter to translate an object to/from its
 *  persistent POOL representation.
 **/
class AthenaPoolConverter : public ::Converter, public ::AthMessaging, virtual public IAthenaPoolCleanUp {

public:
   /// Destructor
   virtual ~AthenaPoolConverter();

   /// Gaudi Service Interface method implementations:
   virtual StatusCode initialize() override;
   virtual StatusCode finalize() override;

   virtual long repSvcType() const override;

   /// Create a transient object from a POOL persistent representation.
   /// @param pAddr [IN] IOpaqueAddress of POOL persistent representation.
   /// @param pObj [OUT] pointer to the transient object.
   virtual StatusCode createObj(IOpaqueAddress* pAddr, DataObject*& pObj) override;

   /// Create a POOL persistent representation for a transient object.
   /// @param pObj [IN] pointer to the transient object.
   /// @param pAddr [OUT] IOpaqueAddress of POOL persistent representation.
   virtual StatusCode createRep(DataObject* pObj, IOpaqueAddress*& pAddr) override;

   /// Create a POOL persistent representation for a transient object.
   /// @param pAddr [IN] IOpaqueAddress of POOL persistent representation.
   /// @param pObj [IN] pointer to the transient object.
   virtual StatusCode fillRepRefs(IOpaqueAddress* pAddr, DataObject* pObj) override;

   /// @return storage type.
   static long storageType();

   /// @return class ID.
   static const CLID& classID();

   /// Implement cleanUp for AthenaPoolConverter to do nothing.
   virtual StatusCode cleanUp(const std::string& output) override;


protected:
   /// Standard Service Constructor
   AthenaPoolConverter(const CLID& id, ISvcLocator* pSvcLocator,
                       const char* name = nullptr);

   /// Convert an object into Persistent.
   /// @param pObj [IN] pointer to the transient object.
   /// @param key [IN] StoreGate key (string) - placement hint to generate POOL container name
   virtual StatusCode DataObjectToPers(DataObject* pObj, IOpaqueAddress*& pAddr) = 0;

   /// Write an object into POOL.
   /// @param pObj [IN] pointer to the transient object.
   /// @param key [IN] StoreGate key (string) - placement hint to generate POOL container name
   virtual StatusCode DataObjectToPool(IOpaqueAddress* pAddr, DataObject* pObj) = 0;

   /// Read an object from POOL.
   /// @param pObj [OUT] pointer to the transient object.
   /// @param token [IN] POOL token of the persistent representation.
   /// @param key [IN] SG key of the object being read.
   virtual StatusCode PoolToDataObject(DataObject*& pObj, const Token* token,
                                       const std::string& key) = 0;

   /// Set POOL placement hint for a given type.
   /// @param tname [IN] type name.
   /// @param key [IN] SG key.
   virtual Placement setPlacementWithType(const std::string& tname, const std::string& key, const std::string& output);
   /// Set POOL placement hint; pure virtual method implemented by classes templated by type
   virtual Placement setPlacement(const std::string& key, const std::string& output) = 0;

   /// @return data object from the converter.
   virtual const DataObject* getDataObject() const;

   bool compareClassGuid(const Guid &guid) const;

protected: // data
   ServiceHandle<IAthenaPoolCnvSvc> m_athenaPoolCnvSvc;
   RootType              m_classDesc;

   typedef std::map<std::string, RootType>            ClassMap;
   typedef ClassMap::const_iterator                   ClassMapIt;
   std::string           m_className;
   ClassMap              m_classDescs;

   std::string m_containerPrefix;
   std::string m_containerNameHint;
   std::string m_branchNameHint;

   const DataObject*     m_dataObject;
   const Token*          m_i_poolToken;

   typedef std::mutex CallMutex;
   CallMutex m_conv_mut;
};

#endif

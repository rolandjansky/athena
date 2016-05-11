/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
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
   virtual StatusCode initialize();
   virtual StatusCode finalize();

   long repSvcType() const;

   /// Create a transient object from a POOL persistent representation.
   /// @param pAddr [IN] IOpaqueAddress of POOL persistent representation.
   /// @param pObj [OUT] pointer to the transient object.
   virtual StatusCode createObj(IOpaqueAddress* pAddr, DataObject*& pObj);

   /// Create a POOL persistent representation for a transient object.
   /// @param pObj [IN] pointer to the transient object.
   /// @param pAddr [OUT] IOpaqueAddress of POOL persistent representation.
   virtual StatusCode createRep(DataObject* pObj, IOpaqueAddress*& pAddr);

   /// @return storage type.
   static long storageType();

   /// @return class ID.
   static const CLID& classID();

   /// Implement cleanUp for AthenaPoolConverter to do nothing.
   virtual StatusCode cleanUp();

protected:
   /// Standard Service Constructor
   AthenaPoolConverter(const CLID& id, ISvcLocator* pSvcLocator);

   /// Write an object into POOL.
   /// @param pObj [IN] pointer to the transient object.
   /// @param key [IN] StoreGate key (string) - placement hint to generate POOL container name
   virtual StatusCode DataObjectToPool(DataObject* pObj, const std::string& key) = 0;

   /// Read an object from POOL.
   /// @param pObj [OUT] pointer to the transient object.
   /// @param token [IN] POOL token of the persistent representation.
   //virtual StatusCode PoolToDataObject(DataObject*& pObj, const std::string& token) = 0;
   virtual StatusCode PoolToDataObject(DataObject*& pObj, const Token* token) = 0;

   /// Set POOL placement hint for a given type.
   /// @param tname [IN] type name.
   /// @param key [IN] SG key.
   virtual void setPlacementWithType(const std::string& tname, const std::string& key = "");
   /// Set POOL placement hint; pure virtual method implemented by classes templated by type
   virtual void setPlacement(const std::string& key = "") = 0;

   /// @return data object from the converter.
   virtual const DataObject* getDataObject() const;

   bool compareClassGuid(const Guid &guid) const;

protected: // data
   ServiceHandle<IAthenaPoolCnvSvc> m_athenaPoolCnvSvc;
   Placement*            m_placement;
   RootType              m_classDesc;

   typedef std::map<std::string, std::string>         StringMap;
   typedef StringMap::const_iterator                  StringMapIt;
   StringMap             m_placementHints;

   typedef std::map<std::string, RootType>            ClassMap;
   typedef ClassMap::const_iterator                   ClassMapIt;
   std::string           m_className;
   ClassMap              m_classDescs;

   DataObject*           m_dataObject;
   const Token*          m_i_poolToken;
   const Token*          m_o_poolToken;
};

#endif

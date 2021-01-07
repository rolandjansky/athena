/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#ifndef ATHENAPOOLCNVSVC_IATHENAPOOLCNVSVC_H
#define ATHENAPOOLCNVSVC_IATHENAPOOLCNVSVC_H

/** @file IAthenaPoolCnvSvc.h
 *  @brief This file contains the class definition for the IAthenaPoolCnvSvc interface class.
 *  @author Peter van Gemmeren <gemmeren@anl.gov>
 **/

#include "AthenaPoolCnvSvc/IAthenaPoolCleanUpSvc.h"
#include "GaudiKernel/IConversionSvc.h"
#include "AthenaKernel/IDataShare.h"
#include "DataModelRoot/RootType.h"

#include <string>

// Forward declarations
class IOpaqueAddress;
class IPoolSvc;
class Placement;
class Token;

namespace pool {
   class DbType;
}

/// Declaration of the interface ID ( interface id, major version, minor version)
static const InterfaceID IID_IAthenaPoolCnvSvc ("IAthenaPoolCnvSvc", 1 , 0);

/** @class IAthenaPoolCnvSvc
 *  @brief This class provides the interface between Athena and PoolSvc.
 **/
class IAthenaPoolCnvSvc : virtual public IConversionSvc, public IDataShare, public IAthenaPoolCleanUpSvc {
public:
   /// Retrieve interface ID
   static const InterfaceID& interfaceID() { return(IID_IAthenaPoolCnvSvc); }

   virtual ~IAthenaPoolCnvSvc() {}

   /// Disconnect to the output connection.
   virtual StatusCode disconnectOutput(const std::string& outputConnectionSpec) = 0;

   /// @return pointer to PoolSvc instance.
   virtual IPoolSvc* getPoolSvc() = 0;

   /// @return a string token to a Data Object written to Pool
   /// @param placement [IN] pointer to the placement hint
   /// @param obj [IN] pointer to the Data Object to be written to Pool
   /// @param classDesc [IN] pointer to the Seal class description for the Data Object.
   virtual Token* registerForWrite(Placement* placement, const void* obj, const RootType& classDesc) = 0;

   /// @param obj [OUT] pointer to the Data Object.
   /// @param token [IN] string token of the Data Object for which a Pool Ref is filled.
   virtual void setObjPtr(void*& obj, const Token* token) = 0;

   /// @return a boolean for using detailed time and size statistics.
   virtual bool useDetailChronoStat() const = 0;

   /// Create a Generic address using explicit arguments to identify a single object.
   /// @param svcType [IN] service type of the address.
   /// @param clid [IN] class id for the address.
   /// @param par [IN] string containing the database name.
   /// @param ip [IN] object identifier.
   /// @param refpAddress [OUT] converted address.
   virtual StatusCode createAddress(long svcType,
		   const CLID& clid,
		   const std::string* par,
		   const unsigned long* ip,
		   IOpaqueAddress*& refpAddress) = 0;

   /// Create address from string form
   /// @param svcType [IN] service type of the address.
   /// @param clid [IN] class id for the address.
   /// @param refAddress [IN] string form to be converted.
   /// @param refpAddress [OUT] converted address.
   virtual StatusCode createAddress(long svcType,
		   const CLID& clid,
		   const std::string& refAddress,
		   IOpaqueAddress*& refpAddress) = 0;

   /// Convert address to string form
   /// @param pAddress [IN] address to be converted.
   /// @param refAddress [OUT] converted string form.
   virtual StatusCode convertAddress(const IOpaqueAddress* pAddress, std::string& refAddress) = 0;

   /// Extract/deduce the DB technology from the connection
   /// string/file specification
   virtual StatusCode decodeOutputSpec(std::string& connectionSpec, int& outputTech) const = 0;

   /// Make this a server.
   virtual StatusCode makeServer(int num) = 0;

   /// Make this a client.
   virtual StatusCode makeClient(int num) = 0;

   /// Implement registerCleanUp to register a IAthenaPoolCleanUp to be called during cleanUp.
   virtual StatusCode registerCleanUp(IAthenaPoolCleanUp* cnv) = 0;

   /// Implement cleanUp to call all registered IAthenaPoolCleanUp cleanUp() function.
   virtual StatusCode cleanUp(const std::string& connection) = 0;

   /// Set the input file attributes, if any are requested from jobOpts
   /// @param fileName [IN] name of the input file
   virtual StatusCode setInputAttributes(const std::string& fileName) = 0;
};

#endif

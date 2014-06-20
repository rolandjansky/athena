///////////////////////// -*- C++ -*- /////////////////////////////

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// NtupleCnvSvc.h 
// Header file for class NtupleCnvSvc
// Author: S.Binet<binet@cern.ch>
/////////////////////////////////////////////////////////////////// 
#ifndef ATHENAROOTCOMPS_NTUPLECNVSVC_H
#define ATHENAROOTCOMPS_NTUPLECNVSVC_H 1

// STL includes
#include <string>

// FrameWork includes
#include "AthenaBaseComps/AthCnvSvc.h"

// GaudiKernel
#include "GaudiKernel/IConversionSvc.h"

// Forward declaration
class ISvcLocator;
template <class TYPE> class SvcFactory;

namespace Athena {

class NtupleCnvSvc
  : virtual public ::IConversionSvc,
            public ::AthCnvSvc
{ 
  friend class SvcFactory<Athena::NtupleCnvSvc>;

  /////////////////////////////////////////////////////////////////// 
  // Public methods: 
  /////////////////////////////////////////////////////////////////// 
 public: 

  // Copy constructor: 

  /// Constructor with parameters: 
  NtupleCnvSvc( const std::string& name, ISvcLocator* pSvcLocator );

  /// Destructor: 
  virtual ~NtupleCnvSvc(); 

  // Assignment operator: 
  //NtupleCnvSvc &operator=(const NtupleCnvSvc &alg); 

  /// Gaudi Service Implementation
  //@{
  virtual StatusCode initialize();
  virtual StatusCode finalize();
  virtual StatusCode queryInterface( const InterfaceID& riid, 
                                     void** ppvInterface );
  //@}

  /// @c IConversionSvc and @c IConverter interfaces
  //@{

  /// update the service state (well, just check the @c IOpaqueAddress is in
  /// fact a @c GenericAddress )
  virtual 
  StatusCode updateServiceState(IOpaqueAddress* pAddress);

  /// Implementation of IConverter: Create the transient representation of an 
  /// object from persistent state.
  /// @param pAddress [IN] pointer to IOpaqueAddress of the representation.
  /// @param refpObject [OUT] pointer to DataObject to be created.
  virtual 
  StatusCode createObj(IOpaqueAddress* pAddress, DataObject*& refpObject);

  /// Implementation of IConverter: Convert the transient object to the
  /// requested representation.
  /// @param pObject [IN] pointer to DataObject.
  /// @param refpAddress [OUT] pointer to IOpaqueAddress of the representation
  ///        to be created.
  virtual 
  StatusCode createRep(DataObject* pObject, IOpaqueAddress*& refpAddress);

  /// Implementation of IConverter: Resolve the references of the converted
  /// object.
  /// @param pAddress [IN] pointer to IOpaqueAddress of the representation to
  ///        be resolved.
  /// @param pObject [IN] pointer to DataObject to be created.
  virtual 
  StatusCode fillRepRefs(IOpaqueAddress* pAddress, DataObject* pObject);

  /// Connect the output connection specification to the service with open mode.
  /// @param outputConnectionSpec [IN] the name of the output connection 
  ///        specification as string.
  /// @param openMode [IN] the open mode of the file as string.
  virtual 
  StatusCode connectOutput(const std::string& outputConnectionSpec,
                           const std::string& openMode);

  /// Connect the output connection specification to the service with open mode.
  /// @param outputConnectionSpec [IN] the name of the output connection
  ///        specification as string.
  virtual 
  StatusCode connectOutput(const std::string& outputConnectionSpec);

  /// Commit pending output.
  virtual
  StatusCode commitOutput(const std::string& outputConnectionSpec,
                          bool doCommit);

  //@}

  /// @c IAddressCreator interfaces
  //@{

  /// Create a Generic address using explicit arguments to identify a single
  /// object.
  /// @param svcType [IN] service type of the address.
  /// @param clid [IN] class id for the address.
  /// @param par [IN] string containing the database name.
  /// @param ip [IN] object identifier.
  /// @param refpAddress [OUT] converted address.
  virtual 
  StatusCode createAddress(long svcType,
                           const CLID& clid,
                           const std::string* par,
                           const unsigned long* ip,
                           IOpaqueAddress*& refpAddress);

  /// Convert address to string form
  /// @param pAddress [IN] address to be converted.
  /// @param refAddress [OUT] converted string form.
  virtual 
  StatusCode convertAddress(const IOpaqueAddress* pAddress,
                            std::string& refAddress);

  /// Create address from string form
  /// @param svcType [IN] service type of the address.
  /// @param clid [IN] class id for the address.
  /// @param refAddress [IN] string form to be converted.
  /// @param refpAddress [OUT] converted address.
  virtual 
  StatusCode createAddress(long svcType,
                           const CLID& clid,
                           const std::string& refAddress,
                           IOpaqueAddress*& refpAddress);
  //@}

  /////////////////////////////////////////////////////////////////// 
  // Const methods: 
  ///////////////////////////////////////////////////////////////////

  /////////////////////////////////////////////////////////////////// 
  // Non-const methods: 
  /////////////////////////////////////////////////////////////////// 

  static const InterfaceID& interfaceID();

protected:

  /// Create new Converter using factory (override AthCnvSvc::createConverter)
  virtual 
  IConverter* createConverter(long typ, 
                              const CLID& clid, 
                              const ICnvFactory* fac);

  /////////////////////////////////////////////////////////////////// 
  // Private data: 
  /////////////////////////////////////////////////////////////////// 
 private: 

  /// Default constructor: 
  NtupleCnvSvc();

  /// Containers
  

}; 

// I/O operators
//////////////////////

/////////////////////////////////////////////////////////////////// 
// Inline methods: 
/////////////////////////////////////////////////////////////////// 

inline const InterfaceID& NtupleCnvSvc::interfaceID() 
{ 
  return IConversionSvc::interfaceID(); 
}

} //> end namespace Athena

#endif //> !ATHENAROOTCOMPS_NTUPLECNVSVC_H

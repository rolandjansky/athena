///////////////////////// -*- C++ -*- /////////////////////////////

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// LeafCnv.h 
// Header file for class LeafCnv
// Author: S.Binet<binet@cern.ch>
/////////////////////////////////////////////////////////////////// 
#ifndef ATHENAROOTCOMPS_LEAFCNV_H
#define ATHENAROOTCOMPS_LEAFCNV_H 1

// STL includes
#include <iosfwd>

// FrameWork includes
#include "GaudiKernel/Converter.h"
#include "GaudiKernel/ClassID.h"
#include "AthenaBaseComps/AthMessaging.h"
#include "DataModelRoot/RootType.h"

// Forward declaration
class TLeaf;
class ISvcLocator;
class DataObject;
class IOpaqueAddress;
class StatusCode;
/// Abstract factory to create the converter
template <class TYPE> class CnvFactory;

namespace Athena {

class LeafCnv
  : public ::Converter,
    public ::AthMessaging
{ 

  /////////////////////////////////////////////////////////////////// 
  // Public methods: 
  /////////////////////////////////////////////////////////////////// 
 public: 

  /// Constructor with parameters: 
  LeafCnv( const CLID& clid, ISvcLocator* pSvcLocator );
  LeafCnv(ISvcLocator* pSvcLocator);

  /// Destructor: 
  virtual ~LeafCnv(); 

  /// Gaudi Service Implementation
  //@{
  virtual StatusCode initialize();
//   virtual StatusCode queryInterface( const InterfaceID& riid, 
//                                      void** ppvInterface );
  //@}

  /////////////////////////////////////////////////////////////////// 
  // Const methods: 
  ///////////////////////////////////////////////////////////////////

  /////////////////////////////////////////////////////////////////// 
  // Non-const methods: 
  /////////////////////////////////////////////////////////////////// 

  long repSvcType() const;

  /// Create a transient object from a persistent representation.
  /// @param pAddr [IN] IOpaqueAddress of persistent representation.
  /// @param pObj [OUT] pointer to the transient object.
  virtual StatusCode createObj(IOpaqueAddress* pAddr, DataObject*& pObj);

  /// Create a persistent representation for a transient object.
  /// @param pObj [IN] pointer to the transient object.
  /// @param pAddr [OUT] IOpaqueAddress of persistent representation.
  virtual StatusCode createRep(DataObject* pObj, IOpaqueAddress*& pAddr);
  
  /// @return storage type.
  static long storageType();

  /// @return class ID.
  static const CLID& classID() { return CLID_NULL; }


  /////////////////////////////////////////////////////////////////// 
  // Private data: 
  /////////////////////////////////////////////////////////////////// 
 private: 

  /// the type we can convert
  RootType m_type;

}; 

/////////////////////////////////////////////////////////////////// 
// Inline methods: 
/////////////////////////////////////////////////////////////////// 
//std::ostream& operator<<( std::ostream& out, const LeafCnv& o );

} //> end namespace Athena

#endif //> !ATHENAROOTCOMPS_LEAFCNV_H

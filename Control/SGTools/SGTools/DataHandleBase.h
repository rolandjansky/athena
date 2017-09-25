///////////////////////// -*- C++ -*- /////////////////////////////

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// DataHandleBase.h 
// Header file for class DataHandleBase
// Author: S.Binet<binet@cern.ch>
/////////////////////////////////////////////////////////////////// 
#ifndef SGTOOLS_DATAHANDLEBASE_H
#define SGTOOLS_DATAHANDLEBASE_H 1

// STL includes
#include <string>

// fwk includes
#include "AthenaKernel/IProxyDict.h"
#include "AthenaKernel/IResetable.h"

// SGTools includes
#include "SGTools/DataProxy.h"
#include "SGTools/ProxyMap.h"

#include "CxxUtils/checker_macros.h"

// Forward declaration


/**
 * @class DataHandleBase 
 * @brief an iterator over instances of a given type in an @c IProxyDict (such
 * as StoreGateSvc). It d-casts and caches locally the pointed-at object, to 
 * speed-up subsequent accesses.
 * It can be reset by the store for asynchronous updates (IOVSvc)
 */
class ATLAS_NOT_THREAD_SAFE DataHandleBase : public IResetable
{ 

  /////////////////////////////////////////////////////////////////// 
  // Public typedefs: 
  /////////////////////////////////////////////////////////////////// 
public: 
  typedef std::string ID_type;

  /////////////////////////////////////////////////////////////////// 
  // Public methods: 
  /////////////////////////////////////////////////////////////////// 
public: 

  /// Default constructor: 
  DataHandleBase();

  /// Copy constructor: 
  DataHandleBase( const DataHandleBase& rhs );

  /// Assignment operator: 
  DataHandleBase& operator=( const DataHandleBase& rhs ); 

  /// Constructor with parameters: 

  DataHandleBase(SG::DataProxy* proxy); ///< 
  DataHandleBase(const SG::ConstProxyIterator& itr1,
                 const SG::ConstProxyIterator& itr2);

  /// Destructor: 
  virtual ~DataHandleBase(); 

  /////////////////////////////////////////////////////////////////// 
  // Const methods: 
  ///////////////////////////////////////////////////////////////////

  /// \name validity checks
  //@{
  bool isConst() const;
  bool isInitialized() const;    ///<weaker test but it does not touch the disk!
  bool isSet() const { return isInitialized(); }
  //@}

  /// Get the key string with which the current object was stored.
  const std::string& key() const;

  StatusCode setState(SG::DataProxy* proxy) const;
  StatusCode setState(IProxyDict* store, const ID_type& name) const;

  // Note: itr1 may be modified!
  StatusCode setState(SG::ConstProxyIterator& itr1,
                      const SG::ConstProxyIterator& itr2) const;

  /// the CLID of the object we are bound to
  virtual CLID clid() const =0;

  ///get the data object key (ID)
  ID_type ID() const { return isInitialized() ? m_proxy->name() : "NONE"; }

  /////////////////////////////////////////////////////////////////// 
  // Non-const methods: 
  /////////////////////////////////////////////////////////////////// 

  StatusCode setState(SG::DataProxy* proxy);
  StatusCode setState(IProxyDict* store, const ID_type& name);

  /////////////////////////////////////////////////////////////////// 
  // Protected data: 
  /////////////////////////////////////////////////////////////////// 
protected: 

  /// iterator pointing at the beginning of the range of proxies
  mutable SG::ConstProxyIterator m_itr;

  /// iterator pointing at the end of the range of proxies
  mutable SG::ConstProxyIterator m_itrEnd;

  /// the proxy holding the object we are bound to
  mutable SG::DataProxy* m_proxy;

  /// use the proxy-iterator or just the proxy ?
  mutable bool m_useItr;

}; 

/////////////////////////////////////////////////////////////////// 
// Inline methods: 
/////////////////////////////////////////////////////////////////// 
//std::ostream& operator<<( std::ostream& out, const DataHandleBase& o );



#endif //> !SGTOOLS_DATAHANDLEBASE_H

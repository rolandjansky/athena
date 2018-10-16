/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/


////////////////////////////////////////////////////////////////
//                                                            //
//  Header file for class VP1SGAccessHelper                   //
//                                                            //
//  Description: Helper class for safe StoreGate retrieval    //
//                                                            //
//  Author: Thomas H. Kittelmann (Thomas.Kittelmann@cern.ch)  //
//  Initial version: March 2008                               //
//                                                            //
////////////////////////////////////////////////////////////////

#ifndef VP1SGACCESSHELPER_H
#define VP1SGACCESSHELPER_H

#include "VP1Base/VP1HelperClassBase.h"
#include "StoreGate/StoreGate.h"
#include <stdexcept>
class VP1SGContentsHelper;

class VP1SGAccessHelper : public VP1HelperClassBase {
public:

  VP1SGAccessHelper( IVP1System *, const bool detStore = false );//For messages in gui (detStore=true takes detector rather than event store)
  VP1SGAccessHelper( StoreGateSvc * );
  virtual ~VP1SGAccessHelper();

  ////////////////////////////////////////////////////////////////
  //Retrieve objects by type and key:
  template <typename T>
  bool retrieve( const T* &, const QString& key ) const;//returns false and sets to null pointer in case of errors

  template <typename T>
  const T* retrieve( const QString& key ) const;//returns a null pointer in case of errors


  ////////////////////////////////////////////////////////////////
  // Retrieve all objects of type T (use handles as iterators):
  template <typename T>
  bool retrieve(const DataHandle<T>& begin, const DataHandle<T>& end, const bool&silent = false );//Returns false in case of errors.
  //silent = true means all output will be redirected to verbose level
  //  -> NB: This DataHandle retrieval is something we should avoid in VP1!!!
  //  -> If you absolutely need to use it (due to bad implementations
  //  -> in upstream data providers), please use VP1SGContentsHelper
  //  -> to check that there is not more than one collection of the
  //  -> type you re trying to retrieve. If there is, abort!

private:

  VP1SGAccessHelper( const VP1SGAccessHelper & );
  VP1SGAccessHelper & operator= ( const VP1SGAccessHelper & );

  class Imp;
  Imp * m_d;

  StoreGateSvc * storeGate() const;
  bool contains(const CLID& id, const QString&) const;
};

#include "VP1Utils/VP1SGAccessHelper.icc"

#endif

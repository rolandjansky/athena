// -*- c++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef EVENTSHAPEUTILS_EVENTSHAPEHELPERS_H
#define EVENTSHAPEUTILS_EVENTSHAPEHELPERS_H

#include "GaudiKernel/Bootstrap.h"

#include "GaudiKernel/ISvcLocator.h"

#include "StoreGate/StoreGateSvc.h"

#include "NavFourMom/INavigable4MomentumCollection.h"

#include <string>

struct EventShapeHelpers
{
  /*! @brief Format string: reduced to fixed length
   *
   *  This function shortens a string to a specified length @c len . If
   *  the string is of shorter or equal length, a copy is returned. If 
   *  the string length exceeds @c len , the string is shortened to 
   *  @c len and the last @c nf characters as replaced by a fill character
   *  specified in @c fill . 
   * 
   *  @return Modified string formatted to match given length. 
   *
   *  @param[in] str  @c const reference to input string (not modified)
   *  @param[in] len  length of returned string
   *  @param[in] fill (optional) fill character, default is "."
   *  @param[in] nf   (optional) number of characters to be replaced by 
   *                  @c fill at end of string. Default is 3. 
   */
  static std::string stringFormatter(const std::string& str,size_t len,
				     const std::string& fill=".",
				     size_t nf=3)
  {
    if ( str.length() <= len) return str;
    std::string outStr = str.substr(len);
    for ( size_t i(len-nf); i<len; ++i ) { outStr.insert(i,fill); }
    return outStr;
  }
  /*! @brief Retrieve object from @c StoreGate
   *
   *  This function requires an external pointer to the @c StoreGateSvc .
   * 
   *  @param[in] pStoreGate pointer to @c StoreGate service
   *  @param[in] sgKey      key of object in @c StoreGate
   *  @param[out] pObj      reference to variable holding @c const pointer
   *                        to retrieved object
   *
   *  @return @c StatusCode::SUCCESS if retrieval successful, else 
   *  @c StatusCode::FAILURE (can happen if object with specified key is not
   *  in transient store, or pointer to transient store is invalid)
   */
  template<class C>
  static StatusCode retrieveFromSG(StoreGateSvc* pStoreGate, 
				   const std::string& sgKey,
				   const C*& pObj)
  {
    pObj = 0;
    if ( pStoreGate == 0 ) 
      {	return StatusCode::FAILURE; }
    else
      {	return pStoreGate->retrieve(pObj,sgKey); }
  }

  /*! @brief Retrieve object from @c StoreGate
   *
   *  This function requires an external pointer to the @c StoreGateSvc .
   *
   *  @param[in] pStoreGate pointer to @c StoreGate service
   *  @param[in] sgKey      key of object in @c StoreGate
   *  @param[out] pObj      reference to variable holding pointer
   *                        to retrieved object
   *
   *  @return @c StatusCode::SUCCESS if retrieval successful, else 
   *  @c StatusCode::FAILURE (can happen if object with specified key is not
   *  in transient store, or pointer to transient store is invalid)
   */
  template<class C>
  static StatusCode retrieveFromSG(StoreGateSvc* pStoreGate,
				   const std::string& sgKey,
				   C*& pObj)
  {
    pObj = 0;
    if ( pStoreGate == 0 ) 
      {	return StatusCode::FAILURE; }
    else
      {	return pStoreGate->retrieve(pObj,sgKey); }
  }

  /*! @brief Retrieve object from @c StoreGate
   *
   *  This function retrieves the pointer to @c StoreGate itself.
   *
   *  @param[in] pStoreGate pointer to @c StoreGate service
   *  @param[in] sgKey      key of object in @c StoreGate
   *  @param[out] pObj      reference to variable holding @c const pointer
   *                        to retrieved object
   *
   *  @return @c StatusCode::SUCCESS if retrieval successful, else 
   *  @c StatusCode::FAILURE (can happen if object with specified key is not
   *  in transient store, or pointer to transient store is invalid)
   */
  template<class C>
  static StatusCode retrieveFromSG(const std::string& sgKey,const C*& pObj)
  {
    StoreGateSvc* pStoreGate = 0;
    if ( Gaudi::svcLocator()->service("StoreGate",pStoreGate).isFailure() ) 
      { return StatusCode::FAILURE; }
    else 
      { return retrieveFromSG(pStoreGate,sgKey,pObj); }
  }


  /*! @brief Retrieve object from @c StoreGate
   *
   *  This function retrieves the pointer to @c StoreGate itself.
   *
   *  @param[in] pStoreGate pointer to @c StoreGate service
   *  @param[in] sgKey      key of object in @c StoreGate
   *  @param[out] pObj      reference to variable holding pointer
   *                        to retrieved object
   *
   *  @return @c StatusCode::SUCCESS if retrieval successful, else 
   *  @c StatusCode::FAILURE (can happen if object with specified key is not
   *  in transient store, or pointer to transient store is invalid)
   */
  template<class C>
  static StatusCode retrieveFromSG(const std::string& sgKey,C*& pObj)
  {
    StoreGateSvc* pStoreGate = 0;
    if ( Gaudi::svcLocator()->service("StoreGate",pStoreGate).isFailure() ) 
      { return StatusCode::FAILURE; }
    else 
      { return retrieveFromSG(pStoreGate,sgKey,pObj); }
  }
};

/*! @struct EventShapeHelpers
 *
 *  @brief Collection of static functions supporting @c EventShape 
 *         reconstruction
 */
#endif

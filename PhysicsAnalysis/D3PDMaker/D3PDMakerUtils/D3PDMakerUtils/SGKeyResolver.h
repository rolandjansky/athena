// This file's extension implies that it's C, but it's really -*- C++ -*-.

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id$
/**
 * @file D3PDMakerUtils/SGKeyResolver.h
 * @author scott snyder <snyder@bnl.gov>
 * @date Feb, 2010
 * @brief Helper to resolve the SG key to use.
 */


#ifndef D3PDMAKERUTILS_SGKEYRESOLVER_H
#define D3PDMAKERUTILS_SGKEYRESOLVER_H


#include "GaudiKernel/ClassID.h"
#include "GaudiKernel/ServiceHandle.h"
#include <string>


class StoreGateSvc;


namespace D3PD {


/**
 * @brief Helper to resolve the SG key to use.
 *
 * For the D3PD maker, we allow SG keys to be a comma-separated list.
 * When we first look up the object, we try each of these keys in order,
 * and use the first one that actually exists in SG.
 *
 * This simplifies configurations for multiple data formats.
 *
 * This is a helper object to actually to the resolution.
 */
class SGKeyResolver
{
public:
  /**
   * @brief Constructor.
   * @param name The name of the tool/alg in which this is used.
   * @param sg The event store service.
   * @param sgkey The property of the requested key.
   */
  SGKeyResolver (std::string name,
                 ServiceHandle<StoreGateSvc>& sg,
                 std::string& sgkey);


  /**
   * @brief Initialize.
   * @param clid The CLID of the desired type.
   * @param typename The name of the desired type.
   */
  StatusCode initialize (CLID clid, std::string typname);


  /**
   * @brief Initialize, from a type.
   */
  template <typename T>
  StatusCode initialize ();


  /**
   * @brief Return the SG key we should use.
   */
  std::string key();


private:
  /// The name of the tool/alg in which this is used.
  std::string m_name;

  /// The event storage service.
  ServiceHandle<StoreGateSvc>& m_sg;

  /// The CLID of the desired type.
  CLID m_clid;

  /// The name of the desired type.
  std::string m_typname;

  /// The requested SG key.
  std::string& m_sgkey;

  /// The actual key we'll use.
  std::string m_usedKey;

  /// Flag that m_usedKey is valid.
  bool m_haveKey;
};


} // namespace D3PD


#include "D3PDMakerUtils/SGKeyResolver.icc"


#endif // not D3PDMAKERUTILS_SGKEYRESOLVER_H

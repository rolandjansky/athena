// This file's extension implies that it's C, but it's really -*- C++ -*-.

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id$
/**
 * @file D3PDMakerUtils/SGGetterImpl.h
 * @author scott snyder <snyder@bnl.gov>
 * @date Aug, 2009
 * @brief Common object getter code for retrieving from StoreGate.
 */


#ifndef D3PDMAKERUTILS_SGGETTERIMPL_H
#define D3PDMAKERUTILS_SGGETTERIMPL_H


#include "D3PDMakerInterfaces/IObjGetterTool.h"
#include "D3PDMakerUtils/SGKeyResolver.h"
#include "StoreGate/StoreGateSvc.h"
#include "GaudiKernel/IClassIDSvc.h"
#include "GaudiKernel/ServiceHandle.h"


namespace D3PD {


/**
 * @brief Common object getter code for retrieving from StoreGate.
 *
 * This class fulfills part of the @c IObjGetterTool interface,
 * fetching an object from StoreGate.  It's structured as a mixin
 * class, so it can be used both for single-object and container
 * getter tools.
 *
 * This class uses a few properties, but since it's a mixin (and doesn't
 * derive from @c AlgTool) it can't declare the properties itself.
 * Instead, it defines the macro @c SGGETTERIMPL_PROPS, which should
 * be used in the constructor of the derived @c AlgTool class.
 * You should also call @c initializeImpl() from @c initialize().
 *
 * The object being retrieved is specified by the properties
 * @c TypeName and @c SGKey.  SGKey may be a comma or space-separated list;
 * the first entry that's actually present in the event store will be used.
 * In addition, the property @c ClassIDSvc
 * may be used to specify the instance of this service.
 */
class SGGetterImpl
  : virtual public IObjGetterTool
{
public:
  /**
   * @brief Constructor.
   * @param name Name of this tool.
   * @param sg The event storage service.
   */
  SGGetterImpl (const std::string& name, ServiceHandle<StoreGateSvc>& sg);


  /**
   * @brief Return the type of object retrieved by this tool.
   */
  virtual const std::type_info& typeinfo() const;


  /**
   * @brief Return the class ID being read by this tool.
   */
  CLID clid() const;


  /**
   * @brief Return the target object.
   * @param allowMissing If true, then we should not generate errors
   *        if the requested object is missing.
   *
   * Should be of the type given by @c typeinfo.
   * Return 0 on failure.
   */
  virtual const void* getUntyped (bool allowMissing = false);


  /**
   * @brief Use this macro in the constructor of the derived @c AlgTool class.
   *
   * It declares the properties used by this class.
   */
#define SGGETTERIMPL_PROPS                                                   \
  do {                                                                       \
    this->declareProperty ("TypeName",   this->m_typename,                   \
                           "Name of the type of the object being retrieved");\
    this->declareProperty ("SGKey",      this->m_sgkey,                      \
                           "StoreGate key of the object being retrieved.  "  \
                           "This may be a comma or space-separated list; "   \
                           "the first existing key will be used.");          \
    this->declareProperty ("ClassIDSvc", this->m_clidsvc,                    \
                           "ClassIDSvc instance to use.");                   \
  } while (0)


protected:
  /**
   * @brief Initialize this mixin class.
   *
   * Call this from @c initialize.
   */
  StatusCode initializeImpl ();


  /// Property: Name of the type of the object being retrieved.
  std::string m_typename;

  /// Property: StoreGate key of the object being retrieved.
  std::string m_sgkey;

  /// Property: Instance of the ClassIDSvc to use.
  ServiceHandle<IClassIDSvc> m_clidsvc;

  /// Helper: Resolve the SG key to use.
  SGKeyResolver m_resolver;

private:
  /// CLID for the object being retrieved.
  CLID m_clid;

  /// The event storage service.
  ServiceHandle<StoreGateSvc>& m_sg;
};


} // namespace D3PD


#endif // not D3PDMAKERUTILS_SGGETTERIMPL_H

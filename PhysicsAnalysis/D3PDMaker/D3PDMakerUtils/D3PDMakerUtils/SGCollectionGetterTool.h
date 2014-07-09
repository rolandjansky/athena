// This file's extension implies that it's C, but it's really -*- C++ -*-.

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id$
/**
 * @file D3PDMakerUtils/SGCollectionGetterTool.h
 * @author scott snyder <snyder@bnl.gov>
 * @date Aug, 2009
 * @brief Collection getter tool retrieving a container from StoreGate.
 */


#ifndef D3PDMAKERUTILS_SGCOLLECTIONGETTERTOOL_H
#define D3PDMAKERUTILS_SGCOLLECTIONGETTERTOOL_H


#include "D3PDMakerUtils/CollectionGetterTool.h"
#include "D3PDMakerUtils/SGGetterImpl.h"
#include "AthenaKernel/errorcheck.h"


namespace D3PD {


/**
 * @brief Collection getter tool retrieving a container from StoreGate.
 *
 * This is a collection getter tool which retrieves a container from
 * StoreGate and allows iterating over it.  Since the iteration depends
 * on the container type, this tool is templated on the container type.
 * (Note: for a @c DataVector or @c DataList container, you can use
 * @c SGDataVectorGetterTool instead.)  To use this, you need to derive
 * a tool from this for your particular container type.
 */
template <class CONT>
class SGCollectionGetterTool
  : public SGGetterImpl, public CollectionGetterTool<CONT>
{
public:
  /**
   * @brief Standard Gaudi tool constructor.
   * @param type The name of the tool type.
   * @param name The tool name.
   * @param parent The tool's Gaudi parent.
   */
  SGCollectionGetterTool (const std::string& type,
                          const std::string& name,
                          const IInterface* parent);


  /// Standard Gaudi initialize method.
  virtual StatusCode initialize();


  /**
   * @brief Return the type of object retrieved by this tool.
   */
  virtual const std::type_info& typeinfo() const;


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
   * @brief Type-safe wrapper for @c get.
   * @param allowMissing If true, then we should not generate errors
   *        if the requested object is missing.
   *
   * Return the object as a pointer to @c T.
   * Return 0 if the get fails or if the pointer can't be converted.
   */
  virtual const CONT* get (bool allowMissing = false);
};


} // namespace D3PD


#include "D3PDMakerUtils/SGCollectionGetterTool.icc"


#endif // not D3PDMAKERUTILS_SGCOLLECTIONGETTERTOOL_H

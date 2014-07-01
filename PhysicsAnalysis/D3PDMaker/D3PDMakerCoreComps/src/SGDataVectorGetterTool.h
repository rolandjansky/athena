// This file's extension implies that it's C, but it's really -*- C++ -*-.

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: SGDataVectorGetterTool.h 560175 2013-09-03 02:10:39Z ssnyder $
/**
 * @file D3PDMakerCoreComps/src/SGDataVectorGetterTool.h
 * @author scott snyder <snyder@bnl.gov>
 * @date Aug, 2009
 * @brief Getter tool to retrieve DataVector/List collections from StoreGate.
 */


#ifndef D3PDMAKERCORECOMPS_SGDATAVECTORGETTERTOOL_H
#define D3PDMAKERCORECOMPS_SGDATAVECTORGETTERTOOL_H


#include "D3PDMakerUtils/CollectionGetterTool.h"
#include "D3PDMakerUtils/SGGetterImpl.h"
#include "D3PDMakerUtils/TypeConverter.h"
#include "DataModel/DataVector.h"
#include "AthenaPoolCnvSvc/IAthenaPoolCnvSvc.h"
#include "AthenaKernel/errorcheck.h"
#include "GaudiKernel/ServiceHandle.h"


namespace DataModel_detail {
class DVLInfoBase;
} // namespace DataModel_detail


namespace D3PD {


/**
 * @brief Getter tool to retrieve DataVector/List collections from StoreGate.
 *
 * This is a Getter tool that retrieves a collection of objects
 * from StoreGate.  This class handles @c DataVector and @c DataList
 * collections.  (Collections deriving from these may also be used,
 * provided that proper @c SG_BASE macros were used.)
 *
 * Properties:
 *   TypeName   - Name of the type of the object being retrieved.
 *   SGKey      - StoreGate key of the object being retrieved.
 *                Can also be a comma or space-separated list.
 *                In that case, the first key that actually exists
 *                in the data store will be used.
 *   ClassIDSvc - ClassIDSvc instance to use.
 */
class SGDataVectorGetterTool
  : public SGGetterImpl, public CollectionGetterToolImpl
{
public:
  /**
   * @brief Standard Gaudi tool constructor.
   * @param type The name of the tool type.
   * @param name The tool name.
   * @param parent The tool's Gaudi parent.
   */
  SGDataVectorGetterTool (const std::string& type,
                          const std::string& name,
                          const IInterface* parent);


  /// Standard Gaudi @c initialize method.
  virtual StatusCode initialize();


  /// Standard Gaudi @c finalize method.
  virtual StatusCode finalize();


  /**
   * @brief Return the element type of the collection.
   *
   * I.e., @c nextUntyped returns a pointer to this type.
   */
  virtual const std::type_info& elementTypeinfo() const;


  /**
   * @brief Reset the iteration to the start of the collection.
   * @param allowMissing If true, then we should not generate errors
   *        if the requested object is missing.
   *
   * Return failure if the container cannot be retrieved.
   */
  virtual StatusCode reset (bool allowMissing = false);


  /**
   * @brief Return a pointer to the next element in the collection.
   *
   * Return 0 when the collection has been exhausted.
   */
  virtual const void* nextUntyped();


  /**
   * @brief Return an estimate of the number of elements in the iteration.
   * @param allowMissing If true, then we should not generate errors
   *        if the requested object is missing.
   *
   * This can be used to pre-allocate memory.
   * (It's possible that this isn't known in advance of
   * iterating over the entire collection, for example
   * if a selection is being applied, so this is only a hint.)
   */
  virtual size_t sizeHint (bool allowMissing = false);


private:
  /// Used to auto-load converters, if needed.
  ServiceHandle<IAthenaPoolCnvSvc> m_athenaPoolCnvSvc;

  /// Retrieve DataVector/List information for this collection.
  const DataModel_detail::DVLInfoBase* getInfo (const std::type_info& ti);
  
  /// DataVector/List information for this collection.
  const DataModel_detail::DVLInfoBase* m_info;

  /// Current iterator over the collection.
  DataModel_detail::DVLIteratorBase* m_it;

  /// Converter from the pointer that we get from StoreGate to a pointer
  /// to the collection over which we iterate (which may be a base
  /// of what we get from SG).
  TypeConverter m_converter;
};


} // namespace D3PD


#endif // not D3PDMAKERCORECOMPS_SGCOLLECTIONGETTERTOOL_H

// This file's extension implies that it's C, but it's really -*- C++ -*-.

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id$
/**
 * @file CaloIdCnv/CaloIDHelper_IDDetDescrCnv.h
 * @author scott snyder <snyder@bnl.gov>
 * @date Aug, 2012
 * @brief Common code for creating calo ID helpers in the detector store.
 *
 * A converter to make an ID helper and put it in the detector store
 * is mostly boilerplate, so we use the classes here to factor that out.
 * The base class @c CaloIDHelper_IDDetDescrCnv has most of the boilerplate,
 * and also contains the code for initializing the helper.  This relies
 * on the abstract virtual function @c createHelper to do the work
 * of actually creating the helper and storing it into the @c DataObject
 * (as those depend on the actual type of the helper).
 *
 * An implementation of @c createHelper is provided in the class
 * @c CaloIDHelper_IDDetDescrCnv_T<T>, which is templated on the helper
 * type.  Usually, specializations of this class can be used
 * directly as the converter class.  This calls the function
 * @c make_CaloIDHelper to actually create the helper instance.
 * By default, this just does @c new with the default constructor,
 * but it can be overridden for specific types if needed
 * (for example for the @c CaloCell_* helpers).
 */


#ifndef CALOIDCNV_CALOIDHELPER_IDDETDESCRCNV_H
#define CALOIDCNV_CALOIDHELPER_IDDETDESCRCNV_H


#include "DetDescrCnvSvc/DetDescrConverter.h"


class IdHelper;


namespace CaloIdCnv {


/**
 * @brief Base class for calo ID helper converters.
 *
 * This class most of the boilerplate needed toe the converters,
 * and also contains the code for initializing the helper.
 *
 * The abstract virtual function @c createHelper is used to actually
 * create the helper and to initialize the @c DataObject.
 */
class CaloIDHelper_IDDetDescrCnv
  : public DetDescrConverter
{
public:
  /**
   * @brief Called by the converter infrastructure to create an object.
   * @param pAddr Address of the object to create.
   * @param pObj[out] Set to a reference to the created helper.
   */
  virtual StatusCode createObj(IOpaqueAddress* pAddr, DataObject*& pObj);


  /**
   * @brief Return the service type.  Required by the base class.
   */
  virtual long int   repSvcType() const;


protected:
  /**
   * @brief Constructor.
   * @param clid The CLID if the class we're constructing.
   * @param svcloc Gaudi service locator.
   */
  CaloIDHelper_IDDetDescrCnv (const CLID& clid, ISvcLocator* svcloc);


  /**
   * @brief Create the helper object.
   * @param helperKey StoreGate key of the helper.
   * @param idhelper[out] Set to a pointer to the new helper.
   * @param pObj[out] Set to a reference to the created helper.
   */
  virtual StatusCode createHelper (const std::string& helperKey,
                                   IdHelper* & idhelper,
                                   DataObject*& pObj) = 0;
};


/**
 * @brief Helper function to actually create a new helper.
 * @param svcloc Gaudi service locator.
 * @param helperKey The StoreGate key of the helper being created.
 * @param idhelper[out] Set to the created helper.
 *
 * The default implementation just does @c new with the default constructor;
 * however, this function may be overridden for specific types.
 */
template <class T>
StatusCode make_CaloIDHelper (ISvcLocator* svcloc,
                              const std::string& helperKey,
                              T* & idhelper);


/**
 * @brief Class for calo ID helper converters.
 *
 * This class implements a calo ID helper converter for the helper
 * type @T, using the @c CaloIDHelper_IDDetDescrCnv base class
 * defined above.  It uses @c make_CaloIDHelper to actually create
 * the helper instance.
 */
template <class T>
class CaloIDHelper_IDDetDescrCnv_T
  : public CaloIDHelper_IDDetDescrCnv
{
  /// This creates us, so it needs access to the constructor.
  friend class CnvFactory<CaloIDHelper_IDDetDescrCnv_T>;


public:
  /**
   * @brief Return the CLID of the class we create.
   * Required by the converter infrastructure.
   */
  static const CLID& classID();


protected:
  /**
   * @brief Constructor.
   * @param svcloc Gaudi service locator.
   */
  CaloIDHelper_IDDetDescrCnv_T (ISvcLocator* svcloc);


  /**
   * @brief Create the helper object.
   * @param helperKey StoreGate key of the helper.
   * @param idhelper[out] Set to a pointer to the new helper.
   * @param pObj[out] Set to a reference to the created helper.
   */
  virtual StatusCode createHelper (const std::string& helperKey,
                                   IdHelper* & idhelper,
                                   DataObject*& pObj);
};


} // namespace CaloIdCnv


#include "CaloIdCnv/CaloIDHelper_IDDetDescrCnv.icc"


#endif // not CALOIDCNV_CALOIDHELPER_IDDETDESCRCNV_H

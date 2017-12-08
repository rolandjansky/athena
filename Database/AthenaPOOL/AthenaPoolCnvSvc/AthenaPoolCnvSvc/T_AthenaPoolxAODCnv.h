// This file's extension implies that it's C, but it's really -*- C++ -*-.

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id$
/**
 * @file AthenaPoolCnvSvc/T_AthenaPoolxAODCnv.h
 * @author scott snyder <snyder@bnl.gov>
 * @date Jan, 2016
 * @brief Athena pool converter for xAOD classes.
 */


#ifndef ATHENAPOOLCNVSVC_T_ATHENAPOOLXAODCNV_H
#define ATHENAPOOLCNVSVC_T_ATHENAPOOLXAODCNV_H


#include "AthenaPoolCnvSvc/T_AthenaPoolCustomCnv.h"
#include "AthenaPoolCnvSvc/TPCnvList.h"
#include "AthenaPoolCnvSvc/exceptions.h"
#include "AthContainers/AuxVectorBase.h"
#include "AthenaKernel/ClassName.h"


/**
 * @brief Athena pool converter for xAOD classes.
 *
 * XAOD is the class being read/written.
 * TPCNVS is a list of TP converters to handle older versions of the class.
 *
 * On writing, the container is simply copied (thinning is handled by the
 * thinning service).  For reading, we read the object either directly
 * or using one of the TP converters, depending on the saved GUID.
 * The link to the aux store is then set based on the SG key.
 */
template <class XAOD, class ... TPCNVS>
class T_AthenaPoolxAODCnv
  : public T_AthenaPoolCustomCnv<XAOD, XAOD>
{
  friend class CnvFactory< T_AthenaPoolxAODCnv >;

  template <class CNV, class TPCNV>
  friend class AthenaPoolCnvSvc::TPCnvElt;

public:
  typedef T_AthenaPoolCustomCnv<XAOD, XAOD> Base;


  /**
   * @brief Constructor.
   * @param svcLoc Gaudi service locator.
   */
  T_AthenaPoolxAODCnv ( ISvcLocator* svcLoc );


  /**
   * @brief Read an object from persistent storage.
   * @param pAddr Address of the object to read.
   * @param pObj[out] Pointer to the read object.
   *
   * This is overridden here in order to be able to pass the SG key
   * from the address to @c createTransient.
   */
  virtual StatusCode createObj( IOpaqueAddress* pAddr,
                                DataObject*& pObj ) override;


  /**
   * @brief Convert a transient object to persistent form.
   * @param trans The transient object to convert.
   *
   * Returns a newly-allocated persistent object.
   */
  virtual XAOD* createPersistent( XAOD* trans ) override;


  /**
   * @brief Read the persistent object and convert it to transient.
   *
   * Returns a newly-allocated transient object.
   * Errors are reported by raising exceptions.
   */
  virtual XAOD* createTransient() override;

  
private:
  /// SG key for the object being read.
  std::string m_key;

  /// GUID of the object being read.
  Guid m_guid;

  /// List of TP converters.
  AthenaPoolCnvSvc::TPCnvList<T_AthenaPoolxAODCnv, XAOD, TPCNVS...> m_tpcnvs;
};


#include "AthenaPoolCnvSvc/T_AthenaPoolxAODCnv.icc"


#endif // not ATHENAPOOLCNVSVC_T_ATHENAPOOLXAODCNV_H


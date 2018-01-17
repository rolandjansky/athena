// This file's extension implies that it's C, but it's really -*- C++ -*-.

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id$
/**
 * @file AthenaPoolCnvSvc/T_AthenaPoolAuxContainerCnv.h
 * @author scott snyder <snyder@bnl.gov>
 * @date Jan, 2016
 * @brief Athena pool converter for aux store classes.
 */


#ifndef ATHENAPOOLCNVSVC_T_ATHENAPOOLAUXCONTAINERCNV_H
#define ATHENAPOOLCNVSVC_T_ATHENAPOOLAUXCONTAINERCNV_H


#include "AthenaPoolCnvSvc/T_AthenaPoolCustomCnv.h"
#include "AthenaPoolCnvSvc/TPCnvList.h"
#include "AthenaPoolCnvSvc/exceptions.h"
#include "AthenaPoolCnvSvc/debug.h"
#include "AthContainers/tools/copyThinned.h"
#include "AthenaKernel/IThinningSvc.h"
#include "AthenaKernel/ClassID_traits.h"


/**
 * @brief Athena pool converter for aux store classes.
 *
 * AUXSTORE is the class being read/written.
 * TPCNVS is a list of TP converters to handle older versions of the class.
 *
 * On writing, the container is copied (and thinned if required).
 * For reading, we read the object either directly
 * or using one of the TP converters, depending on the saved GUID.
 */
template <class AUXSTORE, class ... TPCNVS>
class T_AthenaPoolAuxContainerCnv
  : public T_AthenaPoolCustomCnv<AUXSTORE, AUXSTORE>
{
  friend class CnvFactory< T_AthenaPoolAuxContainerCnv >;

  template <class CNV, class TPCNV>
  friend class AthenaPoolCnvSvc::TPCnvElt;

public:
  typedef T_AthenaPoolCustomCnv<AUXSTORE, AUXSTORE> Base;


  /**
   * @brief Constructor.
   * @param svcLoc Gaudi service locator.
   */
  T_AthenaPoolAuxContainerCnv ( ISvcLocator* svcLoc );


  /**
   * @brief Convert a transient object to persistent form.
   * @param trans The transient object to convert.
   *
   * Returns a newly-allocated persistent object.
   */
  virtual AUXSTORE* createPersistent( AUXSTORE* trans ) override;


  /**
   * @brief Read the persistent object and convert it to transient.
   *
   * Returns a newly-allocated transient object.
   * Errors are reported by raising exceptions.
   */
  virtual AUXSTORE* createTransient() override;

  
private:
  /// GUID of the object being read.
  Guid m_guid;

  /// List of TP converters.
  AthenaPoolCnvSvc::TPCnvList<T_AthenaPoolAuxContainerCnv, AUXSTORE, TPCNVS...> m_tpcnvs;
};


#include "AthenaPoolCnvSvc/T_AthenaPoolAuxContainerCnv.icc"


#endif // not ATHENAPOOLCNVSVC_T_ATHENAPOOLAUXCONTAINERCNV_H


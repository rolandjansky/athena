// This file's extension implies that it's C, but it's really -*- C++ -*-.

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id$
/**
 * @file AthenaPoolCnvSvc/T_AthenaPoolTPCnvCnv.h
 * @author scott snyder <snyder@bnl.gov>
 * @date Jan, 2016
 * @brief Athena pool converter for a class using TP separation.
 */


#ifndef ATHENAPOOLCNVSVC_T_ATHENAPOOLTPCNVCNV_H
#define ATHENAPOOLCNVSVC_T_ATHENAPOOLTPCNVCNV_H


#include "AthenaPoolCnvSvc/TPCnvList.h"
#include "AthenaPoolCnvSvc/T_AthenaPoolCustomCnv.h"
#include "AthenaPoolCnvSvc/exceptions.h"
#include "AthenaKernel/ClassName.h"


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
template <class TRANS, class TPCNV_CUR, class ... TPCNVS>
class T_AthenaPoolTPCnvCnv
  : public T_AthenaPoolCustomCnv<TRANS, typename TPCNV_CUR::Pers_t>
{
  friend class CnvFactory< T_AthenaPoolTPCnvCnv >;

  template <class CNV, class TPCNV>
  friend class AthenaPoolCnvSvc::TPCnvElt;

public:
  typedef T_AthenaPoolCustomCnv<TRANS, typename TPCNV_CUR::Pers_t> Base;
  typedef typename TPCNV_CUR::Pers_t Pers_t;

  
  /**
   * @brief Constructor.
   * @param svcLoc Gaudi service locator.
   */
  T_AthenaPoolTPCnvCnv ( ISvcLocator* svcLoc );

  
  /**
   * @brief Convert a transient object to persistent form.
   * @param trans The transient object to convert.
   *
   * Returns a newly-allocated persistent object.
   */
  virtual Pers_t* createPersistent( TRANS* trans ) override;


  /**
   * @brief Read the persistent object and convert it to transient.
   *
   * Returns a newly-allocated transient object.
   * Errors are reported by raising exceptions.
   */
  virtual TRANS* createTransient() override;

  
private:
  /// GUID of the object being read.
  Guid m_guid;

  /// TP converter for current persistent class version.
  TPCNV_CUR m_tpcnv_cur;

  /// List of TP converters for older versions.
  AthenaPoolCnvSvc::TPCnvList<T_AthenaPoolTPCnvCnv, TRANS, TPCNVS...> m_tpcnvs;
};


#include "AthenaPoolCnvSvc/T_AthenaPoolTPCnvCnv.icc"


#endif // not ATHENAPOOLCNVSVC_T_ATHENAPOOLTPCNVCNV_H


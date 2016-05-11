// This file's extension implies that it's C, but it's really -*- C++ -*-.

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id$
/**
 * @file AthenaPoolCnvSvc/T_AthenaPoolViewVectorCnv.h
 * @author scott snyder <snyder@bnl.gov>
 * @date Jan, 2016
 * @brief Athena pool converter for a ViewVector class.
 */


#ifndef ATHENAPOOLCNVSVC_T_ATHENAPOOLVIEWVECTORCNV_H
#define ATHENAPOOLCNVSVC_T_ATHENAPOOLVIEWVECTORCNV_H


#include "AthenaPoolCnvSvc/T_AthenaPoolCustomCnv.h"
#include "AthenaPoolCnvSvc/exceptions.h"
#include "StorageSvc/DbReflex.h"
#include "AthContainers/ViewVector.h"
#include "AthContainers/dataVectorAsELV.h"
#include "AthContainers/ConstDataVector.h"
#include "AthLinks/ElementLink.h"
#include "AthenaKernel/errorcheck.h"
#include "CxxUtils/make_unique.h"
#include "CxxUtils/StrFormat.h"
#include <vector>
#include <cstdlib>


/**
 * @brief Athena pool converter for a ViewVector class.
 *
 * This pool converter converts between a transient ViewVector<DV> type
 * and a persistent std::vector<ElementLink<DV> > type, where DV is
 * a DataVector.
 */
template <class DV>
class T_AthenaPoolViewVectorCnv
  : public T_AthenaPoolCustomCnv<ViewVector<DV>,
                                 std::vector<ElementLink<DV> > >
{
public:
  /// The transient and persistent types.
  typedef ViewVector<DV> trans_t;
  typedef std::vector<ElementLink<DV> > pers_t;

  /// Base class.
  typedef T_AthenaPoolCustomCnv<trans_t, pers_t> Base;

  
  /**
   * @brief Constructor.
   * @param svcloc The Gaudi service locator.
   */
  T_AthenaPoolViewVectorCnv (ISvcLocator* svcloc);


  /**
   * @brief Standard Gaudi initialize method.
   */
  virtual StatusCode initialize() override;


  /**
   * @brief Convert a transient object to persistent form.
   * @param trans The transient object to convert.
   *
   * Returns a newly-allocated persistent object.
   */
  virtual pers_t* createPersistent( trans_t* trans ) override;


  /**
   * @brief Read the persistent object and convert it to transient.
   *
   * Returns a newly-allocated transient object.
   * Errors are reported by raising exceptions.
   */
  virtual trans_t* createTransient() override;

private:
  std::vector<pool::Guid> m_guids;
};


#include "AthenaPoolCnvSvc/T_AthenaPoolViewVectorCnv.icc"


#endif // not ATHENAPOOLCNVSVC_T_ATHENAPOOLVIEWVECTORCNV_H

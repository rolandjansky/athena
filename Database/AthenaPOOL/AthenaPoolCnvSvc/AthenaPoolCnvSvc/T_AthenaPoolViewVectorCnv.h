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
#include "AthenaPoolCnvSvc/debug.h"
#include "StorageSvc/DbReflex.h"
#include "AthContainers/ViewVector.h"
#include "AthContainers/dataVectorAsELV.h"
#include "AthContainers/ConstDataVector.h"
#include "AthLinks/ElementLink.h"
#include "AthenaKernel/errorcheck.h"
#include "AthenaKernel/ClassID_traits.h"
#include "CxxUtils/StrFormat.h"
#include <vector>
#include <cstdlib>


/**
 * @brief Athena pool converter for a ViewVector class.
 *
 * This pool converter prepares a @c ViewVector class for writing, and
 * cleans up after it has been read:
 *  - For writing, we make a copy and call @c toPersistent on it, clearing
 *    the transient contents  in the process.
 *  - For writing, we need to call @c toTransient if there was schema
 *    evolution.  We also can remove the persistent data.
 *
 * It can also handle reading from a persistent type  of
 * @c std::vector<ElementLink<DV> >.
 */
template <class DV>
class T_AthenaPoolViewVectorCnv
  : public T_AthenaPoolCustomCnv<ViewVector<DV>, ViewVector<DV> >
{
public:
  /// The transient and persistent types.
  typedef ViewVector<DV> trans_t;
  typedef trans_t pers_t;

  /// Alternate persistent type; raw vector of EL.
  typedef std::vector<ElementLink<DV> > pers2_t;

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
  /**
   * @brief Return the set of GUIDs for persistent type @c ti.
   * 
   * The first entry will be the guid for @c ti itself, followed
   * by entries for any earlier persistent versions.
   */
  std::vector<pool::Guid> initGuids (const std::type_info& ti) const;

  /// List of guids for @c ViewVector classes.
  std::vector<pool::Guid> m_guids;

  /// List of guids for @c std::vector<ElementLink<DV> > classes.
  std::vector<pool::Guid> m_guids2;
};


#include "AthenaPoolCnvSvc/T_AthenaPoolViewVectorCnv.icc"


#endif // not ATHENAPOOLCNVSVC_T_ATHENAPOOLVIEWVECTORCNV_H

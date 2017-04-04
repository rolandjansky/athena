// This file's extension implies that it's C, but it's really -*- C++ -*-.

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id$
/**
 * @file ViewVectorBase.h
 * @author scott snyder <snyder@bnl.gov>
 * @date Sep, 2016
 * @brief Hold the persistent representation for a ViewVector.
 */


#ifndef ATHCONTAINERS_VIEWVECTORBASE_H
#define ATHCONTAINERS_VIEWVECTORBASE_H


#include "AthContainers/dataVectorAsELV.h"
#include "AthContainers/tools/CurrentEventStore.h"
#include "AthLinks/ElementLinkBase.h"
#include <vector>


class ViewVectorBaseTest;


namespace SG {


/**
 * @brief Hold the persistent representation for a ViewVector.
 *
 * Base class for @c ViewVector; not to be used directly.
 */
class ViewVectorBase
{
public:
  /// Destructor.
  virtual ~ViewVectorBase();


  /**
   * @brief Convert the vector to persistent form.
   */
  virtual void toPersistent() = 0;


  /**
   * @brief Convert the vector to transient form.
   */
  virtual void toTransient() = 0;


  /**
   * @brief Clear the persistent data.
   */
  void clearPersistent();

  
  /**
   * @brief Set a flag to declare that the vector should be cleared
   *        on the next call to toPersistent().
   *
   *        This would be used in the case where we make a copy of the
   *        object being written.
   */
  void setClearOnPersistent();

  
protected:
  /**
   * @brief Convert to persistent form.
   * @param v The vector to convert.
   *
   * Called for classes that have a CLID.
   */
  template <class DV>
  void doToPersistent1 (DV& v, const std::true_type&);


  /**
   * @brief Convert to persistent form.
   * @param v The vector to convert.
   *
   * Called for classes that do not have a CLID.
   * This will simply abort.
   */
  template <class DV>
  void doToPersistent1 (DV&, const std::false_type&);


  /**
   * @brief Convert to persistent form.
   * @param v The vector to convert.
   *
   * This will abort if called for a class with no CLID.
   */
  template <class DV>
  void doToPersistent (DV& v);


  /**
   * @brief Convert to transient form.
   * @param v The vector to fill in.
   *
   * Called for classes that have a CLID if DV is not a ConstDataVector.
   */
  template <class DV>
  void doToTransient2 (DV& v, const std::true_type&);


  /**
   * @brief Convert to transient form.
   * @param v The vector to fill in.
   *
   * Called for the case that DV is not a ConstDataVector.  A no-op.
   */
  template <class DV>
  void doToTransient2 (DV&, const std::false_type&) {}


  /**
   * @brief Convert to persistent form.
   * @param v The vector to convert.
   *
   * Class has a CLID; dispatch based on whether DV is a ConstDataVector.
   */
  template <class DV>
  void doToTransient1 (DV& v, const std::true_type&)
  {
    doToTransient2 (v, std::is_same<DV, typename DV::base_data_vector>());
  }


  /**
   * @brief Convert to transient form.
   * @param v The vector to fill in.
   *
   * Called for the case that DV doesn't have a CLID.  A no-op.
   *
   * This is also what will get called for the case of an older version
   * of a schema-evolved vector.
   */
  template <class DV>
  void doToTransient1 (DV&, const std::false_type&) {}


  /**
   * @brief Convert to persistent form.
   * @param v The vector to convert.
   *
   * Dispatch based on whether the class has a CLID.
   */
  template <class DV>
  void doToTransient (DV& v)
  {
#ifdef XAOD_STANDALONE
    doToTransient1 (v, std::true_type());
#else
    doToTransient1 (v, typename ClassID_traits<DV>::has_classID_tag());
#endif
  }

  
private:
  // For unit testing.
  friend class ::ViewVectorBaseTest;

  /// The persistent form.  (sgkey, index)
  std::vector<unsigned int> m_persKey;
  std::vector<unsigned int> m_persIndex;

  /// If true, the vector should be cleared when doPersistent is called.
  bool m_clearOnPersistent = false;
};


} // namespace SG


#include "AthContainers/ViewVectorBase.icc"


#endif // not ATHCONTAINERS_VIEWVECTORBASE_H

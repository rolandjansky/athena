/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// BinnedArrayArray.h, (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////

#ifndef TRKDETDESCRUTILS_BINNEDARRAYARRAY_H
#define TRKDETDESCRUTILS_BINNEDARRAYARRAY_H

#include "TrkDetDescrUtils/BinUtility.h"
#include "TrkDetDescrUtils/BinnedArray.h"
#include "TrkDetDescrUtils/SharedObject.h"
// GaudiKernel
#include "GaudiKernel/GaudiException.h"
// Eigen
#include "GeoPrimitives/GeoPrimitives.h"
// STL
#include <vector>

class MsgStream;

namespace Trk {

/** @class BinnedArrayArray

   the most generic extension of a BinnedArray:
     a BinnedArray of BinnedArrays voila

   @author Andreas.Salzburger@cern.ch
   */

template<class T>
class BinnedArrayArray final : public BinnedArray<T>
{

public:
  /**Default Constructor  */
  BinnedArrayArray(
    const std::vector<std::pair<BinnedArray<T>*, Amg::Vector3D>>& tbas,
    BinUtility* bUtility)
    : m_binUtility(bUtility)
    , m_binnedArrays(bUtility->bins(0), nullptr)
    , m_arrayObjects()
  {
    // the array objects
    m_arrayObjects.reserve(tbas.size() * bUtility->bins(0));
    // looping over the contained binned arraysa
    for (auto& barray : tbas) {
      // binned array ordering
      m_binnedArrays[bUtility->bin(barray.second, 0)] = barray.first;
      // get the array objects
      BinnedArraySpan<T const * const > aObjects = barray.first->arrayObjects();
      for (auto& o : aObjects)
        m_arrayObjects.push_back(o);
    }
  }

  /** Copy Constructor */
  BinnedArrayArray(const BinnedArrayArray& baa)
    : m_binUtility(baa.m_binUtility->clone())
    , m_arrayObjects(baa.m_arrayObjects)
  {
    copyBinnedArrays(baa.m_binnedArrays);
  }

  /**Virtual Destructor*/
  virtual ~BinnedArrayArray()
  {
    delete m_binUtility;
    deleteBinnedArrays();
  }

  /** assignment operator matching the copy constructor */
  BinnedArrayArray& operator=(const BinnedArrayArray& baa)
  {
    if (&baa != this) {
      delete m_binUtility;
      m_binUtility = baa.m_binUtility->clone();

      deleteBinnedArrays();
      copyBinnedArrays(baa.m_binnedArrays);

      m_arrayObjects = baa.m_arrayObjects;
    }
    return *this;
  }

  /** Implicit constructor */
  virtual BinnedArrayArray* clone() const override
  {
    return new BinnedArrayArray(*this);
  }

  /** Returns the pointer to the templated class object from the
     BinnedArrayArray, it returns 0 if not defined, takes local position */
  virtual T* object(const Amg::Vector2D& lp) const override
  {
    if (m_binUtility->inside(lp)) {
      BinnedArray<T>* ba = m_binnedArrays[m_binUtility->bin(lp, 0)];
      if (ba)
        return ba->object(lp);
    }
    return nullptr;
  }

  /** Returns the pointer to the templated class object from the
     BinnedArrayArray it returns 0 if not defined, takes global position */
  virtual T* object(const Amg::Vector3D& gp) const override
  {
    if (m_binUtility->inside(gp)) {
      BinnedArray<T>* ba = m_binnedArrays[m_binUtility->bin(gp, 0)];
      if (ba)
        return ba->object(gp);
    }
    return nullptr;
  }

  /** Returns the pointer to the templated class object from the
   * BinnedArrayArray - entry point*/
  virtual T* entryObject(const Amg::Vector3D& gp) const override
  {
    return object(gp);
  }

  /** Returns the pointer to the templated class object from the
   * BinnedArrayArray, takes 3D position & direction */
  virtual T* nextObject(const Amg::Vector3D& gp,
                        const Amg::Vector3D&,
                        bool) const override
  {
    return object(gp);
  }

  /** Return all objects of the Array non-const T*/
  virtual BinnedArraySpan<T* const > arrayObjects() override final
  {
    return BinnedArraySpan<T* const >(&*m_arrayObjects.begin(), &*m_arrayObjects.end());
  }

  /** Return all objects of the Array const T*/
  virtual BinnedArraySpan<T const * const> arrayObjects() const override final
  {
    return BinnedArraySpan<T const* const>(&*m_arrayObjects.begin(), &*m_arrayObjects.end());
  }

  /** Number of Entries in the Array */
  virtual unsigned int arrayObjectsNumber() const override final
  {
    return m_arrayObjects.size();
  };

  /** Return the BinUtility*/
  virtual const BinUtility* binUtility() const override { return m_binUtility; }

private:
  /** cleanup the array of binned arrays.
   */
  void deleteBinnedArrays()
  {
    for (auto ba : m_binnedArrays) {
      delete ba;
    }
    m_binnedArrays.clear();
  }

  /** Build up binned arrays array from cloned binned arrays.
   *  Note: the original m_binnedArrays is not automatically cleaned but
   * requires
   *  @ref deleteBinnedArrays to be called explicitly. */
  void copyBinnedArrays(const std::vector<BinnedArray<T>*>& binned_arrays)
  {
    m_binnedArrays.reserve(binned_arrays.size());

    for (const auto& a_binned_array : binned_arrays) {
      m_binnedArrays.push_back(a_binned_array->clone());
    }
  }

  BinUtility* m_binUtility;
  std::vector<BinnedArray<T>*> m_binnedArrays;
  std::vector<T*> m_arrayObjects;
};
} // end of namespace Trk

#endif // TRKDETDESCRUTILS_BINNEDARRAYARRAY_H

/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// BinnedArray1D.h, (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////

#ifndef TRKDETDESCRUTILS_BINNEDARRAY1D_H
#define TRKDETDESCRUTILS_BINNEDARRAY1D_H

#include "TrkDetDescrUtils/BinUtility.h"
#include "TrkDetDescrUtils/BinnedArray.h"
#include "TrkDetDescrUtils/SharedObject.h"

// STL
#include <vector>

#include "CxxUtils/CachedUniquePtr.h"

class MsgStream;

namespace Trk {

/** @class BinnedArray1D

    1-dimensional binned arry based on a sorting
    given by the BinUtitlity.

   @author Andreas.Salzburger@cern.ch
   @author Christos Anastopoulos (Athena MT modifications)
   */

template<class T>
class BinnedArray1D final : public BinnedArray<T>
{

public:
  /**Default Constructor - needed for inherited classes */
  BinnedArray1D()
    : BinnedArray<T>()
    , m_array{}
    , m_arrayObjects(nullptr)
    , m_binUtility(nullptr)
  {}

  /**Constructor with std::vector and a  BinUtility - reference counted, will
     delete objects at the end, if this deletion should be turned off, the
     boolean deletion should be switched to false the global position is given
     by object */
  BinnedArray1D(
    const std::vector<std::pair<SharedObject<T>, Amg::Vector3D>>& tclassvector,
    BinUtility* bingen)
    : BinnedArray<T>()
    , m_array{}
    , m_arrayObjects(nullptr)
    , m_binUtility(bingen)
  {
    // prepare the binned Array
    if (bingen) {
      size_t vecsize = tclassvector.size();
      m_array = std::vector<SharedObject<T>>(vecsize);
      for (size_t ivec = 0; ivec < vecsize; ++ivec) {
        const Amg::Vector3D currentGlobal(((tclassvector[ivec]).second));
        if (bingen->inside(currentGlobal)) {
          m_array[bingen->bin(currentGlobal, 0)] = ((tclassvector)[ivec]).first;
        } else
          throw GaudiException("BinnedArray1D constructor",
                               "Object outside bounds",
                               StatusCode::FAILURE);
      }
    }
  }

  /**Copy Constructor - copies only pointers !*/
  BinnedArray1D(const BinnedArray1D& barr)
    : BinnedArray<T>()
    , m_array{}
    , m_arrayObjects(nullptr)
    , m_binUtility(nullptr)
  {
    m_binUtility = (barr.m_binUtility) ? barr.m_binUtility->clone() : nullptr;
    if (m_binUtility) {
      m_array = std::vector<SharedObject<T>>(barr.m_array.size());
      for (unsigned int ient = 0; ient < barr.m_array.size(); ++ient) {
        m_array[ient] = (barr.m_array)[ient];
      }
    }
  }
  /**Assignment operator*/
  BinnedArray1D& operator=(const BinnedArray1D& barr)
  {
    if (this != &barr) {
      m_array.clear();
      m_arrayObjects.release();
      delete m_binUtility;
      // now refill
      m_binUtility = (barr.m_binUtility) ? barr.m_binUtility->clone() : 0;

      if (m_binUtility) {
        m_array = std::vector<SharedObject<T>>(barr.m_array.size());
        for (unsigned int ient = 0; ient < barr.m_array.size(); ++ient) {
          m_array[ient] = (barr.m_array)[ient];
        }
      }
    }
    return *this;
  }
  /** Implicit Constructor */
  BinnedArray1D* clone() const { return new BinnedArray1D(*this); }

  /**Virtual Destructor*/
  ~BinnedArray1D() { delete m_binUtility; }

  /** Returns the pointer to the templated class object from the BinnedArray,
      it returns 0 if not defined;
   */
  T* object(const Amg::Vector2D& lp) const
  {
    if (m_binUtility->inside(lp)) {
      return (m_array[m_binUtility->bin(lp, 0)]).get();
    }
    return nullptr;
  }

  /** Returns the pointer to the templated class object from the BinnedArray
      it returns 0 if not defined;
   */
  T* object(const Amg::Vector3D& gp) const
  {
    return (m_array[m_binUtility->bin(gp, 0)]).get();
  }

  /** Returns the pointer to the templated class object from the BinnedArray -
   * entry point*/
  T* entryObject(const Amg::Vector3D& gp) const
  {
    return (m_array[m_binUtility->entry(gp, 0)]).get();
  }

  /** Returns the pointer to the templated class object from the BinnedArray
   */
  T* nextObject(const Amg::Vector3D& gp,
                const Amg::Vector3D& mom,
                bool associatedResult = true) const
  {
    // the bins
    size_t bin = associatedResult ? m_binUtility->bin(gp, 0)
                                  : m_binUtility->next(gp, mom, 0);
    return (m_array[bin]).get();
  }

  /** Return all objects of the Array non-const T*/
  BinnedArraySpan<T* const> arrayObjects()
  {
    createArrayCache();
    return Trk::BinnedArraySpan<T* const>(&*(m_arrayObjects->begin()), &*(m_arrayObjects->end()));
  }

  /** Return all objects of the Array const T*/
  BinnedArraySpan<T const * const > arrayObjects() const
  {
    createArrayCache();
    return Trk::BinnedArraySpan<const T* const>(&*(m_arrayObjects->begin()), &*(m_arrayObjects->end()));
  }

  /** Number of Entries in the Array */
  unsigned int arrayObjectsNumber() const { return arrayObjects().size(); }

  /** Return the BinUtility*/
  const BinUtility* binUtility() const { return (m_binUtility); }

private:
  void createArrayCache() const
  {
    if (!m_arrayObjects) {
      std::unique_ptr<std::vector<T*>> arrayObjects =
        std::make_unique<std::vector<T*>>();
      auto bins = m_binUtility->bins(0);
      arrayObjects->reserve(bins);
      for (size_t ill = 0; ill < bins; ++ill) {
        arrayObjects->push_back((m_array[ill]).get());
      }
      m_arrayObjects.set(std::move(arrayObjects));
    }
  }

  //!< vector of pointers to the class T
  std::vector<SharedObject<T>> m_array;
  //!< forced 1D vector of pointers to class T
  CxxUtils::CachedUniquePtr<std::vector<T*>> m_arrayObjects;
  //!< binUtility for retrieving and filling the Array
  BinUtility* m_binUtility;
};

} // end of namespace Trk

#endif // TRKSURFACES_BINNEDARRAY1D_H

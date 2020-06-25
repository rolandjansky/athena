/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// BinnedArray2D.h, (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////

#ifndef TRKDETDESCRUTILS_BINNEDARRAY2D_H
#define TRKDETDESCRUTILS_BINNEDARRAY2D_H

#include "GaudiKernel/GaudiException.h"
#include "TrkDetDescrUtils/BinUtility.h"
#include "TrkDetDescrUtils/BinnedArray.h"
#include "TrkDetDescrUtils/SharedObject.h"

// STL
#include <vector>

class MsgStream;

namespace Trk {

/** @class BinnedArray2DT

   Avoiding a map search, the templated BinnedArrayT class can help
   ordereing geometrical objects by providing a dedicated BinUtility.

   dedicated for 2-dim equidistant binning

   @author Andreas.Salzburger@cern.ch
   @author Christos Anastopoulos (Athena MT modifications)
   */

template<class T>
class BinnedArray2DT : public BinnedArrayT<T>
{

public:
  /**Default Constructor - needed for inherited classes */
  BinnedArray2DT()
    : BinnedArrayT<T>()
    , m_array{}
    , m_arrayObjects(nullptr)
    , m_binUtility(nullptr)
  {}

  /**Constructor with std::vector and a BinUtility - reference counted, will
     delete objects at the end, if this deletion should be turned off, the
     boolean deletion should be switched to false the global position is given
     by object! */
  BinnedArray2DT(
    const std::vector<std::pair<SharedObject<T>, Amg::Vector3D>>& tclassvector,
    BinUtility* bingen)
    : BinnedArrayT<T>()
    , m_array{}
    , m_arrayObjects(nullptr)
    , m_binUtility(bingen)
  {

    if (bingen) {
      m_array=std::vector<std::vector<SharedObject<T>>>(bingen->bins(1));
      for (size_t i = 0; i < bingen->bins(1); ++i) {
        m_array[i] = std::vector<SharedObject<T>>(bingen->bins(0));
      }
      // fill the Volume vector into the array
      size_t vecsize = tclassvector.size();
      for (size_t ivec = 0; ivec < vecsize; ++ivec) {
        const Amg::Vector3D currentGlobal(((tclassvector[ivec]).second));
        if (bingen->inside(currentGlobal)) {
          std::vector<SharedObject<T>>& curVec =
            m_array[bingen->bin(currentGlobal, 1)];

          curVec[bingen->bin(currentGlobal, 0)] = ((tclassvector)[ivec]).first;
        } else {
          throw GaudiException(
            "BinnedArray2DT", "Object outside bounds", StatusCode::FAILURE);
        }
      }
    }
  }

  /**Copy Constructor - copies only pointers !*/
  BinnedArray2DT(const BinnedArray2DT& barr)
    : BinnedArrayT<T>()
    , m_array{}
    , m_arrayObjects(nullptr)
    , m_binUtility(nullptr)
  {
    m_binUtility = (barr.m_binUtility) ? barr.m_binUtility->clone() : nullptr;
    // copy over
    m_array=std::vector<std::vector<SharedObject<T>>>(barr.m_array.size());
    for (size_t ihl = 0; ihl < barr.m_array.size(); ++ihl) {
      m_array[ihl] = std::vector<SharedObject<T>>((barr.m_array[0]).size());
      for (size_t ill = 0; ill < (barr.m_array[0]).size(); ++ill) {
        m_array[ihl][ill] = (barr.m_array)[ihl][ill];
      }
    }
  }
  /**Assignment operator*/
  BinnedArray2DT& operator=(const BinnedArray2DT& barr)
  {
    if (this != &barr) {
      m_arrayObjects.release();
      delete m_binUtility;
      // now refill
      m_binUtility = (barr.m_binUtility) ? barr.m_binUtility->clone() : nullptr;
      // assign over
      m_array=std::vector<std::vector<SharedObject<T>>>(barr.m_array.size());
      for (size_t ihl = 0; ihl < barr.m_array.size(); ++ihl) {
        m_array[ihl] = std::vector<SharedObject<T>>((barr.m_array[0]).size());
        for (size_t ill = 0; ill < ((barr.m_array)[0]).size(); ++ill) {
          m_array[ihl][ill] = (barr.m_array)[ihl][ill];
        }
      }
    }
    return *this;
  }
  /** Implizit Constructor */
  BinnedArray2DT* clone() const { return new BinnedArray2DT(*this); }

  /**Virtual Destructor*/
  ~BinnedArray2DT() { delete m_binUtility; }

  /** Returns the pointer to the templated class object from the BinnedArrayT,
      it returns 0 if not defined;
   */
  T* object(const Amg::Vector2D& lp) const
  {
    if (m_binUtility->inside(lp)) {
      return (m_array[m_binUtility->bin(lp, 1)][m_binUtility->bin(lp, 0)]).get();
    }
    return nullptr;
  }

  /** Returns the pointer to the templated class object from the BinnedArrayT
      it returns 0 if not defined;
   */
  T* object(const Amg::Vector3D& gp) const
  {
    if (m_binUtility->inside(gp)) {
      return (m_array[m_binUtility->bin(gp, 1)][m_binUtility->bin(gp, 0)]).get();
    }
    return nullptr;
  }

  /** Returns the pointer to the templated class object
   * from the BinnedArrayT -entry point*/
  T* entryObject(const Amg::Vector3D& pos) const
  {
    return (m_array[m_binUtility->entry(pos, 1)][m_binUtility->entry(pos, 0)]).get();
  }

  /** Returns the pointer to the templated class object
   * from the BinnedArrayT*/
  T* nextObject(const Amg::Vector3D& gp,
                const Amg::Vector3D& mom,
                bool associatedResult = true) const
  {
    // direct access to associated one
    if (associatedResult)
      return object(gp);
    size_t nextFirst = m_binUtility->next(gp, mom, 0);
    size_t nextSecond = m_binUtility->next(gp, mom, 1);

    return (nextFirst > 0 && nextSecond > 0)
             ? (m_array[nextSecond][nextFirst]).get()
             : nullptr;
  }

  /** Return all objects of the Array */
  const std::vector<T*>& arrayObjects() const
  {
    if (!m_arrayObjects) {

      std::unique_ptr<std::vector<T*>> arrayObjects =
        std::make_unique<std::vector<T*>>();

      arrayObjects->reserve(arrayObjectsNumber());

      for (size_t ihl = 0; ihl < (m_binUtility->bins(1)); ++ihl) {
        for (size_t ill = 0; ill < (m_binUtility->bins(0)); ++ill) {
          arrayObjects->push_back((m_array[ihl][ill]).get());
        }
      }
      m_arrayObjects.set(std::move(arrayObjects));
    }

    return (*m_arrayObjects);
  }

  /** Number of Entries in the Array */
  unsigned int arrayObjectsNumber() const
  {
    return (m_array.size() * (m_array[0]).size());
  }

  /** Return the BinUtility*/
  const BinUtility* binUtility() const { return (m_binUtility); }

private:
  //!< vector of pointers to the class T
  std::vector<std::vector<SharedObject<T>>> m_array;
  //!< forced 1D vector of pointers to class T
  CxxUtils::CachedUniquePtr<std::vector<T*>> m_arrayObjects;
  //!< binUtility for retrieving and filling the Array
  BinUtility* m_binUtility;
};

template<class T>
using BinnedArray2D = BinnedArray2DT<const T>;
} // end of namespace Trk

#endif // TRKSURFACES_BINNEDARRAY2D_H

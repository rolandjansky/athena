/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// BinnedArray1D1D.h, (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////

#ifndef TRKDETDESCRUTILS_BINNEDARRAY1D1D_H
#define TRKDETDESCRUTILS_BINNEDARRAY1D1D_H

#include "TrkDetDescrUtils/BinUtility.h"
#include "TrkDetDescrUtils/BinnedArray.h"
#include "TrkDetDescrUtils/SharedObject.h"

#include <vector>
#include "CxxUtils/CachedUniquePtr.h"

class MsgStream;

namespace Trk {

/** @class BinnedArray1D1DT

    2D dimensional binned array, where the binning grid is
    not symmetric.
    One steering bin utility finds the associated array of the other.

    @author Andreas.Salzburger@cern.ch
 */

template<class T>
class BinnedArray1D1DT : public BinnedArrayT<T>
{

public:
  /** Default Constructor - needed for inherited classes */
  BinnedArray1D1DT()
    : BinnedArrayT<T>()
    , m_array{}
    , m_arrayObjects(nullptr)
    , m_steeringBinUtility(nullptr)
    , m_singleBinUtilities(nullptr)
  {}

  /** Constructor with std::vector and a  BinUtility - reference counted, will
     delete objects at the end, if this deletion should be turned off, the
     boolean deletion should be switched to false the global position is given
     by object! */
  BinnedArray1D1DT(
    const std::vector<std::pair<SharedObject<T>, Amg::Vector3D>>& tclassvector,
    BinUtility* steeringBinGen1D,
    std::vector<BinUtility*>* singleBinGen)
    : BinnedArrayT<T>()
    , m_array{}
    , m_arrayObjects(nullptr)
    , m_steeringBinUtility(steeringBinGen1D)
    , m_singleBinUtilities(singleBinGen)
  {

    // prepare the binned Array
    if (steeringBinGen1D) {
      m_array =
        std::vector<std::vector<SharedObject<T>>>(steeringBinGen1D->bins());
      for (size_t i = 0; i < steeringBinGen1D->bins(); ++i) {
        size_t sizeOfSubBin = ((*m_singleBinUtilities)[i])->bins();
        m_array[i] = std::vector<SharedObject<T>>(sizeOfSubBin);
      }

      // fill the Volume vector into the array
      int vecsize(tclassvector.size());
      for (int ivec = 0; ivec < vecsize; ++ivec) {
        const Amg::Vector3D currentGlobal((tclassvector[ivec]).second);
        if (steeringBinGen1D->inside(currentGlobal)) {
          int steeringBin = steeringBinGen1D->bin(currentGlobal, 0);
          int singleBin =
            ((*m_singleBinUtilities)[steeringBin])->bin(currentGlobal, 0);
          std::vector<SharedObject<T>>& curVec = m_array[steeringBin];
          curVec[singleBin] = ((tclassvector)[ivec]).first;
        } else
          throw GaudiException(
            "BinnedArray1D1DT", "Object outside bounds", StatusCode::FAILURE);
      }
    }
  }

  /** Copy Constructor - copies only pointers! */
  BinnedArray1D1DT(const BinnedArray1D1DT& barr)
    : BinnedArrayT<T>()
    , m_array{}
    , m_arrayObjects(nullptr)
    , m_steeringBinUtility(nullptr)
    , m_singleBinUtilities(nullptr)
  {
    // prepare the binUtilities
    m_steeringBinUtility = (barr.m_steeringBinUtility)
                             ? barr.m_steeringBinUtility->clone()
                             : nullptr;
    m_singleBinUtilities = new std::vector<BinUtility*>;
    std::vector<BinUtility*>::iterator singleBinIter =
      barr.m_singleBinUtilities->begin();
    for (; singleBinIter != barr.m_singleBinUtilities->end(); ++singleBinIter){
      m_singleBinUtilities->push_back((*singleBinIter)->clone());
    }

    // prepare the binned Array
    if (m_steeringBinUtility && !m_singleBinUtilities->empty()) {
      // prepare the array
      m_array = std::vector<std::vector<SharedObject<T>>>(
        m_steeringBinUtility->bins(0));
      for (size_t i = 0; i < m_steeringBinUtility->bins(0); ++i) {
        size_t sizeOfSubBin = ((*m_singleBinUtilities)[i])->bins(0);
        m_array[i] = std::vector<SharedObject<T>>(sizeOfSubBin, nullptr);
      }

      // assign the items
      for (size_t isteer = 0; isteer < m_steeringBinUtility->bins(0); ++isteer){
        for (size_t isingle = 0; isingle < (*m_singleBinUtilities)[isteer]->bins(0);++isteer){
          m_array[isteer][isingle] = (barr.m_array)[isteer][isingle];
        }
      }
    }
  }

  /** Assignment operator */
  BinnedArray1D1DT& operator=(const BinnedArray1D1DT& barr)
  {
    if (this != &barr) {

      m_arrayObjects.release();
      delete m_steeringBinUtility;

      std::vector<BinUtility*>::iterator singleBinIter =
        m_singleBinUtilities->begin();
      for (; singleBinIter != m_singleBinUtilities->end(); ++singleBinIter){
        delete *singleBinIter;
      }
      delete m_singleBinUtilities;
      m_singleBinUtilities = new std::vector<BinUtility*>;

      // now assign the stuff
      m_steeringBinUtility =
        (barr.m_steeringBinUtility) ? (barr.m_steeringBinUtility)->clone() : 0;

      singleBinIter = barr.m_singleBinUtilties->begin();
      for (; singleBinIter != barr.m_singleBinUtilities->end(); ++singleBinIter)
        m_singleBinUtilities->push_back((*singleBinIter)->clone());

      // prepare the binned Array
      if (m_steeringBinUtility && m_singleBinUtilities->size()) {
        // prepare the array
        m_array=std::vector<std::vector<SharedObject<T>>>(m_steeringBinUtility->bins(0));
        for (int i = 0; i < m_steeringBinUtility->bins(0); ++i) {
          unsigned int sizeOfSubBin = ((*m_singleBinUtilities)[i])->bins(0);
          m_array[i] = std::vector<SharedObject<T>>(sizeOfSubBin);
        }

        // assign the items
        for (int isteer = 0; isteer < m_steeringBinUtility->bins(0); ++isteer) {
          for (int isingle = 0;isingle < (*m_singleBinUtilities)[isteer]->bins(0);++isteer) {
            m_array[isteer][isingle] = (barr.m_array)[isteer][isingle];
          }
        }
      }
    }
    return *this;
  }

  /** Implicit Constructor */
  BinnedArray1D1DT* clone() const { return new BinnedArray1D1DT(*this); }

  /** Virtual Destructor */
  virtual ~BinnedArray1D1DT()
  {
    delete m_steeringBinUtility;
    if (m_singleBinUtilities) {
      std::vector<BinUtility*>::iterator binIter = m_singleBinUtilities->begin();
      for (; binIter != m_singleBinUtilities->end(); ++binIter){
        delete *binIter;
      }
    }
    delete m_singleBinUtilities;
  }

  /** Returns the pointer to the templated class object from the BinnedArrayT,
      it returns 0 if not defined
   */
  T* object(const Amg::Vector2D& lp) const
  {
    int steerBin = m_steeringBinUtility->bin(lp, 0);
    int singleBin = (*m_singleBinUtilities)[steerBin]->bin(lp, 0);
    return (m_array[steerBin][singleBin]).get();
  }

  /** Returns the pointer to the templated class object from the BinnedArrayT,
      it returns 0 if not defined
   */
  T* object(const Amg::Vector3D& gp) const
  {
    int steerBin = m_steeringBinUtility->bin(gp, 0);
    int singleBin = (*m_singleBinUtilities)[steerBin]->bin(gp, 0);
    return (m_array[steerBin][singleBin]).get();
  }

  /** Returns the pointer to the templated class object from the BinnedArrayT -
   * entry point */
  T* entryObject(const Amg::Vector3D& gp) const
  {
    int steerBin = m_steeringBinUtility->entry(gp, 0);
    int singleBin = (*m_singleBinUtilities)[steerBin]->entry(gp, 0);
    return (m_array[steerBin][singleBin]).get();
  }

  /** Returns the pointer to the templated class object from the BinnedArrayT */
  T* nextObject(const Amg::Vector3D&, const Amg::Vector3D&, bool) const
  {
    return nullptr;
  }

  /** Return all objects of the Array */
  const std::vector<T*>& arrayObjects() const
  {
    if (!m_arrayObjects) {
      std::unique_ptr<std::vector<T*>> arrayObjects =
        std::make_unique<std::vector<T*>>();
      for (size_t isteer = 0; isteer < m_steeringBinUtility->bins(); ++isteer) {
        for (size_t isingle = 0;
             isingle < (*m_singleBinUtilities)[isteer]->bins();
             ++isingle) {
          arrayObjects->push_back((m_array[isteer][isingle]).get());
        }
      }
      m_arrayObjects.set(std::move(arrayObjects));
    }
    return (*m_arrayObjects);
  }

  /** Number of Entries in the Array */
  unsigned int arrayObjectsNumber() const { return arrayObjects().size(); }

  /** Return the BinUtility - returns the steering binUtility in this case*/
  const BinUtility* binUtility() const { return (m_steeringBinUtility); }

private:
  std::vector<std::vector<SharedObject<T>>> m_array; //!< vector of pointers to the class T
  CxxUtils::CachedUniquePtr<std::vector<T*>> m_arrayObjects; //!< forced 1D vector of pointers to class T
  BinUtility* m_steeringBinUtility; //!< binUtility for retrieving and filling the Array
  std::vector<BinUtility*>* m_singleBinUtilities; //!< single bin utilities
};
template<class T>
using BinnedArray1D1D = BinnedArray1D1DT<const T>;
} // end of namespace Trk

#endif // TRKSURFACES_BINNEDARRAY1D1D_H

/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// BinnedArray1D1D1D.h, (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////

#ifndef TRKDETDESCRUTILS_BINNEDARRAY1D1D1D_H
#define TRKDETDESCRUTILS_BINNEDARRAY1D1D1D_H
//
#include "TrkDetDescrUtils/BinnedArray.h"
#include "TrkDetDescrUtils/BinUtility.h"
#include "TrkDetDescrUtils/SharedObject.h"
//
// STL
#include <cassert>
#include <vector>
//
class MsgStream;
//
namespace Trk {

/** @class BinnedArray1D1D1DT
  
    Avoiding a map search, the templated BinnedArrayT class can help
    ordering geometrical objects by providing a dedicated BinUtility.
  
    dedicated for 3-dim (non-eq) binning ; allows variable binning in 1 coordinate
  
    @author Sarka.Todorova@cern.ch
*/

template<class T>
class BinnedArray1D1D1DT : public BinnedArrayT<T>
{

public:
  /**Default Constructor - needed for inherited classes */
  BinnedArray1D1D1DT()
    : BinnedArrayT<T>()
    , m_array{}
    , m_arrayObjects(nullptr)
    , m_binUtil1(nullptr)
    , m_binUtil2(nullptr)
    , m_binUtilArray(nullptr)
  {}

  /**Constructor with std::vector and a  BinUtility - reference counted, will delete objects at the end,
     if this deletion should be turned off, the boolean deletion should be switched to false
     the global position is given by object! */
  BinnedArray1D1D1DT(const std::vector<std::pair<SharedObject<T>, Amg::Vector3D>>& tclassvector,
                    BinUtility* binUtil1,
                    BinUtility* binUtil2,
                    std::vector<std::vector<BinUtility*>>* binUtilVec)
    : BinnedArrayT<T>()
    , m_array{}
    , m_arrayObjects(nullptr)
    , m_binUtil1(binUtil1)
    , m_binUtil2(binUtil2)
    , m_binUtilArray(binUtilVec)
  {
    // prepare the binned Array
    if (!binUtil1 || !binUtil2 || !binUtilVec) {
      throw std::logic_error("Invalid BinUtilities");
    }

    {
      int v1Size = binUtil1->bins();
      int v2Size = binUtil2->bins();
      m_array = std::vector<std::vector<std::vector<SharedObject<T>>>>(v1Size);
      for (int i = 0; i < v1Size; ++i) {
        m_array[i] = std::vector<std::vector<SharedObject<T>>>(v2Size);
        for (int j = 0; j < v2Size; ++j) {
          m_array[i][j] = std::vector<SharedObject<T>>((*binUtilVec)[i][j]->bins());
        }
      }
    }

    // fill the Volume vector into the array
    int vecsize(tclassvector.size());
    for (int ivec = 0; ivec < vecsize; ++ivec) {
      Amg::Vector3D currentGlobal((tclassvector[ivec]).second);
      if (binUtil1->inside(currentGlobal) && binUtil2->inside(currentGlobal)) {
        int bin1 = binUtil1->bin(currentGlobal);
        int bin2 = binUtil2->bin(currentGlobal);
        assert((*binUtilVec)[bin1][bin2]);
        int bin3 = (*binUtilVec)[bin1][bin2]->bin(currentGlobal);
        std::vector<std::vector<SharedObject<T>>>& currArr = m_array[bin1];
        std::vector<SharedObject<T>>& curVec = currArr[bin2];
        curVec[bin3] = ((tclassvector)[ivec]).first;
      } else
        throw GaudiException("BinnedArray1D1D1DT", "Object outside bounds", StatusCode::FAILURE);
    }
  }

  /** Copy Constructor - copies only pointers! */
  BinnedArray1D1D1DT(const BinnedArray1D1D1DT& barr)
    : BinnedArrayT<T>()
    , m_array{}
    , m_arrayObjects(nullptr)
    , m_binUtil1(nullptr)
    , m_binUtil2(nullptr)
    , m_binUtilArray(nullptr)
  {
    // prepare the binUtilities
    m_binUtil1 = (barr.m_binUtil1) ? barr.m_binUtil1->clone() : 0;
    m_binUtil2 = (barr.m_binUtil2) ? barr.m_binUtil2->clone() : 0;
    m_binUtilArray = new std::vector<std::vector<BinUtility*>>;
    std::vector<std::vector<BinUtility*>>::iterator singleBinIter = barr.m_binUtilArray->begin();
    for (; singleBinIter != barr.m_binUtilArray->end(); ++singleBinIter) {
      m_binUtilArray->push_back(std::vector<BinUtility*>((*singleBinIter).size()));
      for (unsigned int ibin = 0; ibin < m_binUtilArray->back().size(); ibin++)
        m_binUtilArray->back()[ibin] = (*singleBinIter)[ibin]->clone();
    }

    // prepare the binned Array
    if (m_binUtil1 && m_binUtil2 && m_binUtilArray->size()) {
      int v1Size = m_binUtil1->bins();
      int v2Size = m_binUtil2->bins();
      m_array = std::vector<std::vector<std::vector<SharedObject<T>>>>(v1Size);
      for (int i = 0; i < v1Size; ++i) {
        m_array[i] = std::vector<std::vector<SharedObject<T>>>(v2Size);
        for (int j = 0; j < v2Size; ++j)
          m_array[i][j] = std::vector<SharedObject<T>>((*m_binUtilArray)[i][j]->bins());
      }

      // assign the items
      for (int ibin1 = 0; ibin1 < v1Size; ++ibin1) {
        for (int ibin2 = 0; ibin2 < v2Size; ++ibin2) {
          for (size_t ibin3 = 0; ibin3 < (*m_binUtilArray)[ibin1][ibin2]->bins(); ++ibin3) {
            m_array[ibin1][ibin2][ibin3] = (barr.m_array)[ibin1][ibin2][ibin3];
          }
        }
      }
    }
  }

  /** Assignment operator */
  BinnedArray1D1D1DT& operator=(const BinnedArray1D1D1DT& barr)
  {
    if (this != &barr) {

      m_arrayObjects.release();
      // bin utilities
      size_t v1Size = m_binUtil1->bins();
      size_t v2Size = m_binUtil2->bins();
      delete m_binUtil1;
      delete m_binUtil2;

      for (size_t ibin1 = 0; ibin1 < v1Size; ibin1++) {
        for (size_t ibin2 = 0; ibin2 < v2Size; ibin2++)
          delete (*m_binUtilArray)[ibin1][ibin2];
      }
      delete m_binUtilArray;
      m_binUtilArray = new std::vector<std::vector<BinUtility*>>;

      // now assign the stuff
      m_binUtil1 = (barr.m_binUtil1) ? (barr.m_binUtil1)->clone() : 0;
      m_binUtil2 = (barr.m_binUtil2) ? (barr.m_binUtil2)->clone() : 0;

      std::vector<std::vector<BinUtility*>>::iterator singleBinIter = barr.m_binUtilArray->begin();
      for (; singleBinIter != barr.m_binUtilArray->end(); ++singleBinIter) {
        m_binUtilArray->push_back(std::vector<BinUtility*>((*singleBinIter).size()));
        for (int ibin = 0; ibin < m_binUtilArray->back().size(); ibin++)
          m_binUtilArray->back()[ibin] = (*singleBinIter)[ibin]->clone();
      }

      // prepare the binned Array
      if (m_binUtil1 && m_binUtil2 && m_binUtilArray->size()) {
        size_t v1Size = m_binUtil1->bins();
        size_t v2Size = m_binUtil2->bins();
        m_array = std::vector<std::vector<std::vector<SharedObject<T>>>>(v1Size);
        for (size_t i = 0; i < v1Size; ++i) {
          m_array[i] = std::vector<std::vector<SharedObject<T>>>(v2Size);
          for (size_t j = 0; j < v2Size; ++j)
            m_array[i][j] = std::vector<SharedObject<T>>((*m_binUtilArray)[i][j]->bins());
        }

        // assign the items
        for (size_t ibin1 = 0; ibin1 < v1Size; ++ibin1) {
          for (size_t ibin2 = 0; ibin2 < v2Size; ++ibin2) {
            for (size_t ibin3 = 0; ibin3 < (*m_binUtilArray)[ibin1][ibin2]->bins(); ++ibin3) {
              m_array[ibin1][ibin2][ibin3] = (barr.m_array)[ibin1][ibin2][ibin3];
            }
          }
        }
      }
    }
    return *this;
  }

  /** Implicit Constructor */
  BinnedArray1D1D1DT* clone() const { return new BinnedArray1D1D1DT(*this); }

  /**Virtual Destructor*/
  ~BinnedArray1D1D1DT()
  {
    int v1Size = m_binUtil1->bins();
    int v2Size = m_binUtil2->bins();
    delete m_binUtil1;
    delete m_binUtil2;

    for (int ibin1 = 0; ibin1 < v1Size; ibin1++) {
      for (int ibin2 = 0; ibin2 < v2Size; ibin2++)
        delete (*m_binUtilArray)[ibin1][ibin2];
    }
    delete m_binUtilArray;
  }

  /** Returns the pointer to the templated class object from the BinnedArrayT,
      it returns 0 if not defined
   */
  T* object(const Amg::Vector2D& lp) const
  {
    if (!m_binUtil1->inside(lp) || !m_binUtil2->inside(lp))
      return 0;
    int bin1 = m_binUtil1->bin(lp);
    int bin2 = m_binUtil2->bin(lp);
    if (!(*m_binUtilArray)[bin1][bin2]->inside(lp))
      return 0;
    int bin3 = (*m_binUtilArray)[bin1][bin2]->bin(lp);

    return (m_array[bin1][bin2][bin3]).get();
  }

  /** Returns the pointer to the templated class object from the BinnedArrayT,
      it returns 0 if not defined
   */
  T* object(const Amg::Vector3D& gp) const
  {
    if (!m_binUtil1->inside(gp) || !m_binUtil2->inside(gp))
      return 0;
    int bin1 = m_binUtil1->bin(gp);
    int bin2 = m_binUtil2->bin(gp);
    if (!(*m_binUtilArray)[bin1][bin2]->inside(gp))
      return 0;
    unsigned int bin3 = (*m_binUtilArray)[bin1][bin2]->bin(gp);

    if (bin3 >= (*m_binUtilArray)[bin1][bin2]->bins())
      return 0;

    return (m_array[bin1][bin2][bin3]).get();
  }

  /** Returns the pointer to the templated class object from the BinnedArrayT - entry point */
  T* entryObject(const Amg::Vector3D& gp) const
  {
    int bin1 = m_binUtil1->entry(gp, 0);
    int bin2 = m_binUtil2->entry(gp, 0);
    int bin3 = (*m_binUtilArray)[bin1][bin2]->entry(gp, 0);

    return (m_array[bin1][bin2][bin3]).get();
  }

  /** Returns the pointer to the templated class object from the BinnedArrayT
      dummy for multidimensional arrays      */
  T* nextObject(const Amg::Vector3D&, const Amg::Vector3D&, bool) const { return 0; }

  /** Return all objects of the Array */
  const std::vector<T*>& arrayObjects() const
  {
    if (!m_arrayObjects) {
      std::unique_ptr<std::vector<T*>> arrayObjects = std::make_unique<std::vector<T*>>();
      for (size_t ibin1 = 0; ibin1 < m_binUtil1->bins(); ++ibin1) {
        for (size_t ibin2 = 0; ibin2 < m_binUtil2->bins(); ++ibin2) {
          for (size_t ibin3 = 0; ibin3 < (*m_binUtilArray)[ibin1][ibin2]->bins(); ++ibin3) {
            arrayObjects->push_back((m_array[ibin1][ibin2][ibin3]).get());
          }
        }
      }
      m_arrayObjects.set(std::move(arrayObjects));
    }
    return (*m_arrayObjects);
  }

  /** Number of Entries in the Array */
  unsigned int arrayObjectsNumber() const { return arrayObjects().size(); }

  /** Return the BinUtility - returns the first binUtility in this case*/
  const BinUtility* binUtility() const { return (m_binUtil1); }

private:
  std::vector<std::vector<std::vector<SharedObject<T>>>> m_array; //!< vector of pointers to the class T
  CxxUtils::CachedUniquePtr<std::vector<T*>> m_arrayObjects;         //!< forced 1D vector of pointers to class T
  BinUtility* m_binUtil1;                                //!< binUtility for retrieving and filling the Array
  BinUtility* m_binUtil2;                                //!< binUtility for retrieving and filling the Array
  std::vector<std::vector<BinUtility*>>* m_binUtilArray; //!< binUtility for retrieving and filling the Array
};
template <class T>
using BinnedArray1D1D1D = BinnedArray1D1D1DT<const T>;
} // end of namespace Trk
#endif // TRKSURFACES_BINNEDARRAY1D1D1D_H
